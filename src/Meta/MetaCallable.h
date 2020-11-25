// David Evans
// 2011/10/02
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "MetaComponent.h"
#include "MetaFunctions.h"
#include "../Containers/HashMap.h"
#include "../Containers/String.h"

namespace Improbability
{

#define MetaRegisterFunction(fn) RegisterFunction(ThisTypeName, #fn, &ThisType::fn)

class MetaCallableBase : public MetaComponent
{
public:
  // Inherited methods
  String GetComponentName() {return "MetaCallable";}
  virtual ~MetaCallableBase();

  // Invokes a method on the object pointed to
  // fn - Name of the function to invoke, it must be registered prior using RegisterFunction
  // self - Pointer to the instance to invoke the function on
  // ... - Up to 4 parameters
  void Invoke(char const *fn, void *self);
  void Invoke(char const *fn, void *self, DynamicValue);
  void Invoke(char const *fn, void *self, DynamicValue, DynamicValue);
  void Invoke(char const *fn, void *self, DynamicValue, DynamicValue, DynamicValue);
  void Invoke(char const *fn, void *self, DynamicValue, DynamicValue, DynamicValue, DynamicValue);

  // Invokes a method on the object pointed to
  // fn - Name of the function to invoke, it must be registered
  // self - Pointer to the instance to invoke the function on
  // ... - Up to 4 parameters with a return
  Variable InvokeRet(char const *fn, void *self);
  Variable InvokeRet(char const *fn, void *self, DynamicValue);
  Variable InvokeRet(char const *fn, void *self, DynamicValue, DynamicValue);
  Variable InvokeRet(char const *fn, void *self, DynamicValue, DynamicValue, DynamicValue);
  Variable InvokeRet(char const *fn, void *self, DynamicValue, DynamicValue, DynamicValue, DynamicValue);

  // Check if a method exists
  bool HasMethod(char const *name);

  // Get information about a particular function
  MemberFnBase* GetMethodInfo(char const *name);

protected:
  // Map of member functions
  HashMap<String, MemberFnBase*> m_functions;
};

template <typename T>
class MetaCallable : public MetaCallableBase
{
public:
  // Registers a member function with the meta data
  // name - Name of the function to register
  // method - Pointer to the member function
  // P0...P3 - Types of the parameters
  void RegisterFunction(char const* className, char const *name, void (T::*method)());
  template <typename P0>
  void RegisterFunction(char const* className, char const *name, void (T::*method)(P0 p0));
  template <typename P0, typename P1>
  void RegisterFunction(char const* className, char const *name, void (T::*method)(P0, P1));
  template <typename P0, typename P1, typename P2>
  void RegisterFunction(char const* className, char const *name, void (T::*method)(P0, P1, P2));
  template <typename P0, typename P1, typename P2, typename P3>
  void RegisterFunction(char const* className, char const *name, void (T::*method)(P0, P1, P2, P3));

  // Registers a member function that has a return type
  // name - Name of the function to register
  // method - Pointer to the member function
  // R - Return type
  // P0...P3 - Parameter types
  template <typename R>
  void RegisterFunction(char const* className, char const *name, R (T::*method)());
  template <typename R, typename P0>
  void RegisterFunction(char const* className, char const *name, R (T::*method)(P0));
  template <typename R, typename P0, typename P1>
  void RegisterFunction(char const* className, char const *name, R (T::*method)(P0, P1));
  template <typename R, typename P0, typename P1, typename P2>
  void RegisterFunction(char const* className, char const *name, R (T::*method)(P0, P1, P2));
  template <typename R, typename P0, typename P1, typename P2, typename P3>
  void RegisterFunction(char const* className, char const *name, R (T::*method)(P0, P1, P2, P3));
};

// RegisterFunction implementation

template <typename T>
void MetaCallable<T>::RegisterFunction(char const* className, char const *name, void (T::*method)())
{
  ErrorIf(m_functions.Find(name) != m_functions.End(), "MetaCallable::RegisterFunction: %s is already registered!", name);
  PyClass::AddFunction(className, name);
  m_functions[name] = new MemberFn00<T>(method);
}

template <typename T>
template <typename P0>
void MetaCallable<T>::RegisterFunction(char const* className, char const *name, void (T::*method)(P0 p0))
{
  ErrorIf(m_functions.Find(name) != m_functions.End(), "MetaCallable::RegisterFunction: %s is already registered!", name);
  PyClass::AddFunction(className, name);
  m_functions[name] = new MemberFn01<T, P0>(method);
}

template <typename T>
template <typename P0, typename P1>
void MetaCallable<T>::RegisterFunction(char const* className, char const *name, void (T::*method)(P0 p0, P1 p1))
{
  ErrorIf(m_functions.Find(name) != m_functions.End(), "MetaCallable::RegisterFunction: %s is already registered!", name);
  PyClass::AddFunction(className, name);
  m_functions[name] = new MemberFn02<T, P0, P1>(method);
}

template <typename T>
template <typename P0, typename P1, typename P2>
void MetaCallable<T>::RegisterFunction(char const* className, char const *name, void (T::*method)(P0 p0, P1 p1, P2 p2))
{
  ErrorIf(m_functions.Find(name) != m_functions.End(), "MetaCallable::RegisterFunction: %s is already registered!", name);
  PyClass::AddFunction(className, name);
  m_functions[name] = new MemberFn03<T, P0, P1, P2>(method);
}

template <typename T>
template <typename P0, typename P1, typename P2, typename P3>
void MetaCallable<T>::RegisterFunction(char const* className, char const *name, void (T::*method)(P0 p0, P1 p1, P2 p2, P3 p3))
{
  ErrorIf(m_functions.Find(name) != m_functions.End(), "MetaCallable::RegisterFunction: %s is already registered!", name);
  PyClass::AddFunction(className, name);
  m_functions[name] = new MemberFn04<T, P0, P1, P2, P3>(method);
}

template <typename T>
template <typename R>
void MetaCallable<T>::RegisterFunction(char const* className, char const *name, R (T::*method)())
{
  ErrorIf(m_functions.Find(name) != m_functions.End(), "MetaCallable::RegisterFunction: %s is already registered!", name);
  PyClass::AddFunction(className, name);
  m_functions[name] = new MemberFnRet00<T, R>(method);
}

template <typename T>
template <typename R, typename P0>
void MetaCallable<T>::RegisterFunction(char const* className, char const *name, R (T::*method)(P0 p0))
{
  ErrorIf(m_functions.Find(name) != m_functions.End(), "MetaCallable::RegisterFunction: %s is already registered!", name);
  PyClass::AddFunction(className, name);
  m_functions[name] = new MemberFnRet01<T, R, P0>(method);
}

template <typename T>
template <typename R, typename P0, typename P1>
void MetaCallable<T>::RegisterFunction(char const* className, char const *name, R (T::*method)(P0 p0, P1 p1))
{
  ErrorIf(m_functions.Find(name) != m_functions.End(), "MetaCallable::RegisterFunction: %s is already registered!", name);
  PyClass::AddFunction(className, name);
  m_functions[name] = new MemberFnRet02<T, R, P0, P1>(method);
}

template <typename T>
template <typename R, typename P0, typename P1, typename P2>
void MetaCallable<T>::RegisterFunction(char const* className, char const *name, R (T::*method)(P0 p0, P1 p1, P2 p2))
{
  ErrorIf(m_functions.Find(name) != m_functions.End(), "MetaCallable::RegisterFunction: %s is already registered!", name);
  PyClass::AddFunction(className, name);
  m_functions[name] = new MemberFnRet03<T, R, P0, P1, P2>(method);
}

template <typename T>
template <typename R, typename P0, typename P1, typename P2, typename P3>
void MetaCallable<T>::RegisterFunction(char const* className, char const *name, R (T::*method)(P0 p0, P1 p1, P2 p2, P3 p3))
{
  ErrorIf(m_functions.Find(name) != m_functions.End(), "MetaCallable::RegisterFunction: %s is already registered!", name);
  PyClass::AddFunction(className, name);
  m_functions[name] = new MemberFnRet04<T, R, P0, P1, P2, P3>(method);
}

}