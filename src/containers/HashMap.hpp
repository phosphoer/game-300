// Nicholas Baldwin
// 2011/06/10
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

//constructor, allows for specified initial capacity and max load factor
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMap(unsigned initial_capacity, float max_load_factor)
{
    //initialize the array of hash elements and create the dummy end
  m_head = new HashElement[initial_capacity + 1];
    //set the free node to the last true element
  m_free = m_head + initial_capacity - 1;
    //set the dummy end's next pointer to an incorrect value that is not 0
  m_head[initial_capacity].m_next = m_head;

  m_size = initial_capacity;
  m_occupied = 0;
  m_max_load_factor = max_load_factor;
}

//destructor
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::~HashMap()
{
  Clear();

  delete [] m_head;
}

//copy constructor
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMap(const HashMap& origin)
{
    //create the new buffer of hash elements and point the free pointer correctly
  m_head = new HashElement[origin.m_size + 1];
  m_free = m_head + origin.m_size - 1;
  m_head[origin.m_size].m_next = m_head;

  m_max_load_factor = origin.m_max_load_factor;
  m_size = origin.m_size;
  m_occupied = 0;

    //iterate over the origin and reinsert all of its stored elements
  Iterator curr_node = origin.Begin();
  Iterator end_node = origin.End();
  for (; curr_node != end_node; curr_node++)
  {
    Insert(curr_node->first, curr_node->second);
  }
}

//assignment operator
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
Improbability::HashMap<Key, Mapped, Hasher, Predicate>&
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::operator=(const HashMap& origin)
{
    //clear and delete the old map
  Clear();
  delete [] m_head;

    //create the new buffer of hash elements and point the free pointer correctly
  m_head = new HashElement[origin.m_size + 1];
  m_free = m_head + origin.m_size - 1;
  m_head[origin.m_size].m_next = m_head;
  
  m_max_load_factor = origin.m_max_load_factor;
  m_size = origin.m_size;
  m_occupied = 0;

    //iterate over the origin and reinsert all of its stored elements
  Iterator curr_node = origin.Begin();
  Iterator end_node = origin.End();
  for (; curr_node != end_node; curr_node++)
  {
    Insert(curr_node->first, curr_node->second);
  }

  return *this;
}

//returns true if empty
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
bool Improbability::HashMap<Key, Mapped, Hasher, Predicate>::IsEmpty() const
{
  return m_occupied == 0;
}

//returns number of elements in map
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
unsigned Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Size() const
{
  return m_occupied;
}

//returns number of elements map can store
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
unsigned Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Capacity() const
{
  return m_size;
}

template<typename Key, typename Mapped, typename Hasher,
typename Predicate>
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashElement::HashElement()
{
  m_next = 0;
}

//does most of the work, hashes the value and finds either where it exists or where it belongs
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
unsigned Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Probe(KeyType const& key, ReturnCode &return_code)
{
  //the hash value for the key, double parenthesis to construct the hasher and use it
  unsigned hash = Hasher()(key, m_size);

  Predicate equals;

  //check if the first element
  if (m_head[hash].m_next)
  {
      //check if the first element matches the key
    if (equals(key, m_head[hash].m_value.first))
    {
      return_code = RC_Found;
      return hash;
    }

    HashElement *first_element = m_head + hash;
    HashElement *curr_element = first_element;

    while(curr_element->m_next != first_element)
    {
      curr_element = curr_element->m_next;

      if (equals(key, curr_element->m_value.first))
      {
        return_code = RC_Found;
        return curr_element - m_head;
      }
    }

    if (return_code == RC_DontCreate)
    {
      return_code = RC_NotFound;
      return m_size;
    }

    // 'create' the new 'node'
    HashElement *temp = curr_element->m_next;
    curr_element->m_next = m_free;
    m_free->m_next = temp;
    m_free->m_value.first = key;
    m_free->m_is_in_correct_spot = false;
    ++m_occupied;

    //find the next free 
    while(m_free->m_next && m_free >= m_head)
      m_free--;

    //if there are none free manually grow
    if (m_free < m_head)
    {
      Grow(m_size * 2);
      //need to reset the curr element after this...
      return Probe(key, return_code);
    }
    
    return_code = RC_Probed;
    return curr_element->m_next - m_head;
  }

  if (return_code == RC_DontCreate)
  {
    return_code = RC_NotFound;
    return m_size;
  }

  //add in the element
  m_head[hash].m_next = m_head + hash;
  m_head[hash].m_value.first = key;
  m_head[hash].m_is_in_correct_spot = true;
  ++m_occupied;

  //if the hashed element is the free node we must move it
  while(m_free->m_next && m_free >= m_head)
    m_free--;

  return_code = RC_Perfect;
  return hash;
}

//grows the table, calls insert on each of the old values
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
void Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Grow(unsigned new_size)
{
  if (new_size <= m_size)
  {
    return;
  }
  
  HashElement* old_head = m_head;
  Iterator old_element = Begin();
  ConstIterator old_end = End();

  // the plus one is for the dummy node
  m_head = new HashElement[new_size + 1]; 
  m_free = m_head + new_size - 1;
  m_head[new_size].m_next = m_head;
  m_size = new_size;
  m_occupied = 0;

  
  for (; old_element != old_end; ++old_element)
  {
    Insert(old_element->first, old_element->second);
  }

  delete [] old_head;
}

//Iterator constructor
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator::HashMapIterator(HashElement *element)
{
  m_element = element;
  while (m_element->m_next == 0)
  {
    ++m_element;
  }
}

//Iterator Dereference
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename Improbability::HashMap<Key, Mapped, Hasher, Predicate>::ValueType& 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator::operator*()
{
  return *reinterpret_cast<ValueType*>(&(m_element->m_value));
}

//ConstIterator Dereference
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename Improbability::HashMap<Key, Mapped, Hasher, Predicate>::ValueType& 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator::operator*() const
{
  return *reinterpret_cast<ValueType const*>(&(m_element->m_value));
}

//Iterator Arrow
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename Improbability::HashMap<Key, Mapped, Hasher, Predicate>::ValueType*
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator::operator->()
{
  return reinterpret_cast<ValueType*>(&(m_element->m_value));
}

//ConstIterator Arrow
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename Improbability::HashMap<Key, Mapped, Hasher, Predicate>::ValueType* 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator::operator->() const
{
  return reinterpret_cast<ValueType const*>(&(m_element->m_value));
}

//Iterator Pre-Increment
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator& 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator::operator++()
{
  ++m_element;
  while (m_element->m_next == 0)
  {
    ++m_element;
  }
  return *this;
}

//Iterator Post-Increment
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator::operator++(int)
{
  HashMapIterator new_it(m_element);
  ++(*this);
  return new_it;
}

//Iterator Pre-Increment
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename const Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator& 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator::operator++() const
{
  ++m_element;
  while (m_element->m_next == 0)
  {
    ++m_element;
  }
  return *this;
}

//Iterator Post-Increment
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename const Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator::operator++(int) const
{
  const HashMapIterator new_it(m_element);
  ++(*this);
  return new_it;
}

//Iterator Equality
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
bool Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator::operator==(HashMapIterator const& rhs) const
{
  return (m_element == rhs.m_element);
}

//Iterator Inequality
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
bool Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator::operator!=(HashMapIterator const& rhs) const
{
  return (m_element != rhs.m_element);
}

//Iterator Pre-Decrement
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator& 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator::operator--()
{
  --m_element;
  while (m_element->m_next == 0)
  {
    --m_element;
  }
  return *this;
}

//Iterator Post-Decrement
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator::operator--(int)
{
  HashMapIterator new_it(m_element);
  --(*this);
  return new_it;
}

//ConstIterator Pre-Decrement
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename const Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator& 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator::operator--() const
{
  --m_element;
  while (m_element->m_next == 0)
  {
    --m_element;
  }
  return *this;
}

//ConstIterator Post-Decrement
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename const Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::HashMapIterator::operator--(int) const
{
  const HashMapIterator new_it(m_element);
  --(*this);
  return new_it;
}

//returns begin iterator
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Iterator 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Begin()
{
  HashElement* first_occupied = m_head;
  while (first_occupied->m_next == 0 && first_occupied != m_head + m_size)
  {
    ++first_occupied;
  }
  return Iterator(first_occupied);
}

//returns const begin iterator
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename const Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Iterator 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Begin() const
{
  HashElement* first_occupied = m_head;
  while (first_occupied->m_next == 0 && first_occupied != m_head + m_size)
  {
    ++first_occupied;
  }
  return ConstIterator(first_occupied);
}

//returns end iterator
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Iterator 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::End()
{
  return Iterator(m_head + m_size);
}

//returns const end iterator
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename const Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Iterator 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::End() const
{
  return ConstIterator(m_head + m_size);
}

//inserts a new value and returns the iterator it inserted at
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Iterator 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Insert(KeyType const& new_key, MappedType const& new_mapped)
{
  if (static_cast<float>(m_occupied + 1) / static_cast<float>(m_size) > m_max_load_factor)
    Grow(static_cast<unsigned>(m_size * HashMapConstants::k_growth_factor));
  
  ReturnCode rc = static_cast<ReturnCode>(0);
  unsigned element = Probe(new_key, rc);

  if (rc != RC_Perfect && rc != RC_Probed)
  {
    return End();
  }

  m_head[element].m_value.second = new_mapped;

  return Iterator(m_head + element);
}

//erases element by a key
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
void Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Erase(KeyType const& key_to_erase)
{
  ReturnCode rc = RC_DontCreate;
  unsigned element = Probe(key_to_erase, rc);

  if (rc == RC_NotFound)
  {
    return;
  }

  // if there are other elements in the bucket
  if (m_head[element].m_next != m_head + element)
  {
    HashElement *deleted_element = m_head + element;
    HashElement *curr_element = deleted_element;
    HashElement *reinserted_elements = 0;

    // queue objects that need to be reinserted
    while (curr_element->m_next != deleted_element)
    {
      // if the next object is not in its correct spot queue it to be reinserted
      if (!curr_element->m_next->m_is_in_correct_spot)
      {
        HashElement *next_to_reinsert = curr_element->m_next;

        curr_element->m_next = curr_element->m_next->m_next;

        next_to_reinsert->m_next = reinserted_elements;
        reinserted_elements = next_to_reinsert;
      }
      // otherwise just move the current element to it
      else
      {
        curr_element = curr_element->m_next;
      }
    }

    // need to actually delete the element and move the next pointer of the 
    // previous node
    curr_element->m_next = curr_element->m_next->m_next;
    --m_occupied;
    m_head[element].m_next = 0;

    // reinsert objects
    while (reinserted_elements)
    {
      // need to 0 out next before reinserting
      HashElement *next_element = reinserted_elements->m_next;
      reinserted_elements->m_next = 0;

      // now reinserted_element counts as being deleted so update occupied and the free pointer
      --m_occupied;
      if (reinserted_elements > m_free)
      {
        m_free = reinserted_elements;
      }

      Insert(reinserted_elements->m_value.first, reinserted_elements->m_value.second);

      reinserted_elements = next_element;
    }


  }
  else
  {
    --m_occupied;
    m_head[element].m_next = 0;
  }

  return;
}

//clears the map
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
void Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Clear()
{
  for (unsigned i = 0; i < m_size ; i++)
  {
    m_head[i].m_next = 0;
  }
}

//swaps this map with other map
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
void Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Swap(HashMap& other_map)
{
  HashElement* temp_element = m_head;
  m_head = other_map.m_head;
  other_map.m_head = temp_element;

  temp_element = m_free;
  m_free = other_map.m_free;
  other_map.m_free = temp_element;

  unsigned temp_num = m_size;
  m_size = other_map.m_size;
  other_map.m_size = temp_num;

  temp_num = m_occupied;
  m_occupied = other_map.m_occupied;
  other_map.m_occupied = temp_num;

  float temp_factor = m_max_load_factor;
  m_max_load_factor = other_map.m_max_load_factor;
  other_map.m_max_load_factor = temp_factor;
}

//finds an element by key and returns an iterator to it
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Iterator 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Find(KeyType const& key)
{
  ReturnCode rc = RC_DontCreate;
  unsigned place = Probe(key, rc);

  if (rc == RC_NotFound)
  {
    return End();
  }

  return Iterator(m_head + place);
}

//finds an element by key and returns a const iterator to it
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename const Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Iterator 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Find(KeyType const& key) const
{
  ReturnCode rc = RC_DontCreate;
  unsigned element = Probe(key, rc);

  if (rc == RC_NotFound)
  {
    return End();
  }
  
  return ConstIterator(m_head + place);
}

//finds an element by key and returns a reference to the mapped value stored there
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename Improbability::HashMap<Key, Mapped, Hasher, Predicate>::MappedType& 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::operator[](KeyType const& key)
{
  if (static_cast<float>(m_occupied + 1) / static_cast<float>(m_size) > m_max_load_factor)
    Grow(static_cast<unsigned>(m_size * HashMapConstants::k_growth_factor));

  ReturnCode rc = static_cast<ReturnCode>(0);
  unsigned element = Probe(key, rc);

  return m_head[element].m_value.second;
}

//similar to the index operator
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename Improbability::HashMap<Key, Mapped, Hasher, Predicate>::MappedType& 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::GetElement(KeyType const& key)
{
  ReturnCode rc = RC_DontCreate;
  unsigned element = Probe(key, rc);

  return m_head[element].m_value.second;
}

//similar to the index operator
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
typename Improbability::HashMap<Key, Mapped, Hasher, Predicate>::MappedType const& 
Improbability::HashMap<Key, Mapped, Hasher, Predicate>::GetElement(KeyType const& key) const
{
  ReturnCode rc = RC_DontCreate;
  unsigned element = Probe(key, rc);

  return m_head[element].m_value.second;
}

//returns the current load factor in the map
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
float Improbability::HashMap<Key, Mapped, Hasher, Predicate>::GetLoadFactor() const
{
  return static_cast<float>(m_occupied) / static_cast<float>(m_size);
}

//returns the max load factor of the map
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
float Improbability::HashMap<Key, Mapped, Hasher, Predicate>::GetMaxLoadFactor() const
{
  return m_max_load_factor;
}

//sets the max load factor of the map
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
void Improbability::HashMap<Key, Mapped, Hasher, Predicate>::SetMaxLoadFactor(float new_max)
{
  m_max_load_factor = new_max;
}

//grows the map if the new size is greater than the previous size
template<typename Key, typename Mapped, typename Hasher, typename Predicate>
void Improbability::HashMap<Key, Mapped, Hasher, Predicate>::Reserve(unsigned new_size)
{
  this->Grow(new_size);
}
