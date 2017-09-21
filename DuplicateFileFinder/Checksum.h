#pragma once

struct MD5Context {
	unsigned char digest[16];
	int hHash;
};

class CChecksum
{
public:
	CChecksum(void);
	virtual ~CChecksum(void);

	BOOL Startup();
	void Cleanup();
	BOOL Initialzie();
	BOOL Update(BYTE* lpBuffer, DWORD dwLen);
	BOOL Finalize(BYTE* pOutBuffer, DWORD* dwLenOut);
protected:
	HCRYPTPROV m_hCryptoProvider;
	HCRYPTHASH m_nCryptoHash;
};

