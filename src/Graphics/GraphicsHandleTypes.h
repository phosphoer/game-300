// Nicholas Baldwin
// 2012/05/17
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{
  
#define MakeHandle(type)                          \
  class type                                      \
  {                                               \
  public:                                         \
    type() : m_handle(unsigned(-1)) {};           \
    type(unsigned handle) : m_handle(handle) {};  \
    bool IsValid();                               \
    operator unsigned int() {return m_handle;}    \
  private:                                        \
    unsigned m_handle;                            \
  }

// this craziness is just to avoid repeated code and keep handles from polluting each other
MakeHandle(DeclarationHandle);
MakeHandle(BufferHandle);
MakeHandle(TextureHandle);
MakeHandle(RenderTargetHandle);
MakeHandle(VertexShaderHandle);
MakeHandle(PixelShaderHandle);
MakeHandle(VertexShaderConstantHandle);
MakeHandle(PixelShaderConstantHandle);

}