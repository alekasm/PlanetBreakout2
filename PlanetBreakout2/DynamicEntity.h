#pragma once
#include "GameTypes.h"
#include <unordered_map>

struct DynamicEntity : Entity
{
  DynamicEntity(EntityType type, //std::wstring sprite,
    unsigned width, unsigned height) :
    Entity(type, width, height)
  {
  }
  void UpdateFrame(int64_t elapsed);
  virtual void Start() = 0;
  //const bool IsActive() const;
  float GetSpeed();
  float GetRealX();
  float GetRealY();
  virtual void PreFrameUpdate();
  virtual void PostFrameUpdate(float elapsed = 0.f);
  //void SetActive(bool);
  void SetPosition(float x, float y);
  void RandomDirection(double min, double max);
  void Reset();
  void SetSpeed(float);
protected:
  //bool active = true;
  float speed = 0.0f;
  double direction = 0.f;
  float real_x = 0.f;
  float real_y = 0.f;
  float old_x = 0.f;
  float old_y = 0.f;
};
