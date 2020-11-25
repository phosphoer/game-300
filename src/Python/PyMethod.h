// Jake Leonard
// 2011/05/26
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{
// This is an empty class. It's used to have a general structure (*salute*) for data containers.
class PythonMethodBase
{
  public:
};

// God help you if you look at the following class. Don't think about it.
template <typename funcPtrType>
class PythonMethod : public PythonMethodBase
{
// Why are you looking at this? Go away.
public:
  // Seriously, stop.
  PythonMethod(funcPtrType func, void* ptrToClass=NULL) :
      m_funcPtr(func),
      m_returnType(0),
      m_args(0),
      m_stackSize(0),
      m_memberFunction(false),
      m_objPtr(ptrToClass)
  {
    bool returnFound = false;
    const char* ptrType = typeid(func).name();
    String args(ptrType), temp;

    int i = 0, strLen = args.GetLength();
    char curChar;
    while (i < strLen)
    {
      curChar = ptrType[i];

      // New Data section found
      if (curChar == '(' || (curChar == ' ' && ptrType[i + 1] == '_') || curChar == ',' || curChar == ')')
      {
        // Finding the return type
        if ((curChar == '(' && returnFound == false) || (curChar == ' ' && ptrType[i + 1] == '_'))
        {
          /* Gotta get rid of the trailing space.
             Could have done this by reworking previous logic into a nightmare
             since unsigned int and char * have spaces no matter what
             so we do this ugly hack*/
          unsigned length = temp.GetLength();
          if (temp[length - 1] == ' ')
          {
            char* buff = new char[length + 1];
            memset(buff, 0, length);
            memcpy(buff, temp.GetCString(), length - 1);
            temp.Clear();
            temp = buff;
            delete [] buff;
          }

          if (temp == "void")
            m_returnType = 'v';
          else if (temp == "int")
            m_returnType = 'i';
          else if (temp == "unsigned" || temp == "unsigned int")
            m_returnType = 'I';
          else if (temp == "char *" || temp == "char const *" || temp == "class Improbability::String")
            m_returnType = 's';
          else if (temp == "double")
            m_returnType = 'd';
          else if (temp == "float")
            m_returnType = 'f';
          else if (temp == "bool")
            m_returnType = 'b';
          else
            m_returnType = 'v';
          returnFound = true;
          temp.Clear();
        }
        else
        {
          // Function call type
          if (curChar == ')' || curChar == '(')
          {
            if (i + 1 != strLen)
            {
              // Used in the check to make sure the item is a member function pointer
              m_memberFunction = (temp != "__cdecl*");

              // Skip the )(
              i += 2;

              temp.Clear();
              continue;
            }
          }

          // Arguments
          ++m_args;
          if (temp == "int")
          {
            m_argFormat += 'i';
            m_stackSize += sizeof(int);
          }
          else if (temp == "unsigned" || temp == "unsigned int")
          {
            m_argFormat += 'I';
            m_stackSize += sizeof(unsigned);
          }
          else if (temp == "char *" || temp == "char const *" || temp == "class Improbability::String")
          {
            m_argFormat += 's';
            m_stackSize += sizeof(const char*);
          }
          else if (temp == "double")
          {
            m_argFormat += 'd';
            m_stackSize += sizeof(double);
          }
          else if (temp == "float")
          {
            m_argFormat += 'f';
            m_stackSize += sizeof(float);
          }
          else if (temp == "bool")
          {
            m_argFormat += 'b';
            m_stackSize += sizeof(int); //sizeof(bool) returns 1 but it's really the size of 4
          }
          else
          {
            i = strLen;
            m_args = 0;
            m_argFormat += 'v';
            break;
          }
          temp.Clear();
        }
      }
      else
        temp += curChar;

      ++i;
    }
  }
  int m_args;
  String m_argFormat;
  char m_returnType;
  int m_stackSize;
  funcPtrType m_funcPtr;
  bool m_memberFunction;
  void* m_objPtr;
};

}
