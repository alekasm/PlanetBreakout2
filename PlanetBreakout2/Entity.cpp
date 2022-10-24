#include "GameTypes.h"

Entity::Entity(EntityType type,
  unsigned width, unsigned height) :
  Drawable(width, height),
  type(type)
{
}

void Entity::SetSprite(std::wstring sprite)
{
  this->sprite = sprite;
}

bool Entity::HasSprite()
{
  return !sprite.empty();
}

const std::wstring& Entity::GetSprite() const
{
  return sprite;
}

void Entity::MoveCenterX(unsigned cx)
{
  Update(cx - (width / 2), y);
}

void Entity::MoveCenterY(unsigned cy)
{
  Update(x, cy - (height / 2));
}

void Entity::MoveCenter(unsigned cx, unsigned cy)
{
  Update(cx - (width / 2), cy - (height / 2));
}
void Entity::SetActive(bool value)
{
  active = value;
}
const bool Entity::IsActive() const
{
  return active;
}