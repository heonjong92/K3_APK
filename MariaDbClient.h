#pragma once

#include <afx.h>
#include <vector>

// MySQL/MariaDB C API
#include <mysql.h>

#pragma comment(lib, "libmariadb.lib")

class CMariaDbClient
{
public:
	CMariaDbClient();
	~CMariaDbClient();

	// 연결/종료
	BOOL Open(LPCTSTR host,
			  LPCTSTR user,
			  LPCTSTR password,
			  LPCTSTR database,
			  UINT port = 3306,
			  UINT connectTimeoutSec = 5,
			  UINT readTimeoutSec = 10,
			  UINT writeTimeoutSec = 10);

	void Close();

	BOOL IsOpen() const;

	// 기본 실행 (INSERT/UPDATE/DELETE/DDL 등)
	BOOL Exec(LPCTSTR sql, my_ulonglong* pAffectedRows = NULL);

	// SELECT 결과를 "문자열 테이블"로 받기
	// outRows[row][col], outColumns[colName]
	BOOL Query(LPCTSTR sql,
			   std::vector<CString>& outColumns,
			   std::vector< std::vector<CString> >& outRows);

	// 트랜잭션
	BOOL Begin();
	BOOL Commit();
	BOOL Rollback();

	// 에러
	DWORD GetLastErrorCode() const { return m_lastErrCode; }
	CString GetLastErrorMsg() const { return m_lastErrMsg; }

	// 유틸: SQL 문자열 이스케이프 (따옴표 제외, 내용만 escape)
	CString Escape(LPCTSTR text);

private:
	CMariaDbClient(const CMariaDbClient&);
	CMariaDbClient& operator=(const CMariaDbClient&);

private:
	// UTF-16(CString) <-> UTF-8(std::string)
	static CString Utf8ToCString(const char* utf8);
	static CStringA CStringToUtf8A(LPCTSTR wide);

	void UpdateLastError();

private:
	MYSQL*  m_conn;
	DWORD   m_lastErrCode;
	CString m_lastErrMsg;
};