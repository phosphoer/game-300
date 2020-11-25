// Nicholas Baldwin
// 2011/06/29
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{
class GraphicsInfoTypes
{
public:
  // various error codes that can be returned from GraphicsDevice calls
  enum ErrorCode
  {
    // used by IsDeviceLost
    EC_OK,
    EC_DeviceLost,
    EC_DeviceNotReset,

    EC_Max
  };

  // all pixel formats that can be used by back, depth, and stencil buffers
  enum Format
  {
    FMT_UNKNOWN              =  0,

    FMT_R8G8B8               = 20,
    FMT_A8R8G8B8             = 21,
    FMT_X8R8G8B8             = 22,
    FMT_R5G6B5               = 23,
    FMT_X1R5G5B5             = 24,
    FMT_A1R5G5B5             = 25,
    FMT_A4R4G4B4             = 26,
    FMT_R3G3B2               = 27,
    FMT_A8                   = 28,
    FMT_A8R3G3B2             = 29,
    FMT_X4R4G4B4             = 30,
    FMT_A2B10G10R10          = 31,
    FMT_A8B8G8R8             = 32,
    FMT_X8B8G8R8             = 33,
    FMT_G16R16               = 34,
    FMT_A2R10G10B10          = 35,
    FMT_A16B16G16R16         = 36,

    FMT_A8P8                 = 40,
    FMT_P8                   = 41,

    FMT_L8                   = 50,
    FMT_A8L8                 = 51,
    FMT_A4L4                 = 52,

    FMT_V8U8                 = 60,
    FMT_L6V5U5               = 61,
    FMT_X8L8V8U8             = 62,
    FMT_Q8W8V8U8             = 63,
    FMT_V16U16               = 64,
    FMT_A2W10V10U10          = 67,

    FMT_D16_LOCKABLE         = 70,
    FMT_D32                  = 71,
    FMT_D15S1                = 73,
    FMT_D24S8                = 75,
    FMT_D24X8                = 77,
    FMT_D24X4S4              = 79,
    FMT_D16                  = 80,

    FMT_D32F_LOCKABLE        = 82,
    FMT_D24FS8               = 83,

    FMT_L16                  = 81,

    FMT_VERTEXDATA           =100,
    FMT_INDEX16              =101,
    FMT_INDEX32              =102,

    FMT_Q16W16V16U16         =110,

    // Floating point surface formats

    // s10e5 formats (16-bits per channel)
    FMT_R16F                 = 111,
    FMT_G16R16F              = 112,
    FMT_A16B16G16R16F        = 113,

    // IEEE s23e8 formats (32-bits per channel)
    FMT_R32F                 = 114,
    FMT_G32R32F              = 115,
    FMT_A32B32G32R32F        = 116,

    FMT_CxV8U8               = 117,

    FMT_FORCE_DWORD          =0x7fffffff
  };

  // used to get display modes from the device
  struct DisplayMode
  {
    unsigned m_width;
    unsigned m_height;
    unsigned m_refresh_rate;
    Format m_format;
  };

  // used to get device capabilities
  struct Capabilities
  {
    // to be filled in as needed
  };

  // used to get display options from the device
  struct DisplayOptions
  {
    // to be filled in as needed
  };
};
}