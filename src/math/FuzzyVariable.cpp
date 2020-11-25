// Nicholas Baldwin
// 2011/11/28
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "FuzzyVariable.h"

namespace Improbability
{

FuzzyVariable::FuzzyVariable( String const &name ) : m_name(name)
{
}

void FuzzyVariable::AddValue( String const &value, float p0, float p1, float p2 )
{
  AddValue(value, p0, p1, p1, p2);
}

void FuzzyVariable::AddValue( String const &value, float p0, float p1, float p2, float p3 )
{
  MembershipFunction new_function = {p0, p1, p2, p3};

  m_membership_functions[value] = new_function;
}

void FuzzyVariable::Fuzzify( float value )
{
  m_value = value;
}

float FuzzyVariable::Defuzzify()
{
  return m_value;
}

}