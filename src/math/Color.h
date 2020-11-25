// Nicholas Baldwin
// 2011/08/13
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

class Color
{
public:
  // useful constructors
  Color() : m_r(0.0f), m_g(0.0f), m_b(0.0f), m_alpha(1.0f) {}
  Color(Vector4 const &original) : m_r(original.m_x), m_g(original.m_y), m_b(original.m_z), m_alpha(original.m_w) {}
  Color(Vector3 const &original) : m_r(original.m_x), m_g(original.m_y), m_b(original.m_z), m_alpha(1.0f) {}
  Color(float r, float g, float b, float alpha = 1.0f) : m_r(r), m_g(g), m_b(b), m_alpha(alpha) {}
  Color(unsigned r, unsigned g, unsigned b, unsigned alpha = 255) : m_r(static_cast<float>(r)/255.0f),
                                                                    m_g(static_cast<float>(g)/255.0f),
                                                                    m_b(static_cast<float>(b)/255.0f),
                                                                    m_alpha(static_cast<float>(alpha)/255.0f) {}

  // component wise multiplication
  Color operator*(Color const &rhs) { return Color(m_r * rhs.m_r, m_g * rhs.m_g, m_b * rhs.m_b, m_alpha * rhs.m_alpha); }

public:
  // Basically the same structure as a Vector4
  union
  {
    struct {float m_r; float m_g; float m_b; float m_alpha;};
    float m_a[4];
  };
};

// stores 32-bit colors with alpha for use by DirectX
class A8R8G8B8
{
public:
  // useful constructors
  A8R8G8B8() {}
  A8R8G8B8(Color const &original) : m_color( (static_cast<unsigned char>(original.m_alpha * 255.0f) << 24) +
                                             (static_cast<unsigned char>(original.m_r * 255.0f)     << 16) +
                                             (static_cast<unsigned char>(original.m_g * 255.0f)     << 8)  +
                                             (static_cast<unsigned char>(original.m_b * 255.0f)) ) {}
  A8R8G8B8(unsigned alpha, unsigned r, unsigned g, unsigned b) : m_color( (static_cast<unsigned char>(alpha) << 24) +
                                                                          (static_cast<unsigned char>(r)     << 16) +
                                                                          (static_cast<unsigned char>(g)     << 8)  +
                                                                          (static_cast<unsigned char>(b)     << 0)) {}

public:
  DWORD m_color;
};

// stores 32-bit colors without alpha for use by DirectX
class X8R8G8B8
{
public:
  // useful constructors
  X8R8G8B8() : m_color(0xff << 24) {}
  X8R8G8B8(Color const &original) : m_color( (0xff                                         << 24) +
                                             (static_cast<unsigned char>(original.m_r * 255.0f)     << 16) +
                                             (static_cast<unsigned char>(original.m_g * 255.0f)     << 8)  +
                                             (static_cast<unsigned char>(original.m_b * 255.0f)     << 0)) {}
  X8R8G8B8(unsigned r, unsigned g, unsigned b) : m_color( (0xff                     << 24) +
                                                          (static_cast<unsigned char>(r)     << 16) +
                                                          (static_cast<unsigned char>(g)     << 8)  +
                                                          (static_cast<unsigned char>(b)     << 0)) {}

public:
  DWORD m_color;
};

}