#pragma once
#include "DynamicEntity.h"
struct Star : DynamicEntity
{
  Star() :
    DynamicEntity(EntityType::STAR, STAR_SPRITE, 4, 4)
  {
    active = false;
    direction = 0.f;
    speed = 0.f;
  }
  void Start() override;
  const float GetOpacity() const;
  virtual void PostFrameUpdate() override;
private:
  enum StarDirection {
    BRIGHTER, DARKER
  } starDirection = StarDirection::BRIGHTER;
  float currentOpacity = 1.f;
  float minOpacity = 0.f;
  float maxOpacity = 1.f;
  float opacityRate = 0.001f;

};