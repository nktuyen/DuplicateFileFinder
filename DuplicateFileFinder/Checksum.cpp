#include "stdafx.h"
#include "Checksum.h"


CChecksum::CChecksum(void)
{
	m_hCryptoProvider = 0;
	m_nCryptoHash = 0;
}


CChecksum::~CChecksum(void)
{
}

BOOL CChecksum::Startup()
{
	BOOL bSuccess = CryptAcquireContext(&m_hCryptoProvider, nullptr, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	if(!bSuccess && (GetLastError() == NTE_EXISTS)) {
		bSuccess = CryptAcquireContext(&m_hCryptoProvider, nullptr, MS_ENHANCED_PROV, PROV_RSA_FULL, 0);
	}

	return bSuccess;
}

void CChecksum::Cleanup()
{
	if(m_hCryptoProvider != 0) {
		CryptReleaseContext(m_hCryptoProvider, 0);
		m_hCryptoProvider = 0;
	}
}

BOOL CChecksum::Initialzie()
{
	return CryptCreateHash(m_hCryptoProvider, CALG_MD5, 0, 0, &m_nCryptoHash);
}

BOOL CChecksum::Update(BYTE* lpBuffer, DWORD dwLen)
{
	return CryptHashData(m_nCryptoHash, lpBuffer, dwLen, 0);
}

BOOL CChecksum::Finalize(BYTE* pOutBuffer, DWORD* dwLenOut)
{
	if(!CryptGetHashParam(m_nCryptoHash, HP_HASHVAL, pOutBuffer, dwLenOut, 0))
		return FALSE;

	if(m_nCryptoHash != 0) {
		if(!CryptDestroyHash(m_nCryptoHash))
			return FALSE;
		m_nCryptoHash = 0;
	}

	return TRUE;
}