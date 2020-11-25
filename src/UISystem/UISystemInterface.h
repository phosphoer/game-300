// Nicholas Baldwin
// 2011/12/04
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

struct UIData;
struct UISingletonData;
class KeyboardEvent;
class MouseEvent;

class UISystemInterface
{
public:
  UISystemInterface();
  UISystemInterface(UISystemInterface const &rhs);
  ~UISystemInterface();
  
  static void BindMeta(MetaComposition *comp);

  static void Initialize();
  static void Uninitialize();

  void OnCharPress(unsigned wparam, unsigned lparam);
  void OnKeyPress(KeyboardEvent event);
  void OnKeyRelease(KeyboardEvent event);
  void OnMouseMove(MouseEvent event);
  void OnMousePress(MouseEvent event);
  void OnMouseRelease(MouseEvent event);
  void OnMouseScroll(MouseEvent event);

  void CreateView(unsigned width, unsigned height);
  void DestroyView();
  void ResizeView(unsigned width, unsigned height);

  void LoadURL(String const &url);
  void LoadHTML(String const &html);
  
  static void Update();
  void Render();

  static void InterceptEvent(String const &message);
  static void InterceptEvent(String const &message, Variable &p0);
  static void InterceptEvent(String const &message, Variable &p0, Variable &p1);
  static void InterceptEvent(String const &message, Variable &p0, Variable &p1, Variable &p2);
  static void InterceptEvent(String const &message, Variable &p0, Variable &p1, Variable &p2, Variable &p3);

  bool IsVisible() {return m_is_visible;}
  void SetVisible(bool visibility) {m_is_visible = visibility;}
  bool IsClickonHTML(int x, int y);

private:
  void EventWork();
  UIData *m_ui_data;
  static UISingletonData *m_ui_singleton;
  static bool m_dev_tools_enabled;

  bool m_is_visible;
  unsigned m_width;
  unsigned m_height;
};

}