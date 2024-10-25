#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <assert.h>
#include <process.h>
#include <strsafe.h>
#include "WinCrashpad/WinCrashpad.h"

using namespace WinCrashpad;

int main(int argc, char* argv[]) {
  UNREFERENCED_PARAMETER(argc);
  UNREFERENCED_PARAMETER(argv);

  WCHAR szCurDir[MAX_PATH] = {0};
  GetModuleFileNameW(NULL, szCurDir, _MAX_PATH);
  WCHAR* ptr = wcsrchr(szCurDir, L'\\');
  if (ptr != NULL)
    *(ptr) = 0;  // remove executable name
  WCHAR szWinCrashpadExePath[MAX_PATH];
#if (defined DEBUG) || (defined _DEBUG)
  StringCchPrintfW(szWinCrashpadExePath, MAX_PATH, L"%s\\%s", szCurDir, L"WinCrashpad-d.exe");
#else
  StringCchPrintfW(szWinCrashpadExePath, MAX_PATH, L"%s\\%s", szCurDir, L"WinCrashpad.exe");
#endif

  // Install crash reporting
  //
  CR_INSTALL_INFO info;
  memset(&info, 0, sizeof(CR_INSTALL_INFO));
  info.cb = sizeof(CR_INSTALL_INFO);           // Size of the structure
  info.pszAppName = L"WinCrashpad Console Test";  // App name
  info.pszAppVersion = L"1.0.0";               // App version
  info.pszCrashReportPath = szWinCrashpadExePath;
  info.nRestartTimeout = 3; // 3s
  info.dwFlags = CR_INST_ALL_POSSIBLE_HANDLERS | CR_INST_STORE_ZIP_ARCHIVES | CR_INST_APP_RESTART;

  // Install crash handlers
  int nInstResult = crInstall(&info);
  assert(nInstResult == 0);

  crAddFile(L"C:\\1989.mp4", nullptr, L"description", CR_AF_MAKE_FILE_COPY);

  crAddScreenshot(CR_AS_VIRTUAL_SCREEN, 0);

  crAddRegKey(L"HKEY_CURRENT_USER\\SOFTWARE\\", L"regkey.xml", 0);

  // Check result
  if (nInstResult != 0) {
    return FALSE;
  }

  printf("Press Enter to simulate a null pointer exception or any other key to exit...\n");
  int n = _getch();
  if (n == 13) {
    int* p = 0;
    *p = 0;  // Access violation
  }

  // Uninstall exception handlers
  //
  int nUninstRes = crUninstall();
  assert(nUninstRes == 0);

  return 0;
}
