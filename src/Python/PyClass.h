// Jake Leonard
// 2011/09/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#ifdef _DEBUG
#undef _DEBUG
#include "Python.h"
#define _DEBUG
#else
#include "Python.h"
#endif

#include "PyClass.hpp"

namespace Improbability
{
  class PythonModule;
  typedef struct {
    PyObject_HEAD
    const char *className; /* Class name */
    const char *funcName;  /* Function name */
    void *obj;             /* Resolves this */
  } PyGeneralObj;

  class PyClass
  {
    public:
      typedef HashMap<String, class PyClass*> PyClassList;

    public:
      PyClass() { /* Seriously does nothing */}

      template<typename T>
      static bool CreateClass(const char *name);

      static void* GetTypeObject(const char *name);

      static void AddFunction(char const *className, char const *funcName);

      static void InitializeSystem();
      
    private:
      ~PyClass();

      String m_name;
      PyTypeObject *m_pyClassType;

      struct PyMethodInfo
      {
        PyMethodInfo(const char *className, const char *funcName);
        ~PyMethodInfo();

        /* The Python Method definition */
        PyMethodDef *m_pyMethDef;
        /* The Python Function Delegate */
        PyGeneralObj *m_pyDelObj;

        /* 
           These are the deadly objects. 
           They are the direct runtime instances of functions. 
        */
        PyObject *m_runMethodObj;
        PyObject *m_runFunctionObj;
      };

      std::vector<PyMethodInfo*> m_func;

      static PyClass* GetClass(const char *name);
      static void AddBaseClass(PythonModule *master);
      static void UnloadClasses();
      static PyClassList *sm_classList;

      static PyTypeObject MetaEngineType;
      static PyTypeObject MetaEngineTempType;

      friend class PythonSystem;
      friend class PythonModule;
  };

  template <typename T>
  bool PyClass::CreateClass(const char  *name)
  {
    PyClass::InitializeSystem();

    if (sm_classList == NULL)
      sm_classList = new PyClassList;

    String className(name);
    PyClass *newClass = new PyClass;

    newClass->m_pyClassType = new PyTypeObject;
    *(newClass->m_pyClassType) = PyClass::MetaEngineTempType;
    newClass->m_name = newClass->m_pyClassType->tp_doc = newClass->m_pyClassType->tp_name = name;
    newClass->m_pyClassType->tp_getattr = (getattrfunc)(pyGetAttr<T>);
    newClass->m_pyClassType->tp_setattr = (setattrfunc)(pySetAttr<T>);

    CriticalErrorIf((PyType_Ready(newClass->m_pyClassType) < 0), "Python could not allocate class object");
    Py_INCREF((newClass->m_pyClassType));

    (*sm_classList)[className] = newClass; 
    return true;
  }
}
