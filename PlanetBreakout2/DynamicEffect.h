#pragma once
#include "DynamicEntity.h"


struct DynamicEffect : DynamicEntity
{
  DynamicEffect(float x, float y);
  ID2D1Geometry* GetGeometry();
  ID2D1SolidColorBrush* GetBrush();
  ~DynamicEffect();
  virtual void Start() override;
protected:
  //float currentOpacity = 1.f;
  //float minOpacity = 0.f;
  //float maxOpacity = 1.f;
  //float opacityRate = 0.001f;
  ID2D1SolidColorBrush* brush = nullptr;
  ID2D1TransformedGeometry* transformedGeometry = nullptr;
  ID2D1Geometry* geometry = nullptr;
  size_t maxUpdates = 100;
  size_t updates = 0;
};

struct RingEffect : DynamicEffect
{
  RingEffect(float x, float y);
  virtual void PostFrameUpdate() override;
private:
  float currentScale = 1.0f;
};

struct SpinSquareEffect : DynamicEffect
{
  SpinSquareEffect(float x, float y);
  virtual void PostFrameUpdate() override;
private:  
  float rotationDirection = 1.f;
  float rotation = 0.f;
};