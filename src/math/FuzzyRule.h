// Nicholas Baldwin
// 2011/11/28
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

struct ExpressionTreeNode
{
  ~ExpressionTreeNode();

  // used to parse the tree, will return 0 when you've reached the end
  ExpressionTreeNode* GetFirst();
  ExpressionTreeNode* GetNextInOrder();

  enum ExpressionType
  {
    ET_Null = 0,
    ET_Is,
    ET_Then,
    ET_And,
    ET_Or,
    ET_Variable,
    ET_Value,
    ET_Hedge,
    ET_Hedge_Value_Undetermined,

    ET_Max
  };

  ExpressionType m_type;
  String m_value;
  ExpressionTreeNode *m_parent;
  ExpressionTreeNode *m_right;
  ExpressionTreeNode *m_left;
};

class FuzzyRule
{
public:
  FuzzyRule();
  FuzzyRule(String const &expression);
  ~FuzzyRule();

  void AssignExpression(String const &expression);

  // returns null if there is no expression
  ExpressionTreeNode* GetExpressionTree();

private:
  ExpressionTreeNode *m_expression_tree_head;
};

}