// Nicholas Baldwin
// 2011/07/07
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

class GraphicsOptionTypes
{
public:
  // instructions to the shader compiler on how to be compiled
  // can be ORed together
  enum ShaderCompileOptions
  {
    SCO_Debug                = (1 << 0),
    SCO_SkipValidation       = (1 << 1),
    SCO_SkipOptimization     = (1 << 2),
    SCO_PartialPrecision     = (1 << 5),
    SCO_NoPreshader          = (1 << 8),
    SCO_OptimizeLevel0       = (1 << 14),
    SCO_OptimizeLevel1       = 0,
    SCO_OptimizeLevel2       = ((1 << 14) | (1 << 15)),
    SCO_OptimizeLevel3       = (1 << 15)
  };

  // options that set various render states
  enum RenderStateOptions
  {
    // to be filled in as needed

    // for RS_CullMode
    RSO_CullNone                    = 1,
    RSO_CullClockwise               = 2,
    RSO_CullCounterColockwise       = 3,
    RSO_FALSE                       = false,
    RSO_TRUE                        = true,
    RSO_Max,
    RSo_ForceDWORD                  = 0x7fffffff
  };

  // every render state that can be set
  enum RenderState
  {
    RS_ZEnable                      = 7,
    RS_FillMode                     = 8,
    RS_ShadeMode                    = 9,
    RS_ZWriteEnable                 = 14,
    RS_AlphaTestEnable              = 15,
    RS_LastPixel                    = 16,
    RS_SourceBlend                  = 19,
    RS_DestinationBlend             = 20,
    RS_CullMode                     = 22,
    RS_ZFunction                    = 23,
    RS_AlphaReference               = 24,
    RS_AlphaFunction                = 25,
    RS_DitherEnable                 = 26,
    RS_AlphaBlendEnable             = 27,
    RS_FogEnable                    = 28,
    RS_SpecularEnable               = 29,
    RS_FogColor                     = 34,
    RS_FogTableMode                 = 35,
    RS_FogStart                     = 36,
    RS_FogEnd                       = 37,
    RS_FogDensity                   = 38,
    RS_RangeFogEnable               = 48,
    RS_StencilEnable                = 52,
    RS_StencilFail                  = 53,
    RS_StencilZFail                 = 54,
    RS_StencilPass                  = 55,
    RS_StencilFunction              = 56,
    RS_StencilReference             = 57,
    RS_StencilMask                  = 58,
    RS_StencilWriteMask             = 59,
    RS_TextureFactor                = 60,
    RS_Wrap0                        = 128,
    RS_Wrap1                        = 129,
    RS_Wrap2                        = 130,
    RS_Wrap3                        = 131,
    RS_Wrap4                        = 132,
    RS_Wrap5                        = 133,
    RS_Wrap6                        = 134,
    RS_Wrap7                        = 135,
    RS_Clipping                     = 136,
    RS_Lighting                     = 137,
    RS_Ambient                      = 139,
    RS_FogVertexMode                = 140,
    RS_ColorVertex                  = 141,
    RS_LocalViewer                  = 142,
    RS_NormalizeNormals             = 143,
    RS_DiffuseMaterialSource        = 145,
    RS_SpecularMaterialSource       = 146,
    RS_AmbientMaterialSource        = 147,
    RS_EmissiveMaterialSource       = 148,
    RS_VertexBlend                  = 151,
    RS_ClipPlaneEnable              = 152,
    RS_PointSize                    = 154,
    RS_PointSize_Min                = 155,
    RS_PointSpriteEnable            = 156,
    RS_PointScaleEnable             = 157,
    RS_PointScale_A                 = 158,
    RS_PointScale_B                 = 159,
    RS_PointScale_C                 = 160,
    RS_MultisampleAntiAlias         = 161,
    RS_MultisampleMask              = 162,
    RS_PatchEdgeStyle               = 163,
    RS_DebugMonitorToken            = 165,
    RS_PointSize_Max                = 166,
    RS_IndexedVertexBlendEnable     = 167,
    RS_ColorWriteEnable             = 168,
    RS_TweenFactor                  = 170,
    RS_BlendOperation               = 171,
    RS_PositionDegree               = 172,
    RS_NormalDegree                 = 173,
    RS_ScissorTestEnable            = 174,
    RS_SlopeScaleDepthBias          = 175,
    RS_AntiAliasedLineEnable        = 176,
    RS_MinTessellationLevel         = 178,
    RS_MaxTessellationLevel         = 179,
    RS_AdaptiveTessellation_X       = 180,
    RS_AdaptiveTessellation_Y       = 181,
    RS_AdaptiveTessellation_Z       = 182,
    RS_AdaptiveTessellation_W       = 183,
    RS_EnableAdaptiveTessellation   = 184,
    RS_TwoSidedStencilMode          = 185,
    RS_CCW_StencilFail              = 186,
    RS_CCW_StencilZFail             = 187,
    RS_CCW_StencilPass              = 188,
    RS_CCW_StencilFunction          = 189,
    RS_ColorWriteEnable1            = 190,
    RS_ColorWriteEnable2            = 191,
    RS_ColorWriteEnable3            = 192,
    RS_BlendFactor                  = 193,
    RS_SRGBWriteEnable              = 194,
    RS_DepthBias                    = 195,
    RS_Wrap8                        = 198,
    RS_Wrap9                        = 199,
    RS_Wrap10                       = 200,
    RS_Wrap11                       = 201,
    RS_Wrap12                       = 202,
    RS_Wrap13                       = 203,
    RS_Wrap14                       = 204,
    RS_Wrap15                       = 205,
    RS_SeperateAlphaBlendEnable     = 206,
    RS_SourceBlendAlpha             = 207,
    RS_DestinationBlendAlpha        = 208,
    RS_BlendOperationAlpha          = 209,
    RS_ForceDWORD                   = 0x7fffffff
  };

  enum Usage
  {
    USG_RenderTarget                 = 0x00000001L,
    USG_DepthStencil                 = 0x00000002L,
    USG_WriteOnly                    = 0x00000008L,
    USG_SoftwareProcessing           = 0x00000010L,
    USG_DoNotClip                    = 0x00000020L,
    USG_Points                       = 0x00000040L,
    USG_RTPatches                    = 0x00000080L,
    USG_NPatches                     = 0x00000100L,
    USG_Dynamic                      = 0x00000200L,
    USG_AutoGenMipMap                = 0x00000400L,
    USG_DMap                         = 0x00004000L,

    USG_ForceDWORD                   = 0x7fffffffL,
  };
};

}