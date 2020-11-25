// David Evans
// 2011/07/14
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Utilities/DynamicValue.h"
#include "../Utilities/Variable.h"
#include "../Containers/String.h"
#include "../Utilities/DebugErrors.h"
#include "MetaComposition.h"

namespace Improbability
{

// The base memberfunction class
// Defines the possible parameter numbers
class MemberFnBase
{
public:
  MemberFnBase():
    m_return_type("void"),
    m_param0_type("void"),
    m_param1_type("void"),
    m_param2_type("void"),
    m_param3_type("void")
  {
  }

  // Invoke the member function on the pointed object
  // self - Object to invoke member function on
  // ... - Up to 4 parameters
  virtual void Invoke(void *self) {ErrorIf(1, "Invalid meta function call.");}
  virtual void Invoke(void *self, DynamicValue) {ErrorIf(1, "Invalid meta function call.");}
  virtual void Invoke(void *self, DynamicValue, DynamicValue) {ErrorIf(1, "Invalid meta function call.");}
  virtual void Invoke(void *self, DynamicValue, DynamicValue, DynamicValue) {ErrorIf(1, "Invalid meta function call.");}
  virtual void Invoke(void *self, DynamicValue, DynamicValue, DynamicValue, DynamicValue) {ErrorIf(1, "Invalid meta function call.");}

  // Invoke the member function on the pointed object
  // self- Object to invoke function on
  // ... - Up to 4 parameters with return type
  virtual Variable InvokeRet(void *self) {ErrorIf(1, "Invalid meta function call."); return 0;}
  virtual Variable InvokeRet(void *self, DynamicValue) {ErrorIf(1, "Invalid meta function call."); return 0;}
  virtual Variable InvokeRet(void *self, DynamicValue, DynamicValue) {ErrorIf(1, "Invalid meta function call."); return 0;}
  virtual Variable InvokeRet(void *self, DynamicValue, DynamicValue, DynamicValue) {ErrorIf(1, "Invalid meta function call."); return 0;}
  virtual Variable InvokeRet(void *self, DynamicValue, DynamicValue, DynamicValue, DynamicValue) {ErrorIf(1, "Invalid meta function call."); return 0;}

  // Get signature information
  String GetReturnType() {return m_return_type;}
  String GetParam0Type() {return m_param0_type;}
  String GetParam1Type() {return m_param1_type;}
  String GetParam2Type() {return m_param2_type;}
  String GetParam3Type() {return m_param3_type;}

protected:
  // Signature information
  String m_return_type;
  String m_param0_type;
  String m_param1_type;
  String m_param2_type;
  String m_param3_type;
};

// Templated memberfunction class for a function with no parameters
// T - Class the memberfunction is a part of
template <typename T>
class MemberFn00 : public MemberFnBase
{
public:
  // Constructor
  MemberFn00(void (T::*method)()) : m_method(method) {}

  // Invokes the member function
  // self - Object to invoke function on
  void Invoke(void *self) 
  {
    T *obj = (T*)self; (obj->*m_method)();
  }

private:
  // The member function pointer
  void (T::*m_method)();
};

// Memberfunction class for a single parameter
// T - Class type
// P0 - Type of first parameter
template <typename T, typename P0>
class MemberFn01 : public MemberFnBase
{
public:
  // Constructor
  MemberFn01(void (T::*method)(P0)) : m_method(method) 
  {
    m_param0_type = MetaGetter<P0>::s_meta->GetName();
    if (m_param0_type == "char const *") m_param0_type = "String";
  }

  // Invoke the member function
  // self - Object to invoke function on
  // p0 - Parameter to function
  void Invoke(void *self, DynamicValue p0) 
  {
    char const *class_name = MetaGetter<T>::s_meta->GetName();
    ErrorIf(m_param0_type != p0.GetType(), "%s: Given %s, expected %s.", class_name, p0.GetType().GetCString(), m_param0_type.GetCString());
    T *obj = (T*)self; (obj->*m_method)(p0);
  }

private:
  // The member function pointer
  void (T::*m_method)(P0 p0);
};

// Memberfunction class for a single parameter
// T - Class type
// P0...P1 - Types of parameters
template <typename T, typename P0, typename P1>
class MemberFn02 : public MemberFnBase
{
public:
  // Constructor
  MemberFn02(void (T::*method)(P0, P1)) : m_method(method)
  {
    m_param0_type = MetaGetter<P0>::s_meta->GetName();
    m_param1_type = MetaGetter<P1>::s_meta->GetName();
    if (m_param0_type == "char const *") m_param0_type = "String";
    if (m_param1_type == "char const *") m_param1_type = "String";
  }

  // Invoke the member function
  // self - Object to invoke function on
  // p0...p1 - Parameters to function
  void Invoke(void *self, DynamicValue p0, DynamicValue p1) 
  {
    char const *class_name = MetaGetter<T>::s_meta->GetName();
    ErrorIf(m_param0_type != p0.GetType(), "%s: Given %s, expected %s.", class_name, p0.GetType().GetCString(), m_param0_type.GetCString());
    ErrorIf(m_param1_type != p1.GetType(), "%s: Given %s, expected %s.", class_name, p1.GetType().GetCString(), m_param1_type.GetCString());
    T *obj = (T*)self; (obj->*m_method)(p0, p1);
  }

private:
  // The member function pointer
  void (T::*m_method)(P0 p0, P1 p1);
};

// Memberfunction class for a single parameter
// T - Class type
// P0...P2 - Types of parameters
template <typename T, typename P0, typename P1, typename P2>
class MemberFn03 : public MemberFnBase
{
public:
  // Constructor
  MemberFn03(void (T::*method)(P0, P1, P2)) : m_method(method)
  {
    m_param0_type = MetaGetter<P0>::s_meta->GetName();
    m_param1_type = MetaGetter<P1>::s_meta->GetName();
    m_param2_type = MetaGetter<P2>::s_meta->GetName();
    if (m_param0_type == "char const *") m_param0_type = "String";
    if (m_param1_type == "char const *") m_param1_type = "String";
    if (m_param2_type == "char const *") m_param2_type = "String";
  }

  // Invoke the member function
  // self - Object to invoke function on
  // p0...p2 - Parameters to function
  void Invoke(void *self, DynamicValue p0, DynamicValue p1, DynamicValue p2) 
  {
    char const *class_name = MetaGetter<T>::s_meta->GetName();
    ErrorIf(m_param0_type != p0.GetType(), "%s: Given %s, expected %s.", class_name, p0.GetType().GetCString(), m_param0_type.GetCString());
    ErrorIf(m_param1_type != p1.GetType(), "%s: Given %s, expected %s.", class_name, p1.GetType().GetCString(), m_param1_type.GetCString());
    ErrorIf(m_param2_type != p2.GetType(), "%s: Given %s, expected %s.", class_name, p2.GetType().GetCString(), m_param2_type.GetCString());
    T *obj = (T*)self; (obj->*m_method)(p0, p1, p2);
  }

private:
  // The member function pointer
  void (T::*m_method)(P0 p0, P1 p1, P2 p2);
};

// Memberfunction class for a single parameter
// T - Class type
// P0...P3 - Types of parameters
template <typename T, typename P0, typename P1, typename P2, typename P3>
class MemberFn04 : public MemberFnBase
{
public:
  // Constructor
  MemberFn04(void (T::*method)(P0, P1, P2, P3)) : m_method(method)
  {
    m_param0_type = MetaGetter<P0>::s_meta->GetName();
    m_param1_type = MetaGetter<P1>::s_meta->GetName();
    m_param2_type = MetaGetter<P2>::s_meta->GetName();
    m_param3_type = MetaGetter<P3>::s_meta->GetName();
    if (m_param0_type == "char const *") m_param0_type = "String";
    if (m_param1_type == "char const *") m_param1_type = "String";
    if (m_param2_type == "char const *") m_param2_type = "String";
    if (m_param3_type == "char const *") m_param3_type = "String";
  }

  // Invoke the member function
  // self - Object to invoke function on
  // p0...p3 - Parameters to function
  void Invoke(void *self, DynamicValue p0, DynamicValue p1, DynamicValue p2, DynamicValue p3) 
  {
    char const *class_name = MetaGetter<T>::s_meta->GetName();
    ErrorIf(m_param0_type != p0.GetType(), "%s: Given %s, expected %s.", class_name, p0.GetType().GetCString(), m_param0_type.GetCString());
    ErrorIf(m_param1_type != p1.GetType(), "%s: Given %s, expected %s.", class_name, p1.GetType().GetCString(), m_param1_type.GetCString());
    ErrorIf(m_param2_type != p2.GetType(), "%s: Given %s, expected %s.", class_name, p2.GetType().GetCString(), m_param2_type.GetCString());
    ErrorIf(m_param3_type != p3.GetType(), "%s: Given %s, expected %s.", class_name, p3.GetType().GetCString(), m_param3_type.GetCString());
    T *obj = (T*)self; (obj->*m_method)(p0, p1, p2, p3);
  }

private:
  // The member function pointer
  void (T::*m_method)(P0 p0, P1 p1, P2 p2, P3 p3);
};

// Templated memberfunction class for a function with no parameters
// T - Class the memberfunction is a part of
template <typename T, typename R>
class MemberFnRet00 : public MemberFnBase
{
public:
  // Constructor
  MemberFnRet00(R (T::*method)()) : m_method(method)
  {
    m_return_type = MetaGetter<R>::s_meta->GetName();
    if (m_return_type == "char const *") m_return_type = "String";
  }

  // Invokes the member function
  // self - Object to invoke function on
  Variable InvokeRet(void *self) {T *obj = (T*)self; return (obj->*m_method)();}

private:
  // The member function pointer
  R (T::*m_method)();
};

// Templated memberfunction class for a function with one parameter
// T - Class the memberfunction is a part of
template <typename T, typename R, typename P0>
class MemberFnRet01 : public MemberFnBase
{
public:
  // Constructor
  MemberFnRet01(R (T::*method)(P0)) : m_method(method)
  {
    m_return_type = MetaGetter<R>::s_meta->GetName();
    m_param0_type = MetaGetter<P0>::s_meta->GetName();
    if (m_return_type == "char const *") m_return_type = "String";
    if (m_param0_type == "char const *") m_param0_type = "String";
  }

  // Invokes the member function
  // self - Object to invoke function on
  Variable InvokeRet(void *self, DynamicValue p0) 
  {
    char const *class_name = MetaGetter<T>::s_meta->GetName();
    ErrorIf(m_param0_type != p0.GetType(), "%s: Given %s, expected %s.", class_name, p0.GetType().GetCString(), m_param0_type.GetCString());
    T *obj = (T*)self; return (obj->*m_method)(p0);
  }

private:
  // The member function pointer
  R (T::*m_method)(P0);
};

// Templated memberfunction class for a function with two parameters
// T - Class the memberfunction is a part of
template <typename T, typename R, typename P0, typename P1>
class MemberFnRet02 : public MemberFnBase
{
public:
  // Constructor
  MemberFnRet02(R (T::*method)(P0, P1)) : m_method(method)
  {
    m_return_type = MetaGetter<R>::s_meta->GetName();
    m_param0_type = MetaGetter<P0>::s_meta->GetName();
    m_param1_type = MetaGetter<P1>::s_meta->GetName();
    if (m_return_type == "char const *") m_return_type = "String";
    if (m_param0_type == "char const *") m_param0_type = "String";
    if (m_param1_type == "char const *") m_param1_type = "String";
  }

  // Invokes the member function
  // self - Object to invoke function on
  Variable InvokeRet(void *self, DynamicValue p0, DynamicValue p1) 
  {
    char const *class_name = MetaGetter<T>::s_meta->GetName();
    ErrorIf(m_param0_type != p0.GetType(), "%s: Given %s, expected %s.", class_name, p0.GetType().GetCString(), m_param0_type.GetCString());
    ErrorIf(m_param1_type != p1.GetType(), "%s: Given %s, expected %s.", class_name, p1.GetType().GetCString(), m_param1_type.GetCString());
    T *obj = (T*)self; return (obj->*m_method)(p0, p1);
  }

private:
  // The member function pointer
  R (T::*m_method)(P0, P1);
};

// Templated memberfunction class for a function with three parameters
// T - Class the memberfunction is a part of
template <typename T, typename R, typename P0, typename P1, typename P2>
class MemberFnRet03 : public MemberFnBase
{
public:
  // Constructor
  MemberFnRet03(R (T::*method)(P0, P1, P2)) : m_method(method)
  {
    m_return_type = MetaGetter<R>::s_meta->GetName();
    m_param0_type = MetaGetter<P0>::s_meta->GetName();
    m_param1_type = MetaGetter<P1>::s_meta->GetName();
    m_param2_type = MetaGetter<P2>::s_meta->GetName();
    if (m_return_type == "char const *") m_return_type = "String";
    if (m_param0_type == "char const *") m_param0_type = "String";
    if (m_param1_type == "char const *") m_param1_type = "String";
    if (m_param2_type == "char const *") m_param2_type = "String";
  }

  // Invokes the member function
  // self - Object to invoke function on
  Variable InvokeRet(void *self, DynamicValue p0, DynamicValue p1, DynamicValue p2) 
  {
    char const *class_name = MetaGetter<T>::s_meta->GetName();
    ErrorIf(m_param0_type != p0.GetType(), "%s: Given %s, expected %s.", class_name, p0.GetType().GetCString(), m_param0_type.GetCString());
    ErrorIf(m_param1_type != p1.GetType(), "%s: Given %s, expected %s.", class_name, p1.GetType().GetCString(), m_param1_type.GetCString());
    ErrorIf(m_param2_type != p2.GetType(), "%s: Given %s, expected %s.", class_name, p2.GetType().GetCString(), m_param2_type.GetCString());
    T *obj = (T*)self; return (obj->*m_method)(p0, p1, p2);
  }

private:
  // The member function pointer
  R (T::*m_method)(P0, P1, P2);
};

// Templated memberfunction class for a function with three parameters
// T - Class the memberfunction is a part of
template <typename T, typename R, typename P0, typename P1, typename P2, typename P3>
class MemberFnRet04 : public MemberFnBase
{
public:
  // Constructor
  MemberFnRet04(R (T::*method)(P0, P1, P2, P3)) : m_method(method)
  {
    m_return_type = MetaGetter<R>::s_meta->GetName();
    m_param0_type = MetaGetter<P0>::s_meta->GetName();
    m_param1_type = MetaGetter<P1>::s_meta->GetName();
    m_param2_type = MetaGetter<P2>::s_meta->GetName();
    m_param3_type = MetaGetter<P3>::s_meta->GetName();
    if (m_return_type == "char const *") m_return_type = "String";
    if (m_param0_type == "char const *") m_param0_type = "String";
    if (m_param1_type == "char const *") m_param1_type = "String";
    if (m_param2_type == "char const *") m_param2_type = "String";
    if (m_param3_type == "char const *") m_param3_type = "String";
  }

  // Invokes the member function
  // self - Object to invoke function on
  Variable InvokeRet(void *self, DynamicValue p0, DynamicValue p1, DynamicValue p2, DynamicValue p3)
  {
    char const *class_name = MetaGetter<T>::s_meta->GetName();
    ErrorIf(m_param0_type != p0.GetType(), "%s: Given %s, expected %s.", class_name, p0.GetType().GetCString(), m_param0_type.GetCString());
    ErrorIf(m_param1_type != p1.GetType(), "%s: Given %s, expected %s.", class_name, p1.GetType().GetCString(), m_param1_type.GetCString());
    ErrorIf(m_param2_type != p2.GetType(), "%s: Given %s, expected %s.", class_name, p2.GetType().GetCString(), m_param2_type.GetCString());
    ErrorIf(m_param3_type != p3.GetType(), "%s: Given %s, expected %s.", class_name, p3.GetType().GetCString(), m_param3_type.GetCString());
    T *obj = (T*)self; return (obj->*m_method)(p0, p1, p2, p3);
  }

private:
  // The member function pointer
  R (T::*m_method)(P0, P1, P2, P3);
};


}