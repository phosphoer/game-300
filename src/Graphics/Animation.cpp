// Nicholas Baldwin
// 2011/10/19
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Animation.h"
#include "../FileIO/Serializer.h"
#include "../math/Matrix4.h"
#include "Texture.h"
#include "TextureFromFile.h"
#include "../Core/EngineCore.h"
#include "../Resource/ResourceManager.h"
#include "VertexShader.h"

namespace Improbability
{

MakeMeta(Animation);

void Animation::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void Animation::Load(char const * filepath)
{
  Serializer serializer(filepath);

  Read(serializer, m_texture_name, "Texture");

  bool max_frames_read = false;
  m_max_frames_per_track = 0;

  if (Read(serializer, m_max_frames_per_track, "MaxFrames"))
    max_frames_read = true;

  serializer.EnterElement("Tracks");
  for (serializer.Begin(); serializer.IsGood(); serializer.Iterate())
  {
    int track_length;
    Read(serializer, track_length);
    m_num_frames_in_track.push_back(track_length);

    if (!max_frames_read && track_length > m_max_frames_per_track)
      m_max_frames_per_track = track_length;
  }
  serializer.LeaveElement();
  serializer.LeaveElement();

  if (!Read(serializer, m_current_track, "StartTrack"))
    m_current_track = 0;
  if (!Read(serializer, m_current_frame, "StartFrame"))
    m_current_frame = 0;
}

void Animation::Set()
{
  Matrix4 texture_matrix;
  texture_matrix.Identity();

  Texture *texture = g_engine_core->GetResourceManager()->GetResource<TextureFromFile>(m_texture_name)->GetTexture();
  texture->Set("s_sampler");

  float frame_width = 1.0f / (float)m_max_frames_per_track;
  float frame_height = 1.0f / (float)m_num_frames_in_track.size();

  texture_matrix = Matrix4::CreateScaling(frame_width, frame_height, 1.0f) 
                 * Matrix4::CreateTranslation((float)m_current_frame, (float)m_current_track, 0.0f);

  VertexShader::GetActiveShader()->SetConstant("u_texture_matrix", texture_matrix);
}

void Animation::Unset()
{
  Texture *texture = g_engine_core->GetResourceManager()->GetResource<TextureFromFile>(m_texture_name)->GetTexture();
  texture->Unset("s_sampler");
}

void Animation::SetTrack(unsigned i)
{
  ErrorIf(i > m_num_frames_in_track.size(), "Attempted to set a bad track");

  m_current_track = i;
}

void Animation::SetFrame(unsigned i)
{
  ErrorIf(i > m_num_frames_in_track[m_current_track], "Attempted to set a bad frame");

  m_current_frame = i;
}

void Animation::IncrementFrame()
{
  m_current_frame = (m_current_frame + 1) % m_num_frames_in_track[m_current_track];
}

}