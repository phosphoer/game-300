// David Evans
// 2011/06/29
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Meta.h"
#include "../Containers/String.h"
#include "../Containers/HashMap.h"

namespace Improbability
{

// Base animator class that all interpolaters inherit from
class AnimatorBase
{
public:
  AnimatorBase();
  void UpdateBase(float dt);
  virtual void Update(float dt) = 0;

  float m_time;
  float m_elapsed;
  float m_progress;
  bool m_complete;
  void *m_done_callback_obj;
  String m_done_callback_type;
  String m_done_callback_function;
};

// Simple linear interpolator
template <typename T>
class Animator : public AnimatorBase
{
public:
  void Update(float dt);

  T *m_val;
  T m_begin;
  T m_end;
};

// Interpolator that does not interpolate a value, 
// instead it just calls a callback on every update
class ProgressCallback : public AnimatorBase
{
public:
  void Update(float dt);

  void *m_progress_obj;
  String m_progress_type;
  String m_progress_callback;
};

class PropertyAnimator
{
public:
  typedef HashMap<int, AnimatorBase*> AnimatorMap;
  typedef std::vector<int> IdArray;

  // Animate a variable
  // val - Reference to the value to animate
  // begin - Value to start at
  // end - Value to end at
  // time - Time the animation should take
  // callback_obj - Pointer to an object to recieve messages about the animation
  // callback - Name of a function of callback_obj to invoke when the animation completes
  // T - Type of the value
  // T2 - Type of the callback object
  // return - Id of the animation, used to remove it
  template <typename T, typename T2>
  static int Animate(T &val, T const &begin, T const &end, float time,
                      T2 *callback_obj, char const *callback = 0);

  // Animate a member variable
  // field_name - Name of the field to animate
  // obj - Reference to the object containing the field
  // begin - Value to start at
  // end - Value to end at
  // time - Time to complete the animation in
  // callback_obj - Pointer to an object to recieve messages about the animation
  // callback - Name of a function of callback_obj to invoke when the animation completes
  // T - Type of the object
  // T1 - Type of the field
  // T2 - Type of the callback object
  // return - Id of the animation, used to remove it
  template <typename T, typename T1, typename T2>
  static int AnimateField(char const *field_name, T &obj, T1 const &begin, T1 const &end, 
                           float time, T2 *callback_obj, char const *callback = 0);

  // Animate a member variable
  // field_name - Name of the field to animate
  // obj_type - String type of the object
  // obj - Reference to the object containing the field
  // begin - Value to start at
  // end - Value to end at
  // time - Time to complete the animation in
  // callback_obj - Pointer to an object to recieve messages about the animation
  // callback - Name of a function of callback_obj to invoke when the animation completes
  // T1 - Type of the field
  // T2 - Type of the callback object
  // return - Id of the animation, used to remove it
  template <typename T1, typename T2>
  static int AnimateField(char const *field_name, char const *obj_type, void *obj, 
                           T1 const &begin, T1 const &end, float time, 
                           T2 *callback_obj, char const *callback = 0);

  // Register a callback that gets called each frame with the percent
  // done of an interpolation. This function does not interpolate a value,
  // instead leaving it up to the user's callback to interpolate whatever they
  // want, however they want.
  // time - Time the callback updating should take
  // progress_obj - instance to call progress_callback on
  // progress_callback - Name of the function
  // done_callback - Optional, name of the function to call on progress_obj, when animation is done
  // T - Type of the object to receive the callback
  // return - Id of the animation, used to remove it
  template <typename T>
  static int AddProgressCallback(float time, T *progress_obj, char const *progress_callback,
                                  char const *done_callback = 0);

  // Remove an animation by its id
  // id - The id of the animation to remove
  static void RemoveAnimation(int id);

  // Update the property animator
  // dt - Elapsed time since last update
  static void Update(float dt);

private:
  static int s_current_id;
  static AnimatorMap s_animators;
  static IdArray s_trash;
};

template <typename T, typename T2>
int PropertyAnimator::Animate(T &val, T const &begin, T const &end, float time,
                               T2 *callback_obj, char const *callback)
{
  Animator<T> *animator = new Animator<T>();
  animator->m_val = &val;
  animator->m_begin = begin;
  animator->m_end = end;
  animator->m_time = time;
  animator->m_done_callback_type = Meta::GetMeta<T2>()->GetName();
  animator->m_done_callback_obj = callback_obj;
  animator->m_done_callback_function = callback;

  s_animators[s_current_id++] = reinterpret_cast<AnimatorBase*>(animator);
  return s_current_id - 1;
}

template <typename T, typename T1, typename T2>
int PropertyAnimator::AnimateField(char const *field_name, T &obj, T1 const &begin, T1 const &end, float time,
                                    T2 *callback_obj, char const *callback)
{
  T1 &field = Meta::GetVariable<T1>(field_name, &obj);
  return Animate(field, begin, end, time, callback_obj, callback);
}

template <typename T1, typename T2>
int PropertyAnimator::AnimateField(char const *field_name, char const *obj_type, void *obj, 
                                    T1 const &begin, T1 const &end, float time,
                                    T2 *callback_obj, char const *callback)
{
  T1 &field = Meta::GetVariableByClassName<T1>(field_name, obj_type, obj);
  return Animate(field, begin, end, time, callback_obj, callback); 
}

template <typename T>
int PropertyAnimator::AddProgressCallback(float time, T *progress_obj, char const *progress_callback,
                                           char const *done_callback)
{
  ProgressCallback *animator = new ProgressCallback();
  animator->m_progress_obj = progress_obj;
  animator->m_progress_type = Meta::GetMeta<T>()->GetName();
  animator->m_progress_callback = progress_callback;
  animator->m_time = time;
  animator->m_done_callback_type = animator->m_progress_type;
  animator->m_done_callback_obj = progress_obj;
  animator->m_done_callback_function = done_callback;

  s_animators[s_current_id++] = reinterpret_cast<AnimatorBase*>(animator);
  return s_current_id - 1;
}

template <typename T>
void Animator<T>::Update(float dt)
{
  // Interpolate the value
  (*m_val) = static_cast<T>(m_begin + (m_end - m_begin) * m_progress);
}

}