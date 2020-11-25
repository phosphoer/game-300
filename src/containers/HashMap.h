// Nicholas Baldwin
// 2011/06/10
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "HashFunction.h"

namespace Improbability
{

  namespace HashMapConstants
  {
    const unsigned k_default_num_elements = 20;
    const float k_default_max_load_factor = 0.8f;
    const float k_growth_factor = 1.5f;
  }

  //hash map implementation, uses circular list to keep "buckets"
  template<typename Key, typename Mapped, typename Hasher=Improbability::Hash<Key>, typename Predicate=std::equal_to<Key>>
  class HashMap
  {
  public:
////////////////////////////////////////////////////////////////////////////////
////////////////////////Typedefs and Constants//////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    typedef Key                                         KeyType;
    typedef std::pair<Key, Mapped>                      StorageType;
    typedef std::pair<Key const, Mapped>                ValueType;
    typedef Mapped                                      MappedType;
    typedef Predicate                                   EgualTo;
    
////////////////////////////////////////////////////////////////////////////////
////////////////////////Copy/Construct/Destruct/////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    HashMap(unsigned initial_capacity = HashMapConstants::k_default_num_elements, 
            float max_load_factor = HashMapConstants::k_default_max_load_factor);
    ~HashMap();
    HashMap(const HashMap& origin);
    HashMap& operator=(const HashMap& origin);

////////////////////////////////////////////////////////////////////////////////
////////////////////////Size and Capacity///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    bool IsEmpty() const;
    unsigned Size() const;
    unsigned Capacity() const;

////////////////////////////////////////////////////////////////////////////////
////////////////////////Element/////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
  private:
    struct HashElement
    {
      HashElement();

      StorageType m_value;
      HashElement* m_next;
      bool m_is_in_correct_spot;
    };

    HashElement* m_head;
    HashElement* m_free;

////////////////////////////////////////////////////////////////////////////////
////////////////////////Data////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    unsigned m_size;
    unsigned m_occupied;
    float m_max_load_factor;

////////////////////////////////////////////////////////////////////////////////
////////////////////////Helper Functions////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    enum ReturnCode
    {
      RC_Found = 0,
      RC_Perfect,
      RC_Probed,
      RC_NotFound,
      RC_DontCreate
    };

    //This function does most of the work, probes for the 
    unsigned Probe(KeyType const& key, ReturnCode &return_code);

    //Grows the table when reserving or gets above max load factor
    void Grow(unsigned new_size);

////////////////////////////////////////////////////////////////////////////////
////////////////////////Iterators///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    class HashMapIterator
    {
    public:
      HashMapIterator(HashElement* element = 0);

      //input/output iterator
      ValueType& operator*();
      ValueType& operator*() const;
      ValueType* operator->();
      ValueType* operator->() const;

      //forward iterator
      HashMapIterator& operator++();
      const HashMapIterator& operator++() const;
      HashMapIterator operator++(int);
      const HashMapIterator operator++(int) const;

      bool operator==(HashMapIterator const& rhs) const;
      bool operator!=(HashMapIterator const& rhs) const;

      //bidirectional iterator
      HashMapIterator& operator--();
      const HashMapIterator& operator--() const;
      HashMapIterator operator--(int);
      const HashMapIterator operator--(int) const;

    private:
      mutable HashElement *m_element;
    };

  public:
    typedef HashMapIterator       Iterator;
    typedef const HashMapIterator ConstIterator;

    Iterator Begin();
    ConstIterator Begin() const;
    Iterator End();
    ConstIterator End() const;

////////////////////////////////////////////////////////////////////////////////
////////////////////////Modifiers///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    Iterator Insert(KeyType const &new_key, MappedType const &new_mapped);
    void Erase(KeyType const &key_to_erase);
    void Clear();
    void Swap(HashMap &other_map);

////////////////////////////////////////////////////////////////////////////////
////////////////////////Lookup//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    Iterator Find(KeyType const& key);
    ConstIterator Find(KeyType const& key) const;
    MappedType& operator[](KeyType const& key);
    MappedType& GetElement(KeyType const& key);
    MappedType const& GetElement(KeyType const& key) const;

////////////////////////////////////////////////////////////////////////////////
////////////////////////Hash Info///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    float GetLoadFactor() const;                //returns the current load factor
    float GetMaxLoadFactor() const;             //returns the max load factor
    void SetMaxLoadFactor(float new_max);       //sets the max load factor
    void Reserve(unsigned new_size);            //resizes the map, 
  };

#include "HashMap.hpp"
}