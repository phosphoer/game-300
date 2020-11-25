// Nicholas Baldwin
// 2011/06/25
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Precompiled.h"

#include "../core/system.h"
#include "../composition/composition.h"
#include "RenderTarget.h"
#include "LightComponent.h"

namespace Improbability
{

class Camera;
class RenderComponent;
class UIComponent;
class MetaComposition;
class AnimationComponent;
class Renderable;
class World;
class SpriteComponent;
class Drawable;
class Slice;
class AnchorVeil;
class Texture;
class AxisDrawPlane;
class UIFrame;
struct MemoryLayer;

class GraphicsSystem : public System
{
public:
  // needed to get meta info about the system
  static void BindMeta(MetaComposition*);

  // non-obvious methods inherited from System
  void InitializeGlobal();
  void InitializeInstance();
  void PostInitializeGlobal();
  void UninitializeGlobal();
  void UninitializeInstance();
  void UpdateInstance(float dt);

  void DrawPlanes( float dt );

  void DrawFromAxisCam();

  void UpdateAnchorSmush( float dt );

  void Copy(System *dest, bool deep);
  void Deserialize(Serializer &serializer);

  // message listeners
  void OnObjectInitialized(CompositionId comp_id);
  void OnObjectDeleted(CompositionId comp_id);
  void OnKeyPress(KeyboardEvent key);
  void OnQueryResolutions();
  void OnQueryCurrentResolution();
  void OnChangeResolution(int width, int height);
  void OnChangeResolutionIndex(int index);
  void OnQueryFullscreen();
  void OnChangeFullscreen(bool fullscreen);
  void OnAnchorEvent(int anchor_event_type);

  // system calls
  void RecieveUIScene(Texture *tex);
  void OnAxisChange(int old_axis, int new_axis);
  void OnAxisChangeComplete(int axis);
  void SetTopMemory(int *top_memory);
  void SetSideMemory(int *Side_memory);
  void SetFrontMemory(int *Front_memory);
  void SetOmmitedLayers(int *Ommited_layers);
  RenderTarget* GetGBuffer();

private:
  // draws all of the elements currently in the draw queue
  void DrawScene();
  // draws the debug shapes
  void DrawDebugScene(float dt);
  // draws the screen space elements
  void DrawScreenSpace();

  // moves the current camera to focus a layer ahead of the back layers
  // then adds the sprites and slices from the back layers and draws them
  // to a render target, then moves the camera back
  void DrawBackLayers(unsigned axis, float dt);
  // adds the sprites and slices from the front layers and draws them to
  // a render target
  void DrawFrontLayers(unsigned axis, float dt);
  // makes the camera orthographic, then adds the sprites and slices from
  // the solid layers and draws them to a g-buffer, adds lights and uses
  // the g-buffer to draw to a render target, then returns the camera to
  // perspective
  void DrawSolidLayers(unsigned axis, float dt);

  // adds elements to the draw queue
  // axis - the axis to draw from in the next draw call
  // dt - the change in time since last update
  // first_layer - the first layer to update and draw from this draw call
  // last_layer - the last layer to update and draw from this draw call
  void AddSprites(unsigned axis, float dt, int first_layer, int last_layer);
  void AddSlices(unsigned axis, float dt, int first_layer, int last_layer);
  void AddLights(unsigned axis, float dt, int first_layer, int last_layer);

  // Draws the anchor veil
  void DrawAnchorVeil(int axis);

  // etc helper calls
  void SortDrawQueue();
  void ResetCameraAspect();

private:
  // deserialized values
  bool m_vsync;
  Vector4 m_clear_color;

  // cameras
  Camera *m_camera;
  Camera *m_follow_cams[3];
  Camera *m_axis_cam;

  // render targets
  RenderTarget *m_g_buffer;
  AxisDrawPlane *m_planes[3];

  // layers to draw
  std::vector<int> m_back_layers[3];
  std::vector<int> m_front_layers[3];
  std::vector<int> m_solid_layers[3];

  // all of the drawable elements
  Slice *m_anchor_filter_slice;
  AnchorVeil *m_anchor_veil;
  std::vector<SpriteComponent*> m_sprites;
  std::vector<LightComponent*> m_lights;
  std::vector<UIComponent*> m_screen_space;
  std::vector<Slice*> m_slices[3];
  UIFrame* m_ui_scene;

  // struct used for sorting the draw queue
  struct DrawElement
  {
    unsigned m_id;
    Drawable *m_drawable;
    Matrix4 m_element_transform;
  };

  // this is the queue of what is drawn when DrawScene is called
  std::vector<DrawElement> m_draw_queue;

  // rotation stuff
  bool m_rotating;
  int m_axis;
  int m_prev_axis;
  Vector3 m_last_target_pos;
  int m_render_range;

  // anchoring stuff
  int m_anchor_axis;
  int m_anchor_start;
  int m_anchor_end;
  int m_anchor_range;
  int m_obscured;
  float m_anchor_flatten_timer;
  float m_anchor_flatten_max_time;

  //Local versions of layer memories
  std::list<MemoryLayer> m_memory[3];
  std::list<MemoryLayer> m_OmmitedLayers;

  int m_reset_device_count;
};

}