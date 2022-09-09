#pragma once
#include "GameTypes.h"
struct Ball : DynamicEntity
{
  Ball(std::wstring sprite) :
    DynamicEntity(EntityType::BALL, sprite, BALL_DIMENSION, BALL_DIMENSION)
  {
  }
  void UpdateFrame(int64_t elapsed) override;
  void Start() override;
  void SetPosition(float x, float y);
private:
  enum CollisionType { HORIZONTAL, VERTICAL };
  void Collision(CollisionType);
  void RandomDirection(float min, float max);
};