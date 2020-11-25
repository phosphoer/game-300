// David Evans
// 2011/05/23
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "MetaConstants.h"
#include "MetaComposition.h"
#include "../Python/PyClass.h"
// Note, the MetaGetter for the const reference is intentionally
// pointing at the regular meta composition. It is not a typo.


// Create a meta description of a type
// T - The type to make meta for
#define MakeMeta(T) \
  MetaComposition g_##T##_metabase(#T, T::BindMeta); \
  static const bool g_##T##_py = PyClass::CreateClass<T>(#T); \
  MetaComposition g_##T##_p_metabase(#T "*", T::BindMeta); \
  MetaComposition g_##T##_cp_metabase(#T " const *", T::BindMeta); \
  MetaComposition g_##T##_r_metabase(#T "&", T::BindMeta); \
  MetaComposition g_##T##_cr_metabase(#T " const &", T::BindMeta); \
  MetaComposition* MetaGetter<T>::s_meta = &g_##T##_metabase; \
  MetaComposition* MetaGetter<T*>::s_meta = &g_##T##_p_metabase; \
  MetaComposition* MetaGetter<T const*>::s_meta = &g_##T##_cp_metabase; \
  MetaComposition* MetaGetter<T &>::s_meta = &g_##T##_r_metabase; \
  MetaComposition* MetaGetter<T const&>::s_meta = &g_##T##_metabase; \
  static char const *ThisTypeName = #T; \
  typedef T ThisType; \
  ForceLinkThis(T);

#define MakeMetaExternal(T, BindFunction) \
  MetaComposition g_##T##_metabase(#T, BindFunction); \
  MetaComposition g_##T##_p_metabase(#T "*", BindFunction); \
  MetaComposition g_##T##_cp_metabase(#T " const *", BindFunction); \
  MetaComposition g_##T##_r_metabase(#T "&", BindFunction); \
  MetaComposition g_##T##_cr_metabase(#T " const &", BindFunction); \
  MetaComposition* MetaGetter<T>::s_meta = &g_##T##_metabase; \
  MetaComposition* MetaGetter<T*>::s_meta = &g_##T##_p_metabase; \
  MetaComposition* MetaGetter<T const*>::s_meta = &g_##T##_cp_metabase; \
  MetaComposition* MetaGetter<T &>::s_meta = &g_##T##_r_metabase; \
  MetaComposition* MetaGetter<T const&>::s_meta = &g_##T##_metabase; \
  typedef T ThisType; \
  ForceLinkThis(T);

// Create a meta description of a type
// T - The type to make meta for
#define MakeMetaBasic(T) \
  MetaComposition g_##T##_metabase(#T, 0); \
  MetaComposition g_##T##_p_metabase(#T "*", 0); \
  MetaComposition g_##T##_cp_metabase(#T " const *", 0); \
  MetaComposition g_##T##_r_metabase(#T "&", 0); \
  MetaComposition g_##T##_cr_metabase(#T " const &", 0); \
  MetaComposition* MetaGetter<T>::s_meta = &g_##T##_metabase; \
  MetaComposition* MetaGetter<T*>::s_meta = &g_##T##_p_metabase; \
  MetaComposition* MetaGetter<T const*>::s_meta = &g_##T##_cp_metabase; \
  MetaComposition* MetaGetter<T &>::s_meta = &g_##T##_r_metabase; \
  MetaComposition* MetaGetter<T const&>::s_meta = &g_##T##_metabase; \
  ForceLinkThis(T);

// Get the meta description of a type
// T - The type to get meta for
#define AccessMeta(T) MetaGetter<T>::s_meta

// Make meta for a global constant value
// type - Type of the constant
// name - Name of the constant
// value - Value of the constant
#define MakeMetaConstant(type, name, value) \
  MetaConstant<type> g_##name##_constant(#name, value); \
  ForceLinkThis(name);
