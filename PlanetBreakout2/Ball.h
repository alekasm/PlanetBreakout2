#pragma once
#include "GameTypes.h"
struct Ball : DynamicEntity
{
  BallType subtype;
  Ball(std::wstring sprite) :
    DynamicEntity(EntityType::BALL, sprite, BALL_DIMENSION, BALL_DIMENSION),
    subtype(BallType::NORMAL_BALL)
  {
  }
  void UpdateFrame(int64_t elapsed) override;
  void Start() override;
  void UpdateType(BallType subtype);
private:
  enum CollisionType { HORIZONTAL, VERTICAL };
  void Collision(CollisionType);
  void RandomDirection(float min, float max);
};