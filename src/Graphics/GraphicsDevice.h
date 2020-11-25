// Nicholas Baldwin
// 2011/06/29
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "GraphicsInfoTypes.h"
#include "GraphicsVertexTypes.h"
#include "GraphicsOptionTypes.h"
#include "GraphicsHandleTypes.h"

#include "../math/matrix4.h"
#include "../math/vector4.h"
#include "../math/MatrixStack.h"

namespace Improbability
{

namespace GraphicsDevice
{
////////////////////////////////////////////////////////////////////////////////
////////    initialization and uninitialization functions for directx///////////
////////////////////////////////////////////////////////////////////////////////
  
  // initializes the DirectX interface, can be called most anytime
  void InitializeInterface();

  // initializes the DirectX devide, must be called AFTER the interface is initialized
  // and the window has been created
  void InitializeDevice(bool start_fullscreen = false, bool start_vsync = true);

  // checks for various required device caps (right now its only for shader support
  bool RequiredDeviceCapabilitiesPresent();

  // uninitializes the interface, can be called anytime
  void UninitializeInterface();

  // uninitializes the device, can be called anytime
  void UninitializeDevice();

////////////////////////////////////////////////////////////////////////////////
////////////////////////Set Configuration functions/////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
  // deal with switching windowed and fullscreen mode
  void SetFullscreenState(bool enable);
  bool GetFullscreenState();
  void ToggleFullscreenState();

  // resize the device when the user maximizes or resizes the window
  std::vector<std::pair<unsigned, unsigned>> GetResolutions();
  void ResizeDevice(int width, int height);
  int GetDeviceWidth();
  int GetDeviceHeight();


////////////////////////////////////////////////////////////////////////////////
////////////////////////lost device functions///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
  // checks for lost device
  // returns EC_OK for not lost, EC_DeviceLost on lost device, and EC_NotReset 
  //   for regained device
  GraphicsInfoTypes::ErrorCode IsDeviceLost();

  // should be called when device is lost, releases all dynamic assets
  void OnLostDevice();

  // needs to be called when the device has been reset, reallocates all dynamic 
  //   assets
  void OnResetDevice();

////////////////////////////////////////////////////////////////////////////////
////////////////////////scene drawing functions/////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  

  // clears the back buffer to white and the z buffer to 1.0
  void Clear(Color const &color, bool clear_z = true);

  // starts the drawing state
  void BeginDraw();
  
  // ends the drawing state
  void EndDraw();
  
  // presents the back buffer to the screen
  void Present();

  // this goes without saying but all handles need to be valid when passed to 
  // all following functions

////////////////////////////////////////////////////////////////////////////////
////////////////////////vertex declaration functions////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
  // creates a vertex declaration
  // elements - an array of elements that define the vertex declaration
  // num_elements - the number of elements in the array
  // returns the handle to the new declaration
  DeclarationHandle CreateVertexDeclaration(GraphicsVertexTypes::VertexElement const *elements, unsigned num_elements);

  // deletes a vertex declaration
  // declaration_handle - the handle of the declaration to delete
  void DeleteVertexDeclaration(DeclarationHandle declaration_handle);

  std::vector<GraphicsVertexTypes::VertexElement> GetVertexDeclaration(DeclarationHandle declaration_handle);

////////////////////////////////////////////////////////////////////////////////
////////////////////////vertex buffer functions/////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
  // creates a vertex (and index) buffer
  // length - the number of vertices to store in the vertex buffer
  // num_indeces - the number of indexes to store in the index buffer, the 
  //               index buffer isn't created if this parameter is 0
  // is_dynamic - specifies if the vertex buffer should be created with dynamic
  //              attributes
  // declaration_handle - the handle to the declarations the vertex buffer will
  //                      use
  // returns the handle to the new buffer
  BufferHandle CreateVertexBuffer(unsigned length, unsigned num_indeces, bool is_dynamic, DeclarationHandle declaration_handle);

  // returns the size of the vertex buffer
  // buffer_handle - the handle to the buffer to get the size of the vertex 
  //                 buffer from
  unsigned GetVertexBufferSize(BufferHandle buffer_handle);

  // locks the vertex buffer for read and write
  // buffer_handle - the handle to the buffer to lock the vertex buffer of
  // offset - the offset (in bytes) from the beginning to lock
  // size - the number of bytes from the offset to lock
  // returns a pointer to the first byte of the vertex buffer
  void* LockVertexBuffer(BufferHandle buffer_handle, unsigned offset = 0, unsigned size = 0);

  // unlocks the vertex buffer, releases it to the graphics card after locking
  // buffer_handle - the handle to the buffer to unlock the vertex buffer of
  void UnlockVertexBuffer(BufferHandle buffer_handle);

  // returns the size of the index buffer
  // buffer_handle - the handle to the buffer to get the size of the index 
  //                 buffer from
  unsigned GetIndexBufferSize(BufferHandle buffer_handle);

  // locks the index buffer for write (NOTE: WRITE ONLY)
  // buffer_handle - the handle to the buffer to lock the index buffer of
  // offset - the offset (in bytes) from the beginning to lock
  // size - the number of bytes from the offset to lock
  // returns a pointer to the first byte of the index buffer
  void* LockIndexBuffer(BufferHandle buffer_handle, unsigned offset = 0, unsigned size = 0);

  // unlocks the index buffer, releases it to the graphics card after locking
  // buffer_handle - the handle to the buffer to unlock the index buffer of
  void UnlockIndexBuffer(BufferHandle buffer_handle);

  // sets the stream source (vertex declaration), vertex buffer source, and, if 
  //   it exists, the index buffer
  // buffer_handle - the handle to the buffer to set the sources from
  void SetVertexBuffer(BufferHandle buffer_handle);

  // deletes the vertex buffer and, if it exists, the index buffer
  // buffer_handle - the handle to the buffer to delete
  void DeleteVertexBuffer(BufferHandle buffer_handle);

////////////////////////////////////////////////////////////////////////////////
////////////////////////texture functions///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
  // creates a texture from a file
  // filename - the file to create the texture
  // returns the handle to the texture
  TextureHandle CreateTexture(String const &filename);

  // creates a dynamic texture of size
  // size - the size of the texture to create
  // returns the handle to the texture
  TextureHandle CreateTexture(unsigned width, unsigned height, unsigned mipmap_levels, 
                              GraphicsOptionTypes::Usage usage, 
                              GraphicsInfoTypes::Format format);

  // deletes a texture
  // texture_handle - the handle to the texture
  void DeleteTexture(TextureHandle texture_handle);

  // locks a texture at a given level
  // texture_handle - the handle to the texture
  // level - the mipmap level to lock, usually 0
  // pitch - an output parameter that returns the pitch of the texture
  // returns a pointer to the buffer of the texture
  void *LockTexture(TextureHandle texture_handle, unsigned level, unsigned &pitch);

  // unlocks a texture at a given level
  // texture_handle - the handle to the texture
  // level - the mipmap level to unlock
  void UnlockTexture(TextureHandle texture_handle, unsigned level);

  // returns a texture's width and height
  unsigned GetTextureWidth(TextureHandle texture_handle);
  unsigned GetTextureHeight(TextureHandle texture_handle);

////////////////////////////////////////////////////////////////////////////////
////////////////////////render target functions/////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
  // creates a render target
  // width - the width of the render target's underlying texture
  // height - the height of the render target's underlying texture
  // usage - the usage of the render target, USG_RenderTarget is automatically
  //   added
  // returns the handle to the new render target
  RenderTargetHandle CreateRenderTarget(float width, float height,
                                        GraphicsOptionTypes::Usage usage);

  // deletes a render target
  // render_target_handle - the handle to the render target to be deleted
  void DeleteRenderTarget(RenderTargetHandle render_target_handle);

  // set a render target to be active
  // render_terget_handle - the handle to the render target to set
  // index - the index to set the render target to
  void SetRenderTarget(RenderTargetHandle render_target_handle, unsigned index);

  // sets the render target to the back buffer
  void UnsetRenderTarget();

  // retrieves the handle to the underlying texture of the render target
  // render_target_handle - the handle to the render target to get the texture 
  //   handle from
  // returns the handle to the underlying texture of the render target
  TextureHandle GetRenderTargetTexture(RenderTargetHandle render_target_handle);

////////////////////////////////////////////////////////////////////////////////
////////////////////////shader calls////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
  // creates a shader from a file
  // filename - the name of the file to create the shader from
  // options - any compiler options to use on the shader
  // returns the handle to the shader that was created
  // Note: there MUST be two uniforms named u_model_view_matrix and
  //       u_projection_matrix, they must be declared as "uniform extern 
  //       float4x4", this is so the standard transform matrices can be passed
  //       automatically.
  VertexShaderHandle CreateVertexShader(String const &filename, 
    GraphicsOptionTypes::ShaderCompileOptions options = GraphicsOptionTypes::SCO_OptimizeLevel1);
  PixelShaderHandle CreatePixelShader(String const &filename, 
    GraphicsOptionTypes::ShaderCompileOptions options = GraphicsOptionTypes::SCO_OptimizeLevel1);

  // deletes a shader
  // shader_handle - the handle to the shader to be deleted
  void DeleteVertexShader(VertexShaderHandle shader_handle);
  void DeletePixelShader(PixelShaderHandle shader_handle);

  // creates a uniform from a shader and stores it
  // shader_handle - the shader to initialize the uniform for
  // uniform_name - the name of the uniform to initialize
  // parent_handle - the handle to the parent uniform of the one to initialize
  //                 (-1 means there is no parent)
  // returns the handle to the uniform in the shader
  VertexShaderConstantHandle InitializeVertexConstant(VertexShaderHandle shader_handle, String const &constant_name, VertexShaderConstantHandle parent_handle = -1);
  PixelShaderConstantHandle InitializePixelConstant(PixelShaderHandle shader_handle, String const &constant_name, PixelShaderConstantHandle parent_handle = -1);

  // starts a shader
  // shader_handle - the handle to the shader to start
  // Note: this just sets the active shader so that other shader functions can
  //       use it automatically
  void SetVertexShader(VertexShaderHandle shader_handle);
  void SetPixelShader(PixelShaderHandle shader_handle);

  // ends a shader
  void UnsetVertexShader();
  void UnsetPixelShader();

////////////////////////////////////////////////////////////////////////////////
////////////////////////shader parameter calls//////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
  // Sets a uniform in the active shader
  // uniform_handle - the handle to the uniform you want to set
  // data - the data you want to set the uniform to
  void SetVertexShaderConstant(VertexShaderConstantHandle constant_handle, bool data);
  void SetVertexShaderConstant(VertexShaderConstantHandle constant_handle, int data);
  void SetVertexShaderConstant(VertexShaderConstantHandle constant_handle, float data);
  void SetVertexShaderConstant(VertexShaderConstantHandle constant_handle, Vector2 data);
  void SetVertexShaderConstant(VertexShaderConstantHandle constant_handle, Vector3 data);
  void SetVertexShaderConstant(VertexShaderConstantHandle constant_handle, Vector4 data);
  void SetVertexShaderConstant(VertexShaderConstantHandle constant_handle, Color data);
  void SetVertexShaderConstant(VertexShaderConstantHandle constant_handle, Matrix2 const &data);
  void SetVertexShaderConstant(VertexShaderConstantHandle constant_handle, Matrix3 const &data);
  void SetVertexShaderConstant(VertexShaderConstantHandle constant_handle, Matrix4 const &data);
  void SetVertexShaderConstant(VertexShaderConstantHandle constant_handle, Matrix3 const *data, unsigned count);
  void SetVertexShaderConstant(VertexShaderConstantHandle constant_handle, Matrix4 const *data, unsigned count);
  void SetPixelShaderConstant(PixelShaderConstantHandle constant_handle, bool data);
  void SetPixelShaderConstant(PixelShaderConstantHandle constant_handle, int data);
  void SetPixelShaderConstant(PixelShaderConstantHandle constant_handle, float data);
  void SetPixelShaderConstant(PixelShaderConstantHandle constant_handle, Vector2 data);
  void SetPixelShaderConstant(PixelShaderConstantHandle constant_handle, Vector3 data);
  void SetPixelShaderConstant(PixelShaderConstantHandle constant_handle, Vector4 data);
  void SetPixelShaderConstant(PixelShaderConstantHandle constant_handle, Color data);
  void SetPixelShaderConstant(PixelShaderConstantHandle constant_handle, Matrix2 const &data);
  void SetPixelShaderConstant(PixelShaderConstantHandle constant_handle, Matrix3 const &data);
  void SetPixelShaderConstant(PixelShaderConstantHandle constant_handle, Matrix4 const &data);
  void SetPixelShaderConstant(PixelShaderConstantHandle constant_handle, Matrix3 const *data, unsigned count);
  void SetPixelShaderConstant(PixelShaderConstantHandle constant_handle, Matrix4 const *data, unsigned count);

  // Sets a texture uniform in the active shader
  // uniform_handle - the handle to the uniform you want to set
  // texture_handle - the handle to the texture you want to set
  void SetPixelShaderSampler(unsigned sampler_number, TextureHandle texture_handle);

  // Unsets a texture in the active shader
  void UnsetPixelShaderSampler(unsigned sampler_number);

  // call these if you have changed the world, view or projection matrices before calling CommitToShader
  // resets the model_view and projection matrices in the active shader
  void CommitModelViewMatrix();
  void CommitProjectionMatrix();

////////////////////////////////////////////////////////////////////////////////
////////////////////////draw calls//////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  // Drawing Methods:
  // DrawIndexed - use Index buffer to draw from vertex buffer
  // DrawBuffer - use Vertex buffer to draw by itself
  // 
  // Primitives:
  // TriList - Triangles are defined separately as either three consecutive 
  //           verts (ex. 123 is a tri the next tri is 456)
  // TriFan - Triangles are defined about one center point with each try also
  //          sharing another vert (ex. 123 then 134 then 145 then 156)
  // TriStrip - Triangles are defined as sharing two verts and the render state
  //            switches it's cull mode for each tri (ex. 123 then 234 then 345)
  // LineList - Lines are defined separately as either two consecutive verts or
  //            indexes (ex. 12 then 34 then 56)
  // LineStrip - Lines are defined as sharing one vert (12 then 23 then 34)
  // 
  // Parameters:
  // num_vertices - the number of vertices to draw from the vertex buffer
  // num_triangles - the number of triangle primitives in the index buffer to
  //                 draw if indexed, other wise the number of triangle 
  //                 primitives in the vertex buffer to draw
  // num_lines - the number of line primitives in the index buffer to draw if
  //             indexed, other wise the number of line primitives in the 
  //             vertex buffer to draw
  // num_segments - same as num_lines but makes more sense for line strip
  // start_index - the offset in the index buffer to start from
  // start_vertex - the offset in the vertex buffer to start from
  void DrawIndexedTriList(unsigned num_vertices, unsigned num_triangles, unsigned start_index = 0, unsigned start_vertex = 0);
  void DrawTriListBuffer(unsigned num_triangles, unsigned start_vertex = 0);
  void DrawIndexedTriFan(unsigned num_vertices, unsigned num_triangles, unsigned start_index = 0, unsigned start_vertex = 0);
  void DrawTriFanBuffer(unsigned num_triangles, unsigned start_vertex = 0);
  void DrawIndexedTriStrip(unsigned num_vertices, unsigned num_triangles, unsigned start_index = 0, unsigned start_vertex = 0);
  void DrawTriStripBuffer(unsigned num_triangles, unsigned start_vertex = 0);
  void DrawIndexedLineList(unsigned num_vertices, unsigned num_lines, unsigned start_index = 0, unsigned start_vertex = 0);
  void DrawLineListBuffer(unsigned num_lines, unsigned start_vertex = 0);
  void DrawIndexedLineStrip(unsigned num_vertices, unsigned num_segments, unsigned start_index = 0, unsigned start_vertex = 0);
  void DrawLineStripBuffer(unsigned num_segments, unsigned start_vertex = 0);

////////////////////////////////////////////////////////////////////////////////
////////////////////////render state calls//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
  // changes the render state
  // render_state - the render state to change
  // option - the option to change it to
  void SetRenderState(GraphicsOptionTypes::RenderState render_state, GraphicsOptionTypes::RenderStateOptions option);

  // enables or disables alpha blending
  // enable_alpha - true if you want to enable alpha, false otherwise
  void SetAlphaEnable(bool enable_alpha);

  // enable or disable depth buffering
  // enable_depth - true if you want to enable depth, false otherwise
  void SetDepthEnable(bool enable_depth);

////////////////////////////////////////////////////////////////////////////////
////////////////////////pipeline calls//////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
  // sets the viewport
  // start_x - the pixel coordinate of the leftmost corner of the viewport
  // start_y - the pixel coordinate of the uppermost corner of the viewport
  // width - the offset from start_x to the rightmost corner of the viewport
  // height - the offset from start_y to the lowermost corner of the viewport
  void SetViewport(unsigned start_x, unsigned start_y, unsigned width, unsigned height);

  // returns a reference to the world matrix stack
  MatrixStack<Matrix4>& GetWorldStack();

  // returns a reference to the view matrix stack
  MatrixStack<Matrix4>& GetViewStack();

  // returns a reference to the projection matrix stack
  MatrixStack<Matrix4>& GetProjectionStack();

  
}

}