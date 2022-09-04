#pragma once
#include "GameTypes.h"

#define POWERUP_HYPERBALL 0x0001
struct Powerup : DynamicEntity
{
  Powerup() :
    DynamicEntity(EntityType::POWERUP, POWERUP_SPRITE,
      POWERUP_DIMENSION, POWERUP_DIMENSION)
  {
  }
  void UpdateFrame(int64_t elapsed) override;
  void Start() override;
};