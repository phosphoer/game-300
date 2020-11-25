// David Evans
// 2011/10/01
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "MetaComposition.h"
#include "MetaNewable.h"
#include "MetaCallable.h"
#include "MetaFieldable.h"

#include "../Containers/String.h"
#include "../Containers/HashMap.h"

namespace Improbability
{

// The Meta allows for working with the global meta data
class Meta
{
public:
  // Map of class names to their meta data
  typedef HashMap<String, MetaComposition*> CompositionMap;
  typedef HashMap<String, MetaConstantBase*> ConstantMap;

public:
  // Allocate an object based on class name
  // name - Name of the class to allocate, must be registered with meta
  // T - Type to cast the return value to, usually the same type or a base class
  // return - Pointer to the allocated object
  template <typename T>
  static T* Create(char const *name);

  // Allocate an object based on class name using the copy constructor
  // name - Name of the class to allocate, must be registered with meta
  // rhs - Object to copy construct from
  // return - Pointer to the allocated object
  static void* Create(char const *name, void *rhs);

  // Destroy an object based on class name
  // name - Name of the class to de-allocate
  // obj - Pointer to the object to de-allocate
  static void Destroy(char const *name, void *obj);

  // Get the value of a constant registered with meta
  // define_name - Name of the constant to get the value of
  // T - Type of the value
  // return - The value of the constant
  template <typename T>
  static T* GetConstantValue(char const *define_name);

  // Get meta information about a class from its name
  // name - Name of the class to get meta for
  // return - Meta information about the class
  static MetaComposition* GetMeta(char const *name);
  
  // Get meta information about a class from its type
  // T - Type of the class
  template <typename T>
  static MetaComposition* GetMeta();

  // Get a reference to a member variable of an instance
  // field - Name of the field
  // obj - Pointer to the instance
  // MemberType - Type of the field
  // ClassType - Type of the containing class
  template <typename MemberType, typename ClassType>
  static MemberType& GetVariable(char const *field, ClassType *obj);

  // Get a reference to a member variable using the string name of a class
  // field - Name of the field
  // class_name - Name of the class
  // obj - Pointer to the object
  // MemberType - Type of the field
  template <typename MemberType>
  static MemberType& GetVariableByClassName(char const *field, char const *class_name, void *obj);

  // Get the meta composition of a member variable
  // type - Type of the containing class
  // field - Name of the field
  static MetaComposition* GetVariableMeta(char const *type, char const *field);

  // Get whether or not a class has a function registered
  // type - String name of the class to check
  // fn - String name of the function to look for
  static bool HasMethod(char const *type, char const *fn);

  // Invoke a member function on a class, assuming the class and function are registered with meta
  // type - Type name of the class possessing the member function
  // fn - Name of the function to call
  // obj - Pointer to the instance to invoke the function on
  // p0...p3 - Parameters to the member function
  // P0...P3 - Type of each parameter
  static void Invoke(char const *type, char const *fn, void *obj);
  template <typename P0>
  static void Invoke(char const *type, char const *fn, void *obj, P0 p0);
  template <typename P0, typename P1>
  static void Invoke(char const *type, char const *fn, void *obj, P0 p0, P1 p1);
  template <typename P0, typename P1, typename P2>
  static void Invoke(char const *type, char const *fn, void *obj, P0 p0, P1 p1, P2 p2);
  template <typename P0, typename P1, typename P2, typename P3>
  static void Invoke(char const *type, char const *fn, void *obj, P0 p0, P1 p1, P2 p2, P3 p3);

  // Invoke a member function on a class, assuming the class and function are registered with meta
  // type - Type name of the class possessing the member function
  // fn - Name of the function to call
  // obj - Pointer to the instance to invoke the function on
  // p0...p3 - Parameters to the member function
  // P0...P3 - Type of each parameter
  // return - Variable containing the return value from the function.
  // IMPORTANT - If the type returned does not have a MetaNewable, it will
  // allocate it as a POD type (the constructor / destructor will NOT be called)
  // MAKE SURE any types you plan to return from a function with meta either have
  // a MetaNewable, or don't need a constructor / destructor to be properly allocated.
  static Variable InvokeRet(char const *type, char const *fn, void *obj);
  template <typename P0>
  static Variable InvokeRet(char const *type, char const *fn, void *obj, P0 p0);
  template <typename P0, typename P1>
  static Variable InvokeRet(char const *type, char const *fn, void *obj, P0 p0, P1 p1);
  template <typename P0, typename P1, typename P2>
  static Variable InvokeRet(char const *type, char const *fn, void *obj, P0 p0, P1 p1, P2 p2);
  template <typename P0, typename P1, typename P2, typename P3>
  static Variable InvokeRet(char const *type, char const *fn, void *obj, P0 p0, P1 p1, P2 p2, P3 p3);

  // Initialize the meta system
  // This *must* be done before using any functionality of the meta system
  static void Initialize();

  // Uninitialize the meta system
  // Will clean up any memory it has allocated
  static void Uninitialize();

private:
  // The map of names to meta data
  static CompositionMap s_compositions;
  static ConstantMap s_constants;
};

template <typename T>
T* Meta::Create(char const *name)
{
  String strname(name);
  if (strname.Find("*") != String::k_end)
    return (T*)(new int*(0));

  auto it = s_compositions.Find(name);
  ErrorIf(it == s_compositions.End(), "The type %s is not registered with the Factory!", name);
  MetaComposition *comp = it->second;
  T *object = (T*)(comp->MetaGetComponentBase(MetaNewable)->New());
  return object;
}

template <typename T>
T* Meta::GetConstantValue(char const *define_name)
{
  auto it = s_constants.Find(define_name);
  ErrorIf(it == s_constants.End(), "The constant %s is not registered with meta. (Did you force link it?)", define_name);
  return reinterpret_cast<T*>(it->second->GetValue());
}

template <typename T>
MetaComposition* Meta::GetMeta()
{
  return MetaGetter<T>::s_meta;
}

template <typename MemberType, typename ClassType>
MemberType& Meta::GetVariable(char const *field, ClassType *obj)
{
  MetaComposition *composition = GetMeta<ClassType>();
  MetaFieldableBase *fieldable = composition->MetaGetComponentBase(MetaFieldable);
  ErrorIf(!fieldable, "Meta::GetVariable: %s does not have MetaFieldable!", composition->m_name);
  void *var = fieldable->GetVariable(field, obj);
  return *(reinterpret_cast<MemberType*>(var));
}

template <typename MemberType>
MemberType& Meta::GetVariableByClassName(char const *field, char const *class_name, void *obj)
{
  MetaComposition *composition = GetMeta(class_name);
  MetaFieldableBase *fieldable = composition->MetaGetComponentBase(MetaFieldable);
  ErrorIf(!fieldable, "Meta::GetVariable: %s does not have MetaFieldable!", composition->m_name);
  void *var = fieldable->GetVariable(field, obj);
  return *(reinterpret_cast<MemberType*>(var));
}

template <typename P0>
void Meta::Invoke(char const *type, char const *fn, void *obj, P0 p0)
{
  auto it = s_compositions.Find(type);
  ErrorIf(it == s_compositions.End(), "The type %s is not registered with meta!", type);
  MetaComposition *comp = it->second;
  comp->MetaGetComponentBase(MetaCallable)->Invoke(fn, obj, p0);
}

template <typename P0, typename P1>
void Meta::Invoke(char const *type, char const *fn, void *obj, P0 p0, P1 p1)
{
  auto it = s_compositions.Find(type);
  ErrorIf(it == s_compositions.End(), "The type %s is not registered with meta!", type);
  MetaComposition *comp = it->second;
  comp->MetaGetComponentBase(MetaCallable)->Invoke(fn, obj, p0, p1);
}

template <typename P0, typename P1, typename P2>
void Meta::Invoke(char const *type, char const *fn, void *obj, P0 p0, P1 p1, P2 p2)
{
  auto it = s_compositions.Find(type);
  ErrorIf(it == s_compositions.End(), "The type %s is not registered with meta!", type);
  MetaComposition *comp = it->second;
  comp->MetaGetComponentBase(MetaCallable)->Invoke(fn, obj, p0, p1, p2);
}

template <typename P0, typename P1, typename P2, typename P3>
void Meta::Invoke(char const *type, char const *fn, void *obj, P0 p0, P1 p1, P2 p2, P3 p3)
{
  auto it = s_compositions.Find(type);
  ErrorIf(it == s_compositions.End(), "The type %s is not registered with meta!", type);
  MetaComposition *comp = it->second;
  comp->MetaGetComponentBase(MetaCallable)->Invoke(fn, obj, p0, p1, p2, p3);
}

template <typename P0>
Variable Meta::InvokeRet(char const *type, char const *fn, void *obj, P0 p0)
{
  auto it = s_compositions.Find(type);
  ErrorIf(it == s_compositions.End(), "The type %s is not registered with meta!", type);
  MetaComposition *comp = it->second;
  return comp->MetaGetComponentBase(MetaCallable)->InvokeRet(fn, obj, p0);
}

template <typename P0, typename P1>
Variable Meta::InvokeRet(char const *type, char const *fn, void *obj, P0 p0, P1 p1)
{
  auto it = s_compositions.Find(type);
  ErrorIf(it == s_compositions.End(), "The type %s is not registered with meta!", type);
  MetaComposition *comp = it->second;
  return comp->MetaGetComponentBase(MetaCallable)->InvokeRet(fn, obj, p0, p1);
}

template <typename P0, typename P1, typename P2>
Variable Meta::InvokeRet(char const *type, char const *fn, void *obj, P0 p0, P1 p1, P2 p2)
{
  auto it = s_compositions.Find(type);
  ErrorIf(it == s_compositions.End(), "The type %s is not registered with meta!", type);
  MetaComposition *comp = it->second;
  return comp->MetaGetComponentBase(MetaCallable)->InvokeRet(fn, obj, p0, p1, p2);
}

template <typename P0, typename P1, typename P2, typename P3>
Variable Meta::InvokeRet(char const *type, char const *fn, void *obj, P0 p0, P1 p1, P2 p2, P3 p3)
{
  auto it = s_compositions.Find(type);
  ErrorIf(it == s_compositions.End(), "The type %s is not registered with meta!", type);
  MetaComposition *comp = it->second;
  return comp->MetaGetComponentBase(MetaCallable)->InvokeRet(fn, obj, p0, p1, p2, p3);
}

}