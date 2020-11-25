// David Evans
// 2011/10/02
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "MetaCallable.h"

namespace Improbability
{

MetaCallableBase::~MetaCallableBase()
{
  for (auto it = m_functions.Begin(); it != m_functions.End(); ++it)
  {
    delete it->second;
  }
}

void MetaCallableBase::Invoke(char const *fn, void *self)
{
  auto it = m_functions.Find(fn);
  ErrorIf(it == m_functions.End(), "Couldn't find function %s!", fn);
  it->second->Invoke(self);
}

void MetaCallableBase::Invoke(char const *fn, void *self, DynamicValue p0)
{
  auto it = m_functions.Find(fn);
  ErrorIf(it == m_functions.End(), "Couldn't find function %s!", fn);
  it->second->Invoke(self, p0);
}

void MetaCallableBase::Invoke(char const *fn, void *self, DynamicValue p0, DynamicValue p1)
{
  auto it = m_functions.Find(fn);
  ErrorIf(it == m_functions.End(), "Couldn't find function %s!", fn);
  it->second->Invoke(self, p0, p1);
}

void MetaCallableBase::Invoke(char const *fn, void *self, DynamicValue p0, DynamicValue p1, DynamicValue p2)
{
  auto it = m_functions.Find(fn);
  ErrorIf(it == m_functions.End(), "Couldn't find function %s!", fn);
  it->second->Invoke(self, p0, p1, p2);
}

void MetaCallableBase::Invoke(char const *fn, void *self, DynamicValue p0, DynamicValue p1, DynamicValue p2, DynamicValue p3)
{
  auto it = m_functions.Find(fn);
  ErrorIf(it == m_functions.End(), "Couldn't find function %s!", fn);
  it->second->Invoke(self, p0, p1, p2, p3);
}

Variable MetaCallableBase::InvokeRet(char const *fn, void *self)
{
  auto it = m_functions.Find(fn);
  ErrorIf(it == m_functions.End(), "Couldn't find function %s!", fn);
  if (it->second->GetReturnType() == "void")
  {
    it->second->Invoke(self);
    return 0;
  }
  return it->second->InvokeRet(self);
}

Variable MetaCallableBase::InvokeRet(char const *fn, void *self, DynamicValue p0)
{
  auto it = m_functions.Find(fn);
  ErrorIf(it == m_functions.End(), "Couldn't find function %s!", fn);
  if (it->second->GetReturnType() == "void")
  {
    it->second->Invoke(self, p0);
    return 0;
  }
  return it->second->InvokeRet(self, p0);
}

Variable MetaCallableBase::InvokeRet(char const *fn, void *self, DynamicValue p0, DynamicValue p1)
{
  auto it = m_functions.Find(fn);
  ErrorIf(it == m_functions.End(), "Couldn't find function %s!", fn);
  if (it->second->GetReturnType() == "void")
  {
    it->second->Invoke(self, p0, p1);
    return 0;
  }
  return it->second->InvokeRet(self, p0, p1);
}

Variable MetaCallableBase::InvokeRet(char const *fn, void *self, DynamicValue p0, DynamicValue p1, DynamicValue p2)
{
  auto it = m_functions.Find(fn);
  ErrorIf(it == m_functions.End(), "Couldn't find function %s!", fn);
  if (it->second->GetReturnType() == "void")
  {
    it->second->Invoke(self, p0, p1, p2);
    return 0;
  }
  return it->second->InvokeRet(self, p0, p1, p2);
}

Variable MetaCallableBase::InvokeRet(char const *fn, void *self, DynamicValue p0, DynamicValue p1, DynamicValue p2, DynamicValue p3)
{
  auto it = m_functions.Find(fn);
  ErrorIf(it == m_functions.End(), "Couldn't find function %s!", fn);
  if (it->second->GetReturnType() == "void")
  {
    it->second->Invoke(self, p0, p1, p2, p3);
    return 0;
  }
  return it->second->InvokeRet(self, p0, p1, p2, p3);
}

bool MetaCallableBase::HasMethod(char const *name)
{
  auto it = m_functions.Find(name);
  return it != m_functions.End();
}

MemberFnBase* MetaCallableBase::GetMethodInfo(char const *name)
{
  auto it = m_functions.Find(name);
  if (it == m_functions.End())
    return 0;

  return it->second;
}


}