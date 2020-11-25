// Nicholas Baldwin
// 2011/10/14
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "RenderTarget.h"
#include "GraphicsDevice.h"
#include "PixelShader.h"

namespace Improbability
{

MakeMeta(RenderTarget);

void RenderTarget::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

RenderTarget::RenderTarget(float width, float height)
{
  m_render_target_handle = GraphicsDevice::CreateRenderTarget(width, height, GraphicsOptionTypes::Usage(0));
}

RenderTarget::RenderTarget(RenderTarget const *original)
{
  unsigned tex_width = GraphicsDevice::GetTextureWidth(GraphicsDevice::GetRenderTargetTexture(original->m_render_target_handle));
  unsigned tex_height = GraphicsDevice::GetTextureWidth(GraphicsDevice::GetRenderTargetTexture(original->m_render_target_handle));
  unsigned screen_width = GraphicsDevice::GetDeviceWidth();
  unsigned screen_height = GraphicsDevice::GetDeviceHeight();

  m_render_target_handle = GraphicsDevice::CreateRenderTarget(float(tex_width) / float(screen_width),
                                                              float(tex_height) / float(screen_height), GraphicsOptionTypes::Usage(0));
}

RenderTarget::~RenderTarget()
{
  GraphicsDevice::DeleteRenderTarget(m_render_target_handle);
}

void RenderTarget::Resize(float width, float height)
{
  GraphicsDevice::DeleteRenderTarget(m_render_target_handle);
  m_render_target_handle = GraphicsDevice::CreateRenderTarget(width, height, GraphicsOptionTypes::Usage(0));
}

void RenderTarget::SetRenderTarget(unsigned index)
{
  GraphicsDevice::SetRenderTarget(m_render_target_handle, index);
}

void RenderTarget::SetUniformTexture(String const &uniform_name)
{
  PixelShader::GetActiveShader()->SetSampler(uniform_name, GraphicsDevice::GetRenderTargetTexture(m_render_target_handle));
}

void RenderTarget::UnsetRenderTarget()
{
  GraphicsDevice::UnsetRenderTarget();
}

}