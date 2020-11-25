// Nicholas Baldwin
// 2011/10/06
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "CrashHandler.h"
#include <DbgHelp.h>

// these are so I can have identifiable names for my game project,
// non full release builds don't get a number because they will most likely be
// worked on at the computer they were built at, full release versions have their
// release number decided upon elsewhere
#ifdef _DEBUG

#define ReleaseVersion "D"
#define ReleaseName "DebugCandidate_"

#elif defined(ReleaseVersion)

#define ReleaseName "ReleaseVersion_"

#else

#define ReleaseVersion "R"
#define ReleaseName "ReleaseCandidate_"

#endif

#define MinidumpName ReleaseName ReleaseVersion ".dmp"
#define StackOverflowName ReleaseName ReleaseVersion "_Stack_Overflow.log"

static char g_stack_overflow_message[256];

// don't want this in the header as it forces you to include windows.h and dbghelp.h
namespace Improbability
{
  void HandleCrash(EXCEPTION_POINTERS *exception_pointers);
}

LONG WINAPI ExceptionFilter(LPEXCEPTION_POINTERS exception_pointers)
{
  // this is how to get debug info when the stack overflows when 
  if (exception_pointers->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
  {
    HANDLE file_handle = CreateFileA(StackOverflowName,
                                     GENERIC_WRITE,
                                     0,
                                     0,
                                     CREATE_ALWAYS,
                                     FILE_ATTRIBUTE_NORMAL,
                                     0);

    int message_size = sprintf_s(g_stack_overflow_message, sizeof(g_stack_overflow_message), 
                                 "There was a stack overflow!\n EIP : %X", 
                                 exception_pointers->ContextRecord->Eip);

    WriteFile(file_handle, g_stack_overflow_message, message_size, 0, 0);

    CloseHandle(file_handle);
  }
  else
  {
    Improbability::HandleCrash(exception_pointers);
  }

  return EXCEPTION_CONTINUE_SEARCH;
}

namespace Improbability
{

void HandleCrash(EXCEPTION_POINTERS *exception_pointers)
{
  char message[] =
"This program has crashed! Would you like to create a minidump so we can figure out what is wrong? \nYes - Large Minidump\nNo - Small Minidump\nCancel - No Minidump";

  int do_minidump = MessageBoxA(0, 
                                message, 
                                "We crashed: output debug information?", 
                                MB_YESNOCANCEL | MB_ICONERROR | MB_TOPMOST);

  if (do_minidump == IDYES || do_minidump == IDNO)
  {
    MINIDUMP_EXCEPTION_INFORMATION exception_information;
    ZeroMemory(&exception_information, sizeof(exception_information));
    exception_information.ThreadId = GetCurrentThreadId();
    exception_information.ExceptionPointers = exception_pointers;
    exception_information.ClientPointers = FALSE;

    HANDLE file_handle = CreateFileA(MinidumpName,
                                    GENERIC_WRITE,
                                    0,
                                    0,
                                    CREATE_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL,
                                    0);

    MINIDUMP_TYPE minidump_type = (MINIDUMP_TYPE)(MiniDumpWithFullMemory | MiniDumpWithHandleData | MiniDumpWithUnloadedModules);

    if (do_minidump == IDNO)
    {
      minidump_type = (MINIDUMP_TYPE)(MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory);
    }

    MiniDumpWriteDump(GetCurrentProcess(),
                      GetCurrentProcessId(),
                      file_handle,
                      minidump_type,
                      &exception_information,
                      0, 0);

    CloseHandle(file_handle);
  }
}

namespace DebugUtilities
{

void RegisterCrashHandler()
{
  SetUnhandledExceptionFilter(&ExceptionFilter);
}

}

}
