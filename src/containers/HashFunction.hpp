// Nicholas Baldwin
// 2011/06/24
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

template<typename T>
unsigned HashFunction(const T& hashed, unsigned table_size)
{
  return hashed.Hash(table_size);
}
