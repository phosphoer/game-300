// Nicholas Baldwin
// 2011/07/27
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

// templated to handle any size matrix
template<typename MatrixT>
class MatrixStack
{
public:
  // constructor loads the identity in first
  MatrixStack()
  {
    MatrixT identity;
    identity.Identity();

    m_stack.push_back(identity);
  }

  // make the current top into an identity
  void LoadIdentity()
  {
    m_stack.back().Identity();
  }

  // concatenate a matrix with the top and make it the new top
  void LoadMatrix(MatrixT const &matrix)
  {
    m_stack.back() = m_stack.back() * matrix;
  }

  // make a new top from the last one
  void Push()
  {
    m_stack.push_back(m_stack.back());
  }
  
  // pop the current top
  void Pop()
  {
    m_stack.pop_back();
  }

  // return the top of the stack
  MatrixT Top()
  {
    return m_stack.back();
  }

private:
  // the stack
  std::vector<MatrixT> m_stack;
};

}