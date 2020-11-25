// Jake Leonard
// 2011/10/02
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "SoundSystem.h"
#include "SoundFile.h"

namespace Improbability
{
  MakeMeta(SoundFile);

  void SoundFile::BindMeta(MetaComposition *comp)
  {
    comp->MetaAddComponent(MetaNewable);
  }

  void SoundFile::Load(char const *filepath)
  {
    m_filePath = filepath;

    String fileNameTemp = filepath;
    String concatStr(fileNameTemp, fileNameTemp.FindLastOf("\\") + 1, fileNameTemp.GetLength());
    m_fileName = concatStr;

    //if(true)
    //CriticalErrorIf(SoundSystem::sm_fmodSys->createSound(m_filePath.GetCString(), FMOD_IGNORETAGS|FMOD_3D, NULL, &m_snd) != FMOD_OK, "FMod cannot create the sound");
    //else
      SoundSystem::sm_fmodSys->createSound(m_filePath.GetCString(), FMOD_IGNORETAGS|FMOD_2D, NULL, &m_snd);
    SoundSystem::AddSound(this);
  }

  void SoundFile::Unload()
  {
    /* We cannot free here because the soundsystem deallocs it's 
      FMod system, thus preventing sounds from deallocating */
  }

}
