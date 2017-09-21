// ScanThread.cpp : implementation file
//

#include "stdafx.h"
#include "DuplicateFileFinder.h"
#include "ScanThread.h"
#include "WorkerDialog.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>


struct _SDuplicateInfo
{
	UINT nMask;
	TCHAR szName[MAX_PATH+1];
	__int64 iSize;
	DWORD dwAttributes;
	CArray<uint16_t>* pCRC;
	SYSTEMTIME tmCreate;
	SYSTEMTIME tmAccess;
	SYSTEMTIME tmWrite;
};

struct _SFilesDuplicateInfo
{
	struct _SDuplicateInfo DuplicateInfo;
	CMapStringToString* DuplicateFiles;
};

#define BUFFER_SIZE	0xFA00

CScanThread* CScanThread::m_sInstance = nullptr;

CScanThread::CScanThread(CWnd* pOwner /* = nullptr */, CWorkerDialog* pProgressDlg /* = nullptr */, UINT nMessageID /* = 0U */)
{
	CScanThread::m_sInstance = this;
	m_pOwnerWnd = pOwner;
	m_pProgressDlg = pProgressDlg;
	m_arrPaths.RemoveAll();
	m_bRunning = FALSE;
	m_bAutoDelete = FALSE;
	m_nMessageID = nMessageID;
	m_nDuplicateMask = (DUPLICATE_CRITERIA_CONTENT | DUPLICATE_CRITERIA_SIZE);
}

CScanThread::~CScanThread()
{
	RemoveAllFilters();
}

BOOL CScanThread::InitInstance()
{
	return TRUE;
}

int CScanThread::ExitInstance()
{
	bool bLocked = m_RunSection.try_lock();
	m_bRunning = FALSE;
	if(bLocked) {
		m_RunSection.unlock();
		bLocked = false;
	}
	CloseHandle(m_hThread);
	m_hThread = nullptr;

	RestoreAllControlsEnableState();

	if(m_pOwnerWnd && m_pOwnerWnd->GetSafeHwnd())
		m_pOwnerWnd->SendMessage(m_nMessageID, (WPARAM)TM_EXIT);

	if(m_pProgressDlg) {
		if(m_pProgressDlg->GetSafeHwnd())
			m_pProgressDlg->SendMessage(WM_CLOSE, 0, 0);
	}
	
	m_arrPaths.RemoveAll();
	return CWinThread::ExitInstance();
}

void CScanThread::Finalize()
{
	if(m_bRunning) {
		bool bLocked = m_RunSection.try_lock();
		m_bRunning = FALSE;
		if(bLocked) {
			m_RunSection.unlock();
			bLocked = false;
		}
	}
}

BOOL CScanThread::Initialize(const CStringArray& strPath, BOOL bRecursive)
{
	m_bRecursive = bRecursive;
	m_arrPaths.Copy(strPath);
	m_arrScannedFiles.RemoveAll();
	m_arrScannedFolder.RemoveAll();

	if(m_arrPaths.GetCount() <= 0)
		return FALSE;

	if(m_pProgressDlg) {
		m_pProgressDlg->SetProgressRange(0, 100);
		m_pProgressDlg->SetProgressType(CWorkerDialog::eMARQUEE);
		m_pProgressDlg->SetProgressValue(0);
	}

	m_bRunning = TRUE;
	
	return TRUE;
}

int CScanThread::Run()
{
	EnableAllControls(FALSE);
	bool bLocked = false;
	m_pProgressDlg->SetJobName(_T("Scan Files..."));
	m_pProgressDlg->SetMessageTitle(_T("Current File:"));
	CString	strCurrentPath;
	CString	strMessage;
	__int64 iTotalSize = 0;
	for(INT_PTR i=0;i<m_arrPaths.GetCount();i++)
	{
		strCurrentPath = m_arrPaths.GetAt(i);

		bLocked = m_RunSection.try_lock();
		if(!m_bRunning)
			break;

		if(bLocked) {
			m_RunSection.unlock();
			bLocked = false;
		}

		m_arrScannedFolder.SetAt(strCurrentPath, strCurrentPath);
		WalkDir(strCurrentPath, m_bRecursive, iTotalSize);
	}
	if(bLocked) {
		m_RunSection.unlock();
		bLocked = false;
	}

	if(m_pProgressDlg) {
		m_pProgressDlg->SetProgressType(CWorkerDialog::eFILL);
		m_pProgressDlg->SetProgressValue(0);
		m_pProgressDlg->SetProgressRange(0, 100);
		m_pProgressDlg->SetJobName(_T("Analyze Files..."));
	}

	CMapStringToString arrDuplicateFiles;
	CMapStringToString arrTotalDuplicateFiles;
	CArray<uint16_t> arrCRCs;
	struct _SDuplicateInfo duplicateInfo={0};
	LARGE_INTEGER li={0};
	POSITION pos = nullptr;
	CString strKey;
	CString strVal;
	for(INT_PTR j=0;j<m_arrScannedFiles.GetCount();j++) {
		bLocked = m_RunSection.try_lock();

		if(!m_bRunning)
			break;

		if(bLocked) {
			m_RunSection.unlock();
			bLocked = false;
		}
		m_strCurrentFile = m_arrScannedFiles.GetAt(j);

		if(m_pProgressDlg) {
			strMessage.Format(_T("Current File (%I64d/%I64d): "), static_cast<__int64>(j), static_cast<__int64>(m_arrScannedFiles.GetCount()));
			m_pProgressDlg->SetMessageTitle(strMessage);
			m_pProgressDlg->SetMessageContent(m_strCurrentFile);
		}

		if(!arrTotalDuplicateFiles.Lookup(m_strCurrentFile, strVal)) {
			//arrDuplicateFiles.RemoveAll();
			arrCRCs.RemoveAll();
			memset(&duplicateInfo, 0, sizeof(struct _SDuplicateInfo));
			duplicateInfo.pCRC = &arrCRCs;
			if(FindDuplicate(m_strCurrentFile, arrDuplicateFiles, &duplicateInfo, &arrTotalDuplicateFiles) > 0) {
				arrDuplicateFiles.SetAt(m_strCurrentFile, m_strCurrentFile);
				struct _SFilesDuplicateInfo* dup = new _SFilesDuplicateInfo();
				dup->DuplicateFiles = new CMapStringToString();
				pos = arrDuplicateFiles.GetStartPosition();
				while (pos) {
					arrDuplicateFiles.GetNextAssoc(pos, strKey, strVal);
					dup->DuplicateFiles->SetAt(strKey, strVal);
				}
			
				memcpy(&dup->DuplicateInfo, &duplicateInfo, sizeof(struct _SDuplicateInfo));
				dup->DuplicateInfo.pCRC = new CArray<uint16_t>();
				dup->DuplicateInfo.pCRC->Copy(arrCRCs);
				if(m_pOwnerWnd && m_pOwnerWnd->GetSafeHwnd()) {
					m_pOwnerWnd->SendMessage(m_nMessageID, (WPARAM)&m_strCurrentFile, (LPARAM)dup);
				}
			}
			else {
				m_arrDuplicateChecks.RemoveKey(m_strCurrentFile);
			}
		}

		if(m_pProgressDlg) {
			m_pProgressDlg->SetProgressValue(static_cast<int>(static_cast<double>(j)/static_cast<double>(m_arrScannedFiles.GetCount())*100.0) );
		}
	}

	if(bLocked) {
		m_RunSection.unlock();
		bLocked = false;
	}

	RestoreAllControlsEnableState();
	return ExitInstance();
}

void CScanThread::RemoveAllFilters()
{
	if(m_ExcludeFilters.GetCount() > 0) {
		POSITION pos = m_ExcludeFilters.GetStartPosition();
		EFileFolderFilterCriteria eCriteria;
		CFileFolderFilter* pFilter = nullptr;
		m_ExcludeFilters.GetNextAssoc(pos, eCriteria, pFilter);
		if(pFilter != nullptr) {
			delete pFilter;
			pFilter = nullptr;
		}
		m_ExcludeFilters.RemoveAll();
	}
}


void CScanThread::EnableAllControls(BOOL bEnabled)
{
	m_Enables.RemoveAll();

	if(m_pOwnerWnd) {
		m_EnumFlag = EEnumChildSetEnable;
		m_bEnumVal = bEnabled;
		EnumChildWindows(m_pOwnerWnd->GetSafeHwnd(), EnumChildProc, reinterpret_cast<LPARAM>(this));
	}
}

void CScanThread::RestoreAllControlsEnableState()
{
	if(m_pOwnerWnd) {
		m_EnumFlag = EEnumChildRestoreEnable;
		EnumChildWindows(m_pOwnerWnd->GetSafeHwnd(), EnumChildProc, reinterpret_cast<LPARAM>(this));
	}
}

BOOL CALLBACK CScanThread::EnumChildProc(HWND hwnd, LPARAM lParam)
{
	CScanThread* instance = reinterpret_cast<CScanThread*>(lParam);
	if(instance != nullptr)
		return instance->OnEnumChild(hwnd);

	return TRUE;
}

BOOL CScanThread::OnEnumChild(HWND hChild)
{
	if(m_EnumFlag == EEnumChildSetEnable) {
		m_Enables.SetAt(hChild, ::IsWindowEnabled(hChild));
		::EnableWindow(hChild, m_bEnumVal);
	}
	else if(m_EnumFlag == EEnumChildRestoreEnable)
	{
		BOOL bEnabled = TRUE;
		m_Enables.Lookup(hChild, bEnabled);
		::EnableWindow(hChild, bEnabled);
	}

	return TRUE;
}

BOOL CScanThread::IsExcludeFile(const CString& strPath)
{
	if(m_ExcludeFilters.GetCount() <= 0)
		return FALSE;

	POSITION pos = m_ExcludeFilters.GetStartPosition();
	EFileFolderFilterCriteria eCriteria;
	CFileFolderFilter* pFilter = nullptr;
	UINT nCount = 0;
	while (pos)
	{
		m_ExcludeFilters.GetNextAssoc(pos, eCriteria, pFilter);
		if(pFilter != nullptr) {
			if(pFilter->Filter(strPath))
				nCount++;

			pFilter = nullptr;
		}
	}
	
	return (nCount == m_ExcludeFilters.GetCount());
}

void CScanThread::WalkDir(const CString& strPath, BOOL bRecursive, __int64& iTotalSize)
{
	WIN32_FIND_DATA fd={0};

	
	CString	strFileName;
	CString	strOriginPath = strPath;
	CString	strFullPath;
	CString	strPattern;
	CString	strLookup;
	if(strOriginPath.Right(1) != '\\') {
		strOriginPath.Append(_T("\\"));
	}

	strPattern = strOriginPath + _T("*");
	LARGE_INTEGER li={0};
	HANDLE hSearch = ::FindFirstFile(strPattern, &fd);
	bool bLocked = false;
	if(hSearch != INVALID_HANDLE_VALUE) {
		do 
		{
			bLocked = m_RunSection.try_lock();
			if(!m_bRunning)
				break;

			if(bLocked) {
				m_RunSection.unlock();
				bLocked = false;
			}

			strFileName = fd.cFileName;
			if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if( (strFileName != _T(".")) && (strFileName != _T("..")) ) {
					strFullPath = strOriginPath + strFileName;
					if(!m_arrScannedFolder.Lookup(strFullPath, strLookup)) {
						m_arrScannedFolder.SetAt(strFullPath, strFullPath);
						WalkDir(strFullPath, bRecursive, iTotalSize);
					}
				}
			}
			else {
				strFullPath = strOriginPath + strFileName;
				if(m_pProgressDlg)
					m_pProgressDlg->SetMessageContent(strFullPath);

				if(!IsExcludeFile(strFullPath)) {
					if(m_pOwnerWnd && m_pOwnerWnd->GetSafeHwnd()) {
						m_arrScannedFiles.Add(strFullPath);
						m_arrDuplicateChecks.SetAt(strFullPath, strFullPath);
						li.LowPart = fd.nFileSizeLow;
						li.HighPart = fd.nFileSizeHigh;
						iTotalSize += static_cast<__int64>(li.QuadPart);
					}
				}
			}

			memset(&fd, 0, sizeof(WIN32_FIND_DATA));
		} while (::FindNextFile(hSearch, &fd));

		if(bLocked) {
			m_RunSection.unlock();
			bLocked = false;
		}
	}

	::FindClose(hSearch);
}

bool CScanThread::CRCFile(const CString& strPath, CArray<uint16_t>& vecCRC, CArray<uint16_t>* vecCRCCmp /* = nullptr */, struct _SDuplicateInfo* pDuplicateInfo /* = nullptr */)
{
	vecCRC.RemoveAll();
	HANDLE hFile = CreateFile(strPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if(hFile != INVALID_HANDLE_VALUE) {

		LARGE_INTEGER li={0};
		__int64 iSize = 0;

		if(GetFileSizeEx(hFile, &li)) {
			iSize = static_cast<__int64>(li.QuadPart);
		}

		__int64 iRemainSize = iSize;
		char buff[BUFFER_SIZE]={0};
		bool bLocked = false;
		DWORD dwRead = 0;
		uint8_t uCRC = 0;
		UINT nCRCLen = 0;

		while (iRemainSize > 0)
		{
			bLocked = m_RunSection.try_lock();
			if(!m_bRunning)
				break;

			if(bLocked) {
				m_RunSection.unlock();
				bLocked = false;
			}

			memset(buff, 0, BUFFER_SIZE);

			if( (!ReadFile(hFile, buff, BUFFER_SIZE, &dwRead, nullptr)) ) {
				CloseHandle(hFile);
				hFile = INVALID_HANDLE_VALUE;
				return false;
			}

			uCRC = CRC(0, reinterpret_cast<uint8_t*>(&buff), static_cast<uint16_t>(dwRead));
			vecCRC.Add(uCRC);
			if(vecCRCCmp) {
				if(vecCRCCmp->GetSize() < vecCRC.GetSize()) {
					CloseHandle(hFile);
					hFile = INVALID_HANDLE_VALUE;
					return false;
				}

				if(vecCRCCmp->GetAt(vecCRC.GetSize()-1) != vecCRC[vecCRC.GetSize()-1]) {
					CloseHandle(hFile);
					hFile = INVALID_HANDLE_VALUE;
					return false;
				}
			}
			iRemainSize-= static_cast<__int64>(dwRead);

			if(pDuplicateInfo && pDuplicateInfo->pCRC)
				pDuplicateInfo->pCRC->Add(uCRC);
		}

		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;

		return true;
	}

	return false;
}

__int64 CScanThread::SizeOfFile(const CString& strPath)
{
	HANDLE hFile = CreateFile(strPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	__int64 iSize = {0};
	LARGE_INTEGER li={0};

	if(hFile != INVALID_HANDLE_VALUE){
		if(GetFileSizeEx(hFile, &li)) {
			iSize = static_cast<__int64>(li.QuadPart);
		}

		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

	return iSize;
}

UINT CScanThread::FindDuplicate(const CString& strPath, CMapStringToString& arrDuplicate, struct _SDuplicateInfo* pDuplicateInfo, CMapStringToString* pTotalDuplicateFiles /* = nullptr */)
{
	UINT nDup = 0U;
	arrDuplicate.RemoveAll();

	//Size
	HANDLE hFile = INVALID_HANDLE_VALUE;
	__int64 iSize0 = {0};
	LARGE_INTEGER li = {0};
	iSize0 = SizeOfFile(strPath);

	bool bLocked = m_RunSection.try_lock();
	if(!m_bRunning)
	{
		if(bLocked) {
			m_RunSection.unlock();
			bLocked = false;
		}
		return nDup;
	}

	if(bLocked) {
		m_RunSection.unlock();
		bLocked = false;
	}

	if(m_arrDuplicateChecks.GetCount() > 0) {
		POSITION pos = m_arrDuplicateChecks.GetStartPosition();
		CString	strCurPath;
		CString	strValue;

		TCHAR szDrive0[_MAX_PATH+1];
		TCHAR szDir0[_MAX_PATH+1];
		TCHAR szName0[_MAX_PATH+1];
		TCHAR szExt0[_MAX_PATH+1];
		TCHAR szDrive1[_MAX_PATH+1];
		TCHAR szDir1[_MAX_PATH+1];
		TCHAR szName1[_MAX_PATH+1];
		TCHAR szExt1[_MAX_PATH+1];

		CString	strFolder0;
		CString	strFolder1;
		CString	strFile0;
		CString	strFile1;
		UINT nMask = 0U;
		UINT nFiles = 0U;
		CArray<uint16_t> vecCRC0;
		CArray<uint16_t> vecCRC1;
		DWORD dwAttr0 = {0};
		DWORD dwAttr1 = 0;
		errno_t err1 = 0;
		__int64 iSize1 = {0};
		INT_PTR vecIndex = 0;
	
		if(m_nDuplicateMask & DUPLICATE_CRITERIA_NAME) {
			memset(szDrive0, 0, (_MAX_PATH+1)*sizeof(TCHAR));
			memset(szDir0, 0, (_MAX_PATH+1)*sizeof(TCHAR));
			memset(szName0, 0, (_MAX_PATH+1)*sizeof(TCHAR));
			memset(szExt0, 0, (_MAX_PATH+1)*sizeof(TCHAR));

			errno_t err0 = _tsplitpath_s(strPath, szDrive0, _MAX_PATH, szDir0, _MAX_PATH, szName0, _MAX_PATH, szExt0, _MAX_PATH);

			if(err0 == 0) {
				strFolder0.Format(_T("%s%s"), szDrive0, szDir0);
				strFile0.Format(_T("%s%s"), szName0, szExt0);
			}
		}

		if(m_nDuplicateMask & DUPLICATE_CRITERIA_CONTENT) {
			CRCFile(strPath, vecCRC0, nullptr, pDuplicateInfo);
		}

		if(m_nDuplicateMask & DUPLICATE_CRITERIA_ATTRIBUTES) {
			dwAttr0 = GetFileAttributes(strPath);
		}

		
		while (pos)
		{
			nMask = 0U;
			bLocked = m_RunSection.try_lock();
			if(!m_bRunning)
				break;

			if(bLocked) {
				m_RunSection.unlock();
				bLocked = false;
			}

			m_arrDuplicateChecks.GetNextAssoc(pos, strCurPath, strValue);
			if(strCurPath.CompareNoCase(strPath) == 0)
				continue;

			if(arrDuplicate.Lookup(strCurPath, strValue))
				continue;

			hFile = CreateFile(strCurPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
			if(hFile != INVALID_HANDLE_VALUE){
				if(GetFileSizeEx(hFile, &li)) {
					iSize1 = static_cast<__int64>(li.QuadPart);
				}

				CloseHandle(hFile);
				hFile = INVALID_HANDLE_VALUE;
			}

			if(m_nDuplicateMask & DUPLICATE_CRITERIA_NAME) {
				memset(szDrive1, 0, (_MAX_PATH+1)*sizeof(TCHAR));
				memset(szDir1, 0, (_MAX_PATH+1)*sizeof(TCHAR));
				memset(szName1, 0, (_MAX_PATH+1)*sizeof(TCHAR));
				memset(szExt1, 0, (_MAX_PATH+1)*sizeof(TCHAR));
				
				err1 = _tsplitpath_s(strCurPath, szDrive1, _MAX_PATH, szDir1, _MAX_PATH, szName1, _MAX_PATH, szExt1, _MAX_PATH);

				if(err1 == 0) {
					strFolder1.Format(_T("%s%s"), szDrive1, szDir1);
					strFile1.Format(_T("%s%s"), szName1, szExt1);

					if(strFile0.CompareNoCase(strFile1) != 0)
						continue;
					else {
						nMask |= DUPLICATE_CRITERIA_NAME;
						if(pDuplicateInfo) {
							_tcscpy_s(pDuplicateInfo->szName, MAX_PATH, strFile0);
						}
					}
				}
			}
		
			if(m_nDuplicateMask & DUPLICATE_CRITERIA_SIZE) {
				if(iSize0 != iSize1) 
					continue;
				else {
					nMask |= DUPLICATE_CRITERIA_SIZE;
					if(pDuplicateInfo) {
						pDuplicateInfo->iSize = iSize0;
					}
				}
			}

			if(m_nDuplicateMask & DUPLICATE_CRITERIA_CONTENT) {
				if( (iSize0 <= 0) || (iSize0 != iSize1) ) {
					continue;
				}
				else {
					//calculate SHA
					vecCRC1.RemoveAll();
					if(!CRCFile(strCurPath, vecCRC1, &vecCRC0, nullptr))
						continue;

					if(vecCRC0.GetSize() != vecCRC1.GetSize()) {
						continue;
					}

					for(vecIndex=0;vecIndex<vecCRC0.GetSize();vecIndex++) {
						bLocked = m_RunSection.try_lock();
						if(!m_bRunning)
							break;

						if(bLocked) {
							m_RunSection.unlock();
							bLocked = false;
						}

						if(vecCRC0[vecIndex] != vecCRC1[vecIndex])
							break;
					}
					if(bLocked) {
						m_RunSection.unlock();
						bLocked = false;
					}

					nMask |= DUPLICATE_CRITERIA_CONTENT;
				}
			}


			if(m_nDuplicateMask & DUPLICATE_CRITERIA_ATTRIBUTES) {
				dwAttr1 = GetFileAttributes(strCurPath);

				if(dwAttr0 != dwAttr1)
					continue;
				else {
					nMask |= DUPLICATE_CRITERIA_ATTRIBUTES;
					if(pDuplicateInfo)
						pDuplicateInfo->dwAttributes = dwAttr0;
				}
			}

			nFiles++;

			if(nMask == m_nDuplicateMask) {
				arrDuplicate.SetAt(strCurPath, strCurPath);
				if(pTotalDuplicateFiles)
					pTotalDuplicateFiles->SetAt(strCurPath, strCurPath);

				if(pDuplicateInfo) {
					pDuplicateInfo->nMask = nMask;
				}
				nDup++;
			}
		}

		if(bLocked) {
			m_RunSection.unlock();
			bLocked = false;
		}
	}

	return nDup;
}

void CScanThread::crc8PushByte(uint8_t *crc, uint8_t ch) {
	uint8_t i;

	*crc = *crc ^ ch;

	for (i=0; i<8; i++) {
		if (*crc & 1) {
			*crc = (*crc >> 1) ^0x8c;
		}
		else {
			*crc = (*crc >> 1);
		}
	}
}

uint8_t CScanThread::CRC(uint8_t *pcrc, uint8_t *block, uint16_t count) {
	uint8_t crc = pcrc ? *pcrc : 0;

	for (; count>0; --count, block++) {
		crc8PushByte(&crc, *block);
	}
	if (pcrc) *pcrc = crc;

	return crc;
}