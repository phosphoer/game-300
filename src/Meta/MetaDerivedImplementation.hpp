// David Evans
// 2011/05/23
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

namespace Improbability
{

// The constructor for the metadata class adds it to
// the global linked list
template <typename T>
MetaData<T>::MetaData(char const *name, unsigned size):
  MetaBase(),
  m_name(name),
  m_size(size)
{
  // Insert the meta data into the linked list
  if (!g_meta_head)
  {
    g_meta_head = this;
  }
  else
  {
    m_next = g_meta_head;
    g_meta_head = this;
  }

  // Assign unique id and increment global one
  m_id = MetaBase::s_unique_id++;

  // Bind member functions
  T::BindMeta();

  g_meta_tail = this;
}

template <typename T>
void MetaData<T>::RegisterVariable(char const *name, int offset)
{
  auto it = m_variables.find(name);
  ErrorIf(it != m_variables.end(), "Variable %s is already registered!", name);
  m_variables[name] = offset;
}

template <typename T>
void* MetaData<T>::GetVariable(char const *name, void *self)
{
  auto it = m_variables.find(name);
  if (it == m_variables.end())
    return 0;

  size_t offset = it->second;
  char *self_byte = (char *)self;
  void *var = (void*)(self_byte + offset);
  return var;
}

template <typename T>
void MetaData<T>::GetVariables(void *self, std::vector<std::pair<String, DynamicValue> > &vars)
{
  auto it = m_variables.begin();
  auto end = m_variables.end();
  for (; it != end; ++it)
  {
    String name = it->first;
    size_t offset = it->second;

    char *self_byte = (char *)self;
    void *var = (void*)(self_byte + offset);
    vars.push_back(std::pair<String, DynamicValue>(name, var));
  }
}

template <typename T>
bool MetaData<T>::HasMethod(char const *fn)
{
  auto it = m_functions.find(fn);
  return it != m_functions.end();
}

template <typename T>
void MetaData<T>::RegisterFunction(char const *name, void (T::*method)())
{
  auto it = m_functions.find(name);
  ErrorIf(it != m_functions.end(), "Function %s already registered!", name);
  m_functions.insert(std::pair<String, MemberFnBase*>(name, new MemberFn00<T>(method)));
}

template <typename T>
template <typename P0>
void MetaData<T>::RegisterFunction(char const *name, void (T::*method)(P0 p0))
{
  auto it = m_functions.find(name);
  ErrorIf(it != m_functions.end(), "Function %s already registered!", name);
  m_functions.insert(std::pair<String, MemberFnBase*>(name, new MemberFn01<T, P0>(method)));
}

template <typename T>
template <typename P0, typename P1>
void MetaData<T>::RegisterFunction(char const *name, void (T::*method)(P0 p0, P1 p1))
{
  auto it = m_functions.find(name);
  ErrorIf(it != m_functions.end(), "Function %s already registered!", name);
  m_functions.insert(std::pair<String, MemberFnBase*>(name, new MemberFn02<T, P0, P1>(method)));
}

template <typename T>
template <typename P0, typename P1, typename P2>
void MetaData<T>::RegisterFunction(char const *name, void (T::*method)(P0 p0, P1 p1, P2 p2))
{
  auto it = m_functions.find(name);
  ErrorIf(it != m_functions.end(), "Function %s already registered!", name);
  m_functions.insert(std::pair<String, MemberFnBase*>(name, new MemberFn03<T, P0, P1, P2>(method)));
}

template <typename T>
template <typename P0, typename P1, typename P2, typename P3>
void MetaData<T>::RegisterFunction(char const *name, void (T::*method)(P0 p0, P1 p1, P2 p2, P3 p3))
{
  auto it = m_functions.find(name);
  ErrorIf(it != m_functions.end(), "Function %s already registered!", name);
  m_functions.insert(std::pair<String, MemberFnBase*>(name, new MemberFn04<T, P0, P1, P2, P3>(method)));
}

template <typename T>
void MetaData<T>::Invoke(char const *fn, void *self)
{
  auto it = m_functions.find(fn);
  ErrorIf(it == m_functions.end(), "Couldn't find function %s!", fn);
  it->second->Invoke(self);
}

template <typename T>
void MetaData<T>::Invoke(char const *fn, void *self, DynamicValue p0)
{
  auto it = m_functions.find(fn);
  ErrorIf(it == m_functions.end(), "Couldn't find function %s!", fn);
  it->second->Invoke(self, p0);
}

template <typename T>
void MetaData<T>::Invoke(char const *fn, void *self, DynamicValue p0, DynamicValue p1)
{
  auto it = m_functions.find(fn);
  ErrorIf(it == m_functions.end(), "Couldn't find function %s!", fn);
  it->second->Invoke(self, p0, p1);
}

template <typename T>
void MetaData<T>::Invoke(char const *fn, void *self, DynamicValue p0, DynamicValue p1, DynamicValue p2)
{
  auto it = m_functions.find(fn);
  ErrorIf(it == m_functions.end(), "Couldn't find function %s!", fn);
  it->second->Invoke(self, p0, p1, p2);
}

template <typename T>
void MetaData<T>::Invoke(char const *fn, void *self, DynamicValue p0, DynamicValue p1, DynamicValue p2, DynamicValue p3)
{
  auto it = m_functions.find(fn);
  ErrorIf(it == m_functions.end(), "Couldn't find function %s!", fn);
  it->second->Invoke(self, p0, p1, p2, p3);
}

}
