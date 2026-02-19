#include "stdafx.h"
#include "LanguageInfo.h"
#include "XUtil/xUtils.h"

//////////////////////////////////////////////////////////////////////
// CAppSettingGC
class CLanguageInfoGC
{
public:
	CLanguageInfoGC() {}
	~CLanguageInfoGC() { delete CLanguageInfo::m_pThis; CLanguageInfo::m_pThis = NULL; }
};
CLanguageInfoGC s_DDMGC;

CLanguageInfo* CLanguageInfo::m_pThis = NULL;

CLanguageInfo::CLanguageInfo(void)
{
	m_nLangType = 1;	// 기본 베이스는 한국어 베이스
	m_nStringCnt = 0;
}

CLanguageInfo::~CLanguageInfo(void)
{
}

CLanguageInfo* CLanguageInfo::Instance()
{
	if( !m_pThis )
		m_pThis = new CLanguageInfo;
	return m_pThis;
}

int CLanguageInfo::ReadLanguageIni()		// INI 등록.
{
	TCHAR swzTemp[MAX_PATH];

	CString strFilePath;
	CString strTemp;
	CString strConver;
	CString strAppName = _T("LANGUAGE");

	m_vecLang.clear();
#ifdef RELEASE_1G
	strFilePath.Format(_T("%s\\%s"), GetExecuteDirectory(), _T("Data\\LANGUAGE_1G.ini"));

#elif RELEASE_SG
	strFilePath.Format(_T("%s\\%s"), GetExecuteDirectory(), _T("Data\\LANGUAGE_SG.ini"));

#elif RELEASE_2G
	strFilePath.Format(_T("%s\\%s"), GetExecuteDirectory(), _T("Data\\LANGUAGE_2G.ini"));

#elif RELEASE_3G
	strFilePath.Format(_T("%s\\%s"), GetExecuteDirectory(), _T("Data\\LANGUAGE_3G.ini"));

#elif RELEASE_4G
	strFilePath.Format(_T("%s\\%s"), GetExecuteDirectory(), _T("Data\\LANGUAGE_4G.ini"));

#elif RELEASE_5G
	strFilePath.Format(_T("%s\\%s"), GetExecuteDirectory(), _T("Data\\LANGUAGE_5G.ini"));

#elif RELEASE_6G
	strFilePath.Format(_T("%s\\%s"), GetExecuteDirectory(), _T("Data\\LANGUAGE_6G.ini"));

#endif
	
	GetPrivateProfileString(strAppName, _T("STATE"), _T("0"), swzTemp, MAX_PATH, strFilePath);
	strTemp.Format(_T("%s"), swzTemp);
	
	m_nLangType = _ttoi(strTemp);
	
	GetPrivateProfileString(strAppName, _T("USE_STRING"), _T("0"), swzTemp, MAX_PATH, strFilePath);
	strTemp.Format(_T("%s"), swzTemp);
	
	m_nStringCnt = _ttoi(strTemp);
	
	for(int i=0; i < m_nStringCnt; i++)
	{
		strConver.Format(_T("STRING%03d"), i+1);
		
		if(m_nLangType == 0)
			GetPrivateProfileString(_T("KOREA"), strConver, _T(""), swzTemp, MAX_PATH, strFilePath);
		else if(m_nLangType == 1)
 			GetPrivateProfileString(_T("ENGLISH"), strConver, _T(""), swzTemp, MAX_PATH, strFilePath);
		else return 0;

		strTemp.Format(_T("%s"), swzTemp);

		m_vecLang.push_back(swzTemp);
	}
	return m_nLangType;
}

// 20170517
void CLanguageInfo::WriteLanguageIni(int nIdx)
{	
	CString strFilePath;
	CString strTemp;
	CString strConver;
	CString strAppName = _T("LANGUAGE");
	
#ifdef RELEASE_1G
	strFilePath.Format(_T("%s\\%s"), GetExecuteDirectory(), _T("Data\\LANGUAGE_1G.ini"));

#elif RELEASE_SG
	strFilePath.Format(_T("%s\\%s"), GetExecuteDirectory(), _T("Data\\LANGUAGE_SG.ini"));

#elif RELEASE_2G
	strFilePath.Format(_T("%s\\%s"), GetExecuteDirectory(), _T("Data\\LANGUAGE_2G.ini"));

#elif RELEASE_3G
	strFilePath.Format(_T("%s\\%s"), GetExecuteDirectory(), _T("Data\\LANGUAGE_3G.ini"));

#elif RELEASE_4G
	strFilePath.Format(_T("%s\\%s"), GetExecuteDirectory(), _T("Data\\LANGUAGE_4G.ini"));

#elif RELEASE_5G
	strFilePath.Format(_T("%s\\%s"), GetExecuteDirectory(), _T("Data\\LANGUAGE_5G.ini"));

#elif RELEASE_6G
	strFilePath.Format(_T("%s\\%s"), GetExecuteDirectory(), _T("Data\\LANGUAGE_6G.ini"));

#endif
	
	strTemp.Format(_T("%d"), nIdx);
	
	WritePrivateProfileString( strAppName, _T("STATE"), strTemp, strFilePath);
	
	// 쓰기 후 다시 읽어오자.
	ReadLanguageIni();
}
// 20170517 end

CString CLanguageInfo::ReadString(int nIdx)	// 등록된 String 처리.
{
	if(m_nStringCnt < nIdx || nIdx < 0) return _T("");
	if(m_vecLang.size() <= 0) return _T("");

	return m_vecLang[nIdx-1];
}