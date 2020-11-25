// David Evans
// 2011/05/12
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "System.h"
#include "SystemProfiler.h"
#include "Space.h"
#include "..\Input\InputEvent.h"
#include "MessageBank.h"
#include "..\UISystem\UISystemInterface.h"
#include "..\Python\PyMessaging.h"

namespace Improbability
{

class EngineCore;
class System;
class ResourceManager;
class MetaComposition;
class WindowSystem;
class PyMessaging;

// Global pointer to the engine core
extern EngineCore *g_engine_core;

// Global player state variables
extern int g_coin_count;
extern int g_curr_coin_count;
extern int g_coins_needed;
extern bool g_rotate_blocks;
extern bool g_rotate_ability;
extern bool g_anchor_blocks;
extern bool g_anchor_ability;
extern bool g_nega_binding;
extern bool g_the_end;
extern int g_player_facing;
extern int g_curr_level;

//Progress Variables
extern bool g_coin_get;
extern bool g_coins_complete;
extern bool g_moved;
extern bool g_jumped;
extern bool g_unlocked_rblocks;
extern bool g_unlocked_rability;
extern bool g_unlocked_ablocks;
extern bool g_unlocked_aability;
extern bool g_first_rotate;
extern bool g_first_level;
extern bool g_second_level;
extern bool g_final_level;
extern bool g_first_anchor;
extern bool g_anchor_cleared;
extern bool g_anchor_dropped;
extern bool g_anchor_set;
extern bool g_anchor_tried;
extern bool g_free_rotated;
extern bool g_warped_back;
extern bool g_walked_behind;

//Saving and loading
extern bool g_isLoaded;

// lost device hack
extern int g_reset_device_count;

// The engine core handles updating and managing all systems
class EngineCore
{
public:
  // Container of spaces
  typedef std::vector<Space*> SpaceArray;
  typedef HashMap<String, Space*> Layer;
  typedef std::vector<Layer> LayerArray;

  static void BindMeta(MetaComposition*);

  // Constructor
  EngineCore();

  // Destructor
  ~EngineCore();

  // Initializes all the systems (only called once)
  void Initialize(char const *space_file, char const *config_file);

  // Begin the main loop
  void MainLoop();

  // Uninitializes all the systems (only called once)
  void Uninitialize();

  // This changes the current space to another space on the same layer
  // if the space does not exist it will create it and initialize it
  // No deep copy is allowed so no Copy method will be called
  // The action is deferred until the end of the frame
  // space_file - Specifies the name of the file to load the space from
  void ChangeSpace(char const *space_name);

  // This pushes a new state, or space onto the engine
  // All the current systems will be cloned using the
  // Clone method and the new space will become the active space
  // The action is deferred until the end of the frame
  // space_file - Specifies the name of the file to load the space from
  // deep - Whether or not the space should be deep copied
  void PushSpace(char const *space_file, bool deep = false);

  // This pops the active space from the engine
  // All the active systems will be uninitialized
  // with UninitializeInstance and then deleted
  // The previous space will become the active space
  // The action is deferred until the end of the frame
  // copy - True if the existing space should copy data to the previous space
  void PopSpace(bool copy = false);

  // Add a system to the active space
  // system - Pointer to the allocated system to add
  void AddSystem(System *system);

  // Add an event listener
  // The listening object *must* implement a function with the signature of the message
  // event - The name of the event (corresponds to the member function to be called)
  // listener - Pointer to the object that will receive the message
  template <typename T>
  void AddEventListener(int msg, T *listener); 
  template <typename T>
  void RemoveEventListener(int msg, T *listener);

  // Triggers an event, resolves to a function call on every registered object (Happens immediately)
  // msg - The name of the event to trigger
  // p0...p3 - Parameters to the event
  void DispatchEvent(int msg);
  template <typename P0>
  void DispatchEvent(int msg, P0 p0);
  template <typename P0, typename P1>
  void DispatchEvent(int msg, P0 p0, P1 p1);
  template <typename P0, typename P1, typename P2>
  void DispatchEvent(int msg, P0 p0, P1 p1, P2 p2);
  template <typename P0, typename P1, typename P2, typename P3>
  void DispatchEvent(int msg, P0 p0, P1 p1, P2 p2, P3 p3);

  void DispatchEvent(String const &msg);
  template <typename P0>
  void DispatchEvent(String const &msg, P0 p0);
  template <typename P0, typename P1>
  void DispatchEvent(String const &msg, P0 p0, P1 p1);
  template <typename P0, typename P1, typename P2>
  void DispatchEvent(String const &msg, P0 p0, P1 p1, P2 p2);
  template <typename P0, typename P1, typename P2, typename P3>
  void DispatchEvent(String const &msg, P0 p0, P1 p1, P2 p2, P3 p3);

  // Triggers an event in another space on the same layer, resolves to the same calls as above
  // space_name - The name of the space to dispatch the event to
  // msg - The name of the event to trigger
  // p0...p3 - Parameters to the event
  void DispatchInterspaceEvent(String const &space_name, int msg);
  template <typename P0>
  void DispatchInterspaceEvent(String const &space_name, int msg, P0 p0);
  template <typename P0, typename P1>
  void DispatchInterspaceEvent(String const &space_name, int msg, P0 p0, P1 p1);
  template <typename P0, typename P1, typename P2>
  void DispatchInterspaceEvent(String const &space_name, int msg, P0 p0, P1 p1, P2 p2);
  template <typename P0, typename P1, typename P2, typename P3>
  void DispatchInterspaceEvent(String const &space_name, int msg, P0 p0, P1 p1, P2 p2, P3 p3);

  void DispatchInterspaceEvent(String const &space_name, String const &msg);
  template <typename P0>
  void DispatchInterspaceEvent(String const &space_name, String const &msg, P0 p0);
  template <typename P0, typename P1>
  void DispatchInterspaceEvent(String const &space_name, String const &msg, P0 p0, P1 p1);
  template <typename P0, typename P1, typename P2>
  void DispatchInterspaceEvent(String const &space_name, String const &msg, P0 p0, P1 p1, P2 p2);
  template <typename P0, typename P1, typename P2, typename P3>
  void DispatchInterspaceEvent(String const &space_name, String const &msg, P0 p0, P1 p1, P2 p2, P3 p3);

  // Calls a function on a system in the active space
  // sys_name - The name of the system to invoke a function on
  // fn - The name of the function to cal
  // p0...p3 - Parameters to the event
  // return - A Variable which can be cast to whatever the expected return is.
  //          If there is not a listener for the message, the Variable will
  //          only have Null in it. 
  Variable SystemCall(char const *sys_name, char const *fn);
  template <typename P0>
  Variable SystemCall(char const *sys_name, char const *fn, P0 p0);
  template <typename P0, typename P1>
  Variable SystemCall(char const *sys_name, char const *fn, P0 p0, P1 p1);
  template <typename P0, typename P1, typename P2>
  Variable SystemCall(char const *sys_name, char const *fn, P0 p0, P1 p1, P2 p2);
  template <typename P0, typename P1, typename P2, typename P3>
  Variable SystemCall(char const *sys_name, char const *fn, P0 p0, P1 p1, P2 p2, P3 p3);

  // Calls a function on a system in a space in the active layer
  // space_name - The name of the space to invoke the system call on
  // sys_name - The name of the system to invoke a function on
  // fn - The name of the function to cal
  // p0...p3 - Parameters to the event
  // return - A Variable which can be cast to whatever the expected return is.
  //          If there is not a listener for the message, the Variable will
  //          only have Null in it. 
  Variable InterspaceSystemCall(String const &space_name, char const *sys_name, char const *fn);
  template <typename P0>
  Variable InterspaceSystemCall(String const &space_name, char const *sys_name, char const *fn, P0 p0);
  template <typename P0, typename P1>
  Variable InterspaceSystemCall(String const &space_name, char const *sys_name, char const *fn, P0 p0, P1 p1);
  template <typename P0, typename P1, typename P2>
  Variable InterspaceSystemCall(String const &space_name, char const *sys_name, char const *fn, P0 p0, P1 p1, P2 p2);
  template <typename P0, typename P1, typename P2, typename P3>
  Variable InterspaceSystemCall(String const &space_name, char const *sys_name, char const *fn, P0 p0, P1 p1, P2 p2, P3 p3);

  // Callback for window being closed
  void OnWindowClose(unsigned int lparam, unsigned int wparam);
  void OnExit();
  void OnKeyDown(KeyboardEvent key_event);
  void OnChangeSpace(String const &space_name);
  void OnPushSpace(String space_name, bool full_copy);
  void OnPopSpace(bool copy);
  void OnResetSpace(String const& resetspace, bool load_reset=false);

  // Setters
  void SetRunning(bool flag) {m_is_running = flag;}

  // Getters
  bool IsRunning() {return m_is_running;}

  ResourceManager* GetResourceManager() {return m_resource_manager;}
  WindowSystem* GetWindow() {return m_window;}

  // Gets the name of the space some depth from the end of the spaces list
  String GetSpaceName(unsigned depth=0) const;

private:
  // Send a message from a particular space
  void DispatchFromSpace(int msg, Space *space);

  // Immediately changes the current space on the stack to another one on the layer
  void ChangeSpaceNow();

  // Immediately pushes a new space onto the stack
  void PushSpaceNow();

  // Immediately pops the active space from the stack
  void PopSpaceNow();
private:
  // Resource manager
  ResourceManager *m_resource_manager;

  // Window
  WindowSystem *m_window;

  // Whether or not the engine should continue running
  bool m_is_running;

  // If this is true the space will chante at the end of the frame
  bool m_pending_change;

  // If this is true a space will be pushed at the end of the frame
  bool m_pending_push;

  // If this is true the pending push will be a full copy
  bool m_push_full_copy;

  // The name of the space to change to when the space changes
  String m_change_space;

  // The file to load the pending push from
  String m_push_file;

  // If this is true a space will be popped at the end of the frame
  bool m_pending_pop;

  // If this is true the pending pop will copy data back
  bool m_pop_copy;

  // Container for the spaces of systems
  SpaceArray m_spaces;
  LayerArray m_layers;

  // Base space for lower level messages
  Space *m_root_space;
  bool m_reset_space;
  bool m_send_load_msg;
  String m_res_return_space;
};

template <typename T>
void EngineCore::AddEventListener(int msg, T *obj)
{
  Space *space = m_spaces.back();

  // Get the listener map for this event
  char const *event = MessageBank::GetMessage(msg);
  Space::ListenerMap &listeners = space->m_listeners[event];

  // Get string name of template type
  String name = AccessMeta(T)->GetName();

  // Add the event listener
  Space::Listener &listener = listeners[obj];
  listener.m_name = name;
  listener.m_obj = obj;
}

template <typename T>
void EngineCore::RemoveEventListener(int msg, T *obj)
{
  Space *space = m_spaces.back();

  // Get the listener map for this event
  char const *event = MessageBank::GetMessage(msg);
  Space::ListenerMap &listeners = space->m_listeners[event];

  auto it = listeners.find(obj);
  if (it != listeners.end())
    listeners.erase(it);
}

template <typename P0>
void EngineCore::DispatchEvent(int msg, P0 p0)
{
  Space *space = m_spaces.back();

  // Get the listener array for this event
  char const *event = MessageBank::GetMessage(msg);

  //Python grabs events
  PyMessaging::CaptureEvent(event, Variable(p0));

  //allow the UI to intercept all events
  UISystemInterface::InterceptEvent(event, Variable(p0));

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
    Meta::Invoke(listener.m_name, event, listener.m_obj, p0);
  }
}

template <typename P0>
void EngineCore::DispatchEvent(String const &msg, P0 p0)
{
  Space *space = m_spaces.back();

  //Python grabs events
  PyMessaging::CaptureEvent(msg, Variable(p0));
  
  // allow the UI to intercept all events
  UISystemInterface::InterceptEvent(msg, Variable(p0));

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
    Meta::Invoke(listener.m_name, msg.GetCString(), listener.m_obj, p0);
  }
}

template <typename P0>
void EngineCore::DispatchInterspaceEvent(String const &space_name, int msg, P0 p0)
{
  Layer *layer = m_layers.back();

  ErrorIf(layer->Find(space_name) == layer->End(), "Could not find space %s in current layer", space_name.GetCString());

  Space *space = layer[space_name];

  // Get the listener array for this event
  char const *event = MessageBank::GetMessage(msg);

  //Python grabs events
  PyMessaging::CaptureEvent(event, Variable(p0));

  //allow the UI to intercept all events
  UISystemInterface::InterceptEvent(event, Variable(p0));

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
    Meta::Invoke(listener.m_name, event, listener.m_obj, p0);
  }
}

template <typename P0>
void EngineCore::DispatchInterspaceEvent(String const &space_name, String const &msg, P0 p0)
{
  Layer *layer = m_layers.back();

  ErrorIf(layer->Find(space_name) == layer->End(), "Could not find space %s in current layer", space_name.GetCString());

  Space *space = layer[space_name];

  //Python grabs events
  PyMessaging::CaptureEvent(msg, Variable(p0));

  // allow the UI to intercept all events
  UISystemInterface::InterceptEvent(msg, Variable(p0));

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
    Meta::Invoke(listener.m_name, msg.GetCString(), listener.m_obj, p0);
  }
}

template <typename P0, typename P1>
void EngineCore::DispatchEvent(int msg, P0 p0, P1 p1)
{
  Space *space = m_spaces.back();

  // Get the listener array for this event
  char const *event = MessageBank::GetMessage(msg);

  //Python grabs events
  PyMessaging::CaptureEvent(event, Variable(p0), Variable(p1));

  //allow the UI to intercept all events
  UISystemInterface::InterceptEvent(event, Variable(p0), Variable(p1));

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
    Meta::Invoke(listener.m_name, event, listener.m_obj, p0, p1);
  }
}

template <typename P0, typename P1>
void EngineCore::DispatchEvent(String const &msg, P0 p0, P1 p1)
{
  Space *space = m_spaces.back();

  //Python grabs events
  PyMessaging::CaptureEvent(msg, Variable(p0), Variable(p1));

  // allow the UI to intercept all events
  UISystemInterface::InterceptEvent(msg, Variable(p0), Variable(p1));

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
    Meta::Invoke(listener.m_name, msg.GetCString(), listener.m_obj, p0, p1);
  }
}

template <typename P0, typename P1>
void EngineCore::DispatchInterspaceEvent(String const &space_name, int msg, P0 p0, P1 p1)
{
  Layer *layer = m_layers.back();

  ErrorIf(layer->Find(space_name) == layer->End(), "Could not find space %s in current layer", space_name.GetCString());

  Space *space = layer[space_name];

  // Get the listener array for this event
  char const *event = MessageBank::GetMessage(msg);

  //Python grabs events
  PyMessaging::CaptureEvent(event, Variable(p0), Variable(p1));

  //allow the UI to intercept all events
  UISystemInterface::InterceptEvent(event, Variable(p0), Variable(p1));

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
    Meta::Invoke(listener.m_name, event, listener.m_obj, p0, p1);
  }
}

template <typename P0, typename P1>
void EngineCore::DispatchInterspaceEvent(String const &space_name, String const &msg, P0 p0, P1 p1)
{
  Layer *layer = m_layers.back();

  ErrorIf(layer->Find(space_name) == layer->End(), "Could not find space %s in current layer", space_name.GetCString());

  Space *space = layer[space_name];

  //Python grabs events
  PyMessaging::CaptureEvent(msg, Variable(p0), Variable(p1));

  // allow the UI to intercept all events
  UISystemInterface::InterceptEvent(msg, Variable(p0), Variable(p1));

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
    Meta::Invoke(listener.m_name, msg.GetCString(), listener.m_obj, p0, p1);
  }
}

template <typename P0, typename P1, typename P2>
void EngineCore::DispatchEvent(int msg, P0 p0, P1 p1, P2 p2)
{
  Space *space = m_spaces.back();

  // Get the listener array for this event
  char const *event = MessageBank::GetMessage(msg);

  //Python grabs events
  PyMessaging::CaptureEvent(event, Variable(p0), Variable(p1), Variable(p2));

  //allow the UI to intercept all events
  UISystemInterface::InterceptEvent(event, Variable(p0), Variable(p1), Variable(p2));

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
    Meta::Invoke(listener.m_name, event, listener.m_obj, p0, p1, p2);
  }
}

template <typename P0, typename P1, typename P2>
void EngineCore::DispatchEvent(String const &msg, P0 p0, P1 p1, P2 p2)
{
  Space *space = m_spaces.back();

  // allow the UI to intercept all events
  UISystemInterface::InterceptEvent(msg, Variable(p0), Variable(p1), Variable(p2));

  //Python grabs events
  PyMessaging::CaptureEvent(msg, Variable(p0), Variable(p1), Variable(p2));

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
    Meta::Invoke(listener.m_name, msg.GetCString(), listener.m_obj, p0, p1, p2);
  }
}

template <typename P0, typename P1, typename P2>
void EngineCore::DispatchInterspaceEvent(String const &space_name, int msg, P0 p0, P1 p1, P2 p2)
{
  Layer *layer = m_layers.back();

  ErrorIf(layer->Find(space_name) == layer->End(), "Could not find space %s in current layer", space_name.GetCString());

  Space *space = layer[space_name];

  // Get the listener array for this event
  char const *event = MessageBank::GetMessage(msg);

  //Python grabs events
  PyMessaging::CaptureEvent(event, Variable(p0), Variable(p1), Variable(p2));

  //allow the UI to intercept all events
  UISystemInterface::InterceptEvent(event, Variable(p0), Variable(p1), Variable(p2));

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
    Meta::Invoke(listener.m_name, event, listener.m_obj, p0, p1, p2);
  }
}

template <typename P0, typename P1, typename P2>
void EngineCore::DispatchInterspaceEvent(String const &space_name, String const &msg, P0 p0, P1 p1, P2 p2)
{
  Layer *layer = m_layers.back();

  ErrorIf(layer->Find(space_name) == layer->End(), "Could not find space %s in current layer", space_name.GetCString());

  Space *space = layer[space_name];

  // allow the UI to intercept all events
  UISystemInterface::InterceptEvent(msg, Variable(p0), Variable(p1), Variable(p2));

  //Python grabs events
  PyMessaging::CaptureEvent(msg, Variable(p0), Variable(p1), Variable(p2));

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
    Meta::Invoke(listener.m_name, msg.GetCString(), listener.m_obj, p0, p1, p2);
  }
}

template <typename P0, typename P1, typename P2, typename P3>
void EngineCore::DispatchEvent(int msg, P0 p0, P1 p1, P2 p2, P3 p3)
{
  Space *space = m_spaces.back();

  // Get the listener array for this event
  char const *event = MessageBank::GetMessage(msg);

  //allow the UI to intercept all events
  UISystemInterface::InterceptEvent(event, Variable(p0), Variable(p1), Variable(p2), Variable(p3));

  //Python grabs events
  PyMessaging::CaptureEvent(event, Variable(p0), Variable(p1), Variable(p2), Variable(p3));

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
    Meta::Invoke(listener.m_name, event, listener.m_obj, p0, p1, p2, p3);
  }
}

template <typename P0, typename P1, typename P2, typename P3>
void EngineCore::DispatchEvent(String const &msg, P0 p0, P1 p1, P2 p2, P3 p3)
{
  Space *space = m_spaces.back();

  // allow the UI to intercept all events
  UISystemInterface::InterceptEvent(msg, Variable(p0), Variable(p1), Variable(p2), Variable(p3));

  //Python grabs events
  PyMessaging::CaptureEvent(msg, Variable(p0), Variable(p1), Variable(p2), Variable(p3));

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
    Meta::Invoke(listener.m_name, msg.GetCString(), listener.m_obj, p0, p1, p2, p3);
  }
}

template <typename P0, typename P1, typename P2, typename P3>
void EngineCore::DispatchInterspaceEvent(String const &space_name, int msg, P0 p0, P1 p1, P2 p2, P3 p3)
{
  Layer *layer = m_layers.back();

  ErrorIf(layer->Find(space_name) == layer->End(), "Could not find space %s in current layer", space_name.GetCString());

  Space *space = layer[space_name];

  // Get the listener array for this event
  char const *event = MessageBank::GetMessage(msg);

  //allow the UI to intercept all events
  UISystemInterface::InterceptEvent(event, Variable(p0), Variable(p1), Variable(p2), Variable(p3));

  //Python grabs events
  PyMessaging::CaptureEvent(event, Variable(p0), Variable(p1), Variable(p2), Variable(p3));

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
    Meta::Invoke(listener.m_name, event, listener.m_obj, p0, p1, p2, p3);
  }
}

template <typename P0, typename P1, typename P2, typename P3>
void EngineCore::DispatchInterspaceEvent(String const &space_name, String const &msg, P0 p0, P1 p1, P2 p2, P3 p3)
{
  Layer *layer = m_layers.back();

  ErrorIf(layer->Find(space_name) == layer->End(), "Could not find space %s in current layer", space_name.GetCString());

  Space *space = layer[space_name];

  // allow the UI to intercept all events
  UISystemInterface::InterceptEvent(msg, Variable(p0), Variable(p1), Variable(p2), Variable(p3));

  //Python grabs events
  PyMessaging::CaptureEvent(msg, Variable(p0), Variable(p1), Variable(p2), Variable(p3));

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
    Meta::Invoke(listener.m_name, msg.GetCString(), listener.m_obj, p0, p1, p2, p3);
  }
}

template <typename P0>
Variable EngineCore::SystemCall(char const *sys_name, char const *fn, P0 p0)
{
  // Get the system from the active space
  Space *space = m_spaces.back();
  System *system = space->GetSystem(sys_name);

  // Invoke the method
  return Meta::InvokeRet(sys_name, fn, system, p0);
}

template <typename P0>
Variable EngineCore::InterspaceSystemCall(String const &space_name, char const *sys_name, char const *fn, P0 p0)
{
  // Get the system from the target space
  Layer *layer = m_layers.back();
  ErrorIf(layer->Find(space_name) == layer->End(), "Could not find space %s in current layer", space_name.GetCString());

  Space *space = layer[space_name];
  System *system = space->GetSystem(sys_name);

  // Invoke the method
  return Meta::InvokeRet(sys_name, fn, system, p0);
}

template <typename P0, typename P1>
Variable EngineCore::SystemCall(char const *sys_name, char const *fn, P0 p0, P1 p1)
{
  // Get the system from the active space
  Space *space = m_spaces.back();
  System *system = space->GetSystem(sys_name);

  // Invoke the method
  return Meta::InvokeRet(sys_name, fn, system, p0, p1);
}

template <typename P0, typename P1>
Variable EngineCore::InterspaceSystemCall(String const &space_name, char const *sys_name, char const *fn, P0 p0, P1 p1)
{
  // Get the system from the target space
  Layer *layer = m_layers.back();
  ErrorIf(layer->Find(space_name) == layer->End(), "Could not find space %s in current layer", space_name.GetCString());

  Space *space = layer[space_name];
  System *system = space->GetSystem(sys_name);

  // Invoke the method
  return Meta::InvokeRet(sys_name, fn, system, p0, p1);
}

template <typename P0, typename P1, typename P2>
Variable EngineCore::SystemCall(char const *sys_name, char const *fn, P0 p0, P1 p1, P2 p2)
{
  // Get the system from the active space
  Space *space = m_spaces.back();
  System *system = space->GetSystem(sys_name);

  // Invoke the method
  return Meta::InvokeRet(sys_name, fn, system, p0, p1, p2);
}

template <typename P0, typename P1, typename P2>
Variable EngineCore::InterspaceSystemCall(String const &space_name, char const *sys_name, char const *fn, P0 p0, P1 p1, P2 p2)
{
  // Get the system from the target space
  Layer *layer = m_layers.back();
  ErrorIf(layer->Find(space_name) == layer->End(), "Could not find space %s in current layer", space_name.GetCString());

  Space *space = layer[space_name];
  System *system = space->GetSystem(sys_name);

  // Invoke the method
  return Meta::InvokeRet(sys_name, fn, system, p0, p1, p2);
}

template <typename P0, typename P1, typename P2, typename P3>
Variable EngineCore::SystemCall(char const *sys_name, char const *event, P0 p0, P1 p1, P2 p2, P3 p3)
{
  // Get the system from the active space
  Space *space = m_spaces.back();
  System *system = space->GetSystem(sys_name);

  // Invoke the method
  return Meta::InvokeRet(sys_name, fn, system, p0, p1, p2, p3);
}

template <typename P0, typename P1, typename P2, typename P3>
Variable EngineCore::InterspaceSystemCall(String const &space_name, char const *sys_name, char const *event, P0 p0, P1 p1, P2 p2, P3 p3)
{
  // Get the system from the target space
  Layer *layer = m_layers.back();
  ErrorIf(layer->Find(space_name) == layer->End(), "Could not find space %s in current layer", space_name.GetCString());

  Space *space = layer[space_name];
  System *system = space->GetSystem(sys_name);

  // Invoke the method
  return Meta::InvokeRet(sys_name, fn, system, p0, p1, p2, p3);
}

}