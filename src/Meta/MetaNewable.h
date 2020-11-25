// David Evans
// 2011/10/01
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "MetaComponent.h"

namespace Improbability
{

class MetaNewableBase : public MetaComponent
{
public:
  // Allocate a new instance of the meta type
  virtual void* New() = 0;
  virtual void* New(void *rhs) = 0;

  // De-allocate an instanc eof the meta type
  virtual void Delete(void *obj) = 0;

  // Inherited methods
  String GetComponentName() {return "MetaNewable";}
};

template <typename T>
class MetaNewable : public MetaNewableBase
{
public:
  // Get size from template
  MetaNewable(): m_size(sizeof(T)) {}

  // Allocate a new instance of the meta type
  void* New() {return new T();}
  void* New(void *rhs) {return new T(*((T*)rhs));}

  // De-allocate an instance of the meta type
  void Delete(void *obj) {delete reinterpret_cast<T*>(obj);} 

  // Get the size
  int GetSize() {return m_size;}

private:
  // Size of the object to new
  int m_size;
};

}