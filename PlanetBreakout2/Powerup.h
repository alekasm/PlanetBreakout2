#pragma once
#include "DynamicEntity.h"

#define POWERUP_HYPERBALL 0x0001
struct Powerup : DynamicEntity
{
  Powerup() :
    DynamicEntity(EntityType::POWERUP, POWERUP_SPRITE,
      POWERUP_DIMENSION, POWERUP_DIMENSION)
  {
    RegisterCollision(CollisionType::BAT);
  }
  void Start() override;
  virtual void CollisionBat(float x1, float x2) override;
};