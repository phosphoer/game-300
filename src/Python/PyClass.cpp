// Jake Leonard
// 2011/09/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "PyModule.h"
#include "PyClass.h"
#include "PyWrapper.h"
#include "PySystem.h"
#include "../Core/Space.h"
#include "../Core/EngineCore.h"
#include "structmember.h"


/* Python Wrapper Types */
namespace Improbability
{
  static PyObject* pythonFunc(PyObject *self, PyObject *args)
  {
    
    ErrorIf((self == NULL), "Python: self is NULL.");
    PyGeneralObj *pyObjInfo = (PyGeneralObj*)self;
    String funcName(pyObjInfo->funcName), className(pyObjInfo->className);

    ErrorIf((args == NULL), "Args was NULL for function %s. Pass self everyday.", pyObjInfo->funcName);

    int passedArgs = PyTuple_GET_SIZE(args) - 1;
    ErrorIf(passedArgs == -1, "WHY YOU NO PASS SELF!");

    PyGeneralObj *pyObj = (PyGeneralObj*)(PyGetItemFromTuple(args, 0));
    void *cObj = reinterpret_cast<void*>(pyObj->obj);
    DynamicValue varArgs[4];

    // Pull arguments
    PyObject *arg;
    for(int i = 1; i < passedArgs; ++i)
    {
      arg = PyGetItemFromTuple(args, i);

      if (arg == NULL)
      {
        varArgs[i] = NULL;
        continue;
      }

      if (PyLong_CheckExact(arg)) // int
        varArgs[i] = DynamicValue(PyGetInt(arg));
      else if (PyLong_Check(arg)) // unsigned
        varArgs[i] = DynamicValue(PyGetUInt(arg));
      else if (PyFloat_CheckExact(arg)) // float
        varArgs[i] = DynamicValue(PyGetFloat(arg));
      else if (PyFloat_Check(arg)) // double
        varArgs[i] = DynamicValue(PyGetDouble(arg));
      else if (PyBool_Check(arg)) // bool
        varArgs[i] = DynamicValue(PyGetBool(arg));
      else if (PyUnicode_Check(arg)) // string
        varArgs[i] = DynamicValue(PyGetString(arg));
      else // Check
      {
        PyGeneralObj* pyObj = reinterpret_cast<PyGeneralObj*>(arg);
        varArgs[i] = DynamicValue(pyObj->obj, pyObj->className);
      }
    }
   
    Variable returnVal;

    MetaComposition* classMeta = Meta::GetMeta(className);
    MetaCallableBase* classMetaData = classMeta->GetComponent<MetaCallableBase>("MetaCallable");
    bool hasReturn = ((classMetaData->GetMethodInfo(funcName))->GetReturnType() != "void");

    // Build returns
    if(hasReturn)
    {
      if (passedArgs == 0)
        returnVal = Meta::InvokeRet(className, funcName, cObj);
      else if (passedArgs == 1)
        returnVal = Meta::InvokeRet(className, funcName, cObj, varArgs[0]);
      else if (passedArgs == 2)
        returnVal = Meta::InvokeRet(className, funcName, cObj, varArgs[0], varArgs[1]);
      else if (passedArgs == 3)
        returnVal = Meta::InvokeRet(className, funcName, cObj, varArgs[0], varArgs[1], varArgs[2]);
      else if (passedArgs == 4)
        returnVal = Meta::InvokeRet(className, funcName, cObj, varArgs[0], varArgs[1], varArgs[2], varArgs[3]);
    }
    else
    {
      if (passedArgs == 0)
        Meta::Invoke(className, funcName, cObj);
      else if (passedArgs == 1)
        Meta::Invoke(className, funcName, cObj, varArgs[0]);
      else if (passedArgs == 2)
        Meta::Invoke(className, funcName, cObj, varArgs[0], varArgs[1]);
      else if (passedArgs == 3)
        Meta::Invoke(className, funcName, cObj, varArgs[0], varArgs[1], varArgs[2]);
      else if (passedArgs == 4)
        Meta::Invoke(className, funcName, cObj, varArgs[0], varArgs[1], varArgs[2], varArgs[3]);

      Py_RETURN_NONE;
    }

    // Return wrap
    if (returnVal.GetType() == "int") // int
      return PyReturnInt(returnVal);
    else if (returnVal.GetType() == "unsigned") // unsigned
      return PyReturnUInt(returnVal);
    else if (returnVal.GetType() == "float") // float
      return PyReturnFloat(returnVal);
    else if (returnVal.GetType() == "double") // double
      return PyReturnDouble(returnVal);
    else if (returnVal.GetType() == "bool") // bool
      return PyReturnBool(returnVal);
    else if (returnVal.GetType() == "String") // string
      return PyReturnStr(returnVal);
    else
    {
      // Create an object
      PyGeneralObj newObject;
      newObject.obj = returnVal;
      newObject.className = returnVal.GetType().GetCString();
      PyObject *pyNewObj = Py_BuildValue("O!", (PyTypeObject*)PyClass::GetTypeObject(returnVal.GetType().GetCString()), newObject);

      if (pyNewObj)
        return pyNewObj;
      else
        Py_RETURN_NONE;
    }
  }

  static PyObject* pyCreate(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
    PyGeneralObj *newObj = (PyGeneralObj*)(type->tp_alloc(type, 0));
    if (newObj == NULL)
      return NULL;

    const char *typeName = (type->tp_name);

    newObj->className = typeName;
    newObj->funcName = NULL;

    /* Returns the new object that has been manipulated */
    return (PyObject*)newObj;
  }

  static int pyInit(PyGeneralObj *self, PyObject *args, PyObject *kwds)
  {
    const char *typeName = self->className;

    PyTypeObject *typeObj = reinterpret_cast<PyTypeObject*>(PyClass::GetTypeObject(typeName));

    if (typeObj->tp_base->tp_init((PyObject *)self, args, kwds) < 0)
      return -1;

    /*void *checkObj = reinterpret_cast<void*>(g_engine_core->GetSystem(String(typeName)));
    if (checkObj == NULL)*/
      self->obj = Meta::Create<PyObject>(typeName);
    /*else
      self->obj = reinterpret_cast<PyObject*>(checkObj);*/

    return 0;
  }

  static void pyDealloc(PyObject *obj)
  {
    Py_TYPE(obj)->tp_free((PyObject*)obj);
  }
}

/* Python Types */
namespace Improbability
{
   
   static PyMemberDef generalObjectMembers[] = {
    {"className", T_STRING, offsetof(PyGeneralObj, className), READONLY, "Name of the class"},
    {"funcName", T_STRING, offsetof(PyGeneralObj, funcName), READONLY, "Name of function"},
    {"obj", T_OBJECT, offsetof(PyGeneralObj, obj), READONLY,"C data pointer"},
    {NULL}
  };

  PyTypeObject PyClass::MetaEngineType = {
    PyObject_HEAD_INIT(NULL)
    "ImpBaseType",             /* tp_name */
    sizeof(PyGeneralObj),      /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)pyDealloc,     /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "Global Engine Namespace", /* tp_doc */
    0,		                     /* tp_traverse */
    0,		                     /* tp_clear */
    0,		                     /* tp_richcompare */
    0,		                     /* tp_weaklistoffset */
    0,		                     /* tp_iter */
    0,		                     /* tp_iternext */
    0,                         /* tp_methods */
    generalObjectMembers,      /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    (newfunc)pyCreate,         /* tp_new */
  };

  PyTypeObject PyClass::MetaEngineTempType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "TempObj",                 /* tp_name */
    sizeof(PyGeneralObj),      /* tp_basicsize */
    0,                         /* tp_itemsize */
    0,                         /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "Template Namespace",      /* tp_doc */
    0,		                     /* tp_traverse */
    0,		                     /* tp_clear */
    0,		                     /* tp_richcompare */
    0,		                     /* tp_weaklistoffset */
    0,		                     /* tp_iter */
    0,		                     /* tp_iternext */
    0,                         /* tp_methods */
    generalObjectMembers,      /* tp_members */
    0,                         /* tp_getset */
    &MetaEngineType,           /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)pyInit,          /* tp_init */
    0,                         /* tp_alloc */
    0,                         /* tp_new */
  };
}

/* Python Method Info */
namespace Improbability
{
  PyClass::PyMethodInfo::PyMethodInfo(const char *className, const char *funcName)
  {
    m_pyMethDef = new PyMethodDef;
    m_pyMethDef->ml_flags = METH_VARARGS;
    m_pyMethDef->ml_meth = pythonFunc;
    m_pyMethDef->ml_doc = m_pyMethDef->ml_name = funcName;

    m_pyDelObj = PyObject_New(PyGeneralObj, &(MetaEngineTempType));

    m_pyDelObj->className = className;
    m_pyDelObj->funcName = funcName;

    Py_INCREF(m_pyMethDef);
  }
  PyClass::PyMethodInfo::~PyMethodInfo()
  {
    Py_XDECREF(m_runMethodObj);
    Py_XDECREF(m_runFunctionObj);

    /* So hacks. MetaEngineTempType is not known by Python, but the base class is. */
    m_pyDelObj->ob_base.ob_type = &(PyClass::MetaEngineType);

    Py_DECREF(m_pyDelObj);
    Py_XDECREF(m_pyMethDef);
    
    delete m_pyMethDef;
  }
}

/* Python Class */
namespace Improbability
{
  void PyClass::AddFunction(char const *className, char const *funcName)
  {
    return;

    PyClass *classObj = PyClass::GetClass(className);

    PyMethodInfo *methodInfo = new PyMethodInfo(className, funcName);
    PyMethodDef *newMethod = methodInfo->m_pyMethDef;

    PyObject *func = PyCFunction_New(newMethod, (PyObject*)(methodInfo->m_pyDelObj));
    PyObject *method = PyInstanceMethod_New(func);
    methodInfo->m_runFunctionObj = func;
    methodInfo->m_runMethodObj = method; 
    ErrorIf((method == NULL), "Python: Cannot create instance function. %s", funcName);

    int setItem = PyDict_SetItemString(classObj->m_pyClassType->tp_dict, funcName, method);
    ErrorIf((setItem == -1), "Python: Cannot create function in dictionary.");

    classObj->m_func.push_back(methodInfo);
  }

  void PyClass::AddBaseClass(PythonModule *master)
  {
    Py_INCREF(&MetaEngineType);
    int pyReady = PyType_Ready(&MetaEngineType);
    ErrorIf((pyReady < 0), "Python could not allocate class object. Error: %d", pyReady);

    Py_INCREF(&MetaEngineType);
    Py_INCREF(&MetaEngineTempType); /* To be safe while creating objects */

    PyModule_AddObject(master->m_moduleRef, MetaEngineType.tp_name, (PyObject *)(&MetaEngineType));
  }

  PyClass* PyClass::GetClass(const char *name)
  {
    String className(name);
    ErrorIf((sm_classList->Find(name) == sm_classList->End()), "Python could not add function to non-existant class.");
    return (*sm_classList)[className];
  }

  void PyClass::UnloadClasses()
  {
    if (sm_classList->IsEmpty())
      return;

    for(auto it = sm_classList->Begin(); it != sm_classList->End(); ++it)
      delete it->second;

    Py_DECREF(&MetaEngineType);
    Py_DECREF(&MetaEngineType);
    Py_DECREF(&MetaEngineTempType);
  }

  PyClass::~PyClass()
  {
    for(auto it = m_func.begin(); it != m_func.end(); ++it)
      delete *it;

    Py_XDECREF(m_pyClassType);
    delete m_pyClassType;
  }

  void* PyClass::GetTypeObject(const char *name)
  {
    PyClass *reqClass = GetClass(name);
    return reinterpret_cast<void*>(reqClass->m_pyClassType);
  }

  void PyClass::InitializeSystem()
  {
    if (PythonSystem::IsInitialized() == false)
    {
      PythonSystem::InitializeRuntime();

      Py_INCREF(&(PyClass::MetaEngineType));
    }
  }

  PyClass::PyClassList *PyClass::sm_classList = NULL;

}