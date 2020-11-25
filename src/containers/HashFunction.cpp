// Nicholas Baldwin
// 2011/08/19
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "HashFunction.h"

namespace Improbability
{

unsigned HashFunction(int hashed, unsigned table_size)
{
  return static_cast<unsigned>(hashed) % table_size;
}

unsigned HashFunction(unsigned hashed, unsigned table_size)
{
  return hashed % table_size;
}

unsigned HashFunction(char hashed, unsigned table_size)
{
  return hashed % table_size;
}

// used in superfast hash
#define Get16Bits(data)              \
 ((((unsigned)((data)[1])) << 8)     \
           +(unsigned)((data)[0]))

// uses superfast hash to hash strings
unsigned HashFunction(const char *hashed, unsigned str_length, unsigned table_size)
{
  unsigned hash = str_length, tmp;
  int rem;

  if (str_length <= 0 || hashed == NULL) return 0;

  rem = str_length & 3;

  /* Main loop */
  for (str_length >>= 2; str_length > 0; str_length--) {
    hash   +=  Get16Bits(hashed);
    tmp     = (Get16Bits(hashed+2) << 11) ^ hash;
    hash    = (hash << 16) ^ tmp;
    hashed +=  sizeof(unsigned);
    hash   +=  hash >> 11;
  }

  /* Handle end cases */
  switch (rem) {
  case 3: 
    hash += Get16Bits(hashed);
    hash ^= hash << 16;
    hash ^= hashed[2*sizeof(char)] << 18;
    hash += hash >> 11;
    break;
  case 2: 
    hash += Get16Bits(hashed);
    hash ^= hash << 11;
    hash += hash >> 17;
    break;
  case 1: 
    hash += *hashed;
    hash ^=  hash << 10;
    hash +=  hash >> 1;
  }

  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;

  return hash % table_size;
}

unsigned HashFunction(char const *hashed, unsigned table_size)
{
  return HashFunction(hashed, strlen(hashed), table_size);
}

unsigned HashFunction(String const &hashed, unsigned table_size)
{
  return HashFunction(hashed.GetCString(), hashed.GetLength(), table_size);
}

}