// Nicholas Baldwin
// 2011/11/16
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

template <typename Type, typename Allocator>
  Improbability::Array<Type, Allocator>::Array(unsigned initial_size)
{
  m_size = initial_size;
  m_capacity = initial_size;

  m_first = m_allocator.AllocateArray(initial_size);
}

template <typename Type, typename Allocator>
Improbability::Array<Type, Allocator>::Array(Array const &original)
{
  m_size = original.m_size;
  m_capacity = original.m_capacity;


  m_first = m_allocator.AllocateArray(m_capacity);

  for (unsigned i = 0; i < m_size; ++i)
  {
    m_first[i] = original.m_first[i];
  }
}

template <typename Type, typename Allocator>
template <typename ForwardIterator>
Improbability::Array<Type, Allocator>::Array(ForwardIterator begin, ForwardIterator end)
{
  m_size = 0;
  m_capacity = 0;

  m_first = m_allocator.AllocateArray(0);

  for (; begin != end; ++begin)
  {
    PushBack(*begin);
  }
}

template <typename Type, typename Allocator>
Improbability::Array<Type, Allocator>::~Array()
{
  m_allocator.DeallocateArray(m_first);
}

template <typename Type, typename Allocator>
Improbability::Array<Type, Allocator>& Improbability::Array<Type, Allocator>::operator=(Array const &original)
{
  m_size = original.m_size;
  m_capacity = original.m_capacity;

  m_allocator.DeallocateArray(m_first);

  m_first = m_allocator.AllocateArray(m_capacity);
  
  for (unsigned i = 0; i < m_size; ++i)
  {
    m_first[i] = original.m_first[i];
  }

  return *this;
}

template <typename Type, typename Allocator>
template <typename ForwardIterator>
void Improbability::Array<Type, Allocator>::Assign(ForwardIterator begin, ForwardIterator end)
{  
  m_allocator.DeallocateArray(m_first);

  m_size = 0;
  m_capacity = 0;

  m_first = m_allocator.AllocateArray(0);

  for (; begin != end; ++begin)
  {
    PushBack(*begin);
  }
}

template <typename Type, typename Allocator>
unsigned Improbability::Array<Type, Allocator>::GetSize() const
{
  return m_size;
}

template <typename Type, typename Allocator>
unsigned Improbability::Array<Type, Allocator>::GetCapacity() const
{
  return m_capacity;
}

template <typename Type, typename Allocator>
bool Improbability::Array<Type, Allocator>::IsEmpty() const
{
  return m_size == 0;
}

template <typename Type, typename Allocator>
void Improbability::Array<Type, Allocator>::Resize(unsigned new_size, ValueType const &initial_value)
{
  if (new_size > m_capacity)
    Grow(new_size);

  for (unsigned i = m_size; i < new_size; ++i)
  {
    m_first[i] = initial_value;
  }

  m_size = new_size;
}

template <typename Type, typename Allocator>
void Improbability::Array<Type, Allocator>::Reserve(unsigned new_capacity)
{
  if (new_capacity > m_capacity)
    Grow(new_capacity);
}

template <typename Type, typename Allocator>
typename Improbability::Array<Type, Allocator>::ValueType& 
Improbability::Array<Type, Allocator>::operator[](unsigned index)
{
#ifdef _DEBUG
  ErrorIf(index >= m_size, "index is out of range");
#endif //_DEBUG

  return m_first[index];
}

template <typename Type, typename Allocator>
typename Improbability::Array<Type, Allocator>::ValueType const&
Improbability::Array<Type, Allocator>::operator[](unsigned index) const
{
#ifdef _DEBUG
  ErrorIf(index >= m_size, "index is out of range");
#endif //_DEBUG

  return m_first[index];
}

template <typename Type, typename Allocator>
typename Improbability::Array<Type, Allocator>::ValueType& 
Improbability::Array<Type, Allocator>::GetElement(unsigned index)
{
#ifdef _DEBUG
  ErrorIf(index >= m_size, "index is out of range");
#endif //_DEBUG

  return m_first[index];
}

template <typename Type, typename Allocator>
typename Improbability::Array<Type, Allocator>::ValueType const&
Improbability::Array<Type, Allocator>::GetElement(unsigned index) const
{
#ifdef _DEBUG
  ErrorIf(index >= m_size, "index is out of range");
#endif //_DEBUG

  return m_first[index];
}

template <typename Type, typename Allocator>
typename Improbability::Array<Type, Allocator>::ValueType& 
Improbability::Array<Type, Allocator>::GetFront()
{
  return m_first[0];
}

template <typename Type, typename Allocator>
typename Improbability::Array<Type, Allocator>::ValueType const&
Improbability::Array<Type, Allocator>::GetFront() const
{
  return m_first[0];
}

template <typename Type, typename Allocator>
typename Improbability::Array<Type, Allocator>::ValueType& 
Improbability::Array<Type, Allocator>::GetBack()
{
  return m_first[m_size - 1];
}

template <typename Type, typename Allocator>
typename Improbability::Array<Type, Allocator>::ValueType const&
Improbability::Array<Type, Allocator>::GetBack() const
{
  return m_first[m_size - 1];
}

template <typename Type, typename Allocator>
void Improbability::Array<Type, Allocator>::PushBack(ValueType const &element)
{
  if(m_size + 1 > m_capacity)
    Grow(static_cast<unsigned>((m_size + 1) * ArrayConstants::k_growth_factor));

  m_first[m_size] = element;
  ++m_size;
}

template <typename Type, typename Allocator>
void Improbability::Array<Type, Allocator>::PopBack()
{
  --m_size;
}

template <typename Type, typename Allocator>
void Improbability::Array<Type, Allocator>::Insert(unsigned placement, ValueType const &element)
{
  if (m_size + 1 > m_capacity)
    Grow(static_cast<unsigned>((m_size + 1) * ArrayConstants::k_growth_factor));

  for (unsigned i = m_size; i >= placement; --i)
  {
    m_first[i] = m_first[i - 1];
  }
  ++m_size;

  m_first[placement] = element;
}

template <typename Type, typename Allocator>
void Improbability::Array<Type, Allocator>::Erase(unsigned element)
{
  for (unsigned i = element; i < m_size; ++i)
  {
    m_first[i] = m_first[i + 1];
  }
}

template <typename Type, typename Allocator>
void Improbability::Array<Type, Allocator>::Swap(Array &rhs)
{
  ValueType *temp_pointer = m_first;
  m_first = rhs.m_first;
  rhs.m_first = temp_pointer;

  unsigned temp_number = m_size;
  m_size = rhs.m_size;
  rhs.m_size = temp_number;

  temp_number = m_capacity;
  m_capacity = rhs.m_capacity;
  rhs.m_capacity = temp_number;
}

template <typename Type, typename Allocator>
  void Improbability::Array<Type, Allocator>::Clear()
{
  m_size = 0;
}

template <typename Type, typename Allocator>
typename Improbability::Array<Type, Allocator>::Iterator
Improbability::Array<Type, Allocator>::Begin()
{
  return Iterator(m_first);
}

template <typename Type, typename Allocator>
typename Improbability::Array<Type, Allocator>::Iterator const
Improbability::Array<Type, Allocator>::Begin() const
{
  return ConstIterator(m_first + m_size);
}

template <typename Type, typename Allocator>
typename Improbability::Array<Type, Allocator>::Iterator
Improbability::Array<Type, Allocator>::End()
{
  return Iterator(m_first + m_size);
}

template <typename Type, typename Allocator>
typename Improbability::Array<Type, Allocator>::Iterator const
Improbability::Array<Type, Allocator>::End() const
{
  return ConstIterator(m_first + m_size);
}

template <typename Type, typename Allocator>
void Improbability::Array<Type, Allocator>::Grow(unsigned new_capacity)
{
  ValueType *old_array = m_first;

  m_first = m_allocator.AllocateArray(new_capacity);

  for (unsigned i = 0; i < m_size; ++i)
  {
    m_first[i] = old_array[i];
  }

  m_capacity = new_capacity;

  m_allocator.DeallocateArray(old_array);
}
