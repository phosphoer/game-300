// David Evans
// 2011/10/10
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "InputEvent.h"

namespace Improbability
{

MakeMeta(ActionEvent);
MakeMetaBasic(MouseEvent);
MakeMetaBasic(KeyboardEvent);


void ActionEvent::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

}