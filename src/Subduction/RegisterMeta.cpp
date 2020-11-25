// David Evans
// 2011/05/12
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"

namespace Improbability
{

// Ignore this.
MakeMetaBasic(bool);
MakeMetaBasic(char);
MakeMetaBasic(short);
MakeMetaBasic(int);
MakeMetaBasic(long);
MakeMetaBasic(unsigned);
MakeMetaBasic(float);
MakeMetaBasic(double);
MakeMetaBasic(HWND);

// Visual studio is dumb and optimizes out classes that it thinks aren't in use
// The ForceLink macro will make sure that the symbols from the meta class are imported
// You don't need to include or declare the class
// The function doesn't actually need to get called
void RegisterMeta()
{
  // Add new force links here
  ForceLinkThat(ObjectManager);
  ForceLinkThat(PythonSystem);
  ForceLinkThat(TextResource);
  ForceLinkThat(Blueprint);
  ForceLinkThat(FilepathResource);
  ForceLinkThat(TileSystem);
  ForceLinkThat(Tilemap);
  ForceLinkThat(WindowSystem);
  ForceLinkThat(TileComponent);
  ForceLinkThat(TileGameplayComponent);
  ForceLinkThat(GraphicsSystem);
  ForceLinkThat(World);
  ForceLinkThat(InputSystem);
  ForceLinkThat(InputActions);
  ForceLinkThat(DynamicComponent);
  ForceLinkThat(Physics);
  ForceLinkThat(TileController);
  ForceLinkThat(ResourceManager);
  ForceLinkThat(Camera);
  ForceLinkThat(LogicComponent);
  ForceLinkThat(LogicSystem);
  ForceLinkThat(SetVelocityAction);
  ForceLinkThat(Action);
  ForceLinkThat(ActionList);
  ForceLinkThat(ExecuteList);
  ForceLinkThat(JumpAction);
  ForceLinkThat(VertexShader);
  ForceLinkThat(PixelShader);
  ForceLinkThat(TextureFromFile);
  ForceLinkThat(IntToStringResource);
  ForceLinkThat(EditorSystem);
  ForceLinkThat(PushSpaceAction);
  ForceLinkThat(PopSpaceAction);
  ForceLinkThat(EditorCamera);
  ForceLinkThat(UIComponent);
  ForceLinkThat(Vector2);
  ForceLinkThat(Vector3);
  ForceLinkThat(Vector4);
  ForceLinkThat(AnimatePropertyAction);
  ForceLinkThat(ActionEvent);
  ForceLinkThat(KeyboardEvent);
  ForceLinkThat(MouseEvent);
  ForceLinkThat(Animation);
  ForceLinkThat(WaitAction);
  ForceLinkThat(FuzzyController);
  ForceLinkThat(UISystem);
  ForceLinkThat(UISystemInterface);
  ForceLinkThat(HUDSystem);
  ForceLinkThat(ListenerComponent);
  ForceLinkThat(PlaySoundAction);
  ForceLinkThat(SoundEmitter);
  ForceLinkThat(SoundSystem);
  ForceLinkThat(SpriteComponent);
  /* idk if this really needs to be here */
  ForceLinkThat(TilePos3);

  // Insane shit. Don't touch.
  ForceLinkThat(kMouseLeftButton);
  ForceLinkThat(kMouseLeftButton);
  ForceLinkThat(kMouseRightButton);
  ForceLinkThat(kMouseMiddleButton);
  ForceLinkThat(kMouseX1Button);
  ForceLinkThat(kMouseX2Button);
  ForceLinkThat(kKeyBackspace);
  ForceLinkThat(kKeyTab);
  ForceLinkThat(kKeyClear);
  ForceLinkThat(kKeyEnter);
  ForceLinkThat(kKeyShift);
  ForceLinkThat(kKeyControl);
  ForceLinkThat(kKeyAlt);
  ForceLinkThat(kKeyCapsLock);
  ForceLinkThat(kKeyEscape);
  ForceLinkThat(kKeySpace);
  ForceLinkThat(kKeyPageUp);
  ForceLinkThat(kKeyPageDown);
  ForceLinkThat(kKeyEnd);
  ForceLinkThat(kKeyHome);
  ForceLinkThat(kKeyLeft);
  ForceLinkThat(kKeyUp);
  ForceLinkThat(kKeyRight);
  ForceLinkThat(kKeyDown);
  ForceLinkThat(kKeyInsert);
  ForceLinkThat(kKeyDelete);
  ForceLinkThat(kKey0);
  ForceLinkThat(kKey1);
  ForceLinkThat(kKey2);
  ForceLinkThat(kKey3);
  ForceLinkThat(kKey4);
  ForceLinkThat(kKey5);
  ForceLinkThat(kKey6);
  ForceLinkThat(kKey7);
  ForceLinkThat(kKey8);
  ForceLinkThat(kKey9);
  ForceLinkThat(kKeyA);
  ForceLinkThat(kKeyB);
  ForceLinkThat(kKeyC);
  ForceLinkThat(kKeyD);
  ForceLinkThat(kKeyE);
  ForceLinkThat(kKeyF);
  ForceLinkThat(kKeyG);
  ForceLinkThat(kKeyH);
  ForceLinkThat(kKeyI);
  ForceLinkThat(kKeyJ);
  ForceLinkThat(kKeyK);
  ForceLinkThat(kKeyL);
  ForceLinkThat(kKeyM);
  ForceLinkThat(kKeyN);
  ForceLinkThat(kKeyO);
  ForceLinkThat(kKeyP);
  ForceLinkThat(kKeyQ);
  ForceLinkThat(kKeyR);
  ForceLinkThat(kKeyS);
  ForceLinkThat(kKeyT);
  ForceLinkThat(kKeyU);
  ForceLinkThat(kKeyV);
  ForceLinkThat(kKeyW);
  ForceLinkThat(kKeyX);
  ForceLinkThat(kKeyY);
  ForceLinkThat(kKeyZ);
  ForceLinkThat(kKeyLeftWin);
  ForceLinkThat(kKeyRightWin);
  ForceLinkThat(kKeyNumPad0);
  ForceLinkThat(kKeyNumPad1);
  ForceLinkThat(kKeyNumPad2);
  ForceLinkThat(kKeyNumPad3);
  ForceLinkThat(kKeyNumPad4);
  ForceLinkThat(kKeyNumPad5);
  ForceLinkThat(kKeyNumPad6);
  ForceLinkThat(kKeyNumPad7);
  ForceLinkThat(kKeyNumPad8);
  ForceLinkThat(kKeyNumPad9);
  ForceLinkThat(kKeyNumPadMultiply);
  ForceLinkThat(kKeyNumPadAdd);
  ForceLinkThat(kKeyNumPadSeperator);
  ForceLinkThat(kKeyNumPadSubtract);
  ForceLinkThat(kKeyNumPadDecimal);
  ForceLinkThat(kKeyNumPadDivide);
  ForceLinkThat(kKeyF1);
  ForceLinkThat(kKeyF2);
  ForceLinkThat(kKeyF3);
  ForceLinkThat(kKeyF4);
  ForceLinkThat(kKeyF5);
  ForceLinkThat(kKeyF6);
  ForceLinkThat(kKeyF7);
  ForceLinkThat(kKeyF8);
  ForceLinkThat(kKeyF9);
  ForceLinkThat(kKeyF10);
  ForceLinkThat(kKeyF11);
  ForceLinkThat(kKeyF12);
  ForceLinkThat(kKeyNumLock);
  ForceLinkThat(kKeyScrollLock);
  ForceLinkThat(kKeyLeftShift);
  ForceLinkThat(kKeyRightShift);
  ForceLinkThat(kKeyLeftControl);
  ForceLinkThat(kKeyRightControl);
  ForceLinkThat(kKeyLeftAlt);
  ForceLinkThat(kKeyRightAlt);
  ForceLinkThat(kKeyMinus);
  ForceLinkThat(kKeyPlus);
  ForceLinkThat(kKeyComma);
  ForceLinkThat(kKeyPeriod);
  ForceLinkThat(kKeyForwardSlash);
  ForceLinkThat(kKeyTilde);
  ForceLinkThat(kKeyLeftBracket);
  ForceLinkThat(kKeyBackSlash);
  ForceLinkThat(kKeyRightBracket);
  ForceLinkThat(kKeyQuote);
}

}