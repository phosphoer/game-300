// Nicholas Baldwin
// 2011/06/06
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Core/System.h"
#include "../Containers/String.h"

namespace Improbability
{

class MetaComposition;

class WindowSystem
{
  typedef std::map<unsigned int, int> MessageMap;
public:
  static void BindMeta(MetaComposition*);

  WindowSystem();

  // Methods
  void Initialize();
  void Uninitialize();
  void Update();
  void Deserialize(Serializer &serializer);

  // Give a windows message to the window system for it to be processed by
  // the engine's messaging system, if the message is registered
  // msg - The windows message id
  // wparam - wparam of the windows message
  // lparam - lparam of the windows message
  void HandleMessage(unsigned int msg, unsigned int wparam, unsigned int lparam);

  // This tells the window system that it should dispatch the given message
  // when it recieves the specified windows message
  // msg - The windows message id
  // message_name - String name of the message to dispatch upon receiving msg
  void RegisterMessage(int msg, int message_id);

  // Getters
  HWND GetWindowHandle() {return m_window_handle;}
  String GetTitle() {return m_title;}
  int GetWidth() {return m_width;}
  int GetHeight() {return m_height;}
  bool GetFullscreen() {return m_fullscreen; }
  void SetWidth(int width) {m_width = width;}
  void SetHeight(int height) {m_height = height;}
  bool IsCmd() {return m_has_cmd;}

  static HINSTANCE s_instance;

private:
  HWND m_window_handle;
  String m_title;
  int m_width;
  int m_height;
  bool m_fullscreen;
  bool m_has_cmd;
  MessageMap m_messages;
};

}