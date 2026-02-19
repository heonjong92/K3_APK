
// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// APK.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

#include <Lm.h>
#pragma comment(lib, "netapi32.lib")

void EnumerateSerialPorts( CUIntArray& ports )
{
	ports.RemoveAll();

	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	//	BOOL bGetVer = GetVersionEx(&osvi);

	BOOL bGetVer = FALSE;
	LPWKSTA_INFO_100 pBuf = NULL;
	if (NERR_Success != NetWkstaGetInfo(NULL, 100, (LPBYTE*)&pBuf))
		bGetVer = FALSE;
	else
	{
		osvi.dwMajorVersion = pBuf->wki100_ver_major;
		osvi.dwMinorVersion = pBuf->wki100_ver_minor;
		osvi.dwPlatformId = pBuf->wki100_platform_id;
		bGetVer = TRUE;
	}
	NetApiBufferFree(pBuf);

	if (bGetVer && (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT))
	{
		TCHAR szDevices[65535];
		DWORD dwChars = QueryDosDevice(NULL, szDevices, 65535);
		if (dwChars)
		{
			int i=0;

			for (;;)
			{
				TCHAR* pszCurrentDevice = &szDevices[i];

				//TRACE( _T("device: %s\n"), pszCurrentDevice );

				int nLen = (int)_tcslen(pszCurrentDevice);
				if (nLen > 3 && _tcsnicmp(pszCurrentDevice, _T("COM"), 3) == 0)
				{
					int nPort = _ttoi(&pszCurrentDevice[3]);
					if ( nPort != 0 )
					{
						BOOL bIsDuplicated = FALSE;
						for (INT_PTR nI=0 ; nI<ports.GetCount() ; nI++)
						{
							if (nPort == ports.GetAt(nI))
							{
								bIsDuplicated = TRUE;
								break;
							}
						}
						if (!bIsDuplicated)
							ports.Add(nPort);
					}
				}

				while (szDevices[i] != _T('\0'))
					i++;

				i++;

				if (szDevices[i] == _T('\0'))
					break;
			}
		}
		else
			TRACE(_T("Failed in call to QueryDosDevice, GetLastError:%d\n"), GetLastError());
	}
	else
	{
		for (UINT i=1; i<256; i++)
		{
			CString sPort;
			sPort.Format(_T("\\\\.\\COM%d"), i);

			BOOL bSuccess = FALSE;
			HANDLE hPort = ::CreateFile(sPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
			if (hPort == INVALID_HANDLE_VALUE)
			{
				DWORD dwError = GetLastError();

				if (dwError == ERROR_ACCESS_DENIED || dwError == ERROR_GEN_FAILURE)
					bSuccess = TRUE;
			}
			else
			{
				bSuccess = TRUE;
				CloseHandle(hPort);
			}

			if (bSuccess)
				ports.Add(i);
		}
	}

	for (INT_PTR nI=0 ; nI<ports.GetCount()-1 ; nI++)
	{
		for (INT_PTR nJ=nI+1 ; nJ<ports.GetCount() ; nJ++)
		{
			UINT nPort1 = ports.GetAt(nI);
			UINT nPort2 = ports.GetAt(nJ);
			if (nPort1 > nPort2)
			{
				ports.SetAt(nI, nPort2);
				ports.SetAt(nJ, nPort1);
			}
		}
	}
}
