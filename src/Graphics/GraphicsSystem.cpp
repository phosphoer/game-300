// Nicholas Baldwin
// 2011/06/25
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "../core/EngineCore.h"
#include "../Core/WindowSystem.h"
#include "GraphicsSystem.h"
#include "GraphicsDevice.h"
#include "RenderComponent.h"
#include "Shapes.h"
#include "DebugDraw.h"
#include "../Composition/ObjectManager.h"
#include "Camera.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Texture.h"
#include "../Resource/ResourceManager.h"
#include "Mesh.h"
#include "../FileIO/Serializer.h"
#include "AnimationComponent.h"
#include "../Tile/Tilemap.h"
#include "../Tile/World.h"
#include "Renderable.h"
#include "../Utilities/RadixSort.hpp"
#include "UIComponent.h"
#include "TextureFromFile.h"
#include "../Logic/Action.h"
#include "../Logic/LogicComponent.h"
#include "../Subduction/CameraFollow.h"
#include "../Tile/TileComponent.h"
#include "../Tile/TileGameplayComponent.h"
#include "SpriteComponent.h"
#include "Slice.h"
#include "Drawable.h"
#include "AnchorVeil.h"
#include "UIFrame.h"
#include "AxisDrawPlane.h"

namespace Improbability
{ 

MakeMeta(GraphicsSystem);

void GraphicsSystem::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnObjectInitialized);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnObjectDeleted);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnKeyPress);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnQueryResolutions);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnQueryCurrentResolution);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnChangeResolution);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnChangeResolutionIndex);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnQueryFullscreen);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnChangeFullscreen);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(RecieveUIScene);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisChange);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisChangeComplete);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(SetTopMemory);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(SetFrontMemory);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(SetSideMemory);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(SetOmmitedLayers);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAnchorEvent);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetGBuffer);
}

void GraphicsSystem::InitializeGlobal()
{
  // we can only initialize the interface at this time
  GraphicsDevice::InitializeInterface();
}

void GraphicsSystem::InitializeInstance()
{
  g_engine_core->AddEventListener(MessageBank::OnObjectInitialized, this);
  g_engine_core->AddEventListener(MessageBank::OnObjectDeleted, this);
  g_engine_core->AddEventListener(MessageBank::OnKeyPress, this);
  g_engine_core->AddEventListener(MessageBank::OnAxisChange, this);
  g_engine_core->AddEventListener(MessageBank::OnAxisChangeComplete, this);
  g_engine_core->AddEventListener(MessageBank::OnQueryCurrentResolution, this);
  g_engine_core->AddEventListener(MessageBank::OnQueryResolutions, this);
  g_engine_core->AddEventListener(MessageBank::OnChangeResolution, this);
  g_engine_core->AddEventListener(MessageBank::OnChangeResolutionIndex, this);
  g_engine_core->AddEventListener(MessageBank::OnQueryFullscreen, this);
  g_engine_core->AddEventListener(MessageBank::OnChangeFullscreen, this);
  g_engine_core->AddEventListener(MessageBank::OnAnchorEvent, this);

  m_axis = m_prev_axis = 0;
  m_anchor_axis = -1;
  m_anchor_start = -1;
  m_anchor_end = -1;
  m_camera = 0;
  m_axis_cam = 0;
  ZeroMemory(&m_follow_cams, 3 * sizeof(Camera*));

  for (unsigned i = 0; i < 3; ++i)
  {
    m_planes[i] = new AxisDrawPlane;
  }

  m_planes[0]->SetTransform(Matrix4::CreateRotationY(k_pi/2.0f));
  m_planes[1]->SetTransform(Matrix4::CreateRotationX(-k_pi/2.0f));
  m_planes[2]->SetTransform(Matrix4::CreateRotationZ(0.0f));
  m_g_buffer = new RenderTarget(1.0f, 1.0f);

  m_rotating = false;

  m_anchor_veil = 0;
  m_anchor_flatten_timer = 0.0f;

  m_reset_device_count = g_reset_device_count;

  //Get TileSystem's render borders
  m_ui_scene = new UIFrame;
  m_ui_scene->Initialize();
  m_ui_scene->SetTexture((Texture*)0);
  m_ui_scene->SetPosition(Vector2(0.0f, 0.0f));
  m_ui_scene->SetSize(Vector2(GraphicsDevice::GetDeviceWidth(), GraphicsDevice::GetDeviceHeight()));
}

void GraphicsSystem::PostInitializeGlobal()
{
  // this has to be initialized after the window is so its over here
  bool fullscreen = g_engine_core->GetWindow()->GetFullscreen();
  GraphicsDevice::InitializeDevice(fullscreen, m_vsync);

  // check the required device caps
  CriticalErrorIf(!GraphicsDevice::RequiredDeviceCapabilitiesPresent(), "Device does not support necessary operations to play");

  g_engine_core->DispatchEvent(MessageBank::OnLoadPass, "GraphicsPreInitialize");

  g_engine_core->DispatchEvent(MessageBank::OnLoadPass, "GraphicsInitialize");

  // initialize the shape library
  DebugDraw::Initialize();
  Slice::Initialize();
}

void GraphicsSystem::UninitializeGlobal()
{
  // uninitialize EVERYTHING
  DebugDraw::Uninitialize();
  GraphicsDevice::UninitializeInterface();
  GraphicsDevice::UninitializeDevice();
}

void GraphicsSystem::UninitializeInstance()
{
  for (unsigned i = 0; i < 3; ++i)
  {
    delete m_planes[i];
  }

  m_ui_scene->Uninitialize();
  delete m_ui_scene;

  g_engine_core->RemoveEventListener(MessageBank::OnObjectInitialized, this);
  g_engine_core->RemoveEventListener(MessageBank::OnObjectDeleted, this);
  g_engine_core->RemoveEventListener(MessageBank::OnKeyPress, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAxisChange, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAxisChangeComplete, this);
  g_engine_core->RemoveEventListener(MessageBank::OnQueryCurrentResolution, this);
  g_engine_core->RemoveEventListener(MessageBank::OnQueryResolutions, this);
  g_engine_core->RemoveEventListener(MessageBank::OnChangeResolution, this);
  g_engine_core->RemoveEventListener(MessageBank::OnChangeResolutionIndex, this);
  g_engine_core->RemoveEventListener(MessageBank::OnQueryFullscreen, this);
  g_engine_core->RemoveEventListener(MessageBank::OnChangeFullscreen, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAnchorEvent, this);
}

void GraphicsSystem::UpdateInstance(float dt)
{
  if (GraphicsDevice::IsDeviceLost() != GraphicsInfoTypes::EC_OK)
  {
    return;
  }

  if (m_reset_device_count != g_reset_device_count)
  {
    ResetCameraAspect();

    m_reset_device_count = g_reset_device_count;
  }

  GraphicsDevice::BeginDraw();

  UpdateAnchorSmush(dt);

  // draw the main axis (if rotating this is the destination axis)
  DrawPlanes(dt);

  GraphicsDevice::Clear(m_clear_color);

  if (m_axis_cam)
  {
    DrawFromAxisCam();
  }

  DrawScreenSpace();

  GraphicsDevice::EndDraw();

  GraphicsDevice::Present();
}

void GraphicsSystem::Copy(System *dest, bool deep)
{
  if (deep)
  {
    GraphicsSystem *new_system = reinterpret_cast<GraphicsSystem*>(dest);
  }
}

void GraphicsSystem::Deserialize(Serializer &serializer)
{
  if(!Read(serializer, m_clear_color, "ClearColor"))
  {
    m_clear_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
  }

  if (!Read(serializer, m_vsync, "VSync"))
  {
    m_vsync = false;
  }

  if (!Read(serializer, m_anchor_flatten_max_time, "AnchorFlattenTime"))
  {
    m_anchor_flatten_max_time = 3.0f;
  }
}

void GraphicsSystem::OnObjectInitialized(CompositionId comp_id)
{
  Composition *comp = g_engine_core->SystemCall("ObjectManager", "GetObject", comp_id);
  Camera *cam = comp->GetComponent<Camera>();

  if (cam)
  {
    if (comp->GetType() == "Axis Camera")
    {
      m_axis_cam = cam;
    }
    else if (comp->GetType() == "Follow Camera")
    {
      for (unsigned i = 0; i < 3; ++i)
      {
        if (!m_follow_cams[i])
        {
          // special case for y cam
          if (i == 1)
            cam->SetUpVector(Vector3(0.0f, 0.0f, -1.0f));

          std::list<Action*> actions= cam->GetSibling<LogicComponent>()->GetActionList();

          for (auto action = actions.begin(); action != actions.end(); ++action)
          {
            if ((*action)->GetType() == "CameraFollow")
            {
              static_cast<CameraFollow*>((*action))->SetAxis(i);
            }
          }

          m_follow_cams[i] = cam;

          // found it's place, only three are allowed
          break;
        }
      }
    }
    else
    {
      m_camera = cam;
    }
  }

  SpriteComponent *sprt = comp->GetComponent<SpriteComponent>();

  if (sprt)
    m_sprites.push_back(sprt);

  LightComponent *light = comp->GetComponent<LightComponent>();

  if (light)
    m_lights.push_back(light);

  UIComponent *ssr = comp->GetComponent<UIComponent>();

  if (ssr)
    m_screen_space.push_back(ssr);
}

void GraphicsSystem::OnObjectDeleted(CompositionId comp_id)
{
  Composition *comp = g_engine_core->SystemCall("ObjectManager", "GetObject", comp_id);
  Camera *cam = comp->GetComponent<Camera>();

  if (cam)
  {
    if (cam == m_camera)
      m_camera = 0;
    else if (cam == m_axis_cam)
      m_axis_cam = 0;
    else if (cam == m_follow_cams[0])
      m_follow_cams[0] = 0;
    else if (cam == m_follow_cams[1])
      m_follow_cams[1] = 0;
    else if (cam == m_follow_cams[2])
      m_follow_cams[2] = 0;
  }

  {
    SpriteComponent *sprt = comp->GetComponent<SpriteComponent>();

    auto sprt_elem = std::find(m_sprites.begin(), m_sprites.end(), sprt);

    if (sprt && sprt_elem != m_sprites.end())
      m_sprites.erase(sprt_elem);
  }

  {
    LightComponent *light = comp->GetComponent<LightComponent>();

    auto light_elem = std::find(m_lights.begin(), m_lights.end(), light);

    if (light && light_elem != m_lights.end())
      m_lights.erase(light_elem);
  }

  {
    UIComponent *ssr = comp->GetComponent<UIComponent>();

    auto ssr_elem = std::find(m_screen_space.begin(), m_screen_space.end(), ssr);

    if (ssr && ssr_elem != m_screen_space.end())
      m_screen_space.erase(ssr_elem);
  }
}

void GraphicsSystem::OnKeyPress(KeyboardEvent key)
{
  switch(key.m_keycode)
  {
  case kKeyF11:
    GraphicsDevice::ToggleFullscreenState();  

    if (m_axis_cam)
      m_axis_cam->FixPerspective();

    m_ui_scene->SetSize(Vector2(GraphicsDevice::GetDeviceWidth(), GraphicsDevice::GetDeviceHeight()));
    break;
  }
}

void GraphicsSystem::OnQueryResolutions()
{
  std::vector<std::pair<unsigned, unsigned>> resolutions = GraphicsDevice::GetResolutions();

  for (unsigned i = 0; i < resolutions.size(); ++i)
  {
    g_engine_core->DispatchEvent("OnRecieveResolution", resolutions[i].first, resolutions[i].second, i);
  }
}

void GraphicsSystem::OnQueryCurrentResolution()
{
  g_engine_core->DispatchEvent("OnRecieveCurrentResolution", g_engine_core->GetWindow()->GetWidth(), g_engine_core->GetWindow()->GetHeight());
}

void GraphicsSystem::OnChangeResolution(int width, int height)
{
  GraphicsDevice::ResizeDevice(width, height);

  ResetCameraAspect();
}

void GraphicsSystem::OnChangeResolutionIndex(int index)
{
  std::vector<std::pair<unsigned, unsigned>> resolutions = GraphicsDevice::GetResolutions();

  GraphicsDevice::ResizeDevice(resolutions[index].first, resolutions[index].second);

  ResetCameraAspect();
}

void GraphicsSystem::OnQueryFullscreen()
{
  g_engine_core->DispatchEvent("OnRecieveFullscreen", GraphicsDevice::GetFullscreenState());
}

void GraphicsSystem::OnChangeFullscreen(bool fullscreen)
{
  GraphicsDevice::SetFullscreenState(fullscreen);

  ResetCameraAspect();
}

void GraphicsSystem::RecieveUIScene(Texture *tex)
{
  m_ui_scene->SetTexture(tex);
}

RenderTarget* GraphicsSystem::GetGBuffer()
{
  return m_g_buffer;
}

void GraphicsSystem::DrawBackLayers(unsigned axis, float dt)
{
  if (!m_back_layers[axis].empty())
  {
    // move the camera forward
    Vector4 old_pos = m_camera->GetParent()->GetPosition();
    Vector4 new_pos = old_pos;
    if (m_anchor_flatten_timer > 1.0f - k_epsilon)
    {
      //////////////////////////////////////////////////////////////////////////////////////////////
      /////// Changed the camera offset as it was causing a large gap between solid and back layers
      //////////////////////////////////////////////////////////////////////////////////////////////
      //if (m_solid_layers[axis].size() > 1)
      //  new_pos.m_a[axis] -= abs(m_back_layers[axis].back() - int(m_camera->GetTarget().m_a[axis])) - 1.5f;
      //else
      //  new_pos.m_a[axis] -= abs(m_back_layers[axis].back() - int(m_camera->GetTarget().m_a[axis])) - 2.0f;
      if (m_solid_layers[axis].size() > 1)
        new_pos.m_a[axis] -= abs(m_back_layers[axis].back() - int(m_camera->GetTarget().m_a[axis])) - 1.5f;
      else
        new_pos.m_a[axis] -= abs(m_back_layers[axis].back() - int(m_camera->GetTarget().m_a[axis])) - 1.0f;
    }
    else
    {
      new_pos.m_a[axis] -= (abs(m_back_layers[axis].back() - int(m_camera->GetTarget().m_a[axis])) - 0.5f) * m_anchor_flatten_timer - 1.0f;
    }

    m_camera->GetParent()->SetPosition(new_pos);
    m_camera->Set();

    AddSprites(axis, dt, m_back_layers[axis].front(), m_back_layers[axis].back());
    AddSlices(axis, dt, m_back_layers[axis].front(), m_back_layers[axis].back());

    DrawScene();

    m_camera->Unset();
    m_camera->GetParent()->SetPosition(old_pos);
  }
}

void GraphicsSystem::DrawFrontLayers(unsigned axis, float dt)
{
  if (!m_front_layers[axis].empty())
  {
    // move the camera forward
    Vector4 old_pos = m_camera->GetParent()->GetPosition();
    Vector4 new_pos = old_pos;
    if (m_anchor_flatten_timer > 1.0f - k_epsilon)
    {
      if (m_solid_layers[axis].size() > 1)
        new_pos.m_a[axis] += abs(m_front_layers[axis].front() - int(m_camera->GetTarget().m_a[axis])) - 0.5f;
      else
        new_pos.m_a[axis] += abs(m_front_layers[axis].front() - int(m_camera->GetTarget().m_a[axis])) - 1.0f;
    }
    else
    {
      new_pos.m_a[axis] += (abs(m_front_layers[axis].front() - int(m_camera->GetTarget().m_a[axis])) - 0.5f) * m_anchor_flatten_timer;
    }

    m_camera->GetParent()->SetPosition(new_pos);
    m_camera->Set();

    AddSprites(axis, dt, m_front_layers[axis].front(), m_front_layers[axis].back());
    AddSlices(axis, dt, m_front_layers[axis].front(), m_front_layers[axis].back());

    DrawScene();

    m_camera->Unset();
    m_camera->GetParent()->SetPosition(old_pos);
  }
}

void GraphicsSystem::DrawSolidLayers(unsigned axis, float dt)
{
  if (!m_solid_layers[axis].empty())
  {
    //m_planes[axis]->UnsetRenderTarget();

    //m_g_buffer->SetRenderTarget(0);

    //GraphicsDevice::Clear(Color(0.0f, 0.0f, 0.0f, 0.01f));

    if (m_anchor_flatten_timer > 1.0f)
      m_camera->UseOrthographic();
    else
      m_camera->InterpolateProjection(1.0f - m_anchor_flatten_timer);
    m_camera->Set();

    AddSprites(axis, dt, m_solid_layers[axis].front(), m_solid_layers[axis].back());
    AddSlices(axis, dt, m_solid_layers[axis].front(), m_solid_layers[axis].back());

    DrawScene();

//     m_g_buffer->UnsetRenderTarget();
// 
//     m_planes[axis]->SetRenderTarget();
// 
//     AddLights(axis, dt, m_solid_layers[axis].front(), m_solid_layers[axis].back());
// 
//     DrawScene();
    
    m_camera->Unset();
    m_camera->UsePerspective();
  }
}

void GraphicsSystem::AddSprites(unsigned axis, float dt, int first_layer, int last_layer)
{
  for (auto it = m_sprites.begin(); it != m_sprites.end(); ++it)
  {
    int sprite_layer = int((*it)->GetParent()->GetPosition().m_a[axis]);

    // if the sprite is not in the layers to render just continue
    if (sprite_layer > last_layer || sprite_layer < first_layer)
    {
      continue;
    }

    DrawElement new_element;

    (*it)->Update(dt, axis);

    new_element.m_drawable = *it;
    new_element.m_id = (*it)->GetDrawOrder(m_camera);
    new_element.m_element_transform = (*it)->GetWorldTransform();

    m_draw_queue.push_back(new_element);
  }
}

void GraphicsSystem::AddSlices(unsigned axis, float dt, int first_layer, int last_layer)
{
  for (auto it = m_slices[axis].begin(); it != m_slices[axis].end(); ++it)
  {
    int slice_layer = (*it)->GetLayer();

    // if the slice is not in the layers to render just continue
    if (slice_layer > last_layer || slice_layer < first_layer)
    {
      continue;
    }

    DrawElement new_element;

    (*it)->Update(dt);

    new_element.m_drawable = *it;
    new_element.m_id = (*it)->GetDrawOrder(m_camera);
    new_element.m_element_transform = (*it)->GetWorldTransform();

    m_draw_queue.push_back(new_element);
  }
}

void GraphicsSystem::AddLights(unsigned axis, float dt, int first_layer, int last_layer)
{
  for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
  {
    int light_layer = int((*it)->GetParent()->GetPosition().m_a[axis]);

    // if the sprite is not in the layers to render just continue
    if (light_layer > last_layer || light_layer < first_layer)
    {
      continue;
    }

    DrawElement new_element;

    (*it)->Update(dt, axis);

    new_element.m_drawable = *it;
    new_element.m_id = (*it)->GetDrawOrder(m_camera);
    new_element.m_element_transform = (*it)->GetWorldTransform();

    m_draw_queue.push_back(new_element);
  }
}

void GraphicsSystem::DrawAnchorVeil(int axis)
{
  if (m_anchor_veil != 0 && m_anchor_veil->GetAxis() == m_axis)
  {
    TilePos3 focus_pos = g_engine_core->SystemCall("TileSystem", "GetFocusPosition");

    m_anchor_veil->Update(focus_pos, m_memory[axis]);

    m_anchor_veil->Draw();
  }
}

void GraphicsSystem::DrawScene()
{
  SortDrawQueue();

  MatrixStack<Matrix4> &world = GraphicsDevice::GetWorldStack();

  for (unsigned i = 0; i < m_draw_queue.size(); ++i)
  {
    world.Push();
    world.LoadMatrix(m_draw_queue[i].m_element_transform);

    m_draw_queue[i].m_drawable->Draw();

    world.Pop();
  }

  m_draw_queue.clear();
}

void GraphicsSystem::SortDrawQueue()
{
  if (!m_draw_queue.empty())
    RadixSort(&(m_draw_queue[0]), 0, m_draw_queue.size());
}

void GraphicsSystem::ResetCameraAspect()
{
  if(m_axis_cam)
    m_axis_cam->FixPerspective();

  if (m_follow_cams[0])
    m_follow_cams[0]->FixPerspective();
  if (m_follow_cams[1])
    m_follow_cams[1]->FixPerspective();
  if (m_follow_cams[2])
    m_follow_cams[2]->FixPerspective();

  m_ui_scene->SetSize(Vector2(GraphicsDevice::GetDeviceWidth(), GraphicsDevice::GetDeviceHeight()));
}

void GraphicsSystem::DrawScreenSpace()
{
  int width = GraphicsDevice::GetDeviceWidth();
  int height = GraphicsDevice::GetDeviceHeight();

  GraphicsDevice::SetAlphaEnable(true);
  GraphicsDevice::SetDepthEnable(false);

  for (auto it = m_screen_space.begin(); it != m_screen_space.end(); ++it)
  {
    (*it)->Draw(static_cast<float>(width), static_cast<float>(height));
  }

  if (m_ui_scene->GetTexture() != 0)
    m_ui_scene->Draw(static_cast<float>(width), static_cast<float>(height));

  GraphicsDevice::SetAlphaEnable(false);
  GraphicsDevice::SetDepthEnable(true);
}

void GraphicsSystem::DrawDebugScene(float dt)
{
  // draw all debug draw objects
  DebugDraw::DrawQueue(dt);
}

void GraphicsSystem::OnAxisChange(int old_axis, int new_axis)
{
  g_engine_core->DispatchEvent("OnAnchorSmushComplete");
  m_rotating = true;
  m_axis = new_axis;
  m_prev_axis = old_axis;

  Composition *focus_object = g_engine_core->SystemCall("ObjectManager", "GetObjectByName", "Player");
  if(focus_object != NULL)
  {
    m_last_target_pos = focus_object->GetPosition().ToVector3();
  }

  int anchor_axis = g_engine_core->SystemCall("TileSystem", "GetAnchorAxis");

  // add anchor ranges
  if (m_axis == anchor_axis)
  {
    m_anchor_flatten_timer = 0.0f;

    int anchor_begin = g_engine_core->SystemCall("TileSystem", "GetAnchorBegin");
    int anchor_end = g_engine_core->SystemCall("TileSystem", "GetAnchorEnd");

    if (anchor_begin < anchor_end)
      std::swap(anchor_begin, anchor_end);

    if (anchor_end >= 0 && int(m_last_target_pos.m_a[m_axis]) >= anchor_end && int(m_last_target_pos.m_a[m_axis]) <= anchor_begin)
    {
      for (int depth = anchor_end; depth <= anchor_begin; ++depth)
      {
        m_slices[m_axis].push_back(new Slice(m_axis, depth, 0, 0));
        m_solid_layers[m_axis].push_back(depth);
      }
    }
    else
    {
      // put in the new slice
      m_slices[m_axis].push_back(new Slice(m_axis, int(m_last_target_pos.m_a[m_axis]), 0, 0));
      m_solid_layers[m_axis].push_back(int(m_last_target_pos.m_a[m_axis]));
    }
  }
  else
  {
    // put in the new slice
    m_slices[m_axis].push_back(new Slice(m_axis, int(m_last_target_pos.m_a[m_axis]), 0, 0));
    m_solid_layers[m_axis].push_back(int(m_last_target_pos.m_a[m_axis]));
  }

  // must require that the layers are sorted
  std::sort(m_solid_layers[m_axis].begin(), m_solid_layers[m_axis].end());

  // delete all of the layers from the previous axis
  for (auto it = m_slices[m_prev_axis].begin(); it != m_slices[m_prev_axis].end(); ++it)
  {
    if ((*it)->GetLayer() == int(m_last_target_pos.m_a[m_prev_axis]))
      continue;

    delete *it;
  }

  m_slices[m_prev_axis].clear();
  m_solid_layers[m_prev_axis].clear();
  m_slices[m_prev_axis].push_back(new Slice(m_prev_axis, int(m_last_target_pos.m_a[m_prev_axis]), 0, 0));
  m_solid_layers[m_prev_axis].push_back(int(m_last_target_pos.m_a[m_prev_axis]));

  m_back_layers[m_prev_axis].clear();
  m_front_layers[m_prev_axis].clear();

  for (auto it = m_sprites.begin(); it != m_sprites.end(); ++it)
  {
    (*it)->Reset(m_axis);
  }
}

void GraphicsSystem::OnAxisChangeComplete(int axis)
{
  m_rotating = false;
  if(axis == m_anchor_axis)
    m_anchor_flatten_timer = 0.f;

  TilePos3 focus_pos = g_engine_core->SystemCall("TileSystem", "GetFocusPosition");

  // delete all of the layers from the previous axis
  for (auto it = m_slices[m_prev_axis].begin(); it != m_slices[m_prev_axis].end(); ++it)
  {
    delete *it;
  }

  m_slices[m_prev_axis].clear();
  m_solid_layers[m_prev_axis].clear();

  if (std::find(m_solid_layers[m_axis].begin(), m_solid_layers[m_axis].end(), focus_pos.m_a[m_axis]) == m_solid_layers[m_axis].end())
  {
    m_slices[m_axis].push_back(new Slice(m_axis, focus_pos.m_a[m_axis], 0, 0));
    m_solid_layers[m_axis].push_back(focus_pos.m_a[m_axis]);
  }

  int anchor_axis = g_engine_core->SystemCall("TileSystem", "GetAnchorAxis");

  // add memory that is not in anchor ranges
  if (m_axis == anchor_axis)
  {
    int anchor_begin = g_engine_core->SystemCall("TileSystem", "GetAnchorBegin");
    int anchor_end = g_engine_core->SystemCall("TileSystem", "GetAnchorEnd");

    if (anchor_begin < anchor_end)
      std::swap(anchor_begin, anchor_end);

    // add in all of the memory layers now
    if (anchor_end >= 0 && int(m_last_target_pos.m_a[m_axis]) >= anchor_end && int(m_last_target_pos.m_a[m_axis]) <= anchor_begin)
    {
      for (auto it = m_memory[m_axis].begin(); it != m_memory[m_axis].end(); ++it)
      {
        if (it->layer < anchor_end)
        {
          m_slices[m_axis].push_back(new Slice(m_axis, it->layer, it->age, it->maxAge));
          m_back_layers[m_axis].push_back(it->layer);
        }
        else if (it->layer > anchor_begin)
        {
          m_slices[m_axis].push_back(new Slice(m_axis, it->layer, it->age, it->maxAge));
          m_front_layers[m_axis].push_back(it->layer);
        }
      }
    }
    else
    {
      // add in all of the memory layers now
      for (auto it = m_memory[m_axis].begin(); it != m_memory[m_axis].end(); ++it)
      {
        if (it->layer < focus_pos.m_a[axis])
        {
          m_slices[m_axis].push_back(new Slice(m_axis, it->layer, it->age, it->maxAge));
          m_back_layers[m_axis].push_back(it->layer);
        }
        if (it->layer > focus_pos.m_a[axis])
        {
          m_slices[m_axis].push_back(new Slice(m_axis, it->layer, it->age, it->maxAge));
          m_front_layers[m_axis].push_back(it->layer);
        }
      }
    }
  }
  else
  {
    // add in all of the memory layers now
    for (auto it = m_memory[m_axis].begin(); it != m_memory[m_axis].end(); ++it)
    {
      if (it->layer < focus_pos.m_a[axis])
      {
        m_slices[m_axis].push_back(new Slice(m_axis, it->layer, it->age, it->maxAge));
        m_back_layers[m_axis].push_back(it->layer);
      }
      if (it->layer > focus_pos.m_a[axis])
      {
        m_slices[m_axis].push_back(new Slice(m_axis, it->layer, it->age, it->maxAge));
        m_front_layers[m_axis].push_back(it->layer);
      }
    }
  }

  // must require that the layers are sorted
  std::sort(m_front_layers[m_axis].begin(), m_front_layers[m_axis].end());
  std::sort(m_back_layers[m_axis].begin(), m_back_layers[m_axis].end());

  for (auto it = m_sprites.begin(); it != m_sprites.end(); ++it)
  {
    (*it)->Reset(m_prev_axis);
  }
}

void GraphicsSystem::OnAnchorEvent(int anchor_event_type)
{
  TilePos3 focus_pos = g_engine_core->SystemCall("TileSystem", "GetFocusPosition");

  switch (anchor_event_type)
  {
  case 0: // set
    if (m_axis == 0)
    {
      m_anchor_veil = new AnchorVeil(focus_pos, m_axis);
    }
    else
    {
      m_anchor_veil = new AnchorVeil(focus_pos, m_axis);
    }

    m_anchor_axis = (m_axis == 0) ? 2 : 0;
    m_anchor_start = g_engine_core->SystemCall("TileSystem", "GetAnchorBegin");
    break;
  case 1: // commit
    m_anchor_veil->Commit(focus_pos);
    m_anchor_end = g_engine_core->SystemCall("TileSystem", "GetAnchorEnd");
    break;
  case 2: // clear

    m_anchor_axis = -1;
    m_anchor_end = -1;
    m_anchor_start = -1;

    if (m_anchor_veil)
    {
      delete m_anchor_veil;
      m_anchor_veil = 0;
    }

    // delete all of the layers from the axis
    for (auto it = m_slices[m_axis].begin(); it != m_slices[m_axis].end(); ++it)
    {
      delete *it;
    }

    m_slices[m_axis].clear();
    m_solid_layers[m_axis].clear();
    
    m_slices[m_axis].push_back(new Slice(m_axis, focus_pos.m_a[m_axis], 0, 0));
    m_solid_layers[m_axis].push_back(focus_pos.m_a[m_axis]);

    // add in all of the memory layers now
    for (auto it = m_memory[m_axis].begin(); it != m_memory[m_axis].end(); ++it)
    {
      if (it->layer < focus_pos.m_a[m_axis])
      {
        m_slices[m_axis].push_back(new Slice(m_axis, it->layer, it->age, it->maxAge));
        m_back_layers[m_axis].push_back(it->layer);
      }
      if (it->layer > focus_pos.m_a[m_axis])
      {
        m_slices[m_axis].push_back(new Slice(m_axis, it->layer, it->age, it->maxAge));
        m_front_layers[m_axis].push_back(it->layer);
      }
    }

    for (auto it = m_sprites.begin(); it != m_sprites.end(); ++it)
    {
      (*it)->Reset(m_prev_axis);
    }
    break;
  }
}

void GraphicsSystem::SetTopMemory(int *Top_memory)
{
  unsigned old_size = m_memory[1].size();

  m_memory[1] = *(reinterpret_cast<std::list<MemoryLayer>*>(Top_memory));
}

void GraphicsSystem::SetFrontMemory(int *Front_memory)
{
  unsigned old_size = m_memory[2].size();

  m_memory[2] = *(reinterpret_cast<std::list<MemoryLayer>*>(Front_memory));
}

void GraphicsSystem::SetSideMemory(int *Side_memory)
{
  unsigned old_size = m_memory[0].size();

  m_memory[0] = *(reinterpret_cast<std::list<MemoryLayer>*>(Side_memory));
}

void GraphicsSystem::SetOmmitedLayers(int *Ommited_layers)
{
  m_OmmitedLayers = *(reinterpret_cast<std::list<MemoryLayer>*>(Ommited_layers));
}

void GraphicsSystem::UpdateAnchorSmush(float dt)
{
  if (m_anchor_axis == m_axis && m_anchor_end != -1)
  {
    int focal_pos = static_cast<TilePos3>(g_engine_core->SystemCall("TileSystem", "GetFocusPosition")).m_a[m_axis];
    bool in_anchor_region = false;
    if((focal_pos >= m_anchor_start && focal_pos <= m_anchor_end) ||
      (focal_pos <= m_anchor_start && focal_pos >= m_anchor_end))
      in_anchor_region = true;
    if(!m_rotating && in_anchor_region)
    {
      if(m_anchor_flatten_timer == 0)
        g_engine_core->DispatchEvent("OnAnchorSmushBegin");

      m_anchor_flatten_timer += dt / m_anchor_flatten_max_time;

      if(m_anchor_flatten_timer >= 1.0f)
        g_engine_core->DispatchEvent("OnAnchorSmushComplete");
    }
    else
      m_anchor_flatten_timer = 1.0f;
  }
  else
  {
    m_anchor_flatten_timer = 1.0f;
  }
}

void GraphicsSystem::DrawFromAxisCam()
{
  m_axis_cam->Set();
  m_camera = m_axis_cam;

  VertexShader *basic_vertex = VertexShader::GetShader("basic_texture");
  PixelShader *transparent_pixel = PixelShader::GetShader("constant_transparency");

  basic_vertex->Set();
  transparent_pixel->Set();

  transparent_pixel->SetConstant("u_alpha_value", 0.75f);

  GraphicsDevice::SetAlphaEnable(true);

  if (!m_rotating)
  {
    switch (m_axis)
    {
    case 0:
      m_planes[0]->Draw(2);
      m_planes[0]->Draw(0);
      m_planes[0]->Draw(3);
      m_planes[0]->Draw(1);
      break;
    case 1:
      m_planes[1]->Draw(1);
      m_planes[1]->Draw(0);
      m_planes[1]->Draw(3);
      m_planes[1]->Draw(2);
      break;
    case 2:
      m_planes[2]->Draw(3);
      m_planes[2]->Draw(1);
      m_planes[2]->Draw(2);
      m_planes[2]->Draw(0);
      break;
    }
  }
  else
  {
    if ((m_axis == 0 && m_prev_axis == 1) || (m_axis == 1 && m_prev_axis == 0))
    {
      m_planes[0]->Draw(2);
      m_planes[1]->Draw(1);
      m_planes[0]->Draw(0);
      m_planes[1]->Draw(0);
      m_planes[1]->Draw(3);
      m_planes[0]->Draw(3);
      m_planes[0]->Draw(1);
      m_planes[1]->Draw(2);
    }
    else if ((m_axis == 0 && m_prev_axis == 2) || (m_axis == 2 && m_prev_axis == 0))
    {
      m_planes[0]->Draw(2);
      m_planes[2]->Draw(3);
      m_planes[2]->Draw(1);
      m_planes[0]->Draw(0);
      m_planes[0]->Draw(3);
      m_planes[2]->Draw(2);
      m_planes[0]->Draw(1);
      m_planes[2]->Draw(0);
    }
    else
    {
      m_planes[1]->Draw(1);
      m_planes[2]->Draw(3);
      m_planes[2]->Draw(1);
      m_planes[1]->Draw(0);
      m_planes[1]->Draw(3);
      m_planes[2]->Draw(2);
      m_planes[1]->Draw(2);
      m_planes[2]->Draw(0);
    }
  }

  GraphicsDevice::SetAlphaEnable(false);

  basic_vertex->Unset();
  transparent_pixel->Unset();

  m_camera = 0;

  m_axis_cam->Unset();
}

void GraphicsSystem::DrawPlanes(float dt)
{
  m_planes[m_axis]->SetRenderTarget();

  GraphicsDevice::Clear(m_clear_color);

  if (m_follow_cams[m_axis])
  {
    m_camera = m_follow_cams[m_axis];

    DrawBackLayers(m_axis, dt);
    DrawSolidLayers(m_axis, dt);
    DrawFrontLayers(m_axis, dt);

    m_follow_cams[m_axis]->Set();
    DrawAnchorVeil(m_axis);

    DrawDebugScene(dt);

    m_camera = 0;
    m_follow_cams[m_axis]->Unset();
  }

  m_planes[m_axis]->UnsetRenderTarget();

  if (m_rotating)
  {
    // when rotating we have two axis to draw the source axis
    m_planes[m_prev_axis]->SetRenderTarget();

    GraphicsDevice::Clear(m_clear_color);

    if (m_follow_cams[m_prev_axis])
    {
      m_camera = m_follow_cams[m_prev_axis];

      DrawBackLayers(m_prev_axis, dt);
      DrawSolidLayers(m_prev_axis, dt);
      DrawFrontLayers(m_prev_axis, dt);

      m_follow_cams[m_prev_axis]->Set();
      if (m_anchor_veil != 0 && m_anchor_veil->GetAxis() == m_prev_axis)
      {
        DrawAnchorVeil(m_prev_axis);
      }

      DrawDebugScene(dt);

      m_camera = 0;
      m_follow_cams[m_prev_axis]->Unset();
    }

    m_planes[m_prev_axis]->UnsetRenderTarget();
  }
}

}