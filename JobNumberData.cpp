#include "stdafx.h"
#include "JobNumberData.h"
#include <XUtil/xUtils.h>
#include "ModelInfo.h"
#include "APK.h"
#include "StringMacro.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CJobNumberData::CJobNumberData(void) :
	m_nMaxJobNumber(1000)
{
#ifdef RELEASE_1G
	m_strJobNumberPathName[TEACH_TAB_IDX_TRAYOCR] = GetExecuteDirectory() + _T("\\Data\\Recipe\\1G_TrayOcr\\JobNumber.ini");
	m_strJobNumberPathName[TEACH_TAB_IDX_3DCHIPCNT] = GetExecuteDirectory() + _T("\\Data\\Recipe\\1G_3DChipCnt\\JobNumber.ini");
	m_strJobNumberPathName[TEACH_TAB_IDX_CHIPOCR] = GetExecuteDirectory() + _T("\\Data\\Recipe\\1G_ChipOcr\\JobNumber.ini");
	m_strJobNumberPathName[TEACH_TAB_IDX_CHIP] = GetExecuteDirectory() + _T("\\Data\\Recipe\\1G_Chip\\JobNumber.ini");
	m_strJobNumberPathName[TEACH_TAB_IDX_MIXING] = GetExecuteDirectory() + _T("\\Data\\Recipe\\1G_Mixing\\JobNumber.ini");
	m_strJobNumberPathName[TEACH_TAB_IDX_LIFTINFO] = GetExecuteDirectory() + _T("\\Data\\Recipe\\1G_Lift\\JobNumber.ini");

	SetRecipeKind(TRAYOCR_KIND, TEACH_TAB_IDX_TRAYOCR);
	SetRecipeKind(TOPCHIPCNT_KIND, TEACH_TAB_IDX_3DCHIPCNT);
	SetRecipeKind(CHIPOCR_KIND, TEACH_TAB_IDX_CHIPOCR);
	SetRecipeKind(CHIP_KIND, TEACH_TAB_IDX_CHIP);
	SetRecipeKind(MIXING_KIND, TEACH_TAB_IDX_MIXING);
	SetRecipeKind(LIFT_KIND, TEACH_TAB_IDX_LIFTINFO);

#elif RELEASE_SG
	m_strJobNumberPathName[TEACH_TAB_IDX_STACKER_OCR] = GetExecuteDirectory() + _T("\\Data\\Recipe\\SG_StackerOcr\\JobNumber.ini");

	SetRecipeKind(STACKEROCR_KIND, TEACH_TAB_IDX_STACKER_OCR);

#elif RELEASE_2G
	m_strJobNumberPathName[TEACH_TAB_IDX_BANDING]	= GetExecuteDirectory() + _T("\\Data\\Recipe\\2G_Banding\\JobNumber.ini");
	m_strJobNumberPathName[TEACH_TAB_IDX_HIC]		= GetExecuteDirectory() + _T("\\Data\\Recipe\\2G_HIC\\JobNumber.ini");

	SetRecipeKind(BANDING_KIND, TEACH_TAB_IDX_BANDING);
	SetRecipeKind(HIC_KIND, TEACH_TAB_IDX_HIC);

#elif RELEASE_3G
	m_strJobNumberPathName[TEACH_TAB_IDX_DESICCANT_CUTTING] = GetExecuteDirectory() + _T("\\Data\\Recipe\\3G_Desiccant_Cutting\\JobNumber.ini");
	m_strJobNumberPathName[TEACH_TAB_IDX_DESICCANT_MATERIAL] = GetExecuteDirectory() + _T("\\Data\\Recipe\\3G_Desiccant_Material\\JobNumber.ini");

	SetRecipeKind(DESICCANT_KIND, TEACH_TAB_IDX_DESICCANT_CUTTING);
	SetRecipeKind(SUBMATERIAL_KIND, TEACH_TAB_IDX_DESICCANT_MATERIAL);
	SetRecipeKind(SUBMATERIALTRAY_KIND, TEACH_TAB_IDX_DESICCANT_MATERIAL_TRAY);

#elif RELEASE_4G
	m_strJobNumberPathName[TEACH_TAB_IDX_MBB]		= GetExecuteDirectory() + _T("\\Data\\Recipe\\4G_MBB\\JobNumber.ini");
	m_strJobNumberPathName[TEACH_TAB_IDX_LABEL]		= GetExecuteDirectory() + _T("\\Data\\Recipe\\Label\\JobNumber.ini");

	SetRecipeKind(MBB_KIND, TEACH_TAB_IDX_MBB);
	SetRecipeKind(LABEL_KIND, TEACH_TAB_IDX_LABEL);

#elif RELEASE_5G
	m_strJobNumberPathName[TEACH_TAB_IDX_BOX] = GetExecuteDirectory() + _T("\\Data\\Recipe\\5G_Box\\JobNumber.ini");
	m_strJobNumberPathName[TEACH_TAB_IDX_SEALING] = GetExecuteDirectory() + _T("\\Data\\Recipe\\5G_Sealing\\JobNumber.ini");

	SetRecipeKind(BOXQUALITY_KIND, TEACH_TAB_IDX_BOX);
	SetRecipeKind(SEALINGQUALITY_KIND, TEACH_TAB_IDX_SEALING);

#elif RELEASE_6G
	m_strJobNumberPathName[TEACH_TAB_IDX_LABEL] = GetExecuteDirectory() + _T("\\Data\\Recipe\\Label\\JobNumber.ini");
	m_strJobNumberPathName[TEACH_TAB_IDX_TAPE] = GetExecuteDirectory() + _T("\\Data\\Recipe\\6G_Tape\\JobNumber.ini");

	SetRecipeKind(LABEL_KIND, TEACH_TAB_IDX_LABEL);
	SetRecipeKind(BOXTAPE_KIND, TEACH_TAB_IDX_TAPE);

#endif
//	m_strJobNumberPathName = GetExecuteDirectory() + _T("\\Data\\JobNumber.ini");

	Load();
}

CJobNumberData::~CJobNumberData(void)
{
	Save();
}

void CJobNumberData::Save()
{
	for (int nindex = 0; nindex < TEACH_TAB_IDX_ALL; nindex++)
	{
		WRITE_LOG(WL_DEBUG, _T("JobNumberSize : %d"), m_JobNumberList[nindex].size());

		::DeleteFile(m_strJobNumberPathName[nindex]);

		CString strModelName;
		CString strNumber;
		for (UINT i = 0; i < m_nMaxJobNumber && i < (UINT)m_JobNumberList[nindex].size(); i++)
		{
			strNumber.Format(_T("%d"), i + 1);
			strModelName = m_JobNumberList[nindex].at(i);
			if (!IsValidRecipe(strModelName, (eTeachTabIndex)nindex))
			{
				::WritePrivateProfileString(_T("JOB NUMBER"), strNumber, _T(""), m_strJobNumberPathName[nindex]);
			}
			::WritePrivateProfileString(_T("JOB NUMBER"), strNumber, strModelName, m_strJobNumberPathName[nindex]);
		}
	}
}

void CJobNumberData::Load()
{
	for (int nindex = 0; nindex < TEACH_TAB_IDX_ALL; nindex++)
	{
		m_JobNumberList[nindex].clear();

		TCHAR szTemp[MAX_PATH];

		CString strNumber;
		for (UINT i=0 ; i<m_nMaxJobNumber ; i++)
		{
			strNumber.Format( _T("%d"), i+1 );
			::GetPrivateProfileString( _T("JOB NUMBER"), strNumber, _T(""), szTemp, MAX_PATH, m_strJobNumberPathName[nindex]);
			if (!IsValidRecipe(szTemp, (eTeachTabIndex)nindex) )
				m_JobNumberList[nindex].push_back(CString(_T("")));
			else
				m_JobNumberList[nindex].push_back(CString(szTemp));
		}
	}
}

void CJobNumberData::SetJobNumber( UINT nJobNumber, LPCTSTR lpszModelName, eTeachTabIndex eTeachTabIdx )
{
	nJobNumber -= 1;
	if (nJobNumber > (UINT)m_JobNumberList[eTeachTabIdx].size())
		return;

	CString& strModelName = m_JobNumberList[eTeachTabIdx].at(nJobNumber);
	strModelName = lpszModelName;
}

UINT CJobNumberData::GetJobNumber( LPCTSTR lpszModelName, eTeachTabIndex eTeachTabIdx )
{
	if (CString(lpszModelName).IsEmpty())
		return 0;
	for (UINT i=0 ; i<m_nMaxJobNumber && i<(UINT)m_JobNumberList[eTeachTabIdx].size() ; i++)
	{
		CString& strModelName = m_JobNumberList[eTeachTabIdx].at(i);
		if (strModelName.Compare(lpszModelName) == 0)
		{
			return i+1;
		}
	}

	return 0;
}

BOOL CJobNumberData::IsValidRecipe( LPCTSTR lpszModelName, eTeachTabIndex eTeachTabIdx )
{
	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath();
	CString strKind = GetRecipeKind(eTeachTabIdx);

	if (IsExistFile((LPCTSTR)(strRecipeRootPath + strKind + _T("\\") + lpszModelName + _T("\\ModelInfo.ini"))))
	{
		return TRUE;
	}
	return FALSE;
}

CString CJobNumberData::GetTrayOcrInfo( LPCTSTR lpszModelName, eTeachTabIndex eTeachTabIdx )
{
	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath();
	CString strKind = GetRecipeKind(eTeachTabIdx);

#ifdef RELEASE_1G
	CString strInfo = CModelInfo::Instance()->GetTrayOcrInfo((LPCTSTR)(strRecipeRootPath + strKind + _T("\\") + lpszModelName + _T("\\1G_TrayOcr.ini")));

#else
	CString strInfo = CModelInfo::Instance()->GetTrayOcrInfo((LPCTSTR)(strRecipeRootPath + strKind + _T("\\") + lpszModelName + _T("\\SG_StackerOcr.ini")));

#endif // RELEASE_1G


	return strInfo;
}

CString CJobNumberData::GetModelName( UINT nJobNumber, eTeachTabIndex eTeachTabIdx )
{
	nJobNumber -= 1;
	if (nJobNumber > (UINT)m_JobNumberList[eTeachTabIdx].size())
		return CString(_T(""));

	CString strModelName = m_JobNumberList[eTeachTabIdx].at(nJobNumber);

	
	if (!IsValidRecipe(strModelName, eTeachTabIdx))
		return CString(_T(""));

	return strModelName;
}

void CJobNumberData::DeleteModelName( LPCTSTR lpszModelName, eTeachTabIndex eTeachTabIdx )
{
	for (UINT i=0 ; i<m_nMaxJobNumber && i<(UINT)m_JobNumberList[eTeachTabIdx].size() ; i++)
	{
		CString& strModelName = m_JobNumberList[eTeachTabIdx].at(i);
		if (strModelName.Compare(lpszModelName) == 0)
		{
			strModelName = _T("");
		}
	}

	Save();
}
