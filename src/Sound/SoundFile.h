// Jake Leonard
// 2011/10/02
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Resource/Resource.h"
#include "../Math/Vector3.h"

namespace Improbability
{
  class SoundFile : public Resource
  {
  public:
    static void BindMeta(MetaComposition *comp);

    void Load(char const *filepath);
    void Unload();

  private:
    friend class SoundSystem;
    friend class SoundEmitter;
    friend class Sound;

    String m_fileName, m_filePath;
    FMOD::Sound* m_snd;
  };
}