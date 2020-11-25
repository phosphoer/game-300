// Nicholas Baldwin
// 2011/06/06
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "WindowSystem.h"
#include "../FileIO/Serializer.h"
#include "../Core/EngineCore.h"

namespace Improbability
{

MakeMeta(WindowSystem);

static WindowSystem *s_window = 0;

HINSTANCE WindowSystem::s_instance = 0;

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  s_window->HandleMessage(msg, wparam, lparam);
  return DefWindowProc(hwnd, msg, wparam, lparam);
}

WindowSystem::WindowSystem()
{
  s_window = this;
}

void WindowSystem::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(HandleMessage);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(RegisterMessage);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetWindowHandle);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetWidth);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetHeight);
}

void WindowSystem::Initialize()
{
  // Create the window class
  WNDCLASSEX window_class = {0};
  window_class.cbSize        = sizeof(WNDCLASSEX);
  window_class.style         = CS_HREDRAW | CS_VREDRAW;
  window_class.lpfnWndProc   = WindowProcedure;
  window_class.cbClsExtra    = 0;
  window_class.cbWndExtra    = 0;
  window_class.hInstance     = s_instance;
  window_class.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  window_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
  window_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  window_class.lpszMenuName  = NULL;
  window_class.lpszClassName = "WindowClass";
  window_class.hIconSm       = window_class.hIcon;

  // Register the window class and error on fail
  ErrorOnResult(RegisterClassEx(&window_class), 0, "RegisterClassEx failed!");

  // Adjust the window size
  RECT rect = {0, 0, m_width, m_height};
  AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

  if (m_has_cmd)
  {
    // Spawn a new console
    AllocConsole();

    // Redirect stdout
    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hcrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE *hf_out = _fdopen(hcrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;
  }

  // Spawn the window
  m_window_handle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
                                   "WindowClass",
                                   m_title.GetCString(),
                                   WS_OVERLAPPEDWINDOW,
                                   CW_USEDEFAULT,
                                   CW_USEDEFAULT,
                                   rect.right - rect.left,
                                   rect.bottom - rect.top,
                                   NULL,
                                   NULL,
                                   s_instance,
                                   NULL);

  // Show the window and update it
  ShowWindow(m_window_handle, SW_SHOW);
  UpdateWindow(m_window_handle);

  //g_engine_core->DispatchEvent(MessageBank::OnWindowResize, m_width, m_height, m_fullscreen);
}

void WindowSystem::Update()
{
  MSG msg;
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

void WindowSystem::Uninitialize()
{
  // Unregister the window class
  UnregisterClass(m_title.GetCString(), s_instance);

  if (m_has_cmd)
  {
    // Free the console
    FreeConsole();
  }
}

void WindowSystem::Deserialize(Serializer &serializer)
{
  Read(serializer, m_title, "Title");
  Read(serializer, m_width, "Width");
  Read(serializer, m_height, "Height");
  Read(serializer, m_fullscreen, "Fullscreen");
  Read(serializer, m_has_cmd, "HasCommand");
}

void WindowSystem::HandleMessage(unsigned int msg, unsigned int wparam, unsigned int lparam)
{
  auto it = m_messages.find(msg);

  if (it != m_messages.end())
  {
    g_engine_core->DispatchEvent(it->second, wparam, lparam);
  }
}

void WindowSystem::RegisterMessage(int msg, int message_id)
{
  ErrorIf(m_messages.find(msg) != m_messages.end());
  m_messages[msg] = message_id;
}

}