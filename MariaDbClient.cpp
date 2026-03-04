#include "stdafx.h"
#include "MariaDbClient.h"

CMariaDbClient::CMariaDbClient()
	: m_conn(NULL), m_lastErrCode(0)
{
}

CMariaDbClient::~CMariaDbClient()
{
	Close();
}

BOOL CMariaDbClient::Open(LPCTSTR host,
						  LPCTSTR user,
						  LPCTSTR password,
						  LPCTSTR database,
						  UINT port,
						  UINT connectTimeoutSec,
						  UINT readTimeoutSec,
						  UINT writeTimeoutSec)
{
	Close();

	if (!host || !user || !database)
		return FALSE;

	m_conn = mysql_init(NULL);
	if (!m_conn)
	{
		m_lastErrCode = 1;
		m_lastErrMsg = _T("mysql_init failed");
		return FALSE;
	}

	// timeouts
	mysql_options(m_conn, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&connectTimeoutSec);
	mysql_options(m_conn, MYSQL_OPT_READ_TIMEOUT,    (const char*)&readTimeoutSec);
	mysql_options(m_conn, MYSQL_OPT_WRITE_TIMEOUT,   (const char*)&writeTimeoutSec);

	// UTF-8로 통신 권장 (서버/테이블도 utf8mb4면 베스트)
	// (연결 성공 후 mysql_set_character_set도 호출)
	// mysql_options로 init 단계에서 지정 가능하지만 환경 따라 다르므로 둘 다 처리
	{
		const char* initCmd = "SET NAMES utf8mb4";
		mysql_options(m_conn, MYSQL_INIT_COMMAND, initCmd);
	}

	CStringA hostA = CStringToUtf8A(host);
	CStringA userA = CStringToUtf8A(user);
	CStringA passA = CStringToUtf8A(password ? password : _T(""));
	CStringA dbA   = CStringToUtf8A(database);

	if (!mysql_real_connect(
		m_conn,
		hostA,
		userA,
		passA,
		dbA,
		port,
		NULL,
		0))
	{
		UpdateLastError();
		Close();
		return FALSE;
	}

	// 세션 charset 명시
	mysql_set_character_set(m_conn, "utf8mb4");

	// 정상
	m_lastErrCode = 0;
	m_lastErrMsg.Empty();
	return TRUE;
}

void CMariaDbClient::Close()
{
	if (m_conn)
	{
		mysql_close(m_conn);
		m_conn = NULL;
	}
}

BOOL CMariaDbClient::IsOpen() const
{
	return (m_conn != NULL);
}

BOOL CMariaDbClient::Exec(LPCTSTR sql, my_ulonglong* pAffectedRows)
{
	if (!m_conn || !sql)
		return FALSE;

	CStringA sqlA = CStringToUtf8A(sql);

	if (mysql_real_query(m_conn, sqlA.GetString(), (unsigned long)sqlA.GetLength()) != 0)
	{
		UpdateLastError();
		return FALSE;
	}

	if (pAffectedRows)
		*pAffectedRows = mysql_affected_rows(m_conn);

	m_lastErrCode = 0;
	m_lastErrMsg.Empty();
	return TRUE;
}

BOOL CMariaDbClient::Query(LPCTSTR sql,
						   std::vector<CString>& outColumns,
						   std::vector< std::vector<CString> >& outRows)
{
	outColumns.clear();
	outRows.clear();

	if (!m_conn || !sql)
		return FALSE;

	CStringA sqlA = CStringToUtf8A(sql);

	if (mysql_real_query(m_conn, sqlA.GetString(), (unsigned long)sqlA.GetLength()) != 0)
	{
		UpdateLastError();
		return FALSE;
	}

	MYSQL_RES* res = mysql_store_result(m_conn);
	if (!res)
	{
		// SELECT가 아닌데 Query를 호출했거나, 에러일 수 있음
		if (mysql_field_count(m_conn) == 0)
		{
			// 결과셋 없음(OK)
			m_lastErrCode = 0;
			m_lastErrMsg.Empty();
			return TRUE;
		}
		UpdateLastError();
		return FALSE;
	}

	int numFields = (int)mysql_num_fields(res);
	MYSQL_FIELD* fields = mysql_fetch_fields(res);

	outColumns.reserve(numFields);
	for (int i = 0; i < numFields; ++i)
	{
		outColumns.push_back(Utf8ToCString(fields[i].name));
	}

	MYSQL_ROW row;
	unsigned long* lengths = NULL;

	while ((row = mysql_fetch_row(res)) != NULL)
	{
		lengths = mysql_fetch_lengths(res);

		std::vector<CString> oneRow;
		oneRow.reserve(numFields);

		for (int i = 0; i < numFields; ++i)
		{
			if (!row[i])
			{
				oneRow.push_back(_T("")); // NULL -> 빈문자 (필요하면 별도 표시로 바꿔)
			}
			else
			{
				// row[i]는 null-terminated지만 lengths가 더 정확
				CString cell = Utf8ToCString(row[i]);
				oneRow.push_back(cell);
			}
		}
		outRows.push_back(oneRow);
	}

	mysql_free_result(res);

	m_lastErrCode = 0;
	m_lastErrMsg.Empty();
	return TRUE;
}

BOOL CMariaDbClient::Begin()
{
	return Exec(_T("START TRANSACTION"));
}

BOOL CMariaDbClient::Commit()
{
	return Exec(_T("COMMIT"));
}

BOOL CMariaDbClient::Rollback()
{
	return Exec(_T("ROLLBACK"));
}

CString CMariaDbClient::Escape(LPCTSTR text)
{
	if (!m_conn || !text)
		return _T("");

	// UTF-8로 변환 후 escape (최대 2배 + 1)
	CStringA src = CStringToUtf8A(text);
	unsigned long srcLen = (unsigned long)src.GetLength();

	std::vector<char> buf;
	buf.resize(srcLen * 2 + 1);

	unsigned long outLen = mysql_real_escape_string(m_conn, &buf[0], src.GetString(), srcLen);
	buf[outLen] = '\0';

	return Utf8ToCString(&buf[0]);
}

void CMariaDbClient::UpdateLastError()
{
	if (!m_conn)
	{
		m_lastErrCode = 0xFFFFFFFF;
		m_lastErrMsg = _T("Connection is null");
		return;
	}

	m_lastErrCode = (DWORD)mysql_errno(m_conn);
	m_lastErrMsg  = Utf8ToCString(mysql_error(m_conn));
}

CString CMariaDbClient::Utf8ToCString(const char* utf8)
{
	if (!utf8)
		return _T("");

	// UTF-8 -> UTF-16
	int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	if (wlen <= 0)
		return _T("");

	CString out;
	LPWSTR pBuf = out.GetBuffer(wlen);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, pBuf, wlen);
	out.ReleaseBuffer();
	return out;
}

CStringA CMariaDbClient::CStringToUtf8A(LPCTSTR wide)
{
	if (!wide)
		return CStringA("");

#ifdef UNICODE
	int lenA = WideCharToMultiByte(CP_UTF8, 0, wide, -1, NULL, 0, NULL, NULL);
	if (lenA <= 0)
		return CStringA("");

	CStringA out;
	LPSTR pBuf = out.GetBuffer(lenA);
	WideCharToMultiByte(CP_UTF8, 0, wide, -1, pBuf, lenA, NULL, NULL);
	out.ReleaseBuffer();
	return out;
#else
	// ANSI 빌드면 그대로(권장 X)
	return CStringA(wide);
#endif
}