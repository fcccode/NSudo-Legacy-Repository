void NSudo_Run(HWND hDlg , LPCWSTR szUser, LPCWSTR szPrivilege, LPCWSTR szMandatory, LPCWSTR szCMDLine)
{
	if (_wcsicmp(L"", szCMDLine) == 0)
	{
		MessageBoxW(hDlg, L"请在文本框输入你要执行的命令或者选择下拉框里面的常用命令", L"NSudo 3.0 Release", MB_ICONEXCLAMATION);
	}
	else
	{
		CStringW szBuffer,szCmdSpec;
		
		GetSystemDirectoryW(szCmdSpec.GetBufferSetLength(260), 260);
		szCmdSpec.ReleaseBuffer();
		szCmdSpec += L"\\cmd.exe";

		szBuffer += L"/c start ";

		wchar_t szPath[260];
		DWORD dwLength = GetPrivateProfileStringW(szCMDLine, L"CommandLine",L"",szPath,260, szNSudoPath + L"\\ShortCutList.ini");

		if (wcscmp(szPath,L"") != 0)
		{
			szBuffer += szPath;
		}
		else
		{
			szBuffer += szCMDLine;
		}

		HANDLE hUserToken = INVALID_HANDLE_VALUE;
		if (_wcsicmp(NSudo_Text_CU, szUser) == 0)
		{
			NSudoGetCurrentUserToken(&hUserToken);
		}
		else if (_wcsicmp(NSudo_Text_CP, szUser) == 0)
		{
			NSudoGetCurrentProcessToken(&hUserToken);
		}
		else
		{
			if (_wcsicmp(NSudo_Text_Sys, szUser) == 0)
			{
				NSudoGetSystemToken(&hUserToken);
			}
			else if (_wcsicmp(NSudo_Text_TI, szUser) == 0)
			{
				NSudoGetTrustedInstallerToken(&hUserToken);
			}
		}

		if (_wcsicmp(NSudo_Text_EnableAll, szPrivilege) == 0)
		{
			NSudoAdjustAllTokenPrivileges(hUserToken, true);
		}
		else if (_wcsicmp(NSudo_Text_DisableAll, szPrivilege) == 0)
		{
			NSudoAdjustAllTokenPrivileges(hUserToken, false);
		}

		if (_wcsicmp(NSudo_Text_Low, szMandatory) == 0)
		{
			SetTokenIntegrity(hUserToken, L"S-1-16-4096");
		}
		else if (_wcsicmp(NSudo_Text_Medium, szMandatory) == 0)
		{
			SetTokenIntegrity(hUserToken, L"S-1-16-8192");
		}
		else if (_wcsicmp(NSudo_Text_High, szMandatory) == 0)
		{
			SetTokenIntegrity(hUserToken, L"S-1-16-12288");
		}

		if (NSudoImpersonateSystemToken())
		{
			if (!NSudoCreateProcess(hUserToken, szCmdSpec, szBuffer.GetBuffer()))
			{
				ReturnMessage(L"进程创建失败");
			}
			RevertToSelf();
		}

		CloseHandle(hUserToken);
	}
}