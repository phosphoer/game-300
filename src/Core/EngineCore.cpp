// David Evans
// 2011/05/12
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "EngineCore.h"
#include "Space.h"
#include "Timer.h"
#include "WindowSystem.h"
#include "../FileIO/Serializer.h"
#include "../Resource/ResourceManager.h"
#include "../Resource/FilepathResource.h"
#include "../Meta/PropertyAnimator.h"

namespace Improbability
{

EngineCore *g_engine_core;

//Player State Globals
int g_coin_count = 0;
int g_curr_coin_count = 0;
int g_coins_needed = 0;
bool g_rotate_blocks = false;
bool g_rotate_ability = false;
bool g_anchor_blocks = false;
bool g_anchor_ability = false;
bool g_the_end = false;
bool g_nega_binding = false;
int g_player_facing = -1;
int g_curr_level = 1;

//Progress Variables
bool g_coin_get = false;
bool g_coins_complete = false;
bool g_moved = false;
bool g_jumped = false;
bool g_first_rotate = false;
bool g_first_level = false;
bool g_second_level = false;
bool g_final_level = false;
bool g_unlocked_rblocks = false;
bool g_unlocked_rability = false;
bool g_unlocked_ablocks = false;
bool g_unlocked_aability = false;
bool g_first_anchor = false;
bool g_anchor_cleared = false;
bool g_anchor_dropped = false;
bool g_anchor_set = false;
bool g_anchor_tried = false;
bool g_free_rotated = false;
bool g_warped_back = false;
bool g_walked_behind = false;

//Saving and loading
bool g_isLoaded = false;

// lost device hack
extern int g_reset_device_count = 0;

MakeMeta(EngineCore);

void EngineCore::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnChangeSpace);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnPushSpace);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnPopSpace);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnResetSpace);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnWindowClose);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnExit);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnKeyDown);
}

EngineCore::EngineCore():
  m_resource_manager(0),
  m_is_running(false),
  m_pending_push(false),
  m_pending_pop(false),
  m_pending_change(false),
  m_reset_space(false),
  m_send_load_msg(false),
  m_pop_copy(false)
{
  g_engine_core = this;

  // Load Resources
  m_resource_manager = new ResourceManager();
  m_resource_manager->Load("res");

  // We need a base space and layer to register events in
  m_root_space = new Space();
  HashMap<String, Space*> root_layer;
  root_layer["root"] = m_root_space;
  m_spaces.push_back(m_root_space);
  m_layers.push_back(root_layer);
  m_root_space->Initialize("root");
  m_root_space->SetName("root");

  // Tell the resource manager to listen for on load
  AddEventListener(MessageBank::OnLoadPass, m_resource_manager);
}

EngineCore::~EngineCore()
{

}

void EngineCore::Initialize(char const *space_file, char const *config_file_path)
{
  // Load resources
  DispatchEvent(MessageBank::OnLoadPass, "PreInitialize");

  // Listen for window events
  AddEventListener(MessageBank::OnPushSpace, this);
  AddEventListener(MessageBank::OnPopSpace, this);
  AddEventListener(MessageBank::OnChangeSpace, this);
  AddEventListener(MessageBank::OnResetSpace, this);
  AddEventListener(MessageBank::OnWindowClose, this);
  AddEventListener(MessageBank::OnExit, this);
  AddEventListener(MessageBank::OnKeyDown, this);

  // Create window and load it
  m_window = new WindowSystem();
  Serializer loader(config_file_path);
  Read(loader, *m_window);

  // Show the window
  m_window->Initialize();
  
  PushSpace(space_file);
}

void EngineCore::MainLoop()
{
  ErrorIf(m_spaces.size() == 0, "The main loop cannot be invoked before a space is pushed.");
  m_is_running = true;

  // Start the main loop
  Timer timer;
  timer.GetDt();
  while (m_is_running)
  {
    // Update the active space
    float dt = timer.GetDt();
    Space *space = m_spaces.back();
    space->Update(dt);
    m_window->Update();

    // Update the property animator
    PropertyAnimator::Update(dt);

    // Perform pending space changes pushes and pops
    if (m_pending_change)
    {
      ChangeSpaceNow();
      m_pending_change = false;
    }
    else if (m_pending_push)
    {
      PushSpaceNow();
      m_pending_push = false;
    }
    else if (m_pending_pop)
    {
      PopSpaceNow();
      m_pending_pop = false;
    }
  }
}

void EngineCore::Uninitialize()
{
  // Pop all spaces, thereby uninitializing them
  while (m_spaces.size() > 0)
    PopSpaceNow();

  m_window->Uninitialize();

  // Delete all spaces in all layers
  auto end_layer = m_layers.end();
  for (auto layer = m_layers.begin(); layer != end_layer; ++layer)
  {
    auto end_it = layer->End();
    for (auto it = layer->Begin(); it != end_it; ++it)
    {
      Space *space = it->second;
      space->Uninitialize();
      delete space;
    }
  }

  // Unload resources
  m_resource_manager->Unload();
  delete m_resource_manager;

  // Delete window
  delete m_window;
}

void EngineCore::ChangeSpace(char const *space_name)
{
  ErrorIf(m_pending_change, "Can't call ChangeSpace multiple times per loop");

  // Set change attributes
  m_pending_change = true;
  m_change_space = space_name;

  if (!m_is_running)
  {
    ChangeSpaceNow();
    m_pending_change = false;
  }
}

void EngineCore::PushSpace(char const *space_file, bool full_copy)
{
  // Make sure that we don't already have a pending push
  ErrorIf(m_pending_push, "Can't call PushSpace multiple times per loop!");

  // Set push attributes
  m_pending_push = true;
  m_push_full_copy = full_copy;
  m_push_file = space_file;

  // If we aren't running yet, push the space now
  if (!m_is_running)
  {
    PushSpaceNow();
    m_pending_push = false;
  }
}

void EngineCore::PopSpace(bool copy)
{
  // Make sure that we don't already have a pending pop
  ErrorIf(m_pending_pop, "EngineCore::PopSpace: Can't call PopSpace multiple times per loop!");
  ErrorIf(m_spaces.size() == 1, "EngineCore::PopSpace: Can't pop the last space.");

  m_pending_pop = true;
  m_pop_copy = copy;
}

void EngineCore::AddSystem(System *system)
{
  // Add the system to the current space
  Space *space = m_spaces.back();
  space->AddSystem(system);
}

void EngineCore::DispatchEvent(int msg)
{
  Space *space = m_spaces.back();

  // Get the listener array for this event
  char const *event = MessageBank::GetMessage(msg);

  // python needs to listen to all events
  PyMessaging::CaptureEvent(event);

  // allow the UI to intercept all events
  UISystemInterface::InterceptEvent(event);

  auto it = space->m_listeners.find(event);

  // If no listeners, just return (not an error)
  if (it == space->m_listeners.end())
    return;

  // Iterate over the listeners array and invoke the message on each of them
  Space::ListenerMap &listeners = it->second;
  auto i = listeners.begin();
  auto end = listeners.end();
  for (; i != end; ++i)
  {
    // Get the object
    Space::Listener listener = i->second;

    // Invoke the method
    Meta::Invoke(listener.m_name, event, listener.m_obj);
  }
}

void EngineCore::DispatchEvent(String const &msg)
{
  Space *space = m_spaces.back();

  // allow the UI to intercept all events
  UISystemInterface::InterceptEvent(msg);

  // python needs to listen to all events
  PyMessaging::CaptureEvent(msg);

  auto it = space->m_listeners.find(msg);

  // If no listeners, just return (not an error)
  if (it == space->m_listeners.end())
    return;

  // Iterate over the listeners array and invoke the message on each of them
  Space::ListenerMap &listeners = it->second;
  auto i = listeners.begin();
  auto end = listeners.end();
  for (; i != end; ++i)
  {
    // Get the object
    Space::Listener listener = i->second;

    // Invoke the method
    Meta::Invoke(listener.m_name, msg.GetCString(), listener.m_obj);
  }
}

void EngineCore::DispatchInterspaceEvent(String const &space_name, int msg)
{
  Layer &layer = m_layers.back();

  ErrorIf(layer.Find(space_name) == layer.End(), "Could not find space %s in current layer", space_name.GetCString());

  Space *space = layer[space_name];

  // Get the listener array for this event
  char const *event = MessageBank::GetMessage(msg);

  // python needs to listen to all events
  PyMessaging::CaptureEvent(event);

  // allow the UI to intercept all events
  UISystemInterface::InterceptEvent(event);

  auto it = space->m_listeners.find(event);

  // If no listeners, just return (not an error)
  if (it == space->m_listeners.end())
    return;

  // Iterate over the listeners array and invoke the message on each of them
  Space::ListenerMap &listeners = it->second;
  auto i = listeners.begin();
  auto end = listeners.end();
  for (; i != end; ++i)
  {
    // Get the object
    Space::Listener listener = i->second;

    // Invoke the method
    Meta::Invoke(listener.m_name, event, listener.m_obj);
  }
}

void EngineCore::DispatchInterspaceEvent(String const &space_name, String const &msg)
{
  Layer &layer = m_layers.back();

  ErrorIf(layer.Find(space_name) == layer.End(), "Could not find space %s in current layer", space_name.GetCString());

  Space *space = layer[space_name];

  // allow the UI to intercept all events
  UISystemInterface::InterceptEvent(msg);

  // python needs to listen to all events
  PyMessaging::CaptureEvent(msg);

  auto it = space->m_listeners.find(msg);

  // If no listeners, just return (not an error)
  if (it == space->m_listeners.end())
    return;

  // Iterate over the listeners array and invoke the message on each of them
  Space::ListenerMap &listeners = it->second;
  auto i = listeners.begin();
  auto end = listeners.end();
  for (; i != end; ++i)
  {
    // Get the object
    Space::Listener listener = i->second;

    // Invoke the method
    Meta::Invoke(listener.m_name, msg.GetCString(), listener.m_obj);
  }
}

Variable EngineCore::SystemCall(char const *sys_name, char const *fn)
{
  // Get the system from the active space
  Space *space = m_spaces.back();
  System *system = space->GetSystem(sys_name);

  // Invoke the method
  return Meta::InvokeRet(sys_name, fn, system);
}

Variable EngineCore::InterspaceSystemCall(String const &space_name, char const *sys_name, char const *fn)
{
  // Get the system from the target space
  Layer &layer = m_layers.back();
  ErrorIf(layer.Find(space_name) == layer.End(), "Could not find space %s in current layer", space_name.GetCString());

  Space *space = layer[space_name];
  System *system = space->GetSystem(sys_name);

  // Invoke the method
  return Meta::InvokeRet(sys_name, fn, system);
}

void EngineCore::OnWindowClose(unsigned int lparam, unsigned int wparam)
{
  SetRunning(false);
}

void EngineCore::OnExit()
{
  SetRunning(false);
}

void EngineCore::OnKeyDown(KeyboardEvent key_event)
{

}

void EngineCore::OnChangeSpace(String const &space_name)
{
  ErrorIf(m_pending_change, "Can't call ChangeSpace multiple times per loop");

  // Set change attributes
  m_pending_change = true;
  m_change_space = space_name;

  if (!m_is_running)
  {
    ChangeSpaceNow();
    m_pending_change = false;
  }
}

void EngineCore::OnPushSpace(String space_file, bool full_copy)
{
  // Make sure that we don't already have a pending push
  ErrorIf(m_pending_push, "Can't call PushSpace multiple times per loop!");

  // Set push attributes
  m_pending_push = true;
  m_push_full_copy = full_copy;
  m_push_file = space_file;

  // If we aren't running yet, push the space now
  if (!m_is_running)
  {
    PushSpaceNow();
    m_pending_push = false;
  }
}

void EngineCore::OnPopSpace(bool copy)
{
  // Make sure that we don't already have a pending pop
  ErrorIf(m_pending_pop, "EngineCore::PopSpace: Can't call PopSpace multiple times per loop!");
  ErrorIf(m_spaces.size() == 1, "EngineCore::PopSpace: Can't pop the last space.");

  m_pending_pop = true;
  m_pop_copy = copy;
}

void EngineCore::DispatchFromSpace(int msg, Space *space)
{
  // Get the listener array for this event
  char const *event = MessageBank::GetMessage(msg);
  auto it = space->m_listeners.find(event);

  // If no listeners, just return (not an error)
  if (it == space->m_listeners.end())
    return;

  // Iterate over the listeners array and invoke the message on each of them
  Space::ListenerMap &listeners = it->second;
  auto i = listeners.begin();
  auto end = listeners.end();
  for (; i != end; ++i)
  {
    // Get the object
    Space::Listener listener = i->second;

    // Invoke the method
    Meta::Invoke(listener.m_name, event, listener.m_obj);
  }
}

void EngineCore::ChangeSpaceNow()
{
  Layer &active_layer = m_layers.back();
  if (active_layer.Find(m_change_space) == active_layer.End())
  {
    // could not find the space, create a new one
    // Make a new space and load it from file
    Space *new_space = new Space();
    FilepathResource *pathres = m_resource_manager->GetResource<FilepathResource>(m_push_file.GetCString());
    CriticalErrorIf(!pathres, "EngineCore::ChangeSpaceNow(): Can't find %s", m_push_file.GetCString());
    String file_path = pathres->GetFilepath();
    Serializer serializer(file_path.GetCString());
    ErrorIf(!serializer.IsGood(), "Failed to open %s", m_push_file.GetCString());
    Read(serializer, *new_space);
    active_layer[new_space->GetName()] = new_space;

    // set the active layer to this one
    m_spaces.back() = new_space;

    // Tell the resource manager to listen for on load
    AddEventListener(MessageBank::OnLoadPass, m_resource_manager);

    // Listen for window events
    AddEventListener(MessageBank::OnPushSpace, this);
    AddEventListener(MessageBank::OnPopSpace, this);
    AddEventListener(MessageBank::OnResetSpace, this);
    AddEventListener(MessageBank::OnChangeSpace, this);
    AddEventListener(MessageBank::OnWindowClose, this);
    AddEventListener(MessageBank::OnExit, this);
    AddEventListener(MessageBank::OnKeyDown, this);

    // Initialize the new space
    new_space->Initialize(m_push_file);
  }
  else
  {
    // just set the active layer to this one
    m_spaces.back() = active_layer[m_change_space];
  }
}

void EngineCore::PushSpaceNow()
{
  // Make a new space and load it from file
  Space *new_space = new Space();
  HashMap<String, Space*> new_layer;
  FilepathResource *pathres = m_resource_manager->GetResource<FilepathResource>(m_push_file.GetCString());
  CriticalErrorIf(!pathres, "EngineCore::PushSpaceNow(): Can't find %s", m_push_file.GetCString());
  String file_path = pathres->GetFilepath();
  Serializer serializer(file_path.GetCString());
  ErrorIf(!serializer.IsGood(), "Failed to open %s", m_push_file.GetCString());
  Read(serializer, *new_space);
  new_layer[new_space->GetName()] = new_space;

  // Push the new space
  Space *old_space = 0;
  if (m_spaces.size())
    old_space = m_spaces.back();
  m_spaces.push_back(new_space);
  m_layers.push_back(new_layer);

  // Tell the old space that a new one was pushed
  if (old_space)
    DispatchFromSpace(MessageBank::OnSpacePushed, old_space);

  // Tell the resource manager to listen for on load
  AddEventListener(MessageBank::OnLoadPass, m_resource_manager);
  
  // Listen for window events
  AddEventListener(MessageBank::OnPushSpace, this);
  AddEventListener(MessageBank::OnPopSpace, this);
  AddEventListener(MessageBank::OnResetSpace, this);
  AddEventListener(MessageBank::OnChangeSpace, this);
  AddEventListener(MessageBank::OnWindowClose, this);
  AddEventListener(MessageBank::OnExit, this);
  AddEventListener(MessageBank::OnKeyDown, this);

  // Initialize the new space
  new_space->Initialize(m_push_file);

  // Copy data from the existing space into the new one
  if (old_space)
    old_space->Copy(new_space, m_push_full_copy);

  // Inform the engine that the space is ready
  DispatchEvent(MessageBank::OnSpaceReady);
}

void EngineCore::PopSpaceNow()
{
  // Get the current space
  Layer active_layer = m_layers.back();
  m_spaces.pop_back();

  // If there is a previous space, copy the current space backwards
  if (m_pop_copy && m_spaces.size() > 1)
  {
    Space *old_space = m_spaces.back();
    Space *copy_space = active_layer[old_space->GetName()];
    copy_space->Copy(old_space, true);
  }

  // Pop the active layer
  auto end_it = active_layer.End();
  for (auto it = active_layer.Begin(); it != end_it; ++it)
  {
    Space *space = it->second;
    space->Uninitialize();
    delete space;
  }
  m_layers.pop_back();

  /* This can leak memory. It appears to leak from CEF */
  if(m_reset_space)
  {
    m_reset_space = false;

    unsigned spacePos = 0;
    Space* curSpace;
    /* Find the first occurrence of this space */
    for(auto it = m_spaces.begin(); it != m_spaces.end(); ++it)
    {
      curSpace = *it;
      if(curSpace->GetName() == m_res_return_space)
        break;
      ++spacePos;
    }
    spacePos = m_spaces.size() - spacePos;

    for(unsigned i = 0; i < spacePos; ++i)
      PopSpaceNow();

    OnPushSpace(m_res_return_space, false);
    m_pending_push = false;
    PushSpaceNow();
    if(m_send_load_msg)
      DispatchEvent("GameReadyToLoad");
    m_send_load_msg = false;
  }

  //DispatchEvent(MessageBank::OnSpaceReturn);
}

void EngineCore::OnResetSpace(String const& resetspace, bool load_reset)
{
  m_reset_space = true;
  m_send_load_msg = load_reset;
  m_res_return_space = resetspace;
  DispatchEvent(MessageBank::OnPopSpace, false);
}

Improbability::String EngineCore::GetSpaceName(unsigned depth) const
{
  if(m_spaces.size() <= depth)
    return m_spaces.front()->GetName();

  unsigned curDepth = 0;
  for(auto it = m_spaces.rbegin(); it != m_spaces.rend(); ++it)
  {
    if(curDepth == depth)
      return (*it)->GetName();
    ++curDepth;
  }
  return "";
}

}