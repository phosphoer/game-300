// Nicholas Baldwin
// 2011/11/28
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "FuzzyController.h"
#include "FuzzyRule.h"
#include "FuzzyVariable.h"

namespace Improbability
{

MakeMeta(FuzzyController);

void FuzzyController::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void FuzzyController::Register(FuzzyVariable *new_variable)
{
  m_variables[new_variable->m_name] = new_variable;
}

void FuzzyController::Register(FuzzyRule *new_rule)
{
  m_rules.push_back(new_rule);
}

// void FuzzyController::Register(FuzzyHedge *new_hedge)
// {
//   m_hedges[new_hedge->m_name] = new_hedge;
// }

void FuzzyController::Evaluate()
{
  // evaluate all of the rules
  for (unsigned i = 0; i < m_rules.size(); ++i)
  {
    EvaluateSubTree(m_rules[i]->GetExpressionTree());
  }
}

void FuzzyController::EvaluateSubTree(ExpressionTreeNode *node)
{
  switch(node->m_type)
  {
  case ExpressionTreeNode::ET_Then:
    break;
  case ExpressionTreeNode::ET_Is:
    break;
  case ExpressionTreeNode::ET_And:
    break;
  case ExpressionTreeNode::ET_Or:
    break;
  }
}

}