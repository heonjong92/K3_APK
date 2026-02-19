#pragma once

#include "Common/xSingletoneObject.h"
#include <vector>

class CJobNumberData : public CxSingleton<CJobNumberData>
{
protected:
	typedef std::vector<CString> JobNumberList;

	const UINT	m_nMaxJobNumber;
	CString		m_strJobNumberPathName[TEACH_TAB_IDX_ALL];

	CString		m_strKind[TEACH_TAB_IDX_ALL];

	JobNumberList	m_JobNumberList[TEACH_TAB_IDX_ALL];

	BOOL IsValidRecipe( LPCTSTR lpszModelName, eTeachTabIndex eTeachTabIdx );
public:
	void Save();
	void Load();

	UINT GetMaxCount() const { return m_nMaxJobNumber; }

	// nJobNumber: base 1
	void SetJobNumber( UINT nJobNumber, LPCTSTR lpszModelName, eTeachTabIndex eTeachTabIdx );
	UINT GetJobNumber( LPCTSTR lpszModelName, eTeachTabIndex eTeachTabIdx );
	CString GetModelName( UINT nJobNumber, eTeachTabIndex eTeachTabIdx );
	void DeleteModelName( LPCTSTR lpszModelName, eTeachTabIndex eTeachTabIdx );
	CString GetTrayOcrInfo(LPCTSTR lpszModelName, eTeachTabIndex eTeachTabIdx);

	void SetRecipeKind(CString strKind, eTeachTabIndex eTeachTabIdx) { m_strKind[eTeachTabIdx] = strKind; }
	LPCTSTR GetRecipeKind(eTeachTabIndex eTeachTabIdx) { return m_strKind[eTeachTabIdx]; }

public:
	CJobNumberData(void);
	~CJobNumberData(void);
};

