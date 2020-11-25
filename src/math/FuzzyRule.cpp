// Nicholas Baldwin
// 2011/11/28
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "FuzzyRule.h"

namespace Improbability
{

FuzzyRule::FuzzyRule() : m_expression_tree_head(0)
{
}

FuzzyRule::FuzzyRule(String const &expression) : m_expression_tree_head(0)
{
  AssignExpression(expression);
}

FuzzyRule::~FuzzyRule()
{
  delete m_expression_tree_head;
}

void FuzzyRule::AssignExpression( String const &expression )
{
  delete m_expression_tree_head;

  std::vector<String> tokens = expression.Tokenize();

  ExpressionTreeNode *current_node = 0;

  for(unsigned i = 0; i < tokens.size(); ++i)
  {
    if (tokens[i] == "if")
    { 
      continue;
    }
    else if (tokens[i] == "then") // assumes all preconditions have been stated
    {
      ExpressionTreeNode *new_head = new ExpressionTreeNode;
      new_head->m_type = ExpressionTreeNode::ET_Then;
      new_head->m_value = tokens[i];
      new_head->m_parent = 0;
      new_head->m_right = 0;
      new_head->m_left = m_expression_tree_head;
      m_expression_tree_head->m_parent = new_head;
      m_expression_tree_head = new_head;
      current_node = new_head;
    }
    else if (tokens[i] == "is") // assumes the word prior was a variable
    {
      ErrorIf(current_node->m_type != ExpressionTreeNode::ET_Variable, "unexpected token (%s), expected a variable.", tokens[i].GetCString());

      ExpressionTreeNode *new_node = new ExpressionTreeNode;
      new_node->m_type = ExpressionTreeNode::ET_Is;
      new_node->m_value = tokens[i];
      new_node->m_parent = current_node->m_parent;
      new_node->m_left = current_node;
      if (current_node->m_parent)
      {
        if (current_node->m_parent->m_left->m_value == current_node->m_value)
        {
          current_node->m_parent->m_left = new_node;
        }
        else
        {
          current_node->m_parent->m_right = new_node;
        }
      }
      else // otherwise the variable was the head and we need to update it
      {
        m_expression_tree_head = new_node;
      }
      new_node->m_right = 0;
      current_node = new_node;
    }
    else if (tokens[i] == "and") // assumes the prior word was a value
    {
      ExpressionTreeNode *new_node = new ExpressionTreeNode;
      new_node->m_type = ExpressionTreeNode::ET_And;
      new_node->m_value = tokens[i];
      new_node->m_right = 0;

      if (current_node->m_type == ExpressionTreeNode::ET_Hedge_Value_Undetermined)
      {
        current_node->m_type = ExpressionTreeNode::ET_Value; // set the undetermined node
        new_node->m_parent = current_node->m_parent->m_parent; // get the parent of the 'is' node
        new_node->m_left = current_node->m_parent;
      }
      else
      {
        ErrorIf(current_node->m_type != ExpressionTreeNode::ET_Value, "unexpected token (%s), expected a value", tokens[i].GetCString());

        new_node->m_parent = current_node->m_parent->m_parent->m_parent; // get the parent of the 'is' node
        new_node->m_left = current_node->m_parent->m_parent;
      }

      if (new_node->m_parent) // if the 'is' node has a parent it was the right child
      {
        new_node->m_parent->m_right = new_node;
      }
      else // if it wasn't it was the head of the tree
      {
        m_expression_tree_head = new_node;
      }

      current_node = new_node;
    }
    else if (tokens[i] == "or") // assumes the prior word was a value
    {
      ExpressionTreeNode *new_node = new ExpressionTreeNode;
      new_node->m_type = ExpressionTreeNode::ET_Or;
      new_node->m_value = tokens[i];
      new_node->m_right = 0;

      if (current_node->m_type == ExpressionTreeNode::ET_Hedge_Value_Undetermined)
      {
        current_node->m_type = ExpressionTreeNode::ET_Value; // set the undetermined node
        new_node->m_parent = current_node->m_parent->m_parent; // get the parent of the 'is' node
        new_node->m_left = current_node->m_parent;
      }
      else
      {
        ErrorIf(current_node->m_type != ExpressionTreeNode::ET_Value, "unexpected token (%s), expected a value", tokens[i].GetCString());

        new_node->m_parent = current_node->m_parent->m_parent->m_parent; // get the parent of the 'is' node
        new_node->m_left = current_node->m_parent->m_parent;
      }

      if (new_node->m_parent) // if the 'is' node has a parent it was the right child
      {
        new_node->m_parent->m_right = new_node;
      }
      else // if it wasn't it was the head of the tree
      {
        m_expression_tree_head = new_node;
      }

      current_node = new_node;
    }
    else // the token is either a variable, hedge, value, or invalid
    {
      if (current_node == 0 ||
        current_node->m_type == ExpressionTreeNode::ET_And || 
        current_node->m_type == ExpressionTreeNode::ET_Or || 
        current_node->m_type == ExpressionTreeNode::ET_Then) // token is a variable
      {
        ExpressionTreeNode *new_node = new ExpressionTreeNode;
        new_node->m_type = ExpressionTreeNode::ET_Variable;
        new_node->m_value = tokens[i];
        new_node->m_parent = current_node;
        new_node->m_left = 0;
        new_node->m_right = 0;
        if (current_node)
        {
          current_node->m_right = new_node;
        }
        else // only reason current node is 0 is there is no head yet
        {
          m_expression_tree_head = new_node;
        }
        current_node = new_node;
      }
      else if (current_node->m_type == ExpressionTreeNode::ET_Is) // token is either hedge or value
      {
        ExpressionTreeNode *new_node = new ExpressionTreeNode;
        new_node->m_type = ExpressionTreeNode::ET_Hedge_Value_Undetermined;
        new_node->m_value = tokens[i];
        new_node->m_parent = current_node;
        new_node->m_left = 0;
        new_node->m_right = 0;
        if (current_node)
        {
          current_node->m_right = new_node;
        }
        current_node = new_node;
      }
      else if (current_node->m_type == ExpressionTreeNode::ET_Hedge_Value_Undetermined) // token is value, previous node is hedge
      {
        current_node->m_type = ExpressionTreeNode::ET_Hedge;
        ExpressionTreeNode *new_node = new ExpressionTreeNode;
        new_node->m_type = ExpressionTreeNode::ET_Value;
        new_node->m_value = tokens[i];
        new_node->m_parent = current_node;
        new_node->m_left = 0;
        new_node->m_right = 0;
        if (current_node)
        {
          current_node->m_right = new_node;
        }
        current_node = new_node;
      }
      else
      {
        ErrorIf(true, "token was not expected: %s", tokens[i].GetCString());
      }
    }
  }
}

ExpressionTreeNode* FuzzyRule::GetExpressionTree()
{
  return m_expression_tree_head;
}

ExpressionTreeNode::~ExpressionTreeNode()
{
  // assumes it is being called from its parent so no need to delete that
  delete m_left;
  delete m_right;
}

}