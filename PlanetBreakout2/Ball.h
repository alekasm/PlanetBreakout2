#pragma once
#include "GameTypes.h"
struct Ball : Entity
{
  BallType subtype;
  Ball(std::wstring sprite) :
    Entity(EntityType::BALL, sprite, BALL_DIMENSION, BALL_DIMENSION),
    subtype(BallType::NORMAL_BALL)
  {
  }
  void UpdateFrame(int64_t elapsed);
  void Start();
  void UpdateType(BallType subtype);
  const bool IsActive() const;
  float GetSpeed();
private:
  enum CollisionType { HORIZONTAL, VERTICAL };
  void Collision(CollisionType);
  void RandomDirection(float min, float max);
  bool active = true;
  float speed = 1.0f;
  float direction = 0.f;
  float real_x = 0.f;
  float real_y = 0.f;
};