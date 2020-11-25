// Nicholas Baldwin
// 2012/04/14
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{
  
class Matrix4;
class Camera;

class Drawable
{
public:
  virtual void Draw() = 0;
  virtual unsigned GetDrawOrder(Camera const *) = 0;
  virtual Matrix4 GetWorldTransform() = 0;
};

}