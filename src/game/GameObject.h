
#ifndef BREAKOUT_GAMEOBJECT_H
#  define BREAKOUT_GAMEOBJECT_H

#  include "Vector2.h"
#  include <Engine/Renderer.h>
#  include <Engine/Sprite.h>

#endif // BREAKOUT_GAMEOBJECT_H

class GameObject
{
 public:
  GameObject();
  ~GameObject();

  bool initialiseSprite(ASGE::Renderer* renderer, std::string filename);
  ASGE::Sprite* getSprite();

  Vector2 velocity = Vector2(0, 0);
  const Vector2& getVelocity() const;
  void setVelocity(const Vector2& velocity);

  bool visibility = true;

 private:
  ASGE::Sprite* sprite = nullptr;
};