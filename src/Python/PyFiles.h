// Jake Leonard
// 2011/05/12
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "../containers/HashMap.h"

namespace Improbability
{

class PythonFiles
{
public:
  static void ImportFiles();
  static void ReloadFile(const char* file);

private:
  static void GetFiles(std::vector<String>& fileList);
  static String NoFileExt(String& file);
  static void UnloadAll();
  static void LoadFile(String& file);
  // static bool CheckFileForErrors(String& file); // This is unneeded.
  static PyObject* GetFileObject(char * const fileName);

  // This will allow us to reload files. The reason why this works is that PyObject
  // modules are treated differently than data in Python's garbage collector.
  static HashMap<String, PyObject*> s_fileObjs;

  friend class PythonSystem;
};

}
