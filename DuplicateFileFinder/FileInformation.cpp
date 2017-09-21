#include "stdafx.h"
#include "FileInformation.h"
#include "Checksum.h"

#define CHECK_SUM_BUFFER_SIZE	0xFC00	//63kb

CFileInformation::CFileInformation(CFileInformation* pInfo /* = nullptr */)
{
	if(nullptr == pInfo) {
		nMask = 0U;
		memset(szName, 0, (NAME_MAX_LEN+1)*sizeof(TCHAR));
		memset(szPath, 0, (PATH_MAX_LEN+1)*sizeof(TCHAR));
		memset(szTypeName, 0, (TYPE_NAME_MAX_LEN+1)*sizeof(TCHAR));
		memset(szChecksum, 0, (CHECK_SUM_MAX_LEN+1)*sizeof(TCHAR));
		iSize = 0;
		dwAttributes = 0;
		memset(&tmCreate, 0, sizeof(SYSTEMTIME));
		memset(&tmAccess, 0, sizeof(SYSTEMTIME));
		memset(&tmWrite, 0, sizeof(SYSTEMTIME));
	}
	else {
		nMask = pInfo->nMask;
		setName(pInfo->getName());
		setPathName(pInfo->getPath());
		setChecksum(pInfo->getChecksum(), CHECK_SUM_MAX_LEN);
		setSize(pInfo->getSize());
		setAttributes(pInfo->getAttributes());
		setCreateTime(pInfo->getCreationTime());
		setAccessTime(pInfo->getAccessTime());
		setWriteTime(pInfo->getWriteTime());
		setTypeName(pInfo->getTypeName(), false);
	}
}

CFileInformation::CFileInformation(LPCTSTR lpszFile, CCriticalSection* pSection, BOOL* pCondVar, UINT nMask) 
{
	nMask = nMask;
	memset(szName, 0, (NAME_MAX_LEN+1)*sizeof(TCHAR));
	memset(szPath, 0, (PATH_MAX_LEN+1)*sizeof(TCHAR));
	memset(szChecksum, 0, (CHECK_SUM_MAX_LEN+1)*sizeof(TCHAR));
	memset(szTypeName, 0, (TYPE_NAME_MAX_LEN+1)*sizeof(TCHAR));
	iSize = 0;
	dwAttributes = 0;
	memset(&tmCreate, 0, sizeof(SYSTEMTIME));
	memset(&tmAccess, 0, sizeof(SYSTEMTIME));
	memset(&tmWrite, 0, sizeof(SYSTEMTIME));
	//

	CSingleLock locker(pSection);
	locker.Lock();
	if((pCondVar) && (!(*pCondVar)) ) {
		locker.Unlock();
		return;
	}
	locker.Unlock();

	setPathName(lpszFile);

	if(nMask & DUPLICATE_CRITERIA_ATTRIBUTES)
		setAttributes(lpszFile);

	setTypeName(lpszFile, true);

	HANDLE hFile = CreateFile(lpszFile, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if(hFile != INVALID_HANDLE_VALUE) {
		//if(nMask & DUPLICATE_CRITERIA_SIZE)
			setSize(hFile);

		if(nMask & DUPLICATE_CRITERIA_CONTENT)
			checkSum(hFile, iSize, pSection, pCondVar);
		
		if( (nMask & DUPLICATE_CRITERIA_CREATION_TIME) || (nMask & DUPLICATE_CRITERIA_ACCESS_TIME) || (nMask & DUPLICATE_CRITERIA_WRITE_TIME) )
			setTime(hFile);

		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
}

CFileInformation::~CFileInformation()
{
	
}

void CFileInformation::setMask(UINT nMask)
{
	this->nMask = nMask;
}

BOOL CFileInformation::CheckMask(UINT nMas)
{
	return (this->nMask & nMask);
}

void CFileInformation::setPathName(LPCTSTR lpszFullPathName) 
{
	memset(szPath, 0, PATH_MAX_LEN*sizeof(TCHAR));
	memset(szName, 0, NAME_MAX_LEN*sizeof(TCHAR));

	if(nullptr != lpszFullPathName) {
		TCHAR zDrive[DRIVE_MAX_LEN+1]={0};
		TCHAR zDir[DIR_MAX_LEN+1]={0};
		TCHAR zTitle[TITLE_MAX_LEN+1]={0};
		TCHAR zExt[EXT_MAX_LEN+1]={0};

		errno_t err = _tsplitpath_s(lpszFullPathName, zDrive, DRIVE_MAX_LEN, zDir, DIR_MAX_LEN, zTitle, TITLE_MAX_LEN, zExt, EXT_MAX_LEN);
		if(0 == err) {
			_tcscat_s(szPath, PATH_MAX_LEN, zDrive);
			_tcscat_s(szPath, PATH_MAX_LEN, zDir);
			_tcscat_s(szName, NAME_MAX_LEN, szName);
			_tcscat_s(szName, NAME_MAX_LEN, zExt);
		}
	}
}

void CFileInformation::setName(LPCTSTR lpszName)
{
	memset(szName, 0, NAME_MAX_LEN*sizeof(TCHAR));
	if(nullptr != lpszName) {
		_tcscpy_s(szName, NAME_MAX_LEN, lpszName);
	}
}

LPCTSTR CFileInformation::getName()
{
	return szName;
}

void CFileInformation::setPath(LPCTSTR lpszPath)
{
	memset(szPath, 0, PATH_MAX_LEN*sizeof(TCHAR));
	if(nullptr != lpszPath) {
		_tcscpy_s(szPath, PATH_MAX_LEN, lpszPath);
	}
}

LPCTSTR CFileInformation::getPath()
{
	return szPath;
}

void CFileInformation::crc8PushByte(uint8_t *crc, uint8_t ch, CCriticalSection* pSection, BOOL* pCondVar) 
{
	uint8_t i;

	*crc = *crc ^ ch;
	CSingleLock locker(pSection);

	for (i=0; i<8; i++) {
		locker.Lock();
		if((pCondVar) && (!(*pCondVar))) {
			locker.Unlock();
			break;
		}
		locker.Unlock();

		if (*crc & 1) {
			*crc = (*crc >> 1) ^0x8c;
		}
		else {
			*crc = (*crc >> 1);
		}
	}
}

uint8_t CFileInformation::CRC(uint8_t *pcrc, uint8_t *block, uint16_t count, CCriticalSection* pSection, BOOL* pCondVar)
{
	uint8_t crc = pcrc ? *pcrc : 0;
	CSingleLock locker(pSection);

	for (; count>0; --count, block++) {
		locker.Lock();
		if((pCondVar) && (!(*pCondVar))) {
			locker.Unlock();
			break;
		}
		locker.Unlock();
		crc8PushByte(&crc, *block, pSection, pCondVar);
	}
	if (pcrc) *pcrc = crc;

	return crc;
}

void CFileInformation::checkSum(LPCTSTR lpszFile, CCriticalSection* pSection, BOOL* pCondVar)
{

}

void CFileInformation::checkSum(HANDLE hFile, CCriticalSection* pSection, BOOL* pCondVar)
{

}

void CFileInformation::checkSum(HANDLE hFile, __int64 iSzie, CCriticalSection* pSection, BOOL* pCondVar)
{
	if(hFile == INVALID_HANDLE_VALUE)
		return;

	__int64	iRemain = iSize;
	uint8_t uCRC = 0;
	size_t nCRC = 0;
	size_t nGrow = 1;
	char szBuffer[CHECK_SUM_BUFFER_SIZE]={0};
	DWORD dwNumBytesRead = 0;
	CSingleLock locker(pSection);
	memset(szPath, 0, (PATH_MAX_LEN+1)*sizeof(TCHAR));

	CChecksum MD5;

	MD5.Startup();
	MD5.Initialzie();
	while (iRemain > 0){
		if(pSection) {
			locker.Lock();
			if((pCondVar) && (!(*pCondVar)) ) {
				locker.Unlock();
				MD5.Cleanup();
				return;
			}
			locker.Unlock();
		}

		if(!ReadFile(hFile, szBuffer, CHECK_SUM_BUFFER_SIZE, &dwNumBytesRead, nullptr)) {
			break;
		}

		MD5.Update((BYTE*)szBuffer, dwNumBytesRead);
		iRemain -= static_cast<__int64>(dwNumBytesRead);
	}

	DWORD dwChecksum = CHECK_SUM_MAX_LEN;
	BYTE zChecksum[CHECK_SUM_MAX_LEN]={0};
	TCHAR szHex[10]={0};
	MD5.Finalize((BYTE*)zChecksum, &dwChecksum);
	for(DWORD d=0;d<dwChecksum;d++) {
		memset(szHex, 0, 10*sizeof(TCHAR));
		_stprintf_s(szHex, 10, _T("%x"), static_cast<int>(zChecksum[d]));
		_tcscat_s(szChecksum, CHECK_SUM_MAX_LEN, szHex);
	}
}

void CFileInformation::setChecksum(LPCTSTR lpszChecksum, size_t len)
{
	memset(szPath, 0, (PATH_MAX_LEN+1)*sizeof(TCHAR));

	if( (nullptr != lpszChecksum) && (len > 0) ) {
		_tcscpy_s(szChecksum, CHECK_SUM_MAX_LEN, lpszChecksum);
	}
}

LPCTSTR CFileInformation::getChecksum()
{
	return szChecksum;
}

void CFileInformation::setSize(LPCTSTR lpszFile)
{
	HANDLE hFile = CreateFile(lpszFile, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if(hFile != INVALID_HANDLE_VALUE) {
		LARGE_INTEGER li={0};
		if(GetFileSizeEx(hFile, &li)) {
			iSize = static_cast<__int64>(li.QuadPart);
		}
		//
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
}

void CFileInformation::setSize(HANDLE hFile)
{
	if(hFile != INVALID_HANDLE_VALUE) {
		LARGE_INTEGER li={0};
		if(GetFileSizeEx(hFile, &li)) {
			iSize = static_cast<__int64>(li.QuadPart);
		}
	}
}

void  CFileInformation::setSize(__int64 iSize)
{
	this->iSize = iSize;
}
void  CFileInformation::setSize(LARGE_INTEGER li)
{
	this->iSize = static_cast<__int64>(li.QuadPart);
}

__int64 CFileInformation::getSize()
{
	return iSize;
}

void CFileInformation::setAttributes(LPCTSTR lpszFile)
{
	DWORD dwAttr = GetFileAttributes(lpszFile);
	if(INVALID_FILE_ATTRIBUTES == dwAttr)
		this->dwAttributes = 0;
	else
		this->dwAttributes = dwAttr;
}
void CFileInformation::setAttributes(DWORD dwAttr)
{
	this->dwAttributes = dwAttr;
}

DWORD CFileInformation::getAttributes()
{
	return dwAttributes;
}

void CFileInformation::setTime(LPCTSTR lpszFile)
{
	HANDLE hFile = CreateFile(lpszFile, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if(hFile != INVALID_HANDLE_VALUE) {
		FILETIME c={0};
		FILETIME a={0};
		FILETIME w={0};
		if(GetFileTime(hFile, &c, &a, &w)) {
			FILETIME c1={0};
			FILETIME a1={0};
			FILETIME w1={0};
			if(FileTimeToLocalFileTime(&c, &c1)) {
				FileTimeToSystemTime(&c1, &tmCreate);
			}
			if(FileTimeToLocalFileTime(&a, &a1)) {
				FileTimeToSystemTime(&a1, &tmAccess);
			}
			if(FileTimeToLocalFileTime(&w, &w1)) {
				FileTimeToSystemTime(&w1, &tmWrite);
			}
		}
		//
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
}

void CFileInformation::setTime(HANDLE hFile)
{
	if(hFile != INVALID_HANDLE_VALUE) {
		FILETIME c={0};
		FILETIME a={0};
		FILETIME w={0};
		if(GetFileTime(hFile, &c, &a, &w)) {
			FILETIME c1={0};
			FILETIME a1={0};
			FILETIME w1={0};
			if(FileTimeToLocalFileTime(&c, &c1)) {
				FileTimeToSystemTime(&c1, &tmCreate);
			}
			if(FileTimeToLocalFileTime(&a, &a1)) {
				FileTimeToSystemTime(&a1, &tmAccess);
			}
			if(FileTimeToLocalFileTime(&w, &w1)) {
				FileTimeToSystemTime(&w1, &tmWrite);
			}
		}
	}
}
void CFileInformation::setCreateTime(SYSTEMTIME* c) 
{
	if(nullptr != c) {
		memcpy(&tmCreate, c, sizeof(SYSTEMTIME));
	}
}
void CFileInformation::setCreateTime(FILETIME* c)
{
	if(nullptr != c) {
		FileTimeToSystemTime(c, &tmCreate);
	}
}
void CFileInformation::setAccessTime(SYSTEMTIME* a)
{
	if(nullptr != a) {
		memcpy(&tmAccess, a, sizeof(SYSTEMTIME));
	}
}
void CFileInformation::setAccessTime(FILETIME* a)
{
	if(nullptr != a) {
		FileTimeToSystemTime(a, &tmAccess);
	}
}
void CFileInformation::setWriteTime(SYSTEMTIME* w)
{
	if(nullptr != w) {
		memcpy(&tmWrite, w, sizeof(SYSTEMTIME));
	}
}
void CFileInformation::setWriteTime(FILETIME* w) 
{
	if(nullptr != w) {
		FileTimeToSystemTime(w, &tmWrite);
	}
}
SYSTEMTIME* CFileInformation::getCreationTime()
{
	return &tmCreate;
}
SYSTEMTIME* CFileInformation::getAccessTime()
{
	return &tmAccess;
}

SYSTEMTIME* CFileInformation::getWriteTime() 
{
	return &tmWrite;
}

BOOL CFileInformation::compareTime(SYSTEMTIME* p1, SYSTEMTIME* p2)
{
	if( (p1 == nullptr) && (p2 == nullptr) )
		return TRUE;

	if( ( (p1 != nullptr) && (p2 == nullptr) ) || ( (p1 == nullptr) && (p2 != nullptr) ) )
		return FALSE;

	if(p1->wDay != p2->wDay)
		return FALSE;

	if(p1->wDayOfWeek != p2->wDayOfWeek)
		return FALSE;

	if(p1->wHour != p2->wHour)
		return FALSE;

	if(p1->wMilliseconds != p2->wMilliseconds)
		return FALSE;

	if(p1->wMinute != p2->wMinute)
		return FALSE;

	if(p1->wMonth != p2->wMonth)
		return FALSE;

	if(p1->wSecond != p2->wSecond)
		return FALSE;

	if(p1->wYear != p2->wYear)
		return FALSE;

	return TRUE;
}



void CFileInformation::setTypeName(LPCTSTR lpsz, bool bIsPath)
{
	if(!bIsPath) {
		memset(szTypeName, 0, (TYPE_NAME_MAX_LEN+1)*sizeof(TCHAR));
		if(lpsz != nullptr) {
			_tcscpy_s(szTypeName, TYPE_NAME_MAX_LEN, lpsz);
		}
	}
	else {
		SHFILEINFO si = {0};
		memset(szTypeName, 0, (TYPE_NAME_MAX_LEN+1)*sizeof(TCHAR));
		if(SHGetFileInfo(lpsz, 0, &si, sizeof(SHFILEINFO), SHGFI_TYPENAME ) > 0)
		{
			_tcscpy_s(szTypeName, TYPE_NAME_MAX_LEN, si.szTypeName);
		}
	}
}

LPCTSTR CFileInformation::getTypeName()
{
	return szTypeName;
}