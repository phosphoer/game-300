// Nicholas Baldwin
// 2011/11/07
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

class FuzzyController;

class FuzzyVariable
{
public:
  FuzzyVariable(String const &name);

  // creates a new triangular membership function
  // value - the name of the membership function
  // p0 - the starting point of the membership function
  // p1 - the apex of the membership function
  // p2 - the ending point of the membership function
  void AddValue(String const &value, float p0, float p1, float p2);

  // creates a new trapezoidal membership function
  // value - the name of the membership function
  // p0 - the starting point of the membership function
  // p1 - the starting apex of the membership function
  // p2 - the ending apex of the membership function
  // p3 - the ending point of the membership function
  void AddValue(String const &value, float p0, float p1, float p2, float p3);

  // sets the variable to input and stores the value such that the output's defuzzify will return 
  // the correct value
  // value - the value to set the fuzzy variable to
  void Fuzzify(float value);

  // returns the value variable stored, if Evaluate is called on the corresponding fuzzy controller
  // it will run the rules and correctly store the defuzzified value in this variable.
  float Defuzzify();

private:
  friend class FuzzyController;

  String m_name;
  float m_value;
  FuzzyController *m_controller;

  struct MembershipFunction
  {
    float p0, p1, p2, p3;
  };
  HashMap<String, MembershipFunction> m_membership_functions;
};

}