// Nicholas Baldwin
// 2011/12/04
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "UISystemInterface.h"
#include "CefHandler.h"
#include "..\Core\EngineCore.h"
#include "..\Resource\ResourceManager.h"
#include "..\Core\WindowSystem.h"
#include "..\Input\InputEvent.h"

namespace Improbability
{

struct UIData
{
  UIData() : m_handler(0), m_scene(0){}

  Texture *m_scene;
  CefRefPtr<CefClient> m_handler;
};

struct UISingletonData
{
  struct Quad
  {
    unsigned m_x0, m_x1, m_y0, m_y1;
  };

  Quad m_scene;
  std::vector<JSMessage> m_event_queue;
  std::vector<JSMessage> m_updating_event_queue;
  std::vector<JSMessage> m_remote_event_queue;
  bool m_in0, m_in1;
  String m_html_directory;
};

UISingletonData* UISystemInterface::m_ui_singleton = 0;
bool UISystemInterface::m_dev_tools_enabled = false;

MakeMeta(UISystemInterface);

UISystemInterface::UISystemInterface() : m_is_visible(false), m_width(0), m_height(0)
{
  m_ui_data = new UIData;

  g_engine_core->AddEventListener(MessageBank::OnKeyPress, this);
  g_engine_core->AddEventListener(MessageBank::OnCharPress, this);
  g_engine_core->AddEventListener(MessageBank::OnKeyRelease, this);
  g_engine_core->AddEventListener(MessageBank::OnMouseMove, this);
  g_engine_core->AddEventListener(MessageBank::OnMousePress, this);
  g_engine_core->AddEventListener(MessageBank::OnMouseRelease, this);
  g_engine_core->AddEventListener(MessageBank::OnMouseScroll, this);
}

UISystemInterface::UISystemInterface(UISystemInterface const &rhs)
{
  m_ui_data = new UIData;
  m_is_visible = rhs.m_is_visible;
}

UISystemInterface::~UISystemInterface()
{
  DestroyView();
  delete m_ui_data;
  m_ui_data = 0;

  g_engine_core->RemoveEventListener(MessageBank::OnKeyPress, this);
  g_engine_core->RemoveEventListener(MessageBank::OnCharPress, this);
  g_engine_core->RemoveEventListener(MessageBank::OnKeyRelease, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMouseMove, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMousePress, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMouseRelease, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMouseScroll, this);
}

void UISystemInterface::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnKeyPress);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnCharPress);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnKeyRelease);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMouseMove);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMousePress);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMouseRelease);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMouseScroll);
}

void UISystemInterface::Initialize()
{
  m_ui_singleton = new UISingletonData;
  
  m_ui_singleton->m_in0 = false;
  m_ui_singleton->m_in1 = false;

  String dir = "file:///";
  char directory_buffer[1024] = {0};

  GetCurrentDirectoryA(1024, directory_buffer);
  String current_dir = directory_buffer;
  std::vector<String> current_dir_dirs = current_dir.Tokenize('\\');

  for (unsigned i = 0; i < current_dir_dirs.size(); ++i)
  {
    dir += current_dir_dirs[i];
    dir += "/";
  }

  dir += "res_ui/";
  m_ui_singleton->m_html_directory = dir;

  CefSettings settings;
  settings.multi_threaded_message_loop = true;
  settings.log_severity = LOGSEVERITY_DISABLE;
  CefInitialize(settings);
}

void UISystemInterface::Uninitialize()
{
  delete m_ui_singleton;
  m_ui_singleton = 0;

  CefShutdown();
}

void UISystemInterface::OnKeyPress(KeyboardEvent event)
{
  if (!m_is_visible || !m_ui_data->m_handler.get()) return;

  reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->SendKeyEvent( KT_KEYDOWN, event.m_keycode, event.m_modifiers, false, false ); 

//   if (event.m_keycode == kKeyTilde)
//   {
//     if (!m_dev_tools_enabled)
//     {
//       reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->ShowDevTools();
//     }
//     else
//     {
//       reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->CloseDevTools();
//     }
//     m_dev_tools_enabled = !m_dev_tools_enabled;
//   }
}

void UISystemInterface::OnCharPress(unsigned wparam, unsigned lparam)
{
  if (!m_is_visible || !m_ui_data->m_handler.get()) return;

  reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->SendKeyEvent( KT_CHAR, wparam, lparam, false, false ); 
}

void UISystemInterface::OnKeyRelease(KeyboardEvent event)
{
  if (!m_is_visible || !m_ui_data->m_handler.get()) return;

  reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->SendKeyEvent( KT_KEYUP, event.m_keycode, event.m_modifiers, false, false ); 
}

void UISystemInterface::OnMouseMove(MouseEvent event)
{
  if (!m_is_visible || !m_ui_data->m_handler.get()) return;

  reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->SendMouseMoveEvent(event.m_x, event.m_y, false);
}

void UISystemInterface::OnMousePress(MouseEvent event)
{
  if (!m_is_visible || !m_ui_data->m_handler.get()) return;

  reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->SendFocusEvent( true );

  switch (event.m_buttoncode)
  {
  case kMouseLeftButton:
    reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->SendMouseClickEvent(event.m_x, event.m_y, MBT_LEFT, false, 1);
    break;
  case kMouseRightButton:
    reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->SendMouseClickEvent(event.m_x, event.m_y, MBT_RIGHT, false, 1);
    break;
  case kMouseMiddleButton:
    reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->SendMouseClickEvent(event.m_x, event.m_y, MBT_MIDDLE, false, 1);
    break;
  }
}

void UISystemInterface::OnMouseRelease(MouseEvent event)
{
  if (!m_is_visible || !m_ui_data->m_handler.get()) return;

  switch (event.m_buttoncode)
  {
  case kMouseLeftButton:
    reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->SendMouseClickEvent(event.m_x, event.m_y, MBT_LEFT, true, 1);
    break;
  case kMouseRightButton:
    reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->SendMouseClickEvent(event.m_x, event.m_y, MBT_RIGHT, true, 1);
    break;
  case kMouseMiddleButton:
    reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->SendMouseClickEvent(event.m_x, event.m_y, MBT_MIDDLE, true, 1);
    break;
  }
}

void UISystemInterface::OnMouseScroll(MouseEvent event)
{
  if (!m_is_visible || !m_ui_data->m_handler.get()) return;

  reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->SendMouseWheelEvent(event.m_x, event.m_y, event.m_scroll_move);
}

void UISystemInterface::CreateView(unsigned width, unsigned height)
{
  m_width  = width;
  m_height = height;
  DestroyView();

  m_ui_data->m_handler = new ClientHandler(width, height);
  m_ui_data->m_scene = new Texture(width, height);
  CefWindowInfo window_info;
  window_info.SetTransparentPainting(true);
  window_info.SetAsOffScreen(g_engine_core->GetWindow()->GetWindowHandle());
  CefBrowserSettings browser_settings;
  CefBrowser::CreateBrowser(window_info, m_ui_data->m_handler, "about:blank", browser_settings);

  while (m_ui_data->m_handler.get() == 0)
  {
    Sleep(0);
  }
  while (reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser() == 0)
  {
    Sleep(0);
  }
}

void UISystemInterface::DestroyView()
{
  delete m_ui_data->m_scene;
  m_ui_data->m_scene = 0;
  if (m_ui_data->m_handler.get()) 
    reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->CloseBrowser();
  m_ui_data->m_handler = 0;
}

void UISystemInterface::ResizeView(unsigned width, unsigned height)
{
  m_width = width;
  m_height = height;

  delete m_ui_data->m_scene;
  int title_size = GetSystemMetrics(SM_CYCAPTION);
  int border_x = GetSystemMetrics(SM_CXBORDER);
  int border_y = GetSystemMetrics(SM_CYBORDER);
  m_ui_data->m_scene = new Texture(m_width - border_x, m_height - (title_size + border_y));
  if ( m_ui_data->m_handler.get() ) 
  {
    reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->ResizeHandler( m_width - border_x, m_height - (title_size + border_y));
  }
  g_engine_core->SystemCall("GraphicsSystem", "RecieveUIScene", (Texture*)0);
}

void UISystemInterface::LoadURL(String const &url)
{
  reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->GetMainFrame()->LoadURL( url.GetCString() );
}

void UISystemInterface::LoadHTML(String const &html)
{
  reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetBrowser()->GetMainFrame()->LoadURL((m_ui_singleton->m_html_directory + html).GetCString());
}

void UISystemInterface::Update()
{
  ErrorIf(m_ui_singleton == 0, "Update called either after close or before begin");
  //m_ui_singleton->m_event_queue.clear();
  //m_ui_singleton->m_event_queue = m_ui_singleton->m_updating_event_queue;
  m_ui_singleton->m_event_queue.insert(m_ui_singleton->m_event_queue.end(), m_ui_singleton->m_updating_event_queue.begin(), m_ui_singleton->m_updating_event_queue.end());
  m_ui_singleton->m_updating_event_queue.clear();
}

void UISystemInterface::Render()
{
  EventWork();
  m_ui_singleton->m_event_queue.clear();
  reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->GetTexture(m_ui_data->m_scene);
  g_engine_core->SystemCall("GraphicsSystem", "RecieveUIScene", m_ui_data->m_scene);
}

bool ArgIsGood(Variable &var)
{
  if (var.GetType() == "String")
  {
    return true;
  }
  else if (var.GetType() == "int" || var.GetType() == "unsigned")
  {
    return true;
  }
  else if (var.GetType() == "float" || var.GetType() == "double")
  {
    return true;
  }
  else if (var.GetType() == "bool")
  {
    return true;
  }
  else
  {
    return false;
  }
}

Variable FixVariable(Variable &var)
{
  if (var.GetType() == "String")
  {
    return Variable(static_cast<String>(var).CreateOrphan());
  }
  else
  {
    return var;
  }
}

void UISystemInterface::InterceptEvent(String const &message)
{
  if (m_ui_singleton->m_in0) return;
  m_ui_singleton->m_in0 = true;
  JSMessage msg;
  msg.name = message;
  (m_ui_singleton->m_in1 ? m_ui_singleton->m_updating_event_queue : m_ui_singleton->m_event_queue).push_back(msg);
  m_ui_singleton->m_in0 = false;
}

void UISystemInterface::InterceptEvent(String const &message, Variable &p0)
{
  if (!ArgIsGood(p0)) return;
  Variable arg0 = FixVariable(p0);
  if (m_ui_singleton->m_in0) return;
  m_ui_singleton->m_in0 = true;
  JSMessage msg;
  msg.name = message;
  msg.args.push_back(arg0);
  (m_ui_singleton->m_in1 ? m_ui_singleton->m_updating_event_queue : m_ui_singleton->m_event_queue).push_back(msg);
  m_ui_singleton->m_in0 = false;
}

void UISystemInterface::InterceptEvent(String const &message, Variable &p0, Variable &p1)
{
  if (!ArgIsGood(p0)) return;
  if (!ArgIsGood(p1)) return;
  Variable arg0 = FixVariable(p0);
  Variable arg1 = FixVariable(p1);
  if (m_ui_singleton->m_in0) return;
  m_ui_singleton->m_in0 = true;
  JSMessage msg;
  msg.name = message;
  msg.args.push_back(arg0);
  msg.args.push_back(arg1);
  (m_ui_singleton->m_in1 ? m_ui_singleton->m_updating_event_queue : m_ui_singleton->m_event_queue).push_back(msg);
  m_ui_singleton->m_in0 = false;
}

void UISystemInterface::InterceptEvent(String const &message, Variable &p0, Variable &p1, Variable &p2)
{
  if (!ArgIsGood(p0)) return;
  if (!ArgIsGood(p1)) return;
  if (!ArgIsGood(p2)) return;
  Variable arg0 = FixVariable(p0);
  Variable arg1 = FixVariable(p1);
  Variable arg2 = FixVariable(p2);
  if (m_ui_singleton->m_in0) return;
  m_ui_singleton->m_in0 = true;
  JSMessage msg;
  msg.name = message;
  msg.args.push_back(arg0);
  msg.args.push_back(arg1);
  msg.args.push_back(arg2);
  (m_ui_singleton->m_in1 ? m_ui_singleton->m_updating_event_queue : m_ui_singleton->m_event_queue).push_back(msg);
  m_ui_singleton->m_in0 = false;
}

void UISystemInterface::InterceptEvent(String const &message, Variable &p0, Variable &p1, Variable &p2, Variable &p3)
{
  if (!ArgIsGood(p0)) return;
  if (!ArgIsGood(p1)) return;
  if (!ArgIsGood(p2)) return;
  if (!ArgIsGood(p3)) return;
  Variable arg0 = FixVariable(p0);
  Variable arg1 = FixVariable(p1);
  Variable arg2 = FixVariable(p2);
  Variable arg3 = FixVariable(p3);
  if (m_ui_singleton->m_in0) return;
  m_ui_singleton->m_in0 = true;
  JSMessage msg;
  msg.name = message;
  msg.args.push_back(arg0);
  msg.args.push_back(arg1);
  msg.args.push_back(arg2);
  msg.args.push_back(arg3);
  (m_ui_singleton->m_in1 ? m_ui_singleton->m_updating_event_queue : m_ui_singleton->m_event_queue).push_back(msg);
  m_ui_singleton->m_in0 = false;
}

void UISystemInterface::EventWork()
{
  m_ui_singleton->m_in1 = true;

  reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->SyncronizeMessages(m_ui_singleton->m_event_queue, m_ui_singleton->m_remote_event_queue);

  for (auto it = m_ui_singleton->m_remote_event_queue.begin(); it != m_ui_singleton->m_remote_event_queue.end(); ++it)
  {
    switch (it->args.size())
    {
    case 0:
      g_engine_core->DispatchEvent(it->name);
      break;
    case 1:
      g_engine_core->DispatchEvent(it->name, it->args[0]);
      break;
    case 2:
      g_engine_core->DispatchEvent(it->name, it->args[0], it->args[1]);
      break;
    case 3:
      g_engine_core->DispatchEvent(it->name, it->args[0], it->args[1], it->args[2]);
      break;
    case 4:
      g_engine_core->DispatchEvent(it->name, it->args[0], it->args[1], it->args[2], it->args[3]);
      break;
    default:
      CriticalErrorIf(true, "Too many arguments in the message");
    }
  }

  m_ui_singleton->m_remote_event_queue.clear();
  m_ui_singleton->m_in1 = false;
}

bool UISystemInterface::IsClickonHTML(int x, int y)
{
  return reinterpret_cast<ClientHandler*>(m_ui_data->m_handler.get())->IsClickOnHTMLElement(x, y);
}

}