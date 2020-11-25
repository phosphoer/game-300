// Nicholas Baldwin
// 2011/11/05
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

class FuzzyVariable;
class FuzzyRule;
//class FuzzyHedge;
struct ExpressionTreeNode;

class FuzzyController
{
public:
  static void BindMeta(MetaComposition *comp);

  void Register(FuzzyVariable *new_variable);
  void Register(FuzzyRule *new_rule);
  //void Register(FuzzyHedge *new_hedge);

  void Compile();

  void Evaluate();

private:

  void EvaluateSubTree(ExpressionTreeNode *node);

  HashMap<String, FuzzyVariable*> m_variables;
  //HashMap<String, FuzzyHedge*> m_hedges;

  std::vector<FuzzyRule*> m_rules;
};

}