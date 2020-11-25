// Jake Leonard
// 2011/09/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "PyWrapper.h"

namespace Improbability
{
  class PyClass;

  /* Gets an attribute */
  template <typename T>
  static PyObject* pyGetAttr(PyObject *o, const char *attr_name)
  {
    PyGeneralObj *pyObj = (PyGeneralObj*)o;
    const char *className = pyObj->className;
    PyTypeObject *pyType = reinterpret_cast<PyTypeObject*>(PyClass::GetTypeObject(className));

    /* If it's a request for the class, give it the current symbols */
    if (strcmp(attr_name, "__class__") == 0)
      return (PyObject*)pyType;

    /* Figure out if it's a method that should be called. */
    PyObject *chkDict = PyDict_GetItemString(pyType->tp_dict, attr_name);
    if (chkDict != NULL)
      return chkDict;

    /* Otherwise, it's an attribute */
    MetaComposition *metaData = Meta::GetVariableMeta(pyObj->className, attr_name);
    T *cObj = reinterpret_cast<T*>(pyObj->obj);

    /* Check if a member var */
    if (metaData)
    {
      if (metaData->GetName() == "float")
        return PyReturnFloat(Meta::GetVariable<float, T>(attr_name, cObj));
      else if (metaData->GetName() == "int")
        return PyReturnInt(Meta::GetVariable<int, T>(attr_name, cObj));
      else if (metaData->GetName() == "char*")
        return PyReturnStr(Meta::GetVariable<char*, T>(attr_name, cObj));
      else if (metaData->GetName() == "String")
        return PyReturnStr(Meta::GetVariable<String, T>(attr_name, cObj).GetCString());
      else if (metaData->GetName() == "double")
        return PyReturnDouble(Meta::GetVariable<double, T>(attr_name, cObj));
      else if (metaData->GetName() == "unsigned")
        return PyReturnUInt(Meta::GetVariable<unsigned, T>(attr_name, cObj));
      else if (metaData->GetName() == "bool")
        return PyReturnBool(Meta::GetVariable<bool, T>(attr_name, cObj));
      else
      {
        /* Recreate the object as a PyObject */
        PyGeneralObj newObject;
        newObject.obj = Meta::GetVariable<void*, T>(attr_name, cObj);
        newObject.className = metaData->GetName().GetCString();
        PyObject *pyNewObj = Py_BuildValue("O!", (PyTypeObject*)PyClass::GetTypeObject(metaData->GetName()), newObject);

        if (pyNewObj)
          return pyNewObj;
        else
          Py_RETURN_NONE;
      }
    }

    /* Returns NULL on failure */
    Py_RETURN_NONE;
  }

  template <typename T>
  static int pySetAttr(PyObject *o, const char *attr_name, PyObject *v)
  {
    PyGeneralObj *lhs = (PyGeneralObj*)o;
    PyGeneralObj *rhs = (PyGeneralObj*)v;
    T *cObj = reinterpret_cast<T*>(lhs->obj);
    MetaComposition *metaData = Meta::GetVariableMeta(lhs->className, attr_name);

    /* TODO: Make this raise an attribute error */
    ErrorIf((metaData == NULL), "Could not get Meta Data");

    if (metaData)
    {
      if (PyLong_CheckExact(v) && metaData->GetName() == "int") /* int */
        Meta::GetVariable<int>(attr_name, cObj) = PyGetInt(v);
      else if (PyLong_Check(v) && metaData->GetName() == "unsigned") /* unsigned */
        Meta::GetVariable<unsigned>(attr_name, cObj) = PyGetUInt(v);
      else if (PyFloat_CheckExact(v) && metaData->GetName() == "float") /* float */
        Meta::GetVariable<float>(attr_name, cObj) = PyGetFloat(v);
      else if (PyFloat_Check(v) && metaData->GetName() == "double") /* double */
        Meta::GetVariable<double>(attr_name, cObj) = PyGetDouble(v);
      else if (PyBool_Check(v) && metaData->GetName() == "bool") /* bool */
        Meta::GetVariable<bool>(attr_name, cObj) = PyGetBool(v);
      else if (PyUnicode_Check(v))
        Meta::GetVariable<String>(attr_name, cObj) = String(PyGetString(v));
      else
      {
        /* Assignment because types match */
        if (String(lhs->className) == String(rhs->className))
          *(cObj) = *(reinterpret_cast<T*>(rhs->obj));
      }
      return 0;
    }

    /* Return -1 on failure */
    return -1;
  }
}