
#include "GameObject.h"

#include <Engine/DebugPrinter.h>

GameObject::GameObject() {}

GameObject::~GameObject()
{
  delete (sprite);
}

bool GameObject::initialiseSprite(ASGE::Renderer* renderer,
                                  std::string filename)
{
  sprite = renderer->createRawSprite();

  if (!sprite->loadTexture("/data/images/" + filename + ".png"))
  {
    ASGE::DebugPrinter{} << "init::Failed to load sprite" << std::endl;
    return false;
  }
  if (!visibility)
  {
    sprite->opacity(0);
  }

  return true;
}

ASGE::Sprite* GameObject::getSprite()
{
  return sprite;
}

const Vector2& GameObject::getVelocity() const
{
  return velocity;
}

void GameObject::setVelocity(const Vector2& velocity)
{
  GameObject::velocity = velocity;
}
