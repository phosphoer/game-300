// Jake Leonard
// 2011/05/25
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{
class PyClass;
class PythonModule
{
public:
  // Allows you to create a module. Checks to see if this exists first. Returns the existing module if it does.
  static PythonModule* createModule(char * const moduleName);

  // You will never use this. This is a public method for python to use to assign modules. It's super stupid.
  PyModuleDef* GetModuleContainer() { return m_moduleDef; }

  // Add a method to the python table
  void AddMethod(PyMethodDef* newMethod);

  // Add the class to the namespace
  void AddClass(PyClass* classObj);

  static PyMethodDef sm_terminatingMethod;

private:
  PythonModule(char * const methodName);
  ~PythonModule();

  /* Data structure holding the python methods for this module */
  std::vector<PyMethodDef> m_pymethods;

  /* Method def */
  PyModuleDef *m_moduleDef;
  PyObject *m_moduleRef;

  /* Module name */
  String m_name;

  /* Python hooks. These are used by the PySystem to create the bindings correctly. */
  void appendInit();
  void importModule();

  // Makes the sentient terminator python method for the m_methods vectors
  static void makeSentient();

  /* Friends */
  friend class PythonSystem;
  friend class PyClass;
};

}
