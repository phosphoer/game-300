// David Evans
// 2011/06/29
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "PropertyAnimator.h"

namespace Improbability
{

PropertyAnimator::AnimatorMap PropertyAnimator::s_animators;
PropertyAnimator::IdArray PropertyAnimator::s_trash;
int PropertyAnimator::s_current_id = 1;

AnimatorBase::AnimatorBase():
  m_time(0), 
  m_elapsed(0), 
  m_progress(0), 
  m_complete(false), 
  m_done_callback_obj(0)
{
}

void AnimatorBase::UpdateBase(float dt)
{
  // Update progress
  m_elapsed += dt;
  m_progress = m_elapsed / m_time;
  if (m_progress > 1.0f) 
    m_progress = 1.0f;

  // Call inherited method to do interpolation
  Update(dt);

  // Check if we are done
  if (m_progress >= 1.0f && !m_complete)
  {
    m_complete = true;

    // Invoke the complete callback
    if (m_done_callback_function.GetLength() > 0 && m_done_callback_obj)
      Meta::Invoke(m_done_callback_type, m_done_callback_function, m_done_callback_obj);
  }
}

void ProgressCallback::Update(float dt)
{
  // Invoke the progress callback
  if (m_progress_callback.GetLength() > 0 && m_progress_obj)
    Meta::Invoke(m_progress_type, m_progress_callback, m_progress_obj, m_progress, dt, m_time);
}

void PropertyAnimator::RemoveAnimation(int id)
{
  ErrorIf(s_animators.Find(id) == s_animators.End(), "PropertyAnimator::RemoveAnimation: Invalid ID of %i.", id);
  s_trash.push_back(id);
}

void PropertyAnimator::Update(float dt)
{
  // Remove things
  auto it_trash = s_trash.begin();
  auto end_trash = s_trash.end();
  for (; it_trash != end_trash; ++it_trash)
    s_animators.Erase(*it_trash);
  s_trash.clear();

  // Iterate over all animators
  auto it = s_animators.Begin();
  auto end = s_animators.End();
  for (; it != end; ++it)
  {
    // Remove any animators that are complete
    AnimatorBase *animator = it->second;
    animator->UpdateBase(dt);
    if (animator->m_complete)
      s_trash.push_back(it->first);
  }
}

}
