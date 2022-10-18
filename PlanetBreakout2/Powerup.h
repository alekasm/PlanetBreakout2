#pragma once
#include "DynamicCollider.h"

struct Powerup : DynamicCollider
{
  Powerup() :
    DynamicCollider(EntityType::POWERUP,
      POWERUP_DIMENSION, POWERUP_DIMENSION)
  {
    SetSprite(POWERUP_SPRITE);
    RegisterCollision(CollisionType::BAT);
  }
  void Start() override;
  virtual void CollisionBat(float x1, float x2) override;
};