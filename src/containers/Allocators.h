// Nicholas Baldwin
// 2011/11/16
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

template <typename Type>
class AllocatorNew
{
public:
  Type* Allocate()
  {
    return new Type();
  }

  void Deallocate(Type* ptr)
  {
    delete ptr;
  }

  Type* AllocateArray(unsigned num_elements)
  {
    return new Type[num_elements];
  }

  void DeallocateArray(Type* ptr)
  {
    delete [] ptr;
  }
};

template <typename Type>
class AllocatorMalloc
{
public:
  Type* Allocate()
  {
    return reinterpret_cast<Type*>(malloc(sizeof(Type)));
  }

  void Deallocate(Type* ptr)
  {
    free(ptr);
  }

  Type* AllocateArray(unsigned num_elements)
  {
    return reinterpret_cast<Type*>(malloc(sizeof(Type) * num_elements));
  }

  void DeallocateArray(Type* ptr)
  {
    free(ptr);
  }
};

};