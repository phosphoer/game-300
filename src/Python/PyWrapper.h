// Jake Leonard
// 2011/05/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

// Internal Python functions, you will never use them, you should never include 
// this file unless you are working in PythonSystem.

extern "C"
{
  // Grab the item from a pytuple
  PyObject* PyGetItemFromTuple(PyObject* args, int pos);

  // Converts the python object into a default type
  int PyGetInt(PyObject* obj);
  unsigned PyGetUInt(PyObject* obj);
  bool PyGetBool(PyObject* obj);
  const char* PyGetString(PyObject* obj);
  double PyGetDouble(PyObject* obj);
  float PyGetFloat(PyObject* obj);

  // Converts default types into a python object
  PyObject* PyReturnFloat(float arg);
  PyObject* PyReturnDouble(double arg);
  PyObject* PyReturnStr(const char* arg);
  PyObject* PyReturnBool(bool arg);
  PyObject* PyReturnInt(int arg);
  PyObject* PyReturnUInt(unsigned arg);
};

// Converts stuff to PyUnicode. Takes a wchar pointer and the string
// If returns true, remember to delete the resultBuf
bool MBCSToWideChar(wchar_t*& resultBuf, const char* string);

// Converts stuff from PyUnicode. Takes a reference to a string and the string itself
// No additional memory management necessary
bool WideCharToMBCS(Improbability::String& strResult, wchar_t* str);