// Nicholas Baldwin
// 2011/08/18
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

class String;

// hashing functions
// hashed - the item to be hashed
// table_size - the size of the table to hash into
// returns the hashed item mod the table size
unsigned HashFunction(int hashed, unsigned table_size);
unsigned HashFunction(unsigned hashed, unsigned table_size);
unsigned HashFunction(char hashed, unsigned table_size);
unsigned HashFunction(char const *hashed, unsigned table_size);
unsigned HashFunction(String const &hashed, unsigned table_size);
template<typename T>
unsigned HashFunction(const T& hashed, unsigned table_size);

template<typename T>
class Hash
{
public:
  unsigned operator()(T const &hashed, unsigned table_size)
  {
    return HashFunction(hashed, table_size);
  }
};

#include "HashFunction.hpp"
}
