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

void DynamicEffect::SetMaxUpdates(size_t value)
{
  maxUpdates = value;
}

DynamicEffect::DynamicEffect(float x, float y) :
  DynamicEntity(EntityType::VISUAL_EFFECT, 1, 1)
{
  ID2D1SolidColorBrush* solidBrush;
  ResourceLoader::GetHwndRenderTarget()->CreateSolidColorBrush(
    D2D1::ColorF(0x98DF50, 0.3f), &solidBrush);
  brush = solidBrush;
  SetPosition(x, y);
}

ID2D1Brush* DynamicEffect::GetBrush()
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
  ID2D1SolidColorBrush* solidBrush;
  ResourceLoader::GetHwndRenderTarget()->CreateSolidColorBrush(
    D2D1::ColorF(0x98DF50, 0.75f), &solidBrush);
  brush = solidBrush;
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

void RingEffect::PostFrameUpdate(float elapsed)
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


SpinSquareEffect::SpinSquareEffect(D2D1_RECT_F rect, ColorBrush colorBrush) :
  DynamicEffect(rect.left, rect.top)
{
  ID2D1Factory* factory = ResourceLoader::GetFactory();
  ID2D1RectangleGeometry* pRectangleGeometry;
  speed = 3.f;
  maxUpdates = 250;
  RandomDirection(0.f, 2.f * M_PI);
  int min_size = 4;
  int max_size = 12;
  int size_int = rand() % (max_size - min_size + 1) + min_size;
  float half_size = (float)size_int / 2.f;
  if ((rand() % 2) == 1)
    rotationDirection = -1.f;
  width = rect.right - rect.left;
  height = rect.bottom - rect.top;
  float half_wsize = width / 2.5f;
  float half_hsize = height / 2.5f;
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

  if (brush != nullptr)
    brush->Release();

  ID2D1SolidColorBrush* solidBrush;
  ResourceLoader::GetHwndRenderTarget()->CreateSolidColorBrush(
    D2D1::ColorF(colorBrush, 0.3f), &solidBrush);
  brush = solidBrush;
  active = true;
}

void SpinSquareEffect::PostFrameUpdate(float elapsed)
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

PlanetEffect::PlanetEffect(float x, float y) : DynamicEffect(x, y)
{
  D2D1_GRADIENT_STOP stops[] =
  {
      //{ 0.0f, D2D1::ColorF(rand() % 0xFFFFFF, 1.0f)},
      { 0.0f, D2D1::ColorF(D2D1::ColorF::DarkBlue)},
      { 1.0f, D2D1::ColorF(D2D1::ColorF::Black) }
  };

  ID2D1HwndRenderTarget* target = ResourceLoader::GetHwndRenderTarget();
  ID2D1GradientStopCollection* collection = nullptr;
  target->CreateGradientStopCollection(stops, _countof(stops), &collection);
  if (collection)
  {
    ID2D1LinearGradientBrush* planetBrush;
    D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES props = {};
    target->CreateLinearGradientBrush(props, collection, &planetBrush);
    planetBrush->SetStartPoint(D2D1::Point2F(0.f, 0.f));
    planetBrush->SetEndPoint(D2D1::Point2F(CLIENT_WIDTH, CLIENT_HEIGHT));
    brush = planetBrush;
  }

  lightAngle1 = (3.f * M_PI / 4.f);
  lightAngle2 = (7.f * M_PI / 4.f);
}

void PlanetEffect::PostFrameUpdate(float elapsed)
{
  lightAngle1 += 0.002f * elapsed;
  if (lightAngle1 > 2.f * M_PI)
    lightAngle1 -= 2.f * M_PI;
  lightAngle2 += 0.002f * elapsed;
  if (lightAngle2 > 2.f * M_PI)
    lightAngle2 -= 2.f * M_PI;
  float cx = CLIENT_WIDTH / 2.f;
  float cy = CLIENT_HEIGHT / 2.f;
 // float dist1 = std::fabs(lightAngle1) * (256.f / (2.f * M_PI));
  float dist1 = 256.f;

  ID2D1LinearGradientBrush* planetBrush = (ID2D1LinearGradientBrush*)brush;
  planetBrush->SetStartPoint(D2D1::Point2F(
    cx + (dist1 * sin(lightAngle1)),
    cy + (dist1 * cos(lightAngle1))));

  float dist2 = 256.f;
  //float dist2 = std::fabs(lightAngle2) * (256.f / (2.f * M_PI));
  planetBrush->SetEndPoint(D2D1::Point2F(
    cx + (dist2 * sin(lightAngle2)),
    cy + (dist2 * cos(lightAngle2))));
}


BrickBurnEffect::BrickBurnEffect(float x, float y) : DynamicEffect(x, y)
{
  ID2D1Factory* factory = ResourceLoader::GetFactory();
  ID2D1RectangleGeometry* pRectangleGeometry;
  //speed = 3.f;
  //maxUpdates = 250;
  //RandomDirection(0, 2.f * M_PI);

  HRESULT hrCreateRectangle =
    factory->CreateRectangleGeometry(
      D2D1::RectF(
        x, y,
        x + BRICK_WIDTH, y + BRICK_HEIGHT),
      &pRectangleGeometry);

  if (hrCreateRectangle == S_OK)
    geometry = pRectangleGeometry;

  HRESULT hrTransformed =
    factory->CreateTransformedGeometry(
      geometry, D2D1::Matrix3x2F::Scale(
        D2D1::SizeF(1.0f, 1.0f),
        D2D1::Point2F(real_x, real_y)),
      &transformedGeometry);

  ID2D1SolidColorBrush* solidBrush;
  ResourceLoader::GetHwndRenderTarget()->CreateSolidColorBrush(
    D2D1::ColorF(0xFF0000, 1.f), &solidBrush);
  brush = solidBrush;

}
void BrickBurnEffect::PostFrameUpdate(float elapsed)
{
  if (opacity <= 0.f)
  {
    active = false;
    return;
  }
  float new_opacity = opacity - (elapsed * 0.005f);
  if (new_opacity < 0.f)
    new_opacity = 0.f;
  opacity = new_opacity;
  ID2D1SolidColorBrush* solidBrush;
  ResourceLoader::GetHwndRenderTarget()->CreateSolidColorBrush(
    D2D1::ColorF(0xFF0000, opacity), &solidBrush);
  ID2D1Brush* old = brush;
  brush = solidBrush;
  old->Release();
}

