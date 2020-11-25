// Jake Leonard
// 2011/05/12
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "PyFiles.h"
#include "PyWrapper.h"

static const Improbability::String s_scriptDir("res\\scripts\\*");


namespace Improbability
{

void PythonFiles::LoadFile(String& file)
{
  String modFile = NoFileExt(file);
  PyObject *modObj, *name = PyReturnStr(modFile.GetCString());
  modObj = PyImport_Import(name);

  Py_DECREF(name);

  if (!modObj)
  {
    std::cerr << "PythonFiles: Could not load file " << file << ".\nPlease fix the following issues and reload...\n" << std::endl;
    s_fileObjs[file] = NULL;
    PyErr_Print();
    PyErr_Clear();
    return;
  }

  s_fileObjs[file] = modObj;
  //std::cout << "PythonFiles: Loaded file " << file << " successfully." << std::endl;
}

void PythonFiles::ImportFiles()
{
  std::vector<String> fileList;
  //std::cout << "--- Importing Python Files ---\nPythonFiles: Parsing Python Directory...\n" << std::endl;

  GetFiles(fileList);

  //std::cout << "\nPythonFiles: Loading Files..." << std::endl;
  for (unsigned i = 0; i < fileList.size(); ++i)
    LoadFile(fileList[i]);

  //std::cout << "--- Python File Loading Completed ---\n" << std::endl;
}

void PythonFiles::ReloadFile(const char* file)
{
  String fileStr(file);
  if (s_fileObjs.Find(fileStr) == s_fileObjs.End())
  {
    std::cerr << "PythonFiles: File to reload does not exist." << std::endl;
    return;
  }

  /* If the file originally failed to import, attempt to load the obj again. */
  PyObject* origMod = s_fileObjs[fileStr];
  Py_XINCREF(origMod);
  if(origMod == NULL)
  {
    LoadFile(fileStr);
    return;
  }

  /* The file has been imported successfully in the past, we can reload safely */
  PyObject* temp;
  temp = PyImport_ReloadModule(origMod);
  Py_XDECREF(origMod);
  if(temp == NULL)
  {
    std::cerr << "PythonFiles: Reload file failed." << std::endl;
    PyErr_Print();
    PyErr_Clear();
    return;
  }

  s_fileObjs[fileStr] = temp;

//   if (CheckFileForErrors(fileStr))
//   {
//     std::cerr << "PythonFiles: Reload file failed." << std::endl;
//     return;
//   }
// 
//   unsigned refCount = s_fileObjs[fileStr]->ob_refcnt;
//   while (refCount > 0)
//   {
//     Py_DECREF(s_fileObjs[fileStr]);
//     --refCount;
//     PyErr_Clear();
//   }
// 
//   LoadFile(fileStr);
}

void PythonFiles::UnloadAll()
{
  for (auto it = s_fileObjs.Begin(); it != s_fileObjs.End(); ++it)
  {
    Py_XDECREF(it->second);
  }
}

// bool PythonFiles::CheckFileForErrors(String& file)
// {
//   /* Additionally, we could attempt to read the file and then load it into memory, but it would look ugly */
//   String tempModule = NoFileExt(file) + "-temp";
//   String tempFile =  s_scriptDir + tempModule + ".py";
//   PyObject* modObj, *name;
//   if (CopyFile((s_scriptDir + file).GetCString(), tempFile.GetCString(), TRUE) == FALSE)
//   {
//     std::cout << "PythonFiles: Could not copy file" << std::endl;
//     return false;
//   }
// 
//   name = PyReturnStr(tempModule.GetCString());
//   modObj = PyImport_Import(name);
//   Py_DECREF(name);
//     
//   if (!modObj)
//   {
//     PyErr_Print();
//     PyErr_Clear();
//     return false;
//   }
//   else
//   {
//     Py_DECREF(modObj);
//     DeleteFile(tempFile.GetCString());
//     return true;
//   }
// }

String PythonFiles::NoFileExt(String& file)
{
  String result;
  int length = file.GetLength();
  char* buffer = new char[length + 1];
  SecureZeroMemory(buffer, length);
  memcpy(buffer, file.GetCString(), length - 3);
  result.Assign(buffer);
  delete [] buffer;
  return result;
}

void PythonFiles::GetFiles(std::vector<String>& fileList)
{
  HANDLE curFile;
  WIN32_FIND_DATA data;
  SecureZeroMemory(&curFile, sizeof(HANDLE));
  SecureZeroMemory(&data, sizeof(WIN32_FIND_DATA));
  int filesRead = 0;
  curFile = FindFirstFile(s_scriptDir.GetCString(), &data);
  bool shouldContinue = (curFile != INVALID_HANDLE_VALUE);

  int continueFile = 1;
  while (continueFile != 0)
  {
    if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // We don't want directories.
    {
      continueFile = FindNextFile(curFile, &data);
      continue;
    }

    ++filesRead;
    //std::cout << "PythonFiles: Found file " << data.cFileName << std::endl;
    String file(data.cFileName);
    fileList.push_back(file);
    continueFile = FindNextFile(curFile, &data);
  }

  FindClose(curFile); // Because it should be all clean.
}

PyObject* PythonFiles::GetFileObject(char * const fileName)
{
  String temp(fileName);
  unsigned length = temp.GetLength() - 1;

  // If the file extension isn't there, add it.
  if (length <= 3 || temp[length - 2] != '.')
    temp += ".py";
  
  if (s_fileObjs.Find(temp) != s_fileObjs.End())
    return s_fileObjs[temp];
    
  return NULL;
}

HashMap<String, PyObject*> PythonFiles::s_fileObjs;

}
