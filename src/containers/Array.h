// Nicholas Baldwin
// 2011/11/16
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "Allocators.h"

namespace Improbability
{

  namespace ArrayConstants
  {
    const float k_growth_factor = 1.5;
  }

template <typename Type, typename Allocator=Improbability::AllocatorNew<Type>>
class Array
{
public:
////////////////////////////////////////////////////////////////////////////////
////////////////////////Types///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  typedef Type      ValueType;
  typedef Allocator AllocatorType;

////////////////////////////////////////////////////////////////////////////////
////////////////////////Copy/Construct/Destruct/////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  Array(unsigned initial_size = 0);
  Array(Array const &original);
  template <typename ForwardIterator>
  Array(ForwardIterator begin, ForwardIterator end);
  ~Array();

////////////////////////////////////////////////////////////////////////////////
////////////////////////Assignment//////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  Array& operator=(Array const &original);
  template <typename ForwardIterator>
  void Assign(ForwardIterator begin, ForwardIterator end);

////////////////////////////////////////////////////////////////////////////////
////////////////////////Size and Capacity///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  unsigned GetSize() const;
  unsigned GetCapacity() const;
  bool IsEmpty() const;
  void Resize(unsigned new_size, ValueType const &initial_value = ValueType());
  void Reserve(unsigned new_capacity);

////////////////////////////////////////////////////////////////////////////////
////////////////////////Accessors///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  ValueType& operator[](unsigned index);
  ValueType const& operator[](unsigned index) const;
  ValueType& GetElement(unsigned index);
  ValueType const& GetElement(unsigned index) const;
  ValueType& GetFront();
  ValueType const& GetFront() const;
  ValueType& GetBack();
  ValueType const& GetBack() const;

////////////////////////////////////////////////////////////////////////////////
////////////////////////Element Modifiers///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  void PushBack(ValueType const &element);
  void PopBack();
  void Insert(unsigned placement, ValueType const &element);
  void Erase(unsigned element);

////////////////////////////////////////////////////////////////////////////////
////////////////////////Array Modifiers/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  void Swap(Array &rhs);
  void Clear();

////////////////////////////////////////////////////////////////////////////////
////////////////////////Iterators///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
private:
  class ArrayIterator
  {
  public:
    ArrayIterator(ValueType *element = 0) : m_element(element) {}

    //input/output iterator
    ValueType& operator*() {return *m_element;}
    ValueType const& operator*() const {return *m_element;}
    ValueType* operator->() {return m_element;}
    ValueType const* operator->() const {return m_element;}

    //forward iterator
    ArrayIterator& operator++() {++m_element; return *this;}
    ArrayIterator const& operator++() const {++m_element; return *this;}
    ArrayIterator operator++(int) {return ArrayIterator(m_element++);}
    ArrayIterator const operator++(int) const {return ArrayIterator(m_element++);}

    bool operator==(ArrayIterator const &rhs) const {return m_element == rhs.m_element;}
    bool operator!=(ArrayIterator const &rhs) const {return m_element != rhs.m_element;}

    //bidirectional iterator
    ArrayIterator& operator--() {--m_element; return *this;}
    ArrayIterator const& operator--() const {--m_element; return *this;}
    ArrayIterator operator--(int) {return ArrayIterator(m_element--);}
    ArrayIterator const operator--(int) const {return ArrayIterator(m_element--);}

    //random access iterator
    ArrayIterator operator+(int offset) {return ArrayIterator(m_element + offset);}
    ArrayIterator operator-(int offset) {return ArrayIterator(m_element - offset);}
    unsigned operator-(ArrayIterator const &rhs) const {return m_element - rhs.m_element;}

    bool operator<(ArrayIterator const &rhs) const {return m_element < rhs.m_element;}
    bool operator>(ArrayIterator const &rhs) const {return m_element > rhs.m_element;}
    bool operator<=(ArrayIterator const &rhs) const {return m_element <= rhs.m_element;}
    bool operator>=(ArrayIterator const &rhs) const {return m_element >= rhs.m_element;}

    ArrayIterator& operator+=(int offset) {m_element += offset; return *this;}
    ArrayIterator& operator-=(int offset) {m_element -= offset; return *this;}

    ValueType& operator[](int offset) {return m_element[offset];}

  private:
    mutable ValueType *m_element;
  };

public:

  typedef ArrayIterator       Iterator;
  typedef ArrayIterator const ConstIterator;

  Iterator Begin();
  ConstIterator Begin() const;
  Iterator End();
  ConstIterator End() const;

////////////////////////////////////////////////////////////////////////////////
////////////////////////Data////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
private:
  ValueType* m_first;
  AllocatorType m_allocator;
  unsigned m_size;
  unsigned m_capacity;

////////////////////////////////////////////////////////////////////////////////
////////////////////////Helper Functions////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  void Grow(unsigned new_capacity);
};

#include "Array.hpp"
}