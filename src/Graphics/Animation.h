// Nicholas Baldwin
// 2011/10/19
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "..\Resource\Resource.h"

namespace Improbability
{

class Animation : Resource
{
public:
  static void BindMeta(MetaComposition *comp);

  void Load(char const * filepath);
  void Unload() {}

  void Set();
  void Unset();

  void SetTrack(unsigned i);
  void SetFrame(unsigned i);
  void IncrementFrame();

  int GetCurrentTrack() { return m_current_track; }
  int GetCurrentFrame() { return m_current_frame; }
  int GetFrameCountInTrack(int track) { return m_num_frames_in_track[track]; }
  int GetTrackCount() {return m_num_frames_in_track.size(); }

private:
  String m_texture_name;
  int m_current_track, m_current_frame, m_max_frames_per_track;
  std::vector<unsigned> m_num_frames_in_track;
};

}