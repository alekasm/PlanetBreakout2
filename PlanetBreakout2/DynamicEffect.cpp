#include "DynamicEffect.h"
#include "ResourceLoader.h"
#define _USE_MATH_DEFINES
#include <math.h>

ID2D1Geometry* DynamicEffect::GetGeometry()
{
  return transformedGeometry;
}

void DynamicEffect::Start()
{
  active = true;
}

DynamicEffect::DynamicEffect(float x, float y) :
  DynamicEntity(EntityType::VISUAL, STAR_SPRITE, 1, 1)
{
  ResourceLoader::GetHwndRenderTarget()->CreateSolidColorBrush(
    D2D1::ColorF(0x98DF50, 0.25f), &brush);
  SetPosition(x, y);
}

ID2D1SolidColorBrush* DynamicEffect::GetBrush()
{
  return brush;
}

DynamicEffect::~DynamicEffect()
{
  if (geometry)
    geometry->Release();
  if (transformedGeometry)
    transformedGeometry->Release();
  if (brush)
    brush->Release();
}

RingEffect::RingEffect(float x, float y) : DynamicEffect(x, y)
{
  ID2D1Factory* factory = ResourceLoader::GetFactory();
  ID2D1EllipseGeometry* m_pEllipseGeometry;
  ResourceLoader::GetHwndRenderTarget()->CreateSolidColorBrush(
    D2D1::ColorF(0x98DF50, 0.75f), &brush);
  maxUpdates = 200;
  HRESULT hrCreateEllipse =
    factory->CreateEllipseGeometry(
      D2D1::Ellipse(D2D1::Point2F(x, y),
        1.f, 1.f),
      &m_pEllipseGeometry);
  if (hrCreateEllipse == S_OK)
    geometry = m_pEllipseGeometry;

  HRESULT hrTransformed =
    factory->CreateTransformedGeometry(
      geometry, D2D1::Matrix3x2F::Scale(
        D2D1::SizeF(1.0f, 1.0f),
        D2D1::Point2F(real_x, real_y)),
      &transformedGeometry);
  active = true;
}

void RingEffect::PostFrameUpdate()
{
  ID2D1Factory* factory = ResourceLoader::GetFactory();
  ID2D1TransformedGeometry* transformed;
  currentScale += 0.075f;
  HRESULT hrTransformed =
    factory->CreateTransformedGeometry(
      geometry, D2D1::Matrix3x2F::Scale(
        D2D1::SizeF(currentScale, currentScale),
        D2D1::Point2F(real_x, real_y)), &transformed);
  if (hrTransformed == S_OK)
  {
    ID2D1TransformedGeometry* old = transformedGeometry;
    transformedGeometry = transformed;
    old->Release();
  }
  ++updates;
  if (updates > maxUpdates)
    active = false;
}


SpinSquareEffect::SpinSquareEffect(float x, float y) : DynamicEffect(x, y)
{
  ID2D1Factory* factory = ResourceLoader::GetFactory();
  ID2D1RectangleGeometry* pRectangleGeometry;
  speed = 3.f;
  maxUpdates = 250;
  RandomDirection(0, 2.f * M_PI);
  int min_size = 4;
  int max_size = 12;
  int size_int = rand() % (max_size - min_size + 1) + min_size;
  float half_size = (float)size_int / 2.f;
  if ((rand() % 2) == 1)
    rotationDirection = -1.f;
  float half_wsize = BRICK_WIDTH / 2.5f;
  float half_hsize = BRICK_HEIGHT / 2.5f;
  HRESULT hrCreateRectangle =
    factory->CreateRectangleGeometry(
      D2D1::RectF(
        x - half_wsize, y - half_hsize,
        x + half_wsize, y + half_hsize),
      &pRectangleGeometry);

  if (hrCreateRectangle == S_OK)
    geometry = pRectangleGeometry;

  HRESULT hrTransformed =
    factory->CreateTransformedGeometry(
      geometry, D2D1::Matrix3x2F::Scale(
        D2D1::SizeF(1.f, 1.f),
        D2D1::Point2F(real_x, real_y)),
      &transformedGeometry);
  active = true;
}

void SpinSquareEffect::PostFrameUpdate()
{
  ID2D1Factory* factory = ResourceLoader::GetFactory();
  ID2D1TransformedGeometry* transformed;
  rotation += 0.25f * rotationDirection;
  HRESULT hrTransformed =
    factory->CreateTransformedGeometry(
      geometry, D2D1::Matrix3x2F::Rotation(
        rotation, D2D1::Point2F(real_x, real_y)),
        &transformed);
  if (hrTransformed == S_OK)
  {
    ID2D1TransformedGeometry* old = transformedGeometry;
    transformedGeometry = transformed;
    old->Release();
  }
  ++updates;
  if (updates > maxUpdates)
    active = false;
}
