// Nicholas Baldwin
// 2011/10/14
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "GraphicsHandleTypes.h"

namespace Improbability
{

class RenderTarget
{
public:
  RenderTarget() {}
  RenderTarget(RenderTarget const *original);
  RenderTarget(float width, float height);

  ~RenderTarget();

  static void BindMeta(MetaComposition*);

  void Resize(float width, float height);
  void SetRenderTarget(unsigned index);
  void UnsetRenderTarget();

  void SetUniformTexture(String const &uniform_name);

private:
  RenderTargetHandle m_render_target_handle;
};

}