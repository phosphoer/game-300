// David Evans
// 2011/05/20
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

class MetaComposition;

// String class to provide basic string functionality that doesn't suck
class String
{
public:
  static void BindMeta(MetaComposition *comp);

  // Default constructor
  String();

  // Destructor
  ~String();

  // Copy constructor
  String(String const &rhs);

  // Constructs a string from a cstring
  // str - Char array to copy from
  String(char const *str);

  // Constructs a string from a std::string
  // str - std::string to copy from
  String(std::string const &str);

  // Construct a string from a range
  String(String const &rhs, int begin, int end);
  String(char const *rhs, int begin, int end);

  // Implicit conversion to cstring
  operator char const*(){return m_buffer;};

  // Makes the string empty (sets length to 0)
  void Clear();

  // Assign a string to this string
  // rhs - String to copy from
  void Assign(String const &rhs);
  void Assign(char const *rhs);

  // Reserves a certain capacity in the string
  void Reserve(unsigned bytes);

  // Find a substring within the string
  // substr - String to search for
  // index - Index to begin searching from
  // return - Index of the substring, or String::kEnd
  int Find(char const *substr, int index = 0) const;
  int Find(String const &substr, int index = 0) const {return Find(substr.GetCString(), index);}

  // Find the last occurence of a substring
  // substr - String to search for
  // return - Index of the substring, or String::kEnd
  int FindLastOf(char const *substr) const;
  int FindLastOf(String const &substr) const {return FindLastOf(substr.GetCString());}

  // Assignment operator
  // rhs - String to assign from
  void operator=(String const &rhs);
  void operator=(char const *rhs);

  // Addition operator
  String operator+(String const &rhs) const;
  String operator+(char const *rhs) const;
  friend String operator+(char const *lhs, String const &rhs);

  // Concat addition
  String& operator+=(String const &rhs);
  String& operator+=(char const *rhs);
  String& operator+=(char rhs);

  // Comparison operator
  bool operator==(String const &rhs) const;
  bool operator==(char const *rhs) const;

  // Inequality operator
  bool operator!=(String const &rhs) const;
  bool operator!=(char const *rhs) const;

  // Less than operator
  bool operator<(String const &rhs) const;
  bool operator<(char const *rhs) const;

  // Index operator
  char operator[](int i) const {return m_buffer[i];}

  // Output operator overload
  friend std::ostream& operator<<(std::ostream &os, String const &rhs);

  // Get the underlying c string buffer
  // return - Constant c string
  char const* GetCString() const {return m_buffer;}

  // Get last character
  char GetBack() const {return m_buffer[m_length - 1];}

  // Get the length of the string
  int GetLength() const {return m_length;}

  // Get the size of the string
  int GetSize() const {return m_size;}

  // Construct a string via sprintf
  // format - Format string
  // ... - Parameters to sprintf
  static String Formatted(char const *format, ...);

  std::vector<String> Tokenize(char delimitter = ' ') const;

  String CreateOrphan() const;

public:
    static int const k_end = -1;

private:
  // Utility to concat a cstr with known length
  void ConcatString(char const *str, int length);

private:
  // Current string length
  int m_length;

  // Size of buffer
  int m_size;

  // Actual string buffer
  char *m_buffer;

  // How many references to this string exist
  volatile unsigned *m_reference_count;
};

}