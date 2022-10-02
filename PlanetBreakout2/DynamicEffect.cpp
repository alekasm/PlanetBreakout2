#include "DynamicEffect.h"
#include "ResourceLoader.h"

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
  SetPosition(x, y);
}

DynamicEffect::~DynamicEffect()
{
  if (geometry)
    delete geometry;
  if (transformedGeometry)
    delete geometry;
}

RingEffect::RingEffect(float x, float y) : DynamicEffect(x, y)
{
  ID2D1Factory* factory = ResourceLoader::GetFactory();
  ID2D1EllipseGeometry* m_pEllipseGeometry;
  HRESULT hrCreateEllipse =
    factory->CreateEllipseGeometry(
      D2D1::Ellipse(D2D1::Point2F(x, y),
        6.f, 1.25f),
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
    //delete transformedGeometry;
  }
  if (currentScale >= maxScale)
    active = false;
}