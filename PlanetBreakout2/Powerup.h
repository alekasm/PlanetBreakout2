#pragma once
#include "DynamicCollider.h"

struct Powerup : DynamicCollider
{
  Powerup() :
    DynamicCollider(EntityType::POWERUP, POWERUP_SPRITE,
      POWERUP_DIMENSION, POWERUP_DIMENSION)
  {
    RegisterCollision(CollisionType::BAT);
  }
  void Start() override;
  virtual void CollisionBat(float x1, float x2) override;
};