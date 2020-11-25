// David Evans
// 2011/09/17
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

class LogicComponent;
class Composition;
class Serializer;
class ActionList;
class KeyboardEvent;
class ActionEvent;
class MetaComposition;

class ActionReturn
{
public:
  enum Type
  {
    Continue,
    Block,
    Remove,
    Count
  };
};

class Action
{
  friend class LogicComponent;
  friend class ActionList;

public:
  static void BindMeta(MetaComposition*);

  Action();
  virtual ~Action() {}

  // Virtual methods
  virtual void Initialize() {}
  virtual void Start() {}
  virtual ActionReturn::Type Update(float dt) {return ActionReturn::Continue;}
  virtual ActionReturn::Type InactiveUpdate(float dt) {return ActionReturn::Continue;}
  virtual void End() {}
  virtual void Uninitialize() {}
  virtual void Deserialize(Serializer &serializer) {};
  virtual Action* Clone() {return new Action(*this);}

  // Messages
  void OnKeyPress(KeyboardEvent);
  void OnKeyRelease(KeyboardEvent);
  void OnActionPress(ActionEvent);
  void OnActionRelease(ActionEvent);  

  // Base methods
  void InitializeBase();
  ActionReturn::Type UpdateBase(float dt);
  void UninitializeBase();
  void DeserializeBase(Serializer &serializer);
  
  // Setters
  void SetLifeTime(float lifetime) {m_lifetime = lifetime;}
  void SetBlocking(bool blocking) {m_is_blocking = blocking;}
  void SetPaused(bool paused) {m_is_paused = paused;}

  // Getters
  float GetTime() {return m_time;}
  float GetLifeTime() {return m_lifetime;}
  bool IsRemovedOnUpdate() {return m_remove_on_update;}
  bool IsBlocking() {return m_is_blocking;}
  bool IsPaused() {return m_is_paused;}
  String GetType() {return m_action_type;}
  Composition* GetParent() {return m_parent;}

protected:
  Composition *m_parent;
  LogicComponent *m_logic;
  bool m_has_run;

private:
  float m_time;
  float m_lifetime;
  bool m_remove_on_update;
  bool m_is_blocking;
  bool m_is_paused;

  // Name of the action type
  String m_action_type;

  // Whether or not to listen for these messages
  bool m_on_key_down;
  bool m_on_key_pressed;
  bool m_on_actionkey_down;
  bool m_on_actionkey_pressed;

  // Whether or not the key / action is down
  bool m_key_down;
  bool m_key_pressed;
  bool m_actionkey_down;
  bool m_actionkey_pressed;

  // Name of the key or action to listen for
  String m_key_name;
  String m_keypressed_name;
  String m_actionkey_name;
  String m_actionkeypressed_name;

  // Key value
  unsigned int m_key;
  unsigned int m_keypressed;
};

}