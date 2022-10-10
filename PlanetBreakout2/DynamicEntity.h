#pragma once
#include "GameTypes.h"
#include <unordered_map>


struct DynamicEntity : Entity
{
  DynamicEntity(EntityType type, std::wstring sprite,
    unsigned width, unsigned height) :
    Entity(type, sprite, width, height)
  {
  }
  void UpdateFrame(int64_t elapsed);
  virtual void Start() = 0;
  const bool IsActive() const;
  float GetSpeed();
  float GetRealX();
  float GetRealY();
  virtual void PreFrameUpdate();
  virtual void PostFrameUpdate(float elapsed = 0.f);
  void SetActive(bool);
  void SetPosition(float x, float y);
  void RandomDirection(float min, float max);
  void Reset();
protected:
  bool active = true;
  float speed = 0.0f;
  float direction = 0.f;
  float real_x = 0.f;
  float real_y = 0.f;
  float old_x = 0.f;
  float old_y = 0.f;
};
