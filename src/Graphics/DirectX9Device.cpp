// Nicholas Baldwin
// 2011/06/29
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"

// this is the ONLY file that should include DirectX
#define D3D_DEBUG_INFO
#include <d3d9.h>
#include <d3dx9.h>
#include <Dxerr.h>

#include "GraphicsDevice.h"

#include "../core/EngineCore.h"
#include "../Core/WindowSystem.h"

#include "../math/MatrixStack.h"
#include <stdint.h>

// wraps the release functionality for component object model objects
#define ReleaseCOM(com) { if(com){ (com)->Release();(com) = 0; } }
#define MaxShaderConstants 256;

namespace Improbability
{

namespace GraphicsDevice
{
// global constants
static bool g_fullscreen_enabled;
const D3DFORMAT k_display_format = D3DFMT_X8R8G8B8;
const D3DFORMAT k_back_buffer_format = D3DFMT_X8R8G8B8;

// global directx interfaces
static IDirect3D9 *g_direct_3d_interface;
static D3DPRESENT_PARAMETERS g_present_parameters = {0};
static IDirect3DDevice9 *g_device;
// stores the DirectX interface to the declaration as well as an easily readable
// definition for the interface
struct VertexDeclaration
{
  IDirect3DVertexDeclaration9 *m_declaration;
  std::vector<GraphicsVertexTypes::VertexElement> m_definition;
  unsigned m_vertex_size;
  unsigned m_instances;
  bool m_uses_instance_stream;
  bool m_was_lost;
};

// stores the DirectX interface to the vertex and index buffers as well as the
// handle to the declaration used and some information about the buffers
struct VertexBuffer
{
  IDirect3DVertexBuffer9 *m_buffer;
  IDirect3DIndexBuffer9 *m_indexes;
  unsigned m_declaration_index;
  bool m_is_dynamic;
  bool m_is_instance_data;
  bool m_was_lost;
  unsigned m_vertex_buffer_size;
  unsigned m_index_buffer_size;
};

// stores the vertex declarations separate from the buffers (in case one declaration is used by multiple buffers)
static std::vector<VertexDeclaration> g_vertex_declarations;
// stores all created
static std::vector<VertexBuffer> g_vertex_buffers;

// stores the DirectX interface to the effect and handles to all of the
// registered techniques and constants in the effect

struct VertexShaderInfo
{
  IDirect3DVertexShader9 *m_shader;
  ID3DXConstantTable *m_constant_table;
  std::vector<D3DXHANDLE> m_constants;
};

struct PixelShaderInfo
{
  IDirect3DPixelShader9 *m_shader;
  ID3DXConstantTable *m_constant_table;
  std::vector<D3DXHANDLE> m_constants;
};

// all of the registered shaders
static std::vector<VertexShaderInfo> g_vertex_shaders;
static std::vector<PixelShaderInfo> g_pixel_shaders;

// stores all info needed to define a DirectX texture
struct TextureInfo
{
  IDirect3DTexture9 *m_texture;
  unsigned m_width;
  unsigned m_height;
  unsigned m_mipmap_levels;
  GraphicsOptionTypes::Usage m_usage;
  GraphicsInfoTypes::Format m_format;
  bool m_is_dynamic;
  bool m_is_render_target;
  bool m_was_lost;
};

// all of the registered textures
static std::vector<TextureInfo> g_textures;

struct RenderTargetInfo
{
  unsigned m_texture_data;
  IDirect3DSurface9 *m_render_target;
  float m_width;
  float m_height;
};

static std::vector<RenderTargetInfo> g_render_targets;
static RenderTargetInfo *g_active_render_target = 0;
static IDirect3DSurface9 *g_back_buffer = 0;

// the current active shader
static VertexShaderInfo *g_active_vertex_shader = 0;
static PixelShaderInfo *g_active_pixel_shader = 0;

// the modelview and projection matrices are automatically sent to whatever
// shader is active (the shaders MUST have uniform extern float4x4's
// u_model_view_matrix and u_projection_matrix)
static MatrixStack<Matrix4> g_world_matrix;
static MatrixStack<Matrix4> g_view_matrix;
static MatrixStack<Matrix4> g_projection_matrix;

// Just a note about comments and documentation:
// I will be leaving off comments on most HandleErrorResult and ErrorIf blocks
// they should either be obvious or have a description in the block that will
// be displayed if it is fired. If there is any question about what a certain
// DirectX call does the MSDN documentation does a much better job than what I
// would be able to do. If I am doing any sort of trick or thing that isn't just
// the standard thing you would do I will document it, but otherwise the code
// should be fairly self documenting
//
// Also this is a huge file and I don't want to do too much documentation...
//
// Another note about HandleErrorResult and ErrorIf in this file:
// I use a strange format for the macro calls for error handling in this file
// mostly it is to keep myself sane in terms of line length. But I've also found
// nice in terms of readability. The expression being tested is the first
// parameter and takes up the lines that it needs to, the format string being
// output is the second parameter and is on the line after the tested expression
// along with the variables that format it. The calling parentheses are in a
// ANSI style block format in which they each get their own line, the
// terminating parenthesis has the semicolon following it. I am consistent in
// this style and find it more readable (also it makes sense as the expression
// is evaluated in it's own scope so the indentation is technically correct),
// therefore I hope to continue to use this style for error checking blocks.

void InitializeInterface()
{
  g_direct_3d_interface = Direct3DCreate9(D3D_SDK_VERSION);
}

void InitializeDevice(bool start_fullscreen, bool start_vsync)
{
  D3DDISPLAYMODE current_display_mode;

  HandleErrorResult
  (
    g_direct_3d_interface->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &current_display_mode),
    "Error getting the display mode"
  );

  HandleErrorResult
  (
    g_direct_3d_interface->CheckDeviceType(D3DADAPTER_DEFAULT,
                                           D3DDEVTYPE_HAL,
                                           current_display_mode.Format,
                                           current_display_mode.Format,
                                           true),
    "Cannot support specified format for windowed mode."
  );
  HandleErrorResult
  (
    g_direct_3d_interface->CheckDeviceType(D3DADAPTER_DEFAULT,
                                           D3DDEVTYPE_HAL,
                                           k_display_format,
                                           k_back_buffer_format,
                                           false),
    "Cannot support specified format for fullscreen mode."
  );

  D3DCAPS9 device_capabilities;

  HandleErrorResult
  (
    g_direct_3d_interface->GetDeviceCaps(D3DADAPTER_DEFAULT,
                                         D3DDEVTYPE_HAL,
                                         &device_capabilities),
    "Problem getting the device capabilities."
  );

  DWORD device_behavior = 0;
  if (device_capabilities.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    device_behavior |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
  else
    device_behavior |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

  //if (device_capabilities.DevCaps & D3DDEVCAPS_PUREDEVICE &&
  //  device_capabilities.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
  //  device_behavior |= D3DCREATE_PUREDEVICE;

  int width  = GetSystemMetrics(SM_CXSCREEN);
  int height  = GetSystemMetrics(SM_CYSCREEN);

  g_present_parameters.BackBufferWidth = (start_fullscreen ? width : 0);
  g_present_parameters.BackBufferHeight = (start_fullscreen ? height : 0);
  g_present_parameters.BackBufferFormat = (start_fullscreen ? k_back_buffer_format : D3DFMT_UNKNOWN);
  g_present_parameters.BackBufferCount = 1;
  g_present_parameters.MultiSampleType = D3DMULTISAMPLE_NONE;
  g_present_parameters.MultiSampleQuality = 0;
  g_present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
  g_present_parameters.hDeviceWindow = g_engine_core->GetWindow()->GetWindowHandle();
  g_present_parameters.Windowed = !start_fullscreen;
  g_present_parameters.EnableAutoDepthStencil = true;
  g_present_parameters.AutoDepthStencilFormat = D3DFMT_D24X8;
  g_present_parameters.Flags = 0;
  g_present_parameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
  g_present_parameters.PresentationInterval = (start_vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE);

  HandleErrorResult
  (
    g_direct_3d_interface->CreateDevice(D3DADAPTER_DEFAULT,
                                        D3DDEVTYPE_HAL,
                                        g_present_parameters.hDeviceWindow,
                                        device_behavior,
                                        &g_present_parameters,
                                        &g_device),
    "Could not create the device."
  );

  SetFullscreenState(start_fullscreen);
}

bool RequiredDeviceCapabilitiesPresent()
{
  D3DCAPS9 capabilities;
  HandleErrorResult
  (
    g_device->GetDeviceCaps(&capabilities),
    "Failed to get device capabilities"
  );

  if (capabilities.VertexShaderVersion < D3DVS_VERSION(3,0))
    return false;

  if (capabilities.PixelShaderVersion < D3DPS_VERSION(3,0))
    return false;

  return true;
}

void UninitializeInterface()
{
  ReleaseCOM(g_direct_3d_interface);
}

void UninitializeDevice()
{
  ReleaseCOM(g_device);
}

void SetFullscreenState(bool enable)
{
  if (g_fullscreen_enabled == enable)
    return;

  g_fullscreen_enabled = enable;

  if (g_fullscreen_enabled)
  {
    int width  = GetSystemMetrics(SM_CXSCREEN);
    int height  = GetSystemMetrics(SM_CYSCREEN);

    g_present_parameters.BackBufferFormat = k_back_buffer_format;
    g_present_parameters.BackBufferWidth = width;
    g_present_parameters.BackBufferHeight = height;
    g_present_parameters.Windowed = false;

    SetWindowLongPtr(g_present_parameters.hDeviceWindow, GWL_STYLE, WS_POPUP);

    SetWindowPos(g_present_parameters.hDeviceWindow, HWND_NOTOPMOST,
                 0, 0, width, height, SWP_SHOWWINDOW);
  }
  else
  {
    int width = g_engine_core->GetWindow()->GetWidth();
    int height = g_engine_core->GetWindow()->GetHeight();

    RECT r = {0, 0, width, height};

    AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);
    g_present_parameters.BackBufferFormat = D3DFMT_UNKNOWN;
    g_present_parameters.BackBufferWidth = width;
    g_present_parameters.BackBufferHeight = height;
    g_present_parameters.Windowed = true;

    SetWindowLongPtr(g_present_parameters.hDeviceWindow, GWL_STYLE, WS_OVERLAPPEDWINDOW);

    SetWindowPos(g_present_parameters.hDeviceWindow, HWND_NOTOPMOST,
                 100, 100, width, height, SWP_SHOWWINDOW);
  }

  OnLostDevice();
  HandleErrorResult
  (
    g_device->Reset(&g_present_parameters),
    "Failed to reset the device"
  );
  OnResetDevice();
}

bool GetFullscreenState()
{
  return g_fullscreen_enabled;
}

void ToggleFullscreenState()
{
  SetFullscreenState(!g_fullscreen_enabled);
}

std::vector<std::pair<unsigned, unsigned>> GetResolutions()
{
  std::vector<std::pair<unsigned, unsigned>> resolutions;

  D3DDISPLAYMODE mode;
  UINT modes = g_direct_3d_interface->GetAdapterModeCount(D3DADAPTER_DEFAULT, k_back_buffer_format);

  for (UINT i = 0; i < modes; ++i)
  {
    g_direct_3d_interface->EnumAdapterModes(D3DADAPTER_DEFAULT, k_back_buffer_format, i, &mode);

    resolutions.push_back(std::make_pair(mode.Width, mode.Height));
  }

  std::sort(resolutions.begin(), resolutions.end());

  return resolutions;
}

void ResizeDevice(int width, int height)
{
  g_engine_core->GetWindow()->SetWidth(width);
  g_engine_core->GetWindow()->SetHeight(height);

  g_present_parameters.BackBufferWidth = width;
  g_present_parameters.BackBufferHeight = height;

  SetWindowLongPtr(g_present_parameters.hDeviceWindow, GWL_STYLE, WS_OVERLAPPEDWINDOW);

  RECT rect = {0, 0, width, height};
  AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

  SetWindowPos(g_present_parameters.hDeviceWindow, HWND_NOTOPMOST,
               100, 100, width, height, SWP_SHOWWINDOW);

  OnLostDevice();
  HandleErrorResult
  (
    g_device->Reset(&g_present_parameters),
    "Failed to reset the device"
  );
  OnResetDevice();
}

int GetDeviceWidth()
{
  return g_present_parameters.BackBufferWidth;
}

int GetDeviceHeight()
{
  return g_present_parameters.BackBufferHeight;
}

GraphicsInfoTypes::ErrorCode IsDeviceLost()
{
  HRESULT result = g_device->TestCooperativeLevel();

  if (result == D3DERR_DEVICELOST)
  {
    Sleep(20);
    return GraphicsInfoTypes::EC_DeviceLost;
  }

  switch (result)
  {
  case D3DERR_DEVICENOTRESET:
    {
      OnLostDevice();
      HandleErrorResult
      (
        g_device->Reset(&g_present_parameters),
        "Failed to reset the device"
      );
      OnResetDevice();

      return GraphicsInfoTypes::EC_OK;
    }
  case D3DERR_DRIVERINTERNALERROR:
    ErrorIf(true, "There was an internal driver error!");
    break;
  }

  return GraphicsInfoTypes::EC_OK;
}

void OnLostDevice()
{
  // release all dynamic vertex buffers
  for (unsigned i = 0; i < g_vertex_buffers.size(); i++)
  {
  	if (g_vertex_buffers[i].m_is_dynamic && g_vertex_buffers[i].m_buffer != 0)
  	{
      g_vertex_buffers[i].m_buffer->Release();
      g_vertex_buffers[i].m_buffer = 0;

      g_vertex_buffers[i].m_was_lost = true;
  	}
  }

  if (g_active_render_target != 0)
  {
    UnsetRenderTarget();
  }

  for (unsigned i = 0; i < g_render_targets.size(); ++i)
  {
    if (g_textures[g_render_targets[i].m_texture_data].m_texture != 0)
    {
      g_render_targets[i].m_render_target->Release();
      g_render_targets[i].m_render_target = 0;
      g_textures[g_render_targets[i].m_texture_data].m_texture->Release();
      g_textures[g_render_targets[i].m_texture_data].m_texture = 0;

      g_textures[g_render_targets[i].m_texture_data].m_was_lost = true;
    }
  }

  // release all dynamic textures
  for (unsigned i = 0; i < g_textures.size(); ++i)
  {
    if (g_textures[i].m_is_dynamic && g_textures[i].m_texture != 0)
    {
      g_textures[i].m_texture->Release();
      g_textures[i].m_texture = 0;

      g_textures[i].m_was_lost = true;
    }
  }
}

// need to predeclare these here as they shouldn't be in the header and need to
// be used for OnResetDevice
// Creates the DirectX declaration interface based on the definition inside the
// VertexDeclaration structure
void CreateVertexDeclaration(VertexDeclaration *declaration);
// Creates the DirectX buffer interfaces based on the definition inside the
// VertexBuffer structure
void CreateVertexBuffer(VertexBuffer *buffer);
// Creates the Shader based on the source inside of the VertexShaderInfo structure
void CreateVertexShader(VertexShaderInfo *shader);
// Creates the Shader based on the source inside of the PixelShaderInfo structure
void CreatePixelShader(PixelShaderInfo *shader);
// Creates the DirectX texture interfaces based on the definition inside the
// TextureInfo stucture
void CreateTexture(TextureInfo *texture);
// Creates the DirectX render target based on the definition inside the RenderTargetInfo
// structure
void CreateRenderTarget(RenderTargetInfo *render_target);

void OnResetDevice()
{
  // recreate all lost and dynamic buffers
  for (unsigned i = 0; i < g_vertex_buffers.size(); i++)
  {
  	if (g_vertex_buffers[i].m_is_dynamic && g_vertex_buffers[i].m_was_lost)
  	{
      // this is so the index buffer isn't recreated
      unsigned index_size = g_vertex_buffers[i].m_index_buffer_size;
      IDirect3DIndexBuffer9* indexes = g_vertex_buffers[i].m_indexes;
      g_vertex_buffers[i].m_index_buffer_size = 0;
      // recreate the vertex buffer
      CreateVertexBuffer(&(g_vertex_buffers[i]));
      // set the index buffer size back
      g_vertex_buffers[i].m_index_buffer_size = index_size;
      g_vertex_buffers[i].m_indexes = indexes;
  	}
  }

  for (unsigned i = 0; i < g_render_targets.size(); ++i)
  {
    if (g_textures[g_render_targets[i].m_texture_data].m_texture == 0 && g_textures[g_render_targets[i].m_texture_data].m_was_lost)
    {
      CreateRenderTarget(&(g_render_targets[i]));
    }
  }

  // release all dynamic textures
  for (unsigned i = 0; i < g_textures.size(); ++i)
  {
    if (g_textures[i].m_is_dynamic && g_textures[i].m_was_lost)
    {
      CreateTexture(&(g_textures[i]));
    }
  }

  g_reset_device_count++;

  g_engine_core->DispatchEvent(MessageBank::OnDeviceReset);
}

void Clear(Color const &color, bool clear_z)
{
  A8R8G8B8 byte_color(color);
  HandleErrorResult
  (
    g_device->Clear(0, 0, (clear_z ? (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER) : D3DCLEAR_TARGET), byte_color.m_color, 1.0f, 0),
    "Failed to clear"
  );
}

void BeginDraw()
{
  HandleErrorResult
  (
    g_device->BeginScene(),
    "Failed to begin scene"
  );
}

void EndDraw()
{
  HandleErrorResult
  (
    g_device->EndScene(),
    "Failed to end scene"
  );
}

void Present()
{
  HRESULT present_result = g_device->Present(0, 0, 0, 0);

  // this is a hack to fix missed device lost checks...
  if (present_result == D3DERR_DEVICELOST)
  {
    IsDeviceLost();
  }
  else
  {
    HandleErrorResult
    (
      present_result,
      "Failed to present"
    );
  }
}

void CreateVertexDeclaration(VertexDeclaration *declaration)
{
  D3DVERTEXELEMENT9 *dx_elements = new D3DVERTEXELEMENT9[declaration->m_definition.size() + 1];
  unsigned offset = 0;
  unsigned useage_index[GraphicsVertexTypes::MU_Max] = {0};

  declaration->m_uses_instance_stream = false;
  declaration->m_was_lost = false;

  for (unsigned i = 0; i < declaration->m_definition.size(); ++i)
  {
    dx_elements[i].Stream = declaration->m_definition[i].m_stream;

    // all elements of the declaration should use the same stream, if any of them
    // use stream 1 then the whole declaration is assumed to be instance data
    if (declaration->m_definition[i].m_stream == 1)
      declaration->m_uses_instance_stream = true;

    dx_elements[i].Offset = offset;

    // translate the vertex member types to the DirectX member types and update
    // the offset
    switch(declaration->m_definition[i].m_type)
    {
    case GraphicsVertexTypes::VM_Float:
      dx_elements[i].Type = D3DDECLTYPE_FLOAT1;
      offset += 4;
      break;
    case GraphicsVertexTypes::VM_Vector2:
      dx_elements[i].Type = D3DDECLTYPE_FLOAT2;
      offset += 4 * 2;
      break;
    case GraphicsVertexTypes::VM_Vector3:
      dx_elements[i].Type = D3DDECLTYPE_FLOAT3;
      offset += 4 * 3;
      break;
    case GraphicsVertexTypes::VM_Vector4:
      dx_elements[i].Type = D3DDECLTYPE_FLOAT4;
      offset += 4 * 4;
      break;
    case GraphicsVertexTypes::VM_Color:
      dx_elements[i].Type = D3DDECLTYPE_D3DCOLOR;
      offset += 4 * 1;
      break;
    default:
      ErrorIf(true, "Not a valid vertex member type");
    }
    // translate the member usage types to the DirectX member usage
    switch(declaration->m_definition[i].m_usage)
    {
    case GraphicsVertexTypes::MU_Position:
      dx_elements[i].Usage = D3DDECLUSAGE_POSITION;
      break;
    case GraphicsVertexTypes::MU_Normal:
      dx_elements[i].Usage = D3DDECLUSAGE_NORMAL;
      break;
    case GraphicsVertexTypes::MU_TexCoord:
      dx_elements[i].Usage = D3DDECLUSAGE_TEXCOORD;
      break;
    case GraphicsVertexTypes::MU_Color:
      dx_elements[i].Usage= D3DDECLUSAGE_COLOR;
      break;
    case GraphicsVertexTypes::MU_Tangent:
      dx_elements[i].Usage = D3DDECLUSAGE_TANGENT;
      break;
    case GraphicsVertexTypes::MU_Bitangent:
      dx_elements[i].Usage = D3DDECLUSAGE_BINORMAL;
      break;
    case GraphicsVertexTypes::MU_BlendIndex:
      dx_elements[i].Usage = D3DDECLUSAGE_BLENDINDICES;
      break;
    default:
      ErrorIf(true, "Not a valid vertex member usage");
    }

    // translate the usage index and update it
    dx_elements[i].UsageIndex = useage_index[declaration->m_definition[i].m_usage];
    ++useage_index[declaration->m_definition[i].m_usage];

    // everything should have the default method
    dx_elements[i].Method = D3DDECLMETHOD_DEFAULT;
  }

  // the end of the element declaration
  D3DVERTEXELEMENT9 end_element = D3DDECL_END();

  dx_elements[declaration->m_definition.size()] = end_element;

  declaration->m_vertex_size = offset;

  HandleErrorResult
  (
    g_device->CreateVertexDeclaration(dx_elements, &(declaration->m_declaration)),
    "Could not create vertex declaration"
  );

  delete [] dx_elements;
}

DeclarationHandle CreateVertexDeclaration(GraphicsVertexTypes::VertexElement const *elements, unsigned num_elements)
{
  VertexDeclaration new_declaration;

  for (unsigned i = 0; i < num_elements; ++i)
  {
    new_declaration.m_definition.push_back(elements[i]);
  }

  // this block of code keeps matching declarations from being made
  bool found_match = true;
  // look through all the declarations
  for (unsigned i = 0; i < g_vertex_declarations.size(); ++i)
  {
    // look through the elements of each declaration
    if (g_vertex_declarations[i].m_declaration == 0)
    {
      found_match = false;
    }
    if (new_declaration.m_definition.size() != g_vertex_declarations[i].m_definition.size())
    {
      found_match = false;
    }
    else
    {
      for (unsigned j = 0; j < g_vertex_declarations[i].m_definition.size() &&
        j < new_declaration.m_definition.size(); ++j)
      {
        // if the elements don't match, break and look at the next element
        if (new_declaration.m_definition[j] != g_vertex_declarations[i].m_definition[j])
        {
          found_match = false;
          break;
        }
      }
    }
    // if all of the elements matched increase the ref count, and return the handle
    if (found_match)
    {
      ++g_vertex_declarations[i].m_instances;
      return DeclarationHandle(i);
    }
    found_match = true;
  }

  new_declaration.m_instances = 1;
  CreateVertexDeclaration(&new_declaration);

  unsigned index = g_vertex_declarations.size();

  g_vertex_declarations.push_back(new_declaration);
  return index;
}

void DeleteVertexDeclaration(DeclarationHandle declaration_handle)
{
  ErrorIf
  (
    !declaration_handle.IsValid(),
    "Attempted to use an invalid declaration handle"
  );

  --g_vertex_declarations[declaration_handle].m_instances;

  if (g_vertex_declarations[declaration_handle].m_instances == 0)
  {
    ReleaseCOM(g_vertex_declarations[declaration_handle].m_declaration);
  }
}

std::vector<GraphicsVertexTypes::VertexElement> GetVertexDeclaration(DeclarationHandle declaration_handle)
{
  ErrorIf
  (
    !declaration_handle.IsValid(),
    "Attempted to use an invalid declaration handle"
  );

  return g_vertex_declarations[declaration_handle].m_definition;
}

void CreateVertexBuffer(VertexBuffer *buffer)
{
  HandleErrorResult
  (
    g_device->CreateVertexBuffer(buffer->m_vertex_buffer_size,
                               ((buffer->m_is_dynamic) ? D3DUSAGE_DYNAMIC : 0),
                                 0,
                               ((buffer->m_is_dynamic) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED),
                                 &(buffer->m_buffer),
                                 0),
    "Failed to create vertex buffer"
  );

  if (buffer->m_index_buffer_size > 0)
  {
    HandleErrorResult
    (
    g_device->CreateIndexBuffer(buffer->m_index_buffer_size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
                                D3DPOOL_MANAGED, &(buffer->m_indexes), 0),
    "Failed to create index buffer"
    );
  }
  else
  {
    buffer->m_indexes = 0;
  }
}

BufferHandle CreateVertexBuffer(unsigned length, unsigned num_indeces, bool is_dynamic, DeclarationHandle declaration_handle)
{
  ErrorIf
  (
    declaration_handle >= g_vertex_declarations.size() ||
    g_vertex_declarations[declaration_handle].m_declaration == 0,
    "Attempted to use an invalid declaration handle"
  );

  unsigned index = g_vertex_buffers.size();

  VertexBuffer new_buffer;
  new_buffer.m_declaration_index = declaration_handle;
  new_buffer.m_is_dynamic = is_dynamic;
  new_buffer.m_is_instance_data = g_vertex_declarations[declaration_handle].m_uses_instance_stream;
  new_buffer.m_vertex_buffer_size = length * g_vertex_declarations[declaration_handle].m_vertex_size;
  new_buffer.m_index_buffer_size = num_indeces * sizeof(int16_t);
  new_buffer.m_was_lost = false;

  CreateVertexBuffer(&new_buffer);

  g_vertex_buffers.push_back(new_buffer);

  return index;
}

unsigned GetVertexBufferSize(BufferHandle buffer_handle)
{
  ErrorIf
  (
    !buffer_handle.IsValid(),
    "Attempted to use an invalid vertex buffer handle"
  );

  return g_vertex_buffers[buffer_handle].m_vertex_buffer_size;
}

void* LockVertexBuffer(BufferHandle buffer_handle, unsigned offset, unsigned size)
{
  char* data;

  ErrorIf
  (
    !buffer_handle.IsValid(),
    "Attempted to use an invalid vertex buffer handle"
  );

  HandleErrorResult
  (
    g_vertex_buffers[buffer_handle].m_buffer->Lock(offset, size, reinterpret_cast<void**>(&data), 0),
    "Failed to lock vertex buffer"
  );

  return static_cast<void*>(data);
}

void UnlockVertexBuffer(BufferHandle buffer_handle)
{
  ErrorIf
  (
    !buffer_handle.IsValid(),
    "Attempted to use an invalid vertex buffer handle"
  );

  HandleErrorResult
  (
    g_vertex_buffers[buffer_handle].m_buffer->Unlock(),
    "Failed to unlock vertex buffer"
  );
}

unsigned GetIndexBufferSize(BufferHandle buffer_handle)
{
  ErrorIf
  (
    !buffer_handle.IsValid(),
    "Attempted to use an invalid vertex buffer handle"
  );

  return g_vertex_buffers[buffer_handle].m_index_buffer_size;
}

void* LockIndexBuffer(BufferHandle buffer_handle, unsigned offset, unsigned size)
{
  char* data;

  ErrorIf
  (
    !buffer_handle.IsValid(),
    "Attempted to use an invalid vertex buffer handle"
  );

  HandleErrorResult
  (
    g_vertex_buffers[buffer_handle].m_indexes->Lock(offset, size, reinterpret_cast<void**>(&data), 0),
    "Failed to lock index buffer"
  );

  return static_cast<void*>(data);
}

void UnlockIndexBuffer(BufferHandle buffer_handle)
{
  ErrorIf
  (
    !buffer_handle.IsValid(),
    "Attempted to use an invalid vertex buffer handle"
  );

  HandleErrorResult
  (
    g_vertex_buffers[buffer_handle].m_indexes->Unlock(),
    "Failed to unlock vertex buffer"
  );
}

void SetVertexBuffer(BufferHandle buffer_handle)
{
  ErrorIf
  (
    !buffer_handle.IsValid(),
    "Attempted to use an invalid vertex buffer handle"
  );

  ErrorIf
  (
    g_vertex_buffers[buffer_handle].m_declaration_index >= g_vertex_declarations.size() ||
    g_vertex_declarations[g_vertex_buffers[buffer_handle].m_declaration_index].m_declaration == 0,
    "Vertex buffer holds an invalid declaration handle"
  );

  if (g_vertex_buffers[buffer_handle].m_is_instance_data)
  {
    HandleErrorResult
    (
      g_device->SetStreamSource(1, g_vertex_buffers[buffer_handle].m_buffer, 0,
                                g_vertex_declarations[g_vertex_buffers[buffer_handle].m_declaration_index].m_vertex_size),
      "Failed to set vertex buffer as stream source"
    );
  }
  else
  {
    HandleErrorResult
    (
      g_device->SetStreamSource(0, g_vertex_buffers[buffer_handle].m_buffer, 0,
                                g_vertex_declarations[g_vertex_buffers[buffer_handle].m_declaration_index].m_vertex_size),
      "Failed to set vertex buffer as stream source"
    );
  }

  if (g_vertex_buffers[buffer_handle].m_indexes)
  {
    HandleErrorResult
      (
      g_device->SetIndices(g_vertex_buffers[buffer_handle].m_indexes),
      "Failed to set index buffer"
      );
  }

  HandleErrorResult
  (
    g_device->SetVertexDeclaration(g_vertex_declarations[g_vertex_buffers[buffer_handle].m_declaration_index].m_declaration),
    "Failed to set vertex declaration"
  );
}

void DeleteVertexBuffer(BufferHandle buffer_handle)
{
  ErrorIf
   (
     !buffer_handle.IsValid(),
     "Attempted to use an invalid vertex buffer handle"
   );

  ReleaseCOM(g_vertex_buffers[buffer_handle].m_buffer);
  ReleaseCOM(g_vertex_buffers[buffer_handle].m_indexes);
}

void CreateTexture(TextureInfo *texture)
{
  D3DPOOL pool = D3DPOOL_MANAGED;

  if (texture->m_usage & D3DUSAGE_DYNAMIC || texture->m_usage & D3DUSAGE_RENDERTARGET)
    pool = D3DPOOL_DEFAULT;

  HandleErrorResult
  (
    D3DXCreateTexture(g_device, texture->m_width, texture->m_height, texture->m_mipmap_levels,
                      texture->m_usage, static_cast<D3DFORMAT>(texture->m_format),
                      pool,
                    &(texture->m_texture)),
    "Failed to create texture of size %d, %d", texture->m_width, texture->m_height
  );
}

TextureHandle CreateTexture(String const &filename)
{
  unsigned index = g_textures.size();

  TextureInfo texture;

  texture.m_was_lost = false;
  texture.m_is_dynamic = false;
  texture.m_is_render_target = false;
  texture.m_mipmap_levels = 0;
  texture.m_usage = static_cast<GraphicsOptionTypes::Usage>(0);
  texture.m_format = static_cast<GraphicsInfoTypes::Format>(0);

  D3DXIMAGE_INFO image_info;

  HandleErrorResult
  (
    D3DXCreateTextureFromFileEx(g_device, filename.GetCString(), 0, 0, 1, 0, D3DFMT_UNKNOWN,
                                D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0,
                                &(image_info), 0, &(texture.m_texture)),
    "Failed to create texture from file %s", filename.GetCString()
  );

  texture.m_width = image_info.Width;
  texture.m_height = image_info.Height;

  g_textures.push_back(texture);

  return index;
}

TextureHandle CreateTexture(unsigned width, unsigned height, unsigned mipmap_levels, GraphicsOptionTypes::Usage usage, GraphicsInfoTypes::Format format)
{
  unsigned index = g_textures.size();

  TextureInfo texture;

  texture.m_texture = 0;
  texture.m_was_lost = false;
  texture.m_is_dynamic = (usage & D3DUSAGE_DYNAMIC) ? true : false;
  texture.m_is_render_target = (usage & D3DUSAGE_RENDERTARGET) ? true : false;
  texture.m_width = width;
  texture.m_height = height;
  texture.m_mipmap_levels = mipmap_levels;
  texture.m_usage = usage;
  texture.m_format = format;

  CreateTexture(&texture);

  g_textures.push_back(texture);

  return index;
}

void DeleteTexture(TextureHandle texture_handle)
{
  ErrorIf
  (
    !texture_handle.IsValid(),
    "Attempted to use an invalid texture handle"
  );

  ReleaseCOM(g_textures[texture_handle].m_texture);
}

void *LockTexture(TextureHandle texture_handle, unsigned level, unsigned &pitch)
{
  ErrorIf
  (
    !texture_handle.IsValid(),
    "Attempted to use an invalid texture handle"
  );

  D3DLOCKED_RECT buffer;

  HandleErrorResult
  (
    g_textures[texture_handle].m_texture->LockRect(level, &buffer, 0, (g_textures[texture_handle].m_is_dynamic) ? D3DLOCK_DISCARD : 0),
    "Could not lock texture"
  );

  pitch = buffer.Pitch;

  return buffer.pBits;
}

void UnlockTexture(TextureHandle texture_handle, unsigned level)
{
  ErrorIf
  (
    !texture_handle.IsValid(),
    "Attempted to use an invalid texture handle"
  );

  HandleErrorResult
  (
    g_textures[texture_handle].m_texture->UnlockRect(level),
    "Could not unlock texture"
  );
}

unsigned GetTextureWidth(TextureHandle texture_handle)
{
  ErrorIf
  (
    !texture_handle.IsValid(),
    "Attempted to use an invalid texture handle"
  );

  return g_textures[texture_handle].m_width;
}

unsigned GetTextureHeight(TextureHandle texture_handle)
{
  ErrorIf
  (
    !texture_handle.IsValid(),
    "Attempted to use an invalid texture handle"
  );

  return g_textures[texture_handle].m_height;
}

void CreateRenderTarget(RenderTargetInfo *render_target)
{
  //fix the render target's aspect ratio
  g_textures[render_target->m_texture_data].m_width = unsigned(render_target->m_width * GetDeviceWidth());
  g_textures[render_target->m_texture_data].m_height = unsigned(render_target->m_height * GetDeviceHeight());

  CreateTexture(&(g_textures[render_target->m_texture_data]));

  HandleErrorResult
  (
    g_textures[render_target->m_texture_data].m_texture->GetSurfaceLevel(0, &(render_target->m_render_target)),
    "Failed to get surface from texture"
  );
}

RenderTargetHandle CreateRenderTarget(float width, float height, GraphicsOptionTypes::Usage usage)
{
  unsigned index = g_render_targets.size();

  RenderTargetInfo new_render_target;

  new_render_target.m_width = width;
  new_render_target.m_height = height;

  new_render_target.m_texture_data = CreateTexture(unsigned(width * GetDeviceWidth()), unsigned(height * GetDeviceHeight()), 1,
                                                   GraphicsOptionTypes::Usage(GraphicsOptionTypes::USG_RenderTarget | usage),
                                                   GraphicsInfoTypes::FMT_A8R8G8B8);

  HandleErrorResult
  (
    g_textures[new_render_target.m_texture_data].m_texture->GetSurfaceLevel(0, &(new_render_target.m_render_target)),
    "Failed to get surface level from texture"
  );

  g_render_targets.push_back(new_render_target);

  return index;
}

void DeleteRenderTarget(RenderTargetHandle render_target_handle)
{
  ErrorIf
  (
    !render_target_handle.IsValid(),
    "Attempted to use an invalid render target handle"
  );

  ReleaseCOM(g_render_targets[render_target_handle].m_render_target);

  DeleteTexture(g_render_targets[render_target_handle].m_texture_data);
}

void SetRenderTarget(RenderTargetHandle render_target_handle, unsigned index)
{
  ErrorIf
  (
    !render_target_handle.IsValid(),
    "Attempted to use an invalid render target handle"
  );

  HandleErrorResult
  (
    g_device->GetRenderTarget(0, &g_back_buffer),
    "Could not get back buffer"
  );

  HandleErrorResult
  (
    g_device->SetRenderTarget(index, g_render_targets[render_target_handle].m_render_target),
    "Could not set render target"
  );

  g_active_render_target = &(g_render_targets[render_target_handle]);
}

void UnsetRenderTarget()
{
  ErrorIf
  (
    g_back_buffer == 0,
    "Back buffer not present"
  );

  HandleErrorResult
  (
    g_device->SetRenderTarget(0, g_back_buffer),
    "Could not unset render target"
  );

  g_back_buffer->Release();
  g_back_buffer = 0;
  g_active_render_target = 0;
}

TextureHandle GetRenderTargetTexture(RenderTargetHandle render_target_handle)
{
  ErrorIf
  (
    !render_target_handle.IsValid(),
    "Attempted to use an invalid render target handle"
  );

  return g_render_targets[render_target_handle].m_texture_data;
}

VertexShaderHandle CreateVertexShader(String const &filename, GraphicsOptionTypes::ShaderCompileOptions options)
{
  VertexShaderInfo new_shader;
  ID3DXBuffer *compiler_errors = 0;
  ID3DXBuffer *source = 0;

  new_shader.m_constant_table = 0;
  new_shader.m_shader = 0;

  HandleErrorResult
  (
    D3DXCompileShaderFromFile(filename.GetCString(), 0, 0, "main",
                              D3DXGetVertexShaderProfile(g_device),
                              options,
                              &source,
                              &compiler_errors,
                              &(new_shader.m_constant_table)),
    "Failed to compile Vertex Shader from file %s", filename.GetCString()
  );

  ErrorIf(compiler_errors, static_cast<char*>(compiler_errors->GetBufferPointer()));

  ReleaseCOM(compiler_errors);

  unsigned index = g_vertex_shaders.size();

  HandleErrorResult
  (
    g_device->CreateVertexShader(static_cast<DWORD*>(source->GetBufferPointer()), &(new_shader.m_shader)),
    "Failed to create Vertex Shader"
  );

  ReleaseCOM(source);

  g_vertex_shaders.push_back(new_shader);

  InitializeVertexConstant(index, "u_world_matrix");
  InitializeVertexConstant(index, "u_model_view_matrix");
  InitializeVertexConstant(index, "u_projection_matrix");
  InitializeVertexConstant(index, "u_vertex_constants");

  return index;
}

PixelShaderHandle CreatePixelShader(String const &filename, GraphicsOptionTypes::ShaderCompileOptions options)
{
  PixelShaderInfo new_shader;
  ID3DXBuffer *compiler_errors = 0;
  ID3DXBuffer *source = 0;

  new_shader.m_constant_table = 0;
  new_shader.m_shader = 0;

  HandleErrorResult
  (
    D3DXCompileShaderFromFile(filename.GetCString(), 0, 0, "main",
                              D3DXGetPixelShaderProfile(g_device),
                              options,
                              &source,
                              &compiler_errors,
                              &(new_shader.m_constant_table)),
    "Failed to compile Pixel Shader from file %s", filename.GetCString()
  );

  ErrorIf(compiler_errors, static_cast<char*>(compiler_errors->GetBufferPointer()));

  ReleaseCOM(compiler_errors);

  unsigned index = g_pixel_shaders.size();

  HandleErrorResult
  (
    g_device->CreatePixelShader(static_cast<DWORD*>(source->GetBufferPointer()), &(new_shader.m_shader)),
    "Failed to create Pixel Shader"
  );

  ReleaseCOM(source);

  g_pixel_shaders.push_back(new_shader);

  InitializePixelConstant(index, "u_pixel_constants");

  return index;
}

void DeleteVertexShader(VertexShaderHandle shader_handle)
{
  ErrorIf
  (
    !shader_handle.IsValid(),
    "Attempted to use an invalid shader handle"
  );

  ReleaseCOM(g_vertex_shaders[shader_handle].m_shader);

  ReleaseCOM(g_vertex_shaders[shader_handle].m_constant_table);

  g_vertex_shaders[shader_handle].m_constants.clear();
}

void DeletePixelShader(PixelShaderHandle shader_handle)
{
  ErrorIf
  (
    !shader_handle.IsValid(),
    "Attempted to use an invalid shader handle"
  );

  ReleaseCOM(g_pixel_shaders[shader_handle].m_shader);

  ReleaseCOM(g_pixel_shaders[shader_handle].m_constant_table);

  g_pixel_shaders[shader_handle].m_constants.clear();
}

VertexShaderConstantHandle InitializeVertexConstant(VertexShaderHandle shader_handle,
                                                    String const &constant_name,
                                                    VertexShaderConstantHandle parent_handle)
{
  ErrorIf
  (
    !shader_handle.IsValid(),
    "Attempted to use an invalid vertex shader handle"
  );

  D3DXHANDLE parent_parameter = 0;

  // in the case there is a parent handle check if it exists too
  if (parent_handle.IsValid())
  {
    parent_parameter = g_vertex_shaders[shader_handle].m_constants[parent_handle];
  }

  unsigned index = g_vertex_shaders[shader_handle].m_constants.size();
  g_vertex_shaders[shader_handle].m_constants.push_back(g_vertex_shaders[shader_handle].m_constant_table->GetConstantByName(parent_parameter, constant_name.GetCString()));

  return index;
}

PixelShaderConstantHandle InitializePixelConstant(PixelShaderHandle shader_handle,
                                                  String const &constant_name,
                                                  PixelShaderConstantHandle parent_handle)
{
  ErrorIf
  (
    !shader_handle.IsValid(),
    "Attempted to use an invalid pixel shader handle"
  );

  D3DXHANDLE parent_parameter = 0;

  // in the case there is a parent handle check if it exists too
  if (parent_handle.IsValid())
  {
    parent_parameter = g_pixel_shaders[shader_handle].m_constants[parent_handle];
  }

  unsigned index = g_pixel_shaders[shader_handle].m_constants.size();
  g_pixel_shaders[shader_handle].m_constants.push_back(g_pixel_shaders[shader_handle].m_constant_table->GetConstantByName(parent_parameter, constant_name.GetCString()));

  return index;
}

void SetVertexShader(VertexShaderHandle shader_handle)
{
  ErrorIf
  (
    g_active_vertex_shader != 0,
    "There is already an active vertex shader"
  );

  ErrorIf
  (
    !shader_handle.IsValid(),
    "Attempted to use an invalid vertex shader handle"
  );

  g_active_vertex_shader = &(g_vertex_shaders[shader_handle]);

  g_device->SetVertexShader(g_active_vertex_shader->m_shader);

  SetVertexShaderConstant(0, g_world_matrix.Top());
  SetVertexShaderConstant(1, g_view_matrix.Top() * g_world_matrix.Top());
  SetVertexShaderConstant(2, g_projection_matrix.Top());

  if (g_active_render_target == 0)
  {
    SetVertexShaderConstant(3, Vector4(1.0f / (float)g_present_parameters.BackBufferWidth,
                                      1.0f / (float)g_present_parameters.BackBufferHeight,
                                      0.0f, 0.0f));
  }
  else
  {
    SetVertexShaderConstant(3, Vector4(1.0f / (float)g_textures[g_active_render_target->m_texture_data].m_width,
                                      1.0f / (float)g_textures[g_active_render_target->m_texture_data].m_height,
                                      0.0f, 0.0f));
  }
}

void SetPixelShader(PixelShaderHandle shader_handle)
{
  ErrorIf
  (
    g_active_pixel_shader != 0,
    "There is already an active pixel shader"
  );

  ErrorIf
  (
    !shader_handle.IsValid(),
    "Attempted to use an invalid vertex shader handle"
  );

  g_active_pixel_shader = &(g_pixel_shaders[shader_handle]);

  g_device->SetPixelShader(g_active_pixel_shader->m_shader);

  if (g_active_render_target == 0)
  {
    SetPixelShaderConstant(0, Vector4(1.0f / (float)g_present_parameters.BackBufferWidth,
                                      1.0f / (float)g_present_parameters.BackBufferHeight,
                                      0.0f, 0.0f));
  }
  else
  {
    SetPixelShaderConstant(0, Vector4(1.0f / (float)g_textures[g_active_render_target->m_texture_data].m_width,
                                      1.0f / (float)g_textures[g_active_render_target->m_texture_data].m_height,
                                      0.0f, 0.0f));
  }
}

void UnsetVertexShader()
{
  g_active_vertex_shader = 0;
}

void UnsetPixelShader()
{
  g_active_pixel_shader = 0;
}

void SetVertexShaderConstant(VertexShaderConstantHandle uniform_handle, bool data)
{
  ErrorIf
  (
    g_active_vertex_shader == 0,
    "There is no active vertex shader to set the uniform on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_vertex_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_vertex_shader->m_constant_table->SetValue(g_device,
                                                       g_active_vertex_shader->m_constants[uniform_handle],
                                                       static_cast<void*>(&data), sizeof(bool)),
    "Failed to set bool constant"
  );
}

void SetVertexShaderConstant(VertexShaderConstantHandle uniform_handle, int data)
{
  ErrorIf
  (
    g_active_vertex_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_vertex_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_vertex_shader->m_constant_table->SetValue(g_device,
                                                       g_active_vertex_shader->m_constants[uniform_handle],
                                                       static_cast<void*>(&data), sizeof(int)),
    "Failed to set int constant"
  );
}

void SetVertexShaderConstant(VertexShaderConstantHandle uniform_handle, float data)
{
  ErrorIf
  (
    g_active_vertex_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_vertex_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_vertex_shader->m_constant_table->SetValue(g_device,
                                                       g_active_vertex_shader->m_constants[uniform_handle],
                                                       static_cast<void*>(&data), sizeof(float)),
    "Failed to set float constant"
  );
}

void SetVertexShaderConstant(VertexShaderConstantHandle uniform_handle, Vector2 data)
{
  ErrorIf
  (
    g_active_vertex_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_vertex_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_vertex_shader->m_constant_table->SetValue(g_device,
                                                       g_active_vertex_shader->m_constants[uniform_handle],
                                                       static_cast<void*>(&data), sizeof(Vector2)),
    "Failed to set Vector2 constant"
  );
}

void SetVertexShaderConstant(VertexShaderConstantHandle uniform_handle, Vector3 data)
{
  ErrorIf
  (
    g_active_vertex_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_vertex_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_vertex_shader->m_constant_table->SetValue(g_device,
                                                       g_active_vertex_shader->m_constants[uniform_handle],
                                                       static_cast<void*>(&data), sizeof(Vector3)),
    "Failed to set Vector3 constant"
  );
}

void SetVertexShaderConstant(VertexShaderConstantHandle uniform_handle, Vector4 data)
{
  ErrorIf
  (
    g_active_vertex_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_vertex_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_vertex_shader->m_constant_table->SetValue(g_device,
                                                       g_active_vertex_shader->m_constants[uniform_handle],
                                                       static_cast<void*>(&data), sizeof(Vector4)),
    "Failed to set Vector4 constant"
  );
}

void SetVertexShaderConstant(VertexShaderConstantHandle uniform_handle, Color data)
{
  ErrorIf
  (
    g_active_vertex_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_vertex_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_vertex_shader->m_constant_table->SetValue(g_device,
                                                       g_active_vertex_shader->m_constants[uniform_handle],
                                                       static_cast<void*>(&data), sizeof(Color)),
    "Failed to set Color constant"
  );
}

void SetVertexShaderConstant(VertexShaderConstantHandle uniform_handle, Matrix2 const &data)
{
  ErrorIf
  (
    g_active_vertex_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

 ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_vertex_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_vertex_shader->m_constant_table->SetValue(g_device,
                                                       g_active_vertex_shader->m_constants[uniform_handle],
                                                       static_cast<void const*>(&data), sizeof(Matrix2)),
    "Failed to set Matrix2 constant"
  );
}

void SetVertexShaderConstant(VertexShaderConstantHandle uniform_handle, Matrix3 const &data)
{
  ErrorIf
  (
    g_active_vertex_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_vertex_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_vertex_shader->m_constant_table->SetValue(g_device,
                                                       g_active_vertex_shader->m_constants[uniform_handle],
                                                       static_cast<void const*>(&data), sizeof(Matrix3)),
    "Failed to set Matrix3 constant"
  );
}

void SetVertexShaderConstant(VertexShaderConstantHandle uniform_handle, Matrix4 const &data)
{
  ErrorIf
  (
    g_active_vertex_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_vertex_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_vertex_shader->m_constant_table->SetValue(g_device,
                                                       g_active_vertex_shader->m_constants[uniform_handle],
                                                       static_cast<void const*>(&data), sizeof(Matrix4)),
    "Failed to set Matrix4 constant"
  );
}

void SetVertexShaderConstant(VertexShaderConstantHandle uniform_handle, Matrix3 const *data, unsigned count)
{
  ErrorIf
  (
    g_active_vertex_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_vertex_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_vertex_shader->m_constant_table->SetValue(g_device,
                                                       g_active_vertex_shader->m_constants[uniform_handle],
                                                       static_cast<void const*>(&data), sizeof(Matrix3) * count),
    "Failed to set Matrix3 array constant"
  );
}

void SetVertexShaderConstant(VertexShaderConstantHandle uniform_handle, Matrix4 const *data, unsigned count)
{
  ErrorIf
  (
    g_active_vertex_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_vertex_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_vertex_shader->m_constant_table->SetValue(g_device,
                                                       g_active_vertex_shader->m_constants[uniform_handle],
                                                       static_cast<void const*>(&data), sizeof(Matrix4) * count),
    "Failed to set Matrix4 array constant"
  );
}

void SetPixelShaderConstant(PixelShaderConstantHandle uniform_handle, bool data)
{
  ErrorIf
  (
    g_active_pixel_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_pixel_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_pixel_shader->m_constant_table->SetValue(g_device,
                                                      g_active_pixel_shader->m_constants[uniform_handle],
                                                      static_cast<void*>(&data), sizeof(bool)),
    "Failed to set bool constant"
  );
}

void SetPixelShaderConstant(PixelShaderConstantHandle uniform_handle, int data)
{
  ErrorIf
  (
    g_active_pixel_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_pixel_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_pixel_shader->m_constant_table->SetValue(g_device,
                                                      g_active_pixel_shader->m_constants[uniform_handle],
                                                      static_cast<void*>(&data), sizeof(int)),
    "Failed to set int constant"
  );
}

void SetPixelShaderConstant(PixelShaderConstantHandle uniform_handle, float data)
{
  ErrorIf
  (
    g_active_pixel_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_pixel_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_pixel_shader->m_constant_table->SetValue(g_device,
                                                      g_active_pixel_shader->m_constants[uniform_handle],
                                                      static_cast<void*>(&data), sizeof(float)),
    "Failed to set float constant"
  );
}

void SetPixelShaderConstant(PixelShaderConstantHandle uniform_handle, Vector2 data)
{
  ErrorIf
  (
    g_active_pixel_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_pixel_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_pixel_shader->m_constant_table->SetValue(g_device,
                                                      g_active_pixel_shader->m_constants[uniform_handle],
                                                      static_cast<void*>(&data), sizeof(Vector2)),
    "Failed to set Vector2 constant"
  );
}

void SetPixelShaderConstant(PixelShaderConstantHandle uniform_handle, Vector3 data)
{
  ErrorIf
  (
    g_active_pixel_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_pixel_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_pixel_shader->m_constant_table->SetValue(g_device,
                                                      g_active_pixel_shader->m_constants[uniform_handle],
                                                      static_cast<void*>(&data), sizeof(Vector3)),
    "Failed to set Vector3 constant"
  );
}

void SetPixelShaderConstant(PixelShaderConstantHandle uniform_handle, Vector4 data)
{
  ErrorIf
  (
    g_active_pixel_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_pixel_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_pixel_shader->m_constant_table->SetValue(g_device,
                                                      g_active_pixel_shader->m_constants[uniform_handle],
                                                      static_cast<void*>(&data), sizeof(Vector4)),
    "Failed to set Vector4 constant"
  );
}

void SetPixelShaderConstant(PixelShaderConstantHandle uniform_handle, Color data)
{
  ErrorIf
  (
    g_active_pixel_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_pixel_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_pixel_shader->m_constant_table->SetValue(g_device,
                                                      g_active_pixel_shader->m_constants[uniform_handle],
                                                      static_cast<void*>(&data), sizeof(Color)),
    "Failed to set Color constant"
  );
}

void SetPixelShaderConstant(PixelShaderConstantHandle uniform_handle, Matrix2 const &data)
{
  ErrorIf
  (
    g_active_pixel_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_pixel_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_pixel_shader->m_constant_table->SetValue(g_device,
                                                      g_active_pixel_shader->m_constants[uniform_handle],
                                                      static_cast<void const*>(&data), sizeof(Matrix2)),
    "Failed to set Matrix2 constant"
  );
}

void SetPixelShaderConstant(PixelShaderConstantHandle uniform_handle, Matrix3 const &data)
{
  ErrorIf
  (
    g_active_pixel_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_pixel_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_pixel_shader->m_constant_table->SetValue(g_device,
                                                      g_active_pixel_shader->m_constants[uniform_handle],
                                                      static_cast<void const*>(&data), sizeof(Matrix3)),
    "Failed to set Matrix3 constant"
  );
}

void SetPixelShaderConstant(PixelShaderConstantHandle uniform_handle, Matrix4 const &data)
{
  ErrorIf
  (
    g_active_pixel_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_pixel_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_pixel_shader->m_constant_table->SetValue(g_device,
                                                      g_active_pixel_shader->m_constants[uniform_handle],
                                                      static_cast<void const*>(&data), sizeof(Matrix4)),
    "Failed to set Matrix4 constant"
  );
}

void SetPixelShaderConstant(PixelShaderConstantHandle uniform_handle, Matrix3 const *data, unsigned count)
{
  ErrorIf
  (
    g_active_pixel_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_pixel_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_pixel_shader->m_constant_table->SetValue(g_device,
                                                      g_active_pixel_shader->m_constants[uniform_handle],
                                                      static_cast<void const*>(&data), sizeof(Matrix3)),
    "Failed to set Matrix3 constant"
  );
}

void SetPixelShaderConstant(PixelShaderConstantHandle uniform_handle, Matrix4 const *data, unsigned count)
{
  ErrorIf
  (
    g_active_pixel_shader == 0,
    "There is no active vertex shader to end a pass on"
  );

  ErrorIf
  (
    !uniform_handle.IsValid(),
    "Attempted to use an invalid uniform handle"
  );

  if (g_active_pixel_shader->m_constants[uniform_handle] == 0)
    return;

  HandleErrorResult
  (
    g_active_pixel_shader->m_constant_table->SetValue(g_device,
                                                      g_active_pixel_shader->m_constants[uniform_handle],
                                                      static_cast<void const*>(&data), sizeof(Matrix4)),
    "Failed to set Matrix4 constant"
  );
}

void SetPixelShaderSampler(unsigned sampler_handle, TextureHandle texture_handle)
{
  ErrorIf
  (
    g_active_pixel_shader == 0,
    "There is no active pixel shader to set the sampler to"
  );

  ErrorIf
  (
    !texture_handle.IsValid(),
    "Attempted to use an invalid texture handle"
  );

  if (g_active_pixel_shader->m_constants[sampler_handle] == 0)
    return;

  DWORD sampler_index = g_active_pixel_shader->m_constant_table->GetSamplerIndex(g_active_pixel_shader->m_constants[sampler_handle]);

  HandleErrorResult
  (
    g_device->SetTexture(sampler_index,
                         g_textures[texture_handle].m_texture),
    "Unable to set texture"
  );

  HandleErrorResult
    (
    g_device->SetSamplerState(sampler_index, D3DSAMP_MAGFILTER, D3DTEXF_POINT),
    "Unable to set proper sampler state"
    );
  HandleErrorResult
    (
    g_device->SetSamplerState(sampler_index, D3DSAMP_MINFILTER, D3DTEXF_POINT),
    "Unable to set proper sampler state"
    );
  HandleErrorResult
  (
    g_device->SetSamplerState(sampler_index, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP),
    "Unable to set proper sampler state"
  );
  HandleErrorResult
  (
    g_device->SetSamplerState(sampler_index, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP),
    "Unable to set proper sampler state"
  );
  HandleErrorResult
  (
    g_device->SetSamplerState(sampler_index, D3DSAMP_MIPFILTER, D3DTEXF_NONE),
    "Unable to set proper sampler state"
  );
}

void UnsetPixelShaderSampler(unsigned sampler_handle)
{
  ErrorIf
  (
    g_active_pixel_shader == 0,
    "There is no active pixel shader to set the sampler to"
  );

  if (g_active_pixel_shader->m_constants[sampler_handle] == 0)
    return;

  DWORD sampler_index = g_active_pixel_shader->m_constant_table->GetSamplerIndex(g_active_pixel_shader->m_constants[sampler_handle]);

  HandleErrorResult
  (
    g_device->SetTexture(sampler_index, 0),
    "Unable to set texture"
  );
}

void CommitModelViewMatrix()
{
  ErrorIf
  (
    g_active_vertex_shader == 0,
    "There is no active vertex shader to commit changes to"
  );

  SetVertexShaderConstant(0, g_world_matrix.Top());
  SetVertexShaderConstant(1, g_view_matrix.Top() * g_world_matrix.Top());
}

void CommitProjectionMatrix()
{
  ErrorIf
  (
    g_active_vertex_shader == 0,
    "There is no active vertex shader to commit changes to"
  );

  SetVertexShaderConstant(2, g_projection_matrix.Top());
}

void DrawIndexedTriList(unsigned num_vertices, unsigned num_triangles, unsigned start_index, unsigned start_vertex)
{
  HandleErrorResult
  (
    g_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
                                   0,
                                   start_vertex,
                                   num_vertices,
                                   start_index,
                                   num_triangles),
    "Failed to draw indexed triangle list"
  );
}

void DrawTriListBuffer(unsigned num_triangles, unsigned start_index)
{
  HandleErrorResult
  (
    g_device->DrawPrimitive(D3DPT_TRIANGLELIST, start_index, num_triangles),
    "Failed to draw triangle list"
  );
}

void DrawIndexedTriFan(unsigned num_vertices, unsigned num_triangles, unsigned start_index, unsigned start_vertex)
{
  HandleErrorResult
  (
    g_device->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN,
                                   0,
                                   start_vertex,
                                   num_vertices,
                                   start_index,
                                   num_triangles),
    "Failed to draw indexed triangle fan"
  );
}

void DrawTriFanBuffer(unsigned num_triangles, unsigned start_index)
{
  HandleErrorResult
  (
    g_device->DrawPrimitive(D3DPT_TRIANGLEFAN, start_index, num_triangles),
    "Failed to draw triangle fan"
  );
}

void DrawIndexedTriStrip(unsigned num_vertices, unsigned num_triangles, unsigned start_index, unsigned start_vertex)
{
  HandleErrorResult
  (
    g_device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
                                   0,
                                   start_vertex,
                                   num_vertices,
                                   start_index,
                                   num_triangles),
    "Failed to draw indexed triangle strip"
  );
}

void DrawTriStripBuffer(unsigned num_triangles, unsigned start_index)
{
  HandleErrorResult
  (
    g_device->DrawPrimitive(D3DPT_TRIANGLESTRIP, start_index, num_triangles),
    "Failed to draw triangle strip"
  );
}

void DrawIndexedLineList(unsigned num_vertices, unsigned num_lines, unsigned start_index, unsigned start_vertex)
{
  HandleErrorResult
  (
    g_device->DrawIndexedPrimitive(D3DPT_LINELIST,
                                   0,
                                   start_vertex,
                                   num_vertices,
                                   start_index,
                                   num_lines),
    "Failed to draw indexed line list"
  );
}

void DrawLineListBuffer(unsigned num_lines, unsigned start_index)
{
  HandleErrorResult
  (
    g_device->DrawPrimitive(D3DPT_LINELIST, start_index, num_lines),
    "Failed to draw line list"
  );
}

void DrawIndexedLineStrip(unsigned num_vertices, unsigned num_segments, unsigned start_index, unsigned start_vertex)
{
  HandleErrorResult
  (
    g_device->DrawIndexedPrimitive(D3DPT_LINESTRIP,
                                   0,
                                   start_vertex,
                                   num_vertices,
                                   start_index,
                                   num_segments),
    "Failed to draw indexed line strip"
  );
}

void DrawLineStripBuffer(unsigned num_segments, unsigned start_index)
{
  HandleErrorResult
  (
    g_device->DrawPrimitive(D3DPT_LINESTRIP, start_index, num_segments),
    "Failed to draw triangle strip"
  );
}

void SetRenderState(GraphicsOptionTypes::RenderState render_state, GraphicsOptionTypes::RenderStateOptions option)
{
  HandleErrorResult
  (
    g_device->SetRenderState(static_cast<D3DRENDERSTATETYPE>(render_state), option),
    "Failed to set render state"
  );
}

void SetAlphaEnable(bool enable_alpha)
{
  HandleErrorResult
  (
    g_device->SetRenderState(D3DRS_ALPHABLENDENABLE, enable_alpha ? TRUE : FALSE),
    "Failed to set alpha blend state"
  );

  if (enable_alpha)
  {
    HandleErrorResult
    (
      g_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA),
        "Failed to set alpha source"
    );

    HandleErrorResult
    (
      g_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA),
      "Failed to set alpha destination"
    );
  }
}

void SetDepthEnable(bool enable_depth)
{
  HandleErrorResult
  (
    g_device->SetRenderState(D3DRS_ZENABLE, enable_depth ? D3DZB_TRUE : D3DZB_FALSE),
    "Failed to set depth buffer state"
  );
}

void SetViewport(unsigned start_x, unsigned start_y, unsigned width, unsigned height)
{
  D3DVIEWPORT9 viewport = {start_x, start_y, width, height, 0, 1};

  HandleErrorResult
  (
    g_device->SetViewport(&viewport),
    "Failed to set viewport"
  );
}

MatrixStack<Matrix4>& GetWorldStack()
{
  return g_world_matrix;
}

MatrixStack<Matrix4>& GetViewStack()
{
  return g_view_matrix;
}

MatrixStack<Matrix4>& GetProjectionStack()
{
  return g_projection_matrix;
}

} // namespace GraphicsDevice

bool DeclarationHandle::IsValid()
{
  return m_handle != unsigned(-1) &&
         m_handle < GraphicsDevice::g_vertex_declarations.size() &&
         GraphicsDevice::g_vertex_declarations[m_handle].m_declaration != 0;
}

bool BufferHandle::IsValid()
{
  return m_handle != unsigned(-1) &&
         m_handle < GraphicsDevice::g_vertex_buffers.size() &&
         GraphicsDevice::g_vertex_buffers[m_handle].m_buffer != 0 &&
         (GraphicsDevice::g_vertex_buffers[m_handle].m_index_buffer_size != 0) ?
         (GraphicsDevice::g_vertex_buffers[m_handle].m_indexes != 0) : true;
}

bool TextureHandle::IsValid()
{
  return m_handle != unsigned(-1) &&
         m_handle < GraphicsDevice::g_textures.size() &&
         GraphicsDevice::g_textures[m_handle].m_texture != 0;
}

bool RenderTargetHandle::IsValid()
{
  return m_handle != unsigned(-1) &&
         m_handle < GraphicsDevice::g_render_targets.size() &&
         GraphicsDevice::g_render_targets[m_handle].m_render_target != 0;
}

bool VertexShaderHandle::IsValid()
{
  return m_handle != unsigned(-1) &&
         m_handle < GraphicsDevice::g_vertex_shaders.size() &&
         GraphicsDevice::g_vertex_shaders[m_handle].m_shader != 0;
}

bool PixelShaderHandle::IsValid()
{
  return m_handle != unsigned(-1) &&
         m_handle < GraphicsDevice::g_pixel_shaders.size() &&
         GraphicsDevice::g_pixel_shaders[m_handle].m_shader != 0;
}

bool VertexShaderConstantHandle::IsValid()
{
  return m_handle != unsigned(-1) &&
         m_handle < GraphicsDevice::g_active_vertex_shader->m_constants.size();
}

bool PixelShaderConstantHandle::IsValid()
{
  return m_handle != unsigned(-1) &&
         m_handle < GraphicsDevice::g_active_pixel_shader->m_constants.size();
}

} // namespace Improbability