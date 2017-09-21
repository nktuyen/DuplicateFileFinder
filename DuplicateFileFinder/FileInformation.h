#pragma once
#include "afx.h"
#include "CommonDef.h"
#include <cstdint>

class CFileInformation : public CObject
{
private:
	UINT nMask;
	TCHAR szName[NAME_MAX_LEN+1];
	TCHAR szPath[PATH_MAX_LEN+1];
	TCHAR szChecksum[CHECK_SUM_MAX_LEN+1];
	TCHAR szTypeName[TYPE_NAME_MAX_LEN+1];
	__int64 iSize;
	size_t nCheckumLen;
	DWORD dwAttributes;
	SYSTEMTIME tmCreate;
	SYSTEMTIME tmAccess;
	SYSTEMTIME tmWrite;
	UINT nBufferSize;
public:
	CFileInformation(CFileInformation* pInfo = nullptr);
	CFileInformation(LPCTSTR lpszFile, CCriticalSection* pSection, BOOL* pCondVar, UINT nMask);
	virtual ~CFileInformation();

	void setMask(UINT nMask);
	BOOL CheckMask(UINT nMas);
	void setPathName(LPCTSTR lpszFullPathName);
	void setName(LPCTSTR lpszName);
	LPCTSTR getName();
	void setPath(LPCTSTR lpszPath);
	LPCTSTR getPath();
	size_t checkSum(LPCTSTR lpszFile, CCriticalSection* pSection, BOOL* pCondVar);
	size_t checkSum(HANDLE hFile, CCriticalSection* pSection, BOOL* pCondVar);
	size_t checkSum(HANDLE hFile, __int64 iSzie, CCriticalSection* pSection, BOOL* pCondVar);
	void setChecksum(LPCTSTR lpszChecksum, size_t len);
	LPCTSTR getChecksum();
	void setSize(LPCTSTR lpszFile);
	void setSize(HANDLE hFile);
	void  setSize(__int64 iSize);
	void  setSize(LARGE_INTEGER li);
	__int64 getSize();
	void setAttributes(LPCTSTR lpszFile);
	void setAttributes(DWORD dwAttr);
	DWORD getAttributes();
	void setTime(LPCTSTR lpszFile);
	void setTime(HANDLE hFile);
	void setCreateTime(SYSTEMTIME* c);
	void setCreateTime(FILETIME* c);
	void setAccessTime(SYSTEMTIME* a);
	void setAccessTime(FILETIME* a);
	void setWriteTime(SYSTEMTIME* w);
	void setWriteTime(FILETIME* w) ;
	SYSTEMTIME* getCreationTime();
	SYSTEMTIME* getAccessTime();
	SYSTEMTIME* getWriteTime() ;
	BOOL compareTime(SYSTEMTIME* p1, SYSTEMTIME* p2);
	size_t getChecksumLength();
	LPCTSTR getTypeName();
	void setTypeName(LPCTSTR lpszTypeName, bool bIsPath);
	void setBufferSize(UINT nSize);
	UINT getBufferSize();
private:
	void crc8PushByte(uint8_t *crc, uint8_t ch, CCriticalSection* pSection, BOOL* pCondVar);
	uint8_t CRC(uint8_t *pcrc, uint8_t *block, uint16_t count, CCriticalSection* pSection, BOOL* pCondVar);
};

typedef struct SFilesDuplicateInfo_
{
	CFileInformation*	DuplicateInfo;
	CMapStringToString*	DuplicateFiles;
}SFilesDuplicateInfo;
