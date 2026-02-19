#pragma once

#include <vector>

class CLanguageInfoGC;
class CLanguageInfo
{
public:
	friend class CLanguageInfoGC;
	static CLanguageInfo* Instance();

private:
	static CLanguageInfo* m_pThis;

public:

	static CLanguageInfo* GetPtr() { return m_pThis; }

	CLanguageInfo(void);
	~CLanguageInfo(void);

	int m_nLangType;	// 0:한국어, 1:영어
	int m_nStringCnt;	// 변경할 String개수
	
	std::vector<CString> m_vecLang;

	int ReadLanguageIni();		// INI 등록.	
	// 20170517
	void WriteLanguageIni(int nIdx);	// INI 변경 
	// 20170517 end

	CString ReadString(int nIdx);	// 등록된 String 처리.
};

