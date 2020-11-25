// Jake Leonard
// 2011/05/25
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "PyModule.h"
#include "PyClass.h"

static PyModuleDef staticModule = {PyModuleDef_HEAD_INIT, "stupidTemp", NULL, -1, NULL, NULL, NULL, NULL, NULL};

/* Couldn't think of a better way to do this, so it's a static global */
static Improbability::PythonModule *sg_modulePtr = NULL;

/* This is just how Python allows us to append modules, we need a function ptr to do it */
static PyObject* appendInitFunc()
{
  return PyModule_Create(sg_modulePtr->GetModuleContainer());
}

namespace Improbability
{

PythonModule::PythonModule(char * const methodName): 
  m_name(methodName)
{
  m_moduleDef = new PyModuleDef;
  // The constructor for this is really really strange, you must construct one of the 
  // members using a macro that only works if the object is static.
  *m_moduleDef = staticModule;
  m_moduleDef->m_doc = m_moduleDef->m_name = m_name.GetCString();
  m_moduleDef->m_size = -1;
  m_moduleDef->m_reload = NULL;
  m_moduleDef->m_traverse = NULL;
  m_moduleDef->m_clear = NULL;
  m_moduleDef->m_free = NULL;
}

PythonModule::~PythonModule()
{
  delete m_moduleDef;
}

void PythonModule::appendInit()
{
  // Finish the method table
  m_pymethods.push_back(sm_terminatingMethod);

  // Pointer to the first element in the vector
  m_moduleDef->m_methods = &(m_pymethods[0]);

  // Set the global pointer so we can link with python correctly
  sg_modulePtr = this;
  CriticalErrorIf(PyImport_AppendInittab(m_name.GetCString(), &appendInitFunc) == -1, "PyMod: Could not append module");
}

void PythonModule::importModule()
{
  // This will return the current runtime re-reference of the object
  m_moduleRef = PyImport_ImportModule(m_name.GetCString());
  ErrorIf((m_moduleRef == NULL), "Module could not be imported.");
}

PythonModule* PythonModule::createModule(char * const moduleName)
{
  return new PythonModule(moduleName);
}

void PythonModule::makeSentient()
{
  // This sets the members of the terminating object to NULL.
  sm_terminatingMethod.ml_doc = NULL;
  sm_terminatingMethod.ml_flags = 0;
  sm_terminatingMethod.ml_meth = NULL;
  sm_terminatingMethod.ml_name = NULL;
}

/* TODO: Rewrite for our master namespace */
void PythonModule::AddMethod(PyMethodDef* newMethod)
{
  m_pymethods.push_back(*newMethod);
}

void PythonModule::AddClass(PyClass* classObj)
{
  /* Inherit the class */
  int objAdd = PyModule_AddObject(m_moduleRef, classObj->m_name.GetCString(), (PyObject *)(classObj->m_pyClassType));
  ErrorIf((objAdd == -1), "Could not add Object");
}

/* Sentient value */
PyMethodDef PythonModule::sm_terminatingMethod;

}
