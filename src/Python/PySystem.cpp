// Jake Leonard
// 2011/05/12
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "PySystem.h"
#include "PyModule.h"
#include "PyFiles.h"
#include "PyWrapper.h"
#include "PyClass.h"
#include "PyMessaging.h"
#include "../Core/EngineCore.h"

namespace Improbability
{

MakeMeta(PythonSystem);

void PythonSystem::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnKeyRelease);
}

void PythonSystem::InitializeGlobal()
{
  // Modify the global Python system
  PyRun_SimpleString("import sys");

  // Add our script directory
  PyRun_SimpleString("sys.path.append(\"res\\scripts\")");

  // Link input and output handles
  //PyRun_SimpleString("ConsoleInLink = open('CONIN$', 'rt')");
  //PyRun_SimpleString("sys.stdin = ConsoleInLink");
}

/* Because I need this stuff initialized before binding even occurs. 
   Since binding is templated, it happens before anything else does. 
*/
void PythonSystem::InitializeRuntime()
{
  if (sm_initialized)
    return;

  // Fill the sentient method. Don't ask why, Python uses C terminators.
  PythonModule::makeSentient();

  // Append the global module to the ext table
  globalModule = PythonModule::createModule("Improb");

  // Append the thing so we can get an instance so that we can add classes
  globalModule->appendInit();

  //std::cout << "\n--- Loading Python ---" << std::endl;

  PyMessaging::MakePyModule();

  // Initialize Python.
  Py_Initialize();

  // Import the module into the runtime
  globalModule->importModule();

  PyImport_ImportModule("Message");

  // Add the base class
  PyClass::AddBaseClass(globalModule);

  sm_initialized = true;

  // Wait for other classes
}


void PythonSystem::PostInitializeGlobal()
{
  /* Build the classes into our modules (functions should be bound at this point) */
  for(auto it = PyClass::sm_classList->Begin(); it != PyClass::sm_classList->End(); ++it)
    globalModule->AddClass(it->second);
  
  //PyRun_SimpleString("print('--- Python C++ Binding Completed ---')");

  // At this point we can start to load scripts
  //PythonFiles::ImportFiles();

  /* Redirect Python Output to the dev console */
  //PythonSystem::CallFunction("console.py", "setUpConsole", NULL);
  //PyRun_SimpleString("print('--- Python Redir Complete ---')");
  PyRun_SimpleString("import Improb");
  PyRun_SimpleString("import Message");
}

void PythonSystem::InitializeInstance()
{
  m_consoleOpened = false;
  g_engine_core->AddEventListener(MessageBank::OnKeyRelease, this);
}

void PythonSystem::UpdateInstance(float dt)
{

  /* Messaging Updater */
  PyMessaging::Update();

  /* Python Debugger lives here. */
  PyObject *except = PyErr_Occurred();
  if (except != NULL)
  {
    std::cerr << "Python Error Detected!" << std::endl;
    int exceptionCode = PyErr_ExceptionMatches(except);
    PyErr_Print();

    // 1 is a recoverable error, normally we shouldn't get it as the FileImporter would find it first.
    if (exceptionCode == 1)
      PyErr_Clear();
    else
      throw std::exception("Python has encountered an unrecoverable error");
  }
}

void PythonSystem::UninitializeInstance()
{
  g_engine_core->RemoveEventListener(MessageBank::OnKeyRelease, this);
}

void PythonSystem::UninitializeGlobal()
{
  // Shut down the console link
  //PyRun_SimpleString("ConsoleInLink.close()");
  //PythonSystem::CallFunction("console.py","closeConsoleHook", NULL);

  // Force it to die. Like right now.
  //PyRun_SimpleString("del ConsoleInLink;");
  //PyRun_SimpleString("del ConsoleInLink;");

  // Shut down the embedded interpreter.
  Py_Finalize();

  // Unload the classes.
  PyClass::UnloadClasses();

  // Deallocate all modules.
  delete globalModule;
}

/* Notes:
    * We need a return type
    * We need something to hold the return type.
*/
void PythonSystem::CallFunction(char * const fileName, char * const funcName, char * const argFormat, ...)
{
  va_list arg;
  PyObject *fileObj = PythonFiles::GetFileObject(fileName);
  PyObject *funcObj, *pReturn, *temp, *pArgs = NULL;
  unsigned numArgs = (argFormat == NULL) ? 0 : strlen(argFormat);

  if (numArgs != 0)
    va_start(arg, argFormat);

  if (fileObj)
  {
    Py_INCREF(fileObj);
    funcObj = PyObject_GetAttrString(fileObj, funcName);
    if (funcObj && PyCallable_Check(funcObj))
    {
      if (numArgs != 0)
      {
        pArgs = PyTuple_New(numArgs);
        for (unsigned i = 0; i < numArgs; ++i)
       {
          if (argFormat[i] == 's')
            temp = PyReturnStr(va_arg(arg, char *));
          else if (argFormat[i] == 'i')
            temp = PyReturnInt(va_arg(arg, int));
          else if (argFormat[i] == 'd')
            temp = PyReturnDouble(va_arg(arg, double));
          else if (argFormat[i] == 'f')
            temp = PyReturnFloat(va_arg(arg, float));
          else if (argFormat[i] == 'u' || argFormat[i] == 'I')
            temp = PyReturnUInt(va_arg(arg, unsigned));
          else if (argFormat[i] == 'b')
            temp = PyReturnBool(va_arg(arg, bool));

          if (!temp)
          {
            PyErr_Print();
            Py_XDECREF(temp);
            Py_XDECREF(pArgs);
            Py_XDECREF(funcObj);
            Py_DECREF(fileObj);
            return;
          }

          PyTuple_SetItem(pArgs, i, temp);
        }
      }

      pReturn = PyObject_CallObject(funcObj, pArgs);

      if (pReturn == NULL)
        std::cerr << "PyCall: An error occurred while calling object" << std::endl;
      /* TODO: Support return types */

      Py_XDECREF(pReturn);

      if (numArgs != 0)
      {
        Py_XDECREF(temp);
        Py_XDECREF(pArgs);
      }
    }
    else
      PyErr_Print();
    Py_XDECREF(funcObj);
    Py_DECREF(fileObj);
  }
  else
    std::cerr << "PyCall: File is not imported" << std::endl;

  if (numArgs != 0)
    va_end(arg);
}

void PythonSystem::ReloadPythonFile(const char* fileName)
{
  PythonFiles::ReloadFile(fileName);
}

void PythonSystem::OnKeyRelease(KeyboardEvent event)
{
  #ifdef _DEBUG
  if(event.m_keycode == kKeyTilde)
  {
    if(m_consoleOpened)
      g_engine_core->DispatchEvent("OnToggleDevConsole", false);
    else
      g_engine_core->DispatchEvent("OnToggleDevConsole", true);
    m_consoleOpened = !m_consoleOpened;
  }
  #endif
}

bool PythonSystem::sm_initialized = false;

PythonModule *PythonSystem::globalModule;

}
