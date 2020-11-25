// Nicholas Baldwin & Jake Leonard
// 2011/05/18
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "../containers/HashMap.h"

class TiXmlDocument;
class TiXmlElement;

namespace Improbability
{

class String;

//handles serialization/deserialization of xml files
class Serializer
{
public:
  // either opens a new xml file or grabs the doc pointer from the map if it exists
  // filename - the name of the file for the serializer to read from
  // note - xml file must have only ONE root node!!! at least for now
  Serializer(String const &filename, String const firstNode="SaveData", bool write=false);
  ~Serializer();

  // reads POD types, aggregates ultimately break down to these
  // name - the name of the element to read
  bool ReadIntAttribute(int &i, String const &name) const;
  bool ReadInt(int &i, String const &name) const;
  void ReadInt(int &i) const;
  bool ReadFloatAttribute(float &f, String const &name) const;
  bool ReadFloat(float &f, String const &name) const;
  void ReadFloat(float &f) const;
  bool ReadStringAttribute(String &str, String const &name) const;
  bool ReadString(String &str, String const &name) const;
  void ReadString(String &str) const;
  bool ReadBoolAttribute(bool &b, String const &name) const;
  bool ReadBool(bool &b, String const &name) const;
  void ReadBool(bool &b) const;

  // Writes POD types, aggregates ultimately break down to these
  // name - the name of the element to write
  void WriteIntAttribute(int &i, String const &name);
  void WriteInt(int &i, String const &name);
  void WriteFloatAttribute(float &f, String const &name);
  void WriteFloat(float &f, String const &name);
  void WriteStringAttribute(String &str, String const &name);
  void WriteString(String &str, String const &name);
  void WriteBoolAttribute(bool &b, String const &name);
  void WriteBool(bool &b, String const &name);

  // steps into an aggregate, deserializes it and steps out, object must have
  //   method "Deserialize" or template errors
  template<typename T>
  bool ReadAggregate(T& object, String const &name);

  // creates an aggregate, serializes it and steps out, object must have
  //   method "Serialize" or template errors
  template<typename T>
  bool WriteAggregate(T& object, String const &name);

  // enters the first sub element of the current element, returns false if there
  //   is no first element
  bool Begin();

  // moves the element to its next sibling returns false if there are no more
  //   siblings
  bool Iterate();

  // checks if the current element is good, returns false if it isn't
  bool IsGood();

  // returns the name of the current element
  String GetName();

  // checks the given type as string against the attribute of the current element
  bool CheckType(String const &type);

  // sets current element to the given element
  bool EnterElement(String const &name);

  // sets current element to parent of itself
  void LeaveElement();

  // sets current element to the given element
  void CreateElementGroup(String const &name);

  // sets current element to parent of itself
  void FinishElementGroup();

  // saves the current document to the given path
  bool SaveFile();

private:
  TiXmlDocument *m_document;
  TiXmlElement *m_root;
  TiXmlElement *m_curr_element;
  TiXmlElement *m_parent_element;
  bool m_writing;
};

// reads in an aggregate who is a child of the current node by name
// object - the object being read in
// name - the name of the child node to be read from
// returns whether the queried object exists or not
template<typename T>
bool Serializer::ReadAggregate(T& object, String const &name)
{
  // go into given element
  if (!this->EnterElement(name))
  {
    this->LeaveElement();
    return false;
  }

  // deserialize element
  object.Deserialize(*this);
  // leave the given element
  this->LeaveElement();

  return true;
}

// syntactic sugar for the function read to work with aggregates as well
// serializer - the serializer to read from
// object - the aggregate to read into
// name - the name of the child node to read from
// returns whether the queried element exists or not
template<typename T>
inline bool Read(Serializer& serializer, T& object, String const &name)
{
  return serializer.ReadAggregate(object, name);
}

// Read overloads for various POD types
inline bool Read(Serializer& serializer, int& i, String const &name)
{
  return serializer.ReadInt(i, name);
}

inline bool Read(Serializer& serializer, float& f, String const &name)
{
  return serializer.ReadFloat(f, name);
}

inline bool Read(Serializer& serializer, String& str, String const &name)
{
  return serializer.ReadString(str, name);
}

inline bool Read(Serializer& serializer, bool& b, String const &name)
{
  return serializer.ReadBool(b, name);
}

// ReadAttribute overloads for various POD types, acts similar to Read with
//   string name
inline bool ReadAttribute(Serializer& serializer, int& i, String const &name)
{
  return serializer.ReadIntAttribute(i, name);
}

inline bool ReadAttribute(Serializer& serializer, float& f, String const &name)
{
  return serializer.ReadFloatAttribute(f, name);
}

inline bool ReadAttribute(Serializer& serializer, String& str, String const &name)
{
  return serializer.ReadStringAttribute(str, name);
}

inline bool ReadAttribute(Serializer& serializer, bool& b, String const &name)
{
  return serializer.ReadBoolAttribute(b, name);
}

// Further syntactic sugar to have read work on the current element if it is a
//   non-POD type
// serializer - the serializer to read from
// object - the object being read into
template <typename T>
inline void Read(Serializer& serializer, T& object)
{
  object.Deserialize(serializer);
}

// More Read overloads for POD types, now nameless for iteration
inline void Read(Serializer& serializer, int& i)
{
  serializer.ReadInt(i);
}

inline void Read(Serializer& serializer, float& f)
{
  serializer.ReadFloat(f);
}

inline void Read(Serializer& serializer, String& str)
{
  serializer.ReadString(str);
}

inline void Read(Serializer& serializer, bool& b)
{
  serializer.ReadBool(b);
}

// Writes in an aggregate who is a child of the current node by name
// object - the object being Write in
// name - the name of the child node to be Write from
// returns whether the queried object exists or not
template<typename T>
bool Serializer::WriteAggregate(T& object, String const &name)
{
  // go into given element
  this->CreateElementGroup(name);

  // serialize element
  object.Serialize(*this);
  // leave the given element
  this->FinishElementGroup();

  return true;
}

// syntactic sugar for the function Write to work with aggregates as well
// serializer - the serializer to Write from
// object - the aggregate to Write into
// name - the name of the child node to Write from
// returns whether the queried element exists or not
template<typename T>
inline void Write(Serializer& serializer, T& object, String const &name)
{
  serializer.WriteAggregate(object, name);
}

// Write overloads for various POD types
inline void Write(Serializer& serializer, int& i, String const &name)
{
   serializer.WriteInt(i, name);
}

inline void Write(Serializer& serializer, float& f, String const &name)
{
  serializer.WriteFloat(f, name);
}

inline void Write(Serializer& serializer, String& str, String const &name)
{
  serializer.WriteString(str, name);
}

inline void Write(Serializer& serializer, bool& b, String const &name)
{
  serializer.WriteBool(b, name);
}

// WriteAttribute overloads for various POD types, acts similar to Write with
//   string name
inline void WriteAttribute(Serializer& serializer, int& i, String const &name)
{
  serializer.WriteIntAttribute(i, name);
}

inline void WriteAttribute(Serializer& serializer, float& f, String const &name)
{
  serializer.WriteFloatAttribute(f, name);
}

inline void WriteAttribute(Serializer& serializer, String& str, String const &name)
{
  serializer.WriteStringAttribute(str, name);
}

inline void WriteAttribute(Serializer& serializer, bool& b, String const &name)
{
  serializer.WriteBoolAttribute(b, name);
}

// Further syntactic sugar to have Write work on the current element if it is a
//   non-POD type
// serializer - the serializer to Write from
// object - the object being Write into
template <typename T>
inline void Write(Serializer& serializer, T& object)
{
  object.Serialize(serializer);
}

}