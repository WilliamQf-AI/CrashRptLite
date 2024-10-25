#include "stdafx.h"
#include "WinCrashpad/WinCrashpad.h"
#include "CrashHandler.h"
#include "Utility.h"
#include "strconv.h"

namespace WinCrashpad {

HANDLE g_hModuleCrashRpt = NULL;  // Handle to CrashRpt.dll module.

CRASHRPTAPI(int) crInstall(CR_INSTALL_INFO* pInfo) {
  int nStatus = -1;
  strconv_t strconv;
  CCrashHandler* pCrashHandler = NULL;

  // Validate input parameters.
  if (pInfo == NULL || pInfo->cb != sizeof(CR_INSTALL_INFO)) {
    nStatus = 1;
    goto cleanup;
  }

  // Check if crInstall() already was called for current process.
  pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

  if (pCrashHandler != NULL && pCrashHandler->IsInitialized()) {
    nStatus = 2;
    goto cleanup;
  }

  if (pCrashHandler == NULL) {
    pCrashHandler = new CCrashHandler();
    if (pCrashHandler == NULL) {
      nStatus = 3;
      goto cleanup;
    }
  }

  LPCTSTR ptszAppName = strconv.w2t((LPWSTR)pInfo->pszAppName);
  LPCTSTR ptszAppVersion = strconv.w2t((LPWSTR)pInfo->pszAppVersion);
  LPCTSTR ptszCrashSenderPath = strconv.w2t((LPWSTR)pInfo->pszCrashReportPath);
  LPCTSTR ptszDebugHelpDLLPath = strconv.w2t((LPWSTR)pInfo->pszDebugHelpDLLPath);
  MINIDUMP_TYPE miniDumpType = pInfo->uMiniDumpType;
  LPCTSTR ptszErrorReportSaveDir = strconv.w2t((LPWSTR)pInfo->pszErrorReportSaveDir);
  LPCTSTR ptszRestartCmdLine = strconv.w2t((LPWSTR)pInfo->pszRestartCmdLine);
  LPCTSTR ptszCustomSenderIcon = strconv.w2t((LPWSTR)pInfo->pszCustomCrashReportIcon);

  int nInitResult = pCrashHandler->Init(ptszAppName, ptszAppVersion, ptszCrashSenderPath, &pInfo->uPriorities, pInfo->dwFlags, ptszDebugHelpDLLPath, miniDumpType,
                                        ptszErrorReportSaveDir, ptszRestartCmdLine, ptszCustomSenderIcon, pInfo->nRestartTimeout);

  if (nInitResult != 0) {
    nStatus = 4;
    goto cleanup;
  }

  // OK.
  nStatus = 0;

cleanup:

  if (nStatus != 0)  // If failed
  {
    if (pCrashHandler != NULL && !pCrashHandler->IsInitialized()) {
      // Release crash handler object
      CCrashHandler::ReleaseCurrentProcessCrashHandler();
    }
  }

  return nStatus;
}

CRASHRPTAPI(int) crUninstall() {
  // Get crash handler singleton
  CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

  // Check if found
  if (pCrashHandler == NULL || !pCrashHandler->IsInitialized()) {
    return 1;
  }

  // Uninstall main thread's C++ exception handlers
  int nUnset = pCrashHandler->UnSetThreadExceptionHandlers();
  if (nUnset != 0)
    return 2;

  // Destroy the crash handler.
  int nDestroy = pCrashHandler->Destroy();
  if (nDestroy != 0)
    return 3;

  // Free the crash handler object.
  delete pCrashHandler;

  return 0;
}

// Sets C++ exception handlers for the calling thread
CRASHRPTAPI(int) crInstallToCurrentThread(DWORD dwFlags) {
  CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

  if (pCrashHandler == NULL) {
    return 1;
  }

  int nResult = pCrashHandler->SetThreadExceptionHandlers(dwFlags);
  if (nResult != 0)
    return 2;  // Error?

  return 0;
}

// Unset C++ exception handlers from the calling thread
CRASHRPTAPI(int) crUninstallFromCurrentThread() {
  CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

  if (pCrashHandler == NULL) {
    ATLASSERT(pCrashHandler != NULL);
    return 1;  // Invalid parameter?
  }

  int nResult = pCrashHandler->UnSetThreadExceptionHandlers();
  if (nResult != 0)
    return 2;  // Error?

  return 0;
}

CRASHRPTAPI(int) crSetCrashCallback(PFNCRASHCALLBACK pfnCallbackFunc, LPVOID lpParam) {
  CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

  if (pCrashHandler == NULL) {
    return 1;  // No handler installed for current process?
  }

  pCrashHandler->SetCrashCallback(pfnCallbackFunc, lpParam);

  return 0;
}

CRASHRPTAPI(int) crAddFile(PCWSTR pszFile, PCWSTR pszDestFile, PCWSTR pszDesc, DWORD dwFlags) {
  strconv_t strconv;

  CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

  if (pCrashHandler == NULL) {
    return 1;  // No handler installed for current process?
  }

  LPCTSTR lptszFile = strconv.w2t((LPWSTR)pszFile);
  LPCTSTR lptszDestFile = strconv.w2t((LPWSTR)pszDestFile);
  LPCTSTR lptszDesc = strconv.w2t((LPWSTR)pszDesc);

  int nAddResult = pCrashHandler->AddFile(lptszFile, lptszDestFile, lptszDesc, dwFlags);
  if (nAddResult != 0) {
    // Couldn't add file
    return 2;
  }

  return 0;
}

CRASHRPTAPI(int) crAddScreenshot(DWORD dwFlags, int nJpegQuality) {
  CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

  if (pCrashHandler == NULL) {
    return 1;  // Invalid parameter?
  }

  return pCrashHandler->AddScreenshot(dwFlags, nJpegQuality);
}

CRASHRPTAPI(int) crAddProperty(LPCWSTR pszPropName, LPCWSTR pszPropValue) {
  strconv_t strconv;
  LPCTSTR pszPropNameT = strconv.w2t(pszPropName);
  LPCTSTR pszPropValueT = strconv.w2t(pszPropValue);

  CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

  if (pCrashHandler == NULL) {
    return 1;  // No handler installed for current process?
  }

  int nResult = pCrashHandler->AddProperty(CString(pszPropNameT), CString(pszPropValueT));
  if (nResult != 0) {
    return 2;  // Failed to add the property
  }

  return 0;
}

CRASHRPTAPI(int) crAddRegKey(LPCWSTR pszRegKey, LPCWSTR pszDstFileName, DWORD dwFlags) {
  strconv_t strconv;
  LPCTSTR pszRegKeyT = strconv.w2t(pszRegKey);
  LPCTSTR pszDstFileNameT = strconv.w2t(pszDstFileName);

  CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

  if (pCrashHandler == NULL) {
    return 1;  // No handler installed for current process?
  }

  int nResult = pCrashHandler->AddRegKey(pszRegKeyT, pszDstFileNameT, dwFlags);
  if (nResult != 0) {
    return 2;  // Failed to add the property
  }

  return 0;
}

CRASHRPTAPI(int) crGenerateErrorReport(CR_EXCEPTION_INFO* pExceptionInfo) {
  if (pExceptionInfo == NULL || pExceptionInfo->cb != sizeof(CR_EXCEPTION_INFO)) {
    return 1;
  }

  CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

  if (pCrashHandler == NULL) {
    ATLASSERT(pCrashHandler != NULL);
    return 2;
  }

  return pCrashHandler->GenerateErrorReport(pExceptionInfo);
}

CRASHRPTAPI(int) crExceptionFilter(unsigned int code, struct _EXCEPTION_POINTERS* ep) {
  CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

  if (pCrashHandler == NULL) {
    return EXCEPTION_CONTINUE_SEARCH;
  }

  CR_EXCEPTION_INFO ei;
  memset(&ei, 0, sizeof(CR_EXCEPTION_INFO));
  ei.cb = sizeof(CR_EXCEPTION_INFO);
  ei.exctype = CR_SEH_EXCEPTION;
  ei.pexcptrs = ep;
  ei.code = code;

  int res = pCrashHandler->GenerateErrorReport(&ei);
  if (res != 0) {
    // If goes here than GenerateErrorReport() failed
    return EXCEPTION_CONTINUE_SEARCH;
  }

  return EXCEPTION_EXECUTE_HANDLER;
}

//-----------------------------------------------------------------------------------------------
// Below crEmulateCrash() related stuff goes

class CDerived;
class CBase {
 public:
  CBase(CDerived* derived) : m_pDerived(derived){};
  ~CBase();
  virtual void function(void) = 0;

  CDerived* m_pDerived;
};

class CDerived : public CBase {
 public:
  CDerived() : CBase(this){};  // C4355
  virtual void function(void){};
};

CBase::~CBase() {
  m_pDerived->function();
}

#include <float.h>
void sigfpe_test() {
  // Code taken from http://www.devx.com/cplus/Article/34993/1954

  //Set the x86 floating-point control word according to what
  //exceptions you want to trap.
  _clearfp();  //Always call _clearfp before setting the control
  //word
  //Because the second parameter in the following call is 0, it
  //only returns the floating-point control word
  unsigned int cw;
#if _MSC_VER < 1400
  cw = _controlfp(0, 0);  //Get the default control
#else
  _controlfp_s(&cw, 0, 0);  //Get the default control
#endif
  //word
  //Set the exception masks off for exceptions that you want to
  //trap.  When a mask bit is set, the corresponding floating-point
  //exception is //blocked from being generating.
  cw &= ~(EM_OVERFLOW | EM_UNDERFLOW | EM_ZERODIVIDE | EM_DENORMAL | EM_INVALID);
  //For any bit in the second parameter (mask) that is 1, the
  //corresponding bit in the first parameter is used to update
  //the control word.
  unsigned int cwOriginal = 0;
#if _MSC_VER < 1400
  cwOriginal = _controlfp(cw, MCW_EM);  //Set it.
#else
  _controlfp_s(&cwOriginal, cw, MCW_EM);  //Set it.
#endif
  //MCW_EM is defined in float.h.

  // Divide by zero

  float a = 1.0f;
  float b = 0.0f;
  float c = a / b;
  c;

  //Restore the original value when done:
  //_controlfp_s(cwOriginal, MCW_EM);
}

#define BIG_NUMBER 0x1fffffff
//#define BIG_NUMBER 0xf
#pragma warning(disable : 4717)  // avoid C4717 warning
int RecurseAlloc() {
  int* pi = NULL;
  for (;;)
    pi = new int[BIG_NUMBER];
  return 0;
}

// Vulnerable function
#pragma warning(disable : 4996)  // for strcpy use
#pragma warning(disable : 6255)  // warning C6255: _alloca indicates failure by raising a stack overflow exception. Consider using _malloca instead
#pragma warning(disable : 6204)  // warning C6204: Possible buffer overrun in call to 'strcpy': use of unchecked parameter 'str'
void test_buffer_overrun(const char* str) {
  char* buffer = (char*)_alloca(10);
  strcpy(buffer, str);  // overrun buffer !!!

  // use a secure CRT function to help prevent buffer overruns
  // truncate string to fit a 10 byte buffer
  // strncpy_s(buffer, _countof(buffer), str, _TRUNCATE);
}
#pragma warning(default : 4996)
#pragma warning(default : 6255)
#pragma warning(default : 6204)

// Stack overflow function
struct DisableTailOptimization {
  ~DisableTailOptimization() { ++v; }
  static int v;
};

int DisableTailOptimization::v = 0;

static void CauseStackOverflow() {
  DisableTailOptimization v;
  CauseStackOverflow();
}

CRASHRPTAPI(int) crEmulateCrash(unsigned ExceptionType) throw(...) {
  switch (ExceptionType) {
    case CR_SEH_EXCEPTION: {
      // Access violation
      int* p = 0;
#pragma warning(disable : 6011)  // warning C6011: Dereferencing NULL pointer 'p'
      *p = 0;
#pragma warning(default : 6011)
    } break;
    case CR_CPP_TERMINATE_CALL: {
      // Call terminate
      terminate();
    } break;
    case CR_CPP_UNEXPECTED_CALL: {
      // Call unexpected
      unexpected();
    } break;
    case CR_CPP_PURE_CALL: {
      // pure virtual method call
      CDerived derived;
    } break;
    case CR_CPP_SECURITY_ERROR: {
      // Cause buffer overrun (/GS compiler option)

      // declare buffer that is bigger than expected
      char large_buffer[] = "This string is longer than 10 characters!!";
      test_buffer_overrun(large_buffer);
    } break;
    case CR_CPP_INVALID_PARAMETER: {
      char* formatString;
      // Call printf_s with invalid parameters.
      formatString = NULL;
#pragma warning(disable : 6387)  // warning C6387: 'argument 1' might be '0': this does not adhere to the specification for the function 'printf'
      printf(formatString);
#pragma warning(default : 6387)

    } break;
    case CR_CPP_NEW_OPERATOR_ERROR: {
      // Cause memory allocation error
      RecurseAlloc();
    } break;
    case CR_CPP_SIGABRT: {
      // Call abort
      abort();
    } break;
    case CR_CPP_SIGFPE: {
      // floating point exception ( /fp:except compiler option)
      sigfpe_test();
      return 1;
    }
    case CR_CPP_SIGILL: {
      int result = raise(SIGILL);
      ATLASSERT(result == 0);
      return result;
    }
    case CR_CPP_SIGINT: {
      int result = raise(SIGINT);
      ATLASSERT(result == 0);
      return result;
    }
    case CR_CPP_SIGSEGV: {
      int result = raise(SIGSEGV);
      ATLASSERT(result == 0);
      return result;
    }
    case CR_CPP_SIGTERM: {
      int result = raise(SIGTERM);
      ATLASSERT(result == 0);
      return result;
    }
    case CR_NONCONTINUABLE_EXCEPTION: {
      // Raise noncontinuable software exception
      RaiseException(123, EXCEPTION_NONCONTINUABLE, 0, NULL);
    } break;
    case CR_THROW: {
      // Throw typed C++ exception.
      throw 13;
    } break;
    case CR_STACK_OVERFLOW: {
      // Infinite recursion and stack overflow.
      CauseStackOverflow();
    }
    default:
      break;
  }

  return 1;
}
}  // namespace WinCrashpad

#ifndef WINCRASHPAD_STATIC_LIB
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID /*lpReserved*/) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    WinCrashpad::g_hModuleCrashRpt = hModule;
  }
  else if (dwReason == DLL_THREAD_ATTACH) {
    // The current process is creating a new thread.
    WinCrashpad::CCrashHandler* pCrashHandler = WinCrashpad::CCrashHandler::GetCurrentProcessCrashHandler();
    if (pCrashHandler != NULL && pCrashHandler->IsInitialized() && (pCrashHandler->GetFlags() & CR_INST_AUTO_THREAD_HANDLERS) != 0) {
      pCrashHandler->SetThreadExceptionHandlers(0);
    }
  }
  else if (dwReason == DLL_THREAD_DETACH) {
    // A thread is exiting cleanly.
    WinCrashpad::CCrashHandler* pCrashHandler = WinCrashpad::CCrashHandler::GetCurrentProcessCrashHandler();
    if (pCrashHandler != NULL && pCrashHandler->IsInitialized() && (pCrashHandler->GetFlags() & CR_INST_AUTO_THREAD_HANDLERS) != 0) {
      pCrashHandler->UnSetThreadExceptionHandlers();
    }
  }

  return TRUE;
}
#endif