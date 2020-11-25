// Jake Leonard
// 2011/05/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "PyWrapper.h"

Improbability::String g_pyConversionString;

PyObject* PyGetItemFromTuple(PyObject* args, int pos)
{
  PyObject* result = PyTuple_GetItem(args, pos);
  if (result == NULL)
    std::cerr << "PyGetItemFromTuple: Cannot get PyObject" << std::endl;
  return result;
}

int PyGetInt(PyObject* obj)
{
  int isInt = PyLong_CheckExact(obj);
  if (isInt)
    return static_cast<int>(PyLong_AsLong(obj));
  else if (PyFloat_Check(obj))
    std::cerr << "PyGetInt: Was passed a float/double" << std::endl;

  return -1;
}

unsigned PyGetUInt(PyObject* obj)
{
  int isInt = PyLong_CheckExact(obj);
  if (isInt)
    return static_cast<unsigned>(PyLong_AsSize_t(obj));
  return 0;
}

bool PyGetBool(PyObject* obj)
{
  int isBool = PyBool_Check(obj);
  bool returnVal = false;
  Py_INCREF(Py_True);
  if (isBool)
     returnVal = (obj == Py_True);
  Py_DECREF(Py_True);
  return returnVal;
}

char const* PyGetString(PyObject* obj)
{
  int sizeOfString = 0;
  wchar_t* pyStr = PyUnicode_AsWideCharString(obj, &sizeOfString);
  if (pyStr == NULL)
    return NULL;

  /* Need to convert unicode to ascii via magic */
  bool result = WideCharToMBCS(g_pyConversionString, pyStr);
  PyMem_Free(pyStr);
  if (result == false)
  {
    std::cerr << "PyGetString: Could not convert string to MBCS, returning NULL" << std::endl;
    return NULL;
  }
  else
    return g_pyConversionString.GetCString();
}

double PyGetDouble(PyObject* obj)
{
  int isDouble = PyFloat_Check(obj);
  if (isDouble)
    return PyFloat_AsDouble(obj);
  return 0.0;
}

float PyGetFloat(PyObject* obj)
{
  int isFloat = PyFloat_CheckExact(obj);
  if (!isFloat)
    std::cerr << "PyGetFloat: Variable was not casted as a float, going to hard cast." << std::endl;
  return static_cast<float>(PyFloat_AS_DOUBLE(obj));
}

PyObject* PyReturnFloat(float arg)
{
  double temp = static_cast<double>(arg);
  return PyFloat_FromDouble(temp);
}

PyObject* PyReturnDouble(double arg)
{
  return PyFloat_FromDouble(arg);
}

PyObject* PyReturnStr(const char* arg)
{
  PyObject* result/* = PyUnicode_DecodeMBCS(arg, strlen(arg), NULL)*/;
  /*if(result == NULL)
    return NULL;*/
  wchar_t* tempBuffer;
  if (MBCSToWideChar(tempBuffer, arg) == false)
  {
    std::cerr << "PyReturnStr: Could not convert MBCS string to wchar, attempting direct unicode..." << std::endl;
    result = PyUnicode_FromString(arg);
  }
  else
  {
    result = PyUnicode_FromWideChar(tempBuffer, -1);
    delete [] tempBuffer;
  }
  return result;
}

PyObject* PyReturnBool(bool arg)
{
  long temp = (arg) ? 1 : 0;
  return PyBool_FromLong(temp);
}

PyObject* PyReturnInt(int arg)
{
  long temp = static_cast<long>(arg);
  return PyLong_FromLong(temp);
}

PyObject* PyReturnUInt(unsigned arg)
{
  return PyLong_FromSize_t(arg);
}

bool MBCSToWideChar(wchar_t*& resultBuf, const char* string)
{
  unsigned wideLen, length = strlen(string);

  // Other functions will need to remember to delete the string
  resultBuf = new wchar_t[length + 1];
  wideLen = mbstowcs(resultBuf, string, length);
  if (wideLen == -1)
  {
    delete [] resultBuf;
    return false;
  }
  else
  {
    resultBuf[length] = 0;
    return true;
  }
}

bool WideCharToMBCS(Improbability::String& strResult, wchar_t* str)
{
  unsigned result, length = wcslen(str);
  char* buffer = new char[length + 2];

  result = wcstombs(buffer, str, length + 1);
  strResult.Assign(buffer);
  delete [] buffer;
  return !(result == 0);
}