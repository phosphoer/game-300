// David Evans
// 2011/05/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "String.h"

namespace Improbability
{

MakeMeta(String);

void String::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

String::String():
  m_length(0),
  m_size(0),
  m_buffer(0),
  m_reference_count(0)
{
}

String::~String()
{
  if (m_reference_count && *m_reference_count == 0)
  {
    if (m_buffer) delete[] m_buffer;
    _aligned_free((void*)m_reference_count);
    m_reference_count = 0;
  }
  else if (m_reference_count)
  {
    InterlockedDecrement(m_reference_count);
  }
}

String::String(String const &rhs)
{
  // Point our data at the other string
  m_buffer = rhs.m_buffer;
  m_size = rhs.m_size;
  m_length = rhs.m_length;

  // Increase reference count
  m_reference_count = rhs.m_reference_count;
  if (m_reference_count)
    InterlockedIncrement(m_reference_count);
}

String::String(char const *str)
{
  // Get the length of the string and allocate our buffer
  if (str)
  {
    m_length = std::strlen(str);
  
    // Length + 1 to get null
    m_size = m_length + 1;
    m_buffer = new char[m_size];
  
    // Copy the buffer and add a null
    std::memcpy(m_buffer, str, m_length);
    m_buffer[m_length] = 0;
  }
  else
  {
    m_length = 0;
    m_size = 0;
    m_buffer = 0;
  }
  *(m_reference_count = (volatile unsigned*)_aligned_malloc(sizeof(volatile unsigned),32)) = 0;
}

String::String(std::string const &str)
{
  // Just build from the cstring
  char const *cstr = str.c_str();

  // Get the length of the string and allocate our buffer
  // Length + 1 to get null
  m_length = str.length();
  m_size = m_length + 1;
  m_buffer = new char[m_size];
  *(m_reference_count = (volatile unsigned*)_aligned_malloc(sizeof(volatile unsigned),32)) = 0;

  // Copy the buffer and add a null
  std::memcpy(m_buffer, cstr, m_length);
  m_buffer[m_length] = 0;
}

String::String(String const &rhs, int begin, int end)
{
  // Get length and allocate buffer
  m_length = end - begin;
  m_size = m_length + 1;
  m_buffer = new char[m_size];
  *(m_reference_count = (volatile unsigned*)_aligned_malloc(sizeof(volatile unsigned),32)) = 0;

  // Copy the buffer
  std::memcpy(m_buffer, rhs.GetCString() + begin, m_length);
  m_buffer[m_length] = 0;
}

String::String(char const *rhs, int begin, int end)
{
  // Get length and allocate buffer
  m_length = end - begin;
  m_size = m_length + 1;
  m_buffer = new char[m_size];
  *(m_reference_count = (volatile unsigned*)_aligned_malloc(sizeof(volatile unsigned),32)) = 0;

  // Copy the buffer
  std::memcpy(m_buffer, rhs + begin, m_length);
  m_buffer[m_length] = 0;
}

void String::Clear()
{
  // Separate from other strings
  if (*m_reference_count > 0)
  {
    InterlockedDecrement(m_reference_count);
    *(m_reference_count = (volatile unsigned*)_aligned_malloc(sizeof(volatile unsigned),32)) = 0;
    m_buffer = new char[m_size];
  }

  // Set length to 0
  if (m_buffer)
    m_buffer[0] = 0;
  m_length = 0;
}

void String::Assign(String const &rhs)
{
  // Check for self assignment
  if (m_buffer == rhs.m_buffer)
    return;

  if (m_buffer != 0)
  {
    // Have to delete our old buffer if no references
    if (*m_reference_count == 0)
    {
      delete[] m_buffer;
      _aligned_free((void*)m_reference_count);
      m_reference_count = 0;
    }
    // Otherwise, decrement old references
    else
    {
      InterlockedDecrement(m_reference_count);
    }
  }

  // Steal their buffer!
  m_buffer = rhs.m_buffer;
  m_length = rhs.m_length;
  m_size = rhs.m_size;
  m_reference_count = rhs.m_reference_count;
  InterlockedIncrement(m_reference_count);
}

void String::Assign(char const *rhs)
{
  // Check for self assignment
  if (m_buffer == rhs)
    return;

  if (m_buffer != 0)
  {
    // Have to delete our old buffer if no references
    if (*m_reference_count == 0)
    {
      delete[] m_buffer;
    }
    // Otherwise, decrement old references
    else
    {
      InterlockedDecrement(m_reference_count);
      *(m_reference_count = (volatile unsigned*)_aligned_malloc(sizeof(volatile unsigned),32)) = 0;
    }
  }

  // Allocate buffer
  m_length = std::strlen(rhs);
  m_size = m_length + 1;
  m_buffer = new char[m_size];

  // Copy the buffer and add a null
  std::memcpy(m_buffer, rhs, m_length);
  m_buffer[m_length] = 0;

  // Optionally create a reference count if it hasn't been created yet
  if (!m_reference_count)
    *(m_reference_count = (volatile unsigned*)_aligned_malloc(sizeof(volatile unsigned),32)) = 0;
}

void String::Reserve(unsigned bytes)
{
  // Store the old buffer
  char *old_buffer = m_buffer;
  
  // Allocate buffer
  m_size = bytes;
  m_buffer = new char[m_size];

  // Copy the old string
  unsigned copy_size = m_length;
  if (m_length >= m_size)
    m_length = m_size - 1;
  std::memcpy(m_buffer, old_buffer, m_length);
  m_buffer[m_length] = 0;

   // Optionally create a reference count if it hasn't been created yet
  if (!m_reference_count)
    *(m_reference_count = (volatile unsigned*)_aligned_malloc(sizeof(volatile unsigned),32)) = 0; 
    
  // Have to delete our old buffer if no references
  if (*m_reference_count == 0)
  {
    delete[] old_buffer;
  }
  // Otherwise, decrement old references
  else
  {
    InterlockedDecrement(m_reference_count);
    *(m_reference_count = (volatile unsigned*)_aligned_malloc(sizeof(volatile unsigned),32)) = 0;
  }
}

int String::Find(char const *substr, int index) const
{
  ErrorIf(index >= m_length, "Attempt to index outside of string range.");

  // Find the substring
  char const *sub = strstr(m_buffer + index, substr);

  // If it's null, it isn't present in the string
  if (!sub)
    return k_end;

  // Otherwise, return the offset
  return sub - m_buffer;
}

int String::FindLastOf(char const *substr) const
{
  int index = k_end;
  do
  {
    int i = Find(substr, index + 1);
    if (i != k_end)
      index = i;
    else if (i == k_end && index == k_end)
      return k_end;
    else if (i == k_end && index != k_end)
      return index;

  } while (index != k_end);

  return index;
}

void String::operator=(String const &rhs)
{
  Assign(rhs);
}

void String::operator=(char const *rhs)
{
  Assign(rhs);
}

String String::operator+(String const &rhs) const
{
  // If either of the buffers are null, return the other
  if (!m_buffer)
    return rhs;

  if (!rhs.m_buffer)
    return *this;

  // Get length of right hand argument
  int concat_length = m_length + rhs.m_length;

  // Allocate the new string
  String concat_string;
  concat_string.m_size = concat_length + 1;
  concat_string.m_length = concat_length;
  concat_string.m_buffer = new char[concat_string.m_size];
  *(concat_string.m_reference_count = (volatile unsigned*)_aligned_malloc(sizeof(volatile unsigned),32)) = 0;

  // Build the string
  std::strcpy(concat_string.m_buffer, m_buffer);
  std::strcpy(concat_string.m_buffer + m_length, rhs.m_buffer);

  return concat_string;
}

String String::operator+(char const *rhs) const
{
  // If either of the buffers are null, return the other
  if (!m_buffer)
    return String(rhs);

  if (!rhs)
    return *this;

  // Get length of right hand argument
  int rhs_length = std::strlen(rhs);
  int concat_length = m_length + rhs_length;

  // Allocate the new string
  String concat_string;
  concat_string.m_size = concat_length + 1;
  concat_string.m_length = concat_length;
  concat_string.m_buffer = new char[concat_string.m_size];
  *(concat_string.m_reference_count = (volatile unsigned*)_aligned_malloc(sizeof(volatile unsigned),32)) = 0;  

  // Build the string
  std::strcpy(concat_string.m_buffer, m_buffer);
  std::strcpy(concat_string.m_buffer + m_length, rhs);

  return concat_string;
}

String operator+(char const *lhs, String const &rhs)
{
  // If either of the buffers are null, return the other
  if (!lhs)
    return rhs;

  if (!rhs.m_buffer)
    return String(lhs);

  // Get length of right hand argument
  int lhs_length = std::strlen(lhs);
  int concat_length = rhs.m_length + lhs_length;

  // Allocate the new string
  String concat_string;
  concat_string.m_size = concat_length + 1;
  concat_string.m_length = concat_length;
  concat_string.m_buffer = new char[concat_string.m_size];
  *(concat_string.m_reference_count = (volatile unsigned*)_aligned_malloc(sizeof(volatile unsigned),32)) = 0;

  // Build the string
  std::strcpy(concat_string.m_buffer, lhs);
  std::strcpy(concat_string.m_buffer + lhs_length, rhs.m_buffer);

  return concat_string;
}

String& String::operator+=(String const &rhs)
{
  // Concat the string
  ConcatString(rhs.m_buffer, rhs.m_length);

  return *this;
}

String& String::operator+=(char const *rhs)
{
  // Get length of string and concat it
  int rhs_length = std::strlen(rhs);
  ConcatString(rhs, rhs_length);

  return *this;
}

String& String::operator+=(char rhs)
{
  ConcatString(&rhs, 1);
  return *this;
}

bool String::operator==(String const &rhs) const
{
  return *this == rhs.m_buffer;
}

bool String::operator==(char const *rhs) const
{
  // Trivial check
  if (m_buffer == rhs)
    return true;

  if (rhs == 0 || m_buffer == 0)
    return false;

  // Compare the strings
  return (std::strcmp(m_buffer, rhs) == 0);
}

bool String::operator!=(String const &rhs) const
{
  return *this != rhs.m_buffer;
}

bool String::operator!=(char const *rhs) const
{
  return !(*this == rhs);
}

bool String::operator<(String const &rhs) const
{
  return *this < rhs.m_buffer;
}

bool String::operator<(char const *rhs) const
{
  // Trivial check
  if (m_buffer == rhs)
    return false;

  // Compare char by char
  for (int i = 0; i < m_length && rhs[i]; ++i)
  {
    if (m_buffer[i] < rhs[i])
      return true;
    else if (m_buffer[i] > rhs[i])
      return false;
  }

  // The strings are equal up to this point, so if this is shorter, it is less
  int rhs_length = std::strlen(rhs);
  if (m_length < rhs_length)
    return true;

  // Otherwise, this is greater
  return false;
}

std::ostream& operator<<(std::ostream &os, String const &rhs)
{
  os << rhs.m_buffer;
  return os;
}

void String::ConcatString(char const *rhs, int rhs_length)
{
  // If either of the buffers are null, return the other
  if (!m_buffer)
  {
    *this = rhs;
    return;
  }

  if (!rhs)
  {
    return;
  }

  // Store a pointer to our old data
  char const *old_buffer = m_buffer;
  bool delete_old_buffer = false;

  if (!m_reference_count)
    *(m_reference_count = (volatile unsigned*)_aligned_malloc(sizeof(volatile unsigned),32)) = 0;

  // Have to seperate from all references to us
  if (*m_reference_count > 0)
  {
    InterlockedDecrement(m_reference_count);
    *(m_reference_count = (volatile unsigned*)_aligned_malloc(sizeof(volatile unsigned),32)) = 0;
    m_size += rhs_length;
    m_size *= 2;
    m_buffer = new char[m_size];
    if (old_buffer)
      std::strcpy(m_buffer, old_buffer);
  }
  // Otherwise we are the only user
  // Check if we need to resize at all
  else if (m_size <= m_length + rhs_length)
  {
    m_size += rhs_length;
    m_size *= 2;
    m_buffer = new char[m_size];
    if (old_buffer)
      std::strcpy(m_buffer, old_buffer);
    delete_old_buffer = true;
  }

  // Copy the new part into our buffer
  // Can't use strcpy in case m_buffer is the same as rhs.m_buffer
  // + 1 to get the null
  std::memcpy(m_buffer + m_length, rhs, rhs_length + 1);
  m_length += rhs_length;

  // Delete the old buffer
  if (delete_old_buffer)
    delete[] old_buffer;
}

String String::Formatted(char const *format, ...)
{
  va_list args;
  va_start(args, format);
  int size = vsnprintf(0, 0, format, args);

  String str;
  str.Reserve(size + 1);
  vsprintf(str.m_buffer, format, args);
  str.m_length = size;
  va_end(args);

  return str;
}

std::vector<String> String::Tokenize(char delimitter) const
{
  std::vector<String> tokens;

  int begin = 0;
  int end = Find(&delimitter);

  while (end != k_end)
  {
    tokens.push_back(String(*this, begin, end));
    begin = end + 1;
    end = Find(&delimitter, begin);
  }

  tokens.push_back(String(*this, begin, m_length));

  return tokens;
}

String String::CreateOrphan() const
{
  String str;
  // Get the length of the string and allocate our buffer
  if (m_buffer)
  {
    str.m_length = m_length;

    // Length + 1 to get null
    str.m_size = m_size;
    str.m_buffer = new char[m_size];

    // Copy the buffer and add a null
    std::memcpy(str.m_buffer, m_buffer, m_length);
    str.m_buffer[m_length] = 0;
  }
  else
  {
    str.m_length = 0;
    str.m_size = 0;
    str.m_buffer = 0;
  }
  *(str.m_reference_count = (volatile unsigned*)_aligned_malloc(sizeof(volatile unsigned),32)) = 0;

  return str;
}

}