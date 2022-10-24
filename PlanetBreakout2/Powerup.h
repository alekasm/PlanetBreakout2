#pragma once
#include "DynamicCollider.h"

struct Powerup : DynamicCollider
{
  Powerup() :
    DynamicCollider(EntityType::SPRITE,
      POWERUP_DIMENSION, POWERUP_DIMENSION)
  {
    SetSprite(POWERUP_SPRITE);
    RegisterCollision(CollisionType::BAT);
  }
  void Start() override;
  virtual void CollisionBat(float x1, float x2) override;
  void AddCount();
  const std::wstring GetCountString() const;
  const D2D1_RECT_F GetCountRect() const;
  const int GetCountColor() const;
private:
  uint32_t count = 1;
  std::wstring countString = L"x1";

};