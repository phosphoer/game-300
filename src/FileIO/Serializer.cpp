// Nicholas Baldwin & Jake Leonard
// 2011/05/18
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"

#include "Serializer.h"

namespace Improbability
{

Serializer::Serializer(String const &filename, String const firstNode, bool write) : m_writing(write)
{
  m_document = new TiXmlDocument(filename.GetCString());
  if(write == false)
  {
    ErrorOnResult(m_document->LoadFile(), false, "document could not be loaded: %s", filename.GetCString());

    m_parent_element = 0;
    m_curr_element = m_document->FirstChildElement();
  }
  else
  {
    m_root = new TiXmlElement(firstNode.GetCString());
    m_document->LinkEndChild(m_root);
  }
}

Serializer::~Serializer()
{
  delete m_document;
}


// converts to several types from a String
// str - the string to convert
// i - an integer to convert into
// f - a float to convert into
// b - a bool to convert into
void ConvertFromString(String const &str, int &i)
{
  if (str == 0)
    return;

  i = atoi(str.GetCString());
}
void ConvertFromString(String const &str, float &f)
{
  if (str == 0)
    return;

  f = static_cast<float>(atof(str.GetCString()));
}
void ConvertFromString(String const &str, bool &b)
{
  if (str == 0)
    return;

  char first_char = *str.GetCString();

  if (first_char == '1' || first_char == 't' || first_char == 'T')
  {
    b = true;
  }
  else
  {
    b = false;
  }
}

bool Serializer::ReadIntAttribute(int &i, String const &name) const
{
  String text = m_curr_element->Attribute(name.GetCString());

  if (text != 0)
  {
    ConvertFromString(text, i);
    return true;
  }

  return false;
}

bool Serializer::ReadInt(int &i, String const &name ) const
{
  // String attrib = m_curr_element->FirstChildElement(name.GetCString())->Attribute("type");
  // ErrorIf(attrib != "int", "Attempt to read int from non-int element");
  
  TiXmlElement *child_node = m_curr_element->FirstChildElement(name.GetCString());

  if (child_node)
  {
    String text = child_node->GetText();

    ConvertFromString(text, i);

    return true;
  }

  return false;
}

void Serializer::ReadInt(int &i) const
{
  // String attrib = m_curr_element->Attribute("type");
  // ErrorIf(attrib != "int", "Attempt to read int from non-int element");
  
  String text = m_curr_element->GetText();

  ConvertFromString(text, i);
}

bool Serializer::ReadFloatAttribute(float &f, String const &name) const
{
  String text = m_curr_element->Attribute(name.GetCString());

  if (text != 0)
  {
    ConvertFromString(text, f);
    return true;
  }

  return false;
}

bool Serializer::ReadFloat(float &f, String const &name) const
{
  // String attrib = m_curr_element->FirstChildElement(name.GetCString())->Attribute("type");
  // ErrorIf(attrib != "float", "Attempt to read float from non-float element");


  TiXmlElement *child_node = m_curr_element->FirstChildElement(name.GetCString());

  if (child_node)
  {
    String text = child_node->GetText();

    ConvertFromString(text, f);

    return true;
  }

  return false;
}

void Serializer::ReadFloat(float &f) const
{
  // String attrib = m_curr_element->Attribute("type");
  // ErrorIf(attrib != "float", "Attempt to read float from non-float element");

  String text = m_curr_element->GetText();

  ConvertFromString(text, f);
}

bool Serializer::ReadStringAttribute(String &str, String const &name) const
{
  String text = m_curr_element->Attribute(name.GetCString());

  if (text != 0)
  {
    str = text;
    return true;
  }

  return false;
}

bool Serializer::ReadString(String &str, String const &name) const
{
  // String attrib = m_curr_element->FirstChildElement(name.GetCString())->Attribute("type");
  // ErrorIf(attrib != "string", "Attempt to read string from non-string element");

  TiXmlElement *child_node = m_curr_element->FirstChildElement(name.GetCString());

  if (child_node)
  {
    String text = child_node->GetText();

    str = text;

    return true;
  }

  return false;
}

void Serializer::ReadString(String &str) const
{
  // String attrib = m_curr_element->Attribute("type");
  // ErrorIf(attrib != "string", "Attempt to read string from non-string element");

  String text = m_curr_element->GetText();

  str = text;
}

bool Serializer::ReadBoolAttribute(bool &b, String const &name) const
{
  String text = m_curr_element->Attribute(name.GetCString());

  if (text != 0)
  {
    ConvertFromString(text, b);
    return true;
  }

  return false;
}

bool Serializer::ReadBool(bool &b, String const &name) const
{
  // String attrib = m_curr_element->FirstChildElement(name.GetCString())->Attribute("type");
  // ErrorIf(attrib != "bool", "Attempt to read bool from non-bool element");

  TiXmlElement *child_node = m_curr_element->FirstChildElement(name.GetCString());

  if (child_node)
  {
    String text = child_node->GetText();

    ConvertFromString(text, b);

    return true;
  }

  return false;
}

void Serializer::ReadBool(bool &b) const
{
  // String attrib = m_curr_element->Attribute("type");
  // ErrorIf(attrib != "bool", "Attempt to read bool from non-bool element");

  String text = m_curr_element->GetText();

  ConvertFromString(text, b);
}

void ConvertToString(String &str, int &i)
{
  char buffer[20];
  SecureZeroMemory(buffer, sizeof(buffer));
  _itoa_s(i, buffer, sizeof(buffer), 10);

  str = buffer;
}

void ConvertToString(String &str, float &f)
{
  char buffer[20];
  sprintf_s(buffer, sizeof(buffer), "%f", f);

  str = buffer;
}

void ConvertToString(String &str, bool &b)
{
  if(b == true)
    str.Assign("true");
  else
    str.Assign("false");
}

void Serializer::WriteIntAttribute(int &i, String const &name)
{
  m_curr_element->SetAttribute(name.GetCString(), i);
}

void Serializer::WriteInt(int &i, String const &name)
{
  String text;
  m_curr_element = new TiXmlElement(name.GetCString());
  ConvertToString(text, i);
  m_curr_element->LinkEndChild(new TiXmlText(text.GetCString()));
  m_parent_element->LinkEndChild(m_curr_element);
}

void Serializer::WriteFloatAttribute(float &f, String const &name)
{
  m_curr_element->SetDoubleAttribute(name.GetCString(), (double)f);
}

void Serializer::WriteFloat(float &f, String const &name)
{
  String text;
  m_curr_element = new TiXmlElement(name.GetCString());
  ConvertToString(text, f);
  m_curr_element->LinkEndChild(new TiXmlText(text.GetCString()));
  m_parent_element->LinkEndChild(m_curr_element);
}

void Serializer::WriteStringAttribute(String &str, String const &name)
{
  m_curr_element->SetAttribute(name.GetCString(), str.GetCString());
}

void Serializer::WriteString(String &str, String const &name)
{
  m_curr_element = new TiXmlElement(name.GetCString());
  m_curr_element->LinkEndChild(new TiXmlText(str.GetCString()));
  m_parent_element->LinkEndChild(m_curr_element);
}

void Serializer::WriteBoolAttribute(bool &b, String const &name)
{
  m_curr_element->SetAttribute(name.GetCString(), (int)b);
}

void Serializer::WriteBool(bool &b, String const &name)
{
  String text;
  m_curr_element = new TiXmlElement(name.GetCString());
  ConvertToString(text, b);
  m_curr_element->LinkEndChild(new TiXmlText(text.GetCString()));
  m_parent_element->LinkEndChild(m_curr_element);
}

bool Serializer::EnterElement(String const &name)
{
  m_parent_element = m_curr_element;

  m_curr_element = m_curr_element->FirstChildElement(name.GetCString());

  return m_curr_element != 0;
}

void Serializer::LeaveElement()
{
  m_curr_element = m_parent_element;
  m_parent_element = m_curr_element->Parent()->ToElement();
}

bool Serializer::Begin()
{
  m_parent_element = m_curr_element;
  m_curr_element = m_curr_element->FirstChildElement();

  return m_curr_element != 0;
}

bool Serializer::Iterate()
{
  m_curr_element = m_curr_element->NextSiblingElement();

  return m_curr_element != 0;
}

bool Serializer::IsGood()
{
  return m_curr_element != 0;
}

String Serializer::GetName()
{
  return m_curr_element->Value();
}

bool Serializer::CheckType(String const &type)
{
  return type == m_curr_element->Attribute("type");
}

void Serializer::CreateElementGroup(String const &name)
{
  m_parent_element = new TiXmlElement(name.GetCString());
}

void Serializer::FinishElementGroup()
{
  m_root->LinkEndChild(m_parent_element);
  m_parent_element = m_root;
}

bool Serializer::SaveFile()
{
  if(m_writing)
  {
    #ifdef _DEBUG
        m_document->Print();
    #endif

    m_document->SaveFile();
    return true;
  }
  return false;
}

}
