#include "ResourceLoader.h"
#include <locale>
#include <codecvt>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <fstream>
#include <d2d1_1.h>
#include <Dwmapi.h>
#include <dwrite.h>
#include "Constants.h"


SpriteMap ResourceLoader::sprite_map;
ID2D1HwndRenderTarget*  ResourceLoader::target;
ID2D1Factory* ResourceLoader::factory;
IDWriteFactory* ResourceLoader::wfactory;
ID2D1SolidColorBrush* ResourceLoader::brushes[16];
IDWriteTextFormat* ResourceLoader::formats[16];
std::filesystem::path ResourceLoader::runpath;

const SpriteMap& ResourceLoader::GetSpriteMap()
{
  return sprite_map;
}

bool ResourceLoader::ContainsSprite(const std::wstring& key)
{
  return sprite_map.find(key) != sprite_map.end();
}

void ResourceLoader::FindFiles(std::wstring directory,
  std::vector<std::filesystem::path>& vector)
{
  for (const std::filesystem::directory_entry& entry :
    std::filesystem::recursive_directory_iterator(directory))
  {
    if (entry.is_regular_file())
    {
      vector.push_back(entry);
    }
  }
}

template <class T> void SafeRelease(T** ppT)
{
  if (*ppT)
  {
    (*ppT)->Release();
    *ppT = NULL;
  }
}


//https://docs.microsoft.com/en-us/windows/win32/direct2d/how-to-load-a-direct2d-bitmap-from-a-file
HRESULT LoadBitmapFromFile(
  ID2D1HwndRenderTarget* pRenderTarget,
  IWICImagingFactory* pIWICFactory,
  PCWSTR uri,
  ID2D1Bitmap** ppBitmap
)
{
  IWICBitmapDecoder* pDecoder = NULL;
  IWICBitmapFrameDecode* pSource = NULL;
  IWICStream* pStream = NULL;
  IWICFormatConverter* pConverter = NULL;

  HRESULT hr = pIWICFactory->CreateDecoderFromFilename(
    uri,
    NULL,
    GENERIC_READ,
    WICDecodeMetadataCacheOnLoad,
    &pDecoder
  );

  if (SUCCEEDED(hr))
  {
    // Create the initial frame.
    hr = pDecoder->GetFrame(0, &pSource);
  }

  if (SUCCEEDED(hr))
  {

    // Convert the image format to 32bppPBGRA
    // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
    hr = pIWICFactory->CreateFormatConverter(&pConverter);

  }


  if (SUCCEEDED(hr))
  {
    hr = pConverter->Initialize(
      pSource,
      GUID_WICPixelFormat32bppPBGRA,
      WICBitmapDitherTypeNone,
      NULL,
      0.f,
      WICBitmapPaletteTypeMedianCut
    );
  }

  if (SUCCEEDED(hr))
  {

    // Create a Direct2D bitmap from the WIC bitmap.
    hr = pRenderTarget->CreateBitmapFromWicBitmap(
      pConverter,
      NULL,
      ppBitmap
    );
  }

  SafeRelease(&pDecoder);
  SafeRelease(&pSource);
  SafeRelease(&pStream);
  SafeRelease(&pConverter);
  return hr;
}

ID2D1HwndRenderTarget* ResourceLoader::GetHwndRenderTarget()
{
  return target;
}


ID2D1SolidColorBrush* ResourceLoader::GetBrush(ColorBrush brush)
{
  return brushes[brush];
}

IDWriteTextFormat* ResourceLoader::GetTextFormat(TextFormat format)
{
  return formats[format];
}


void ResourceLoader::InitializeClient(HWND hWnd)
{
  DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&wfactory));

  wfactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_BOLD,
    DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
    12.0f, L"en-us", &formats[TextFormat::LEFT_12F]);

  wfactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_BOLD,
    DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
    12.0f, L"en-us", &formats[TextFormat::CENTER_12F]);
  formats[TextFormat::CENTER_12F]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

  wfactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_NORMAL,
    DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
    10.0f, L"en-us", &formats[TextFormat::LEFT_10F]);

  wfactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_BOLD,
    DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
    8.0f, L"en-us", &formats[TextFormat::LEFT_8F]);

  D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &factory);
  factory->CreateHwndRenderTarget(
    D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_HARDWARE),
    D2D1::HwndRenderTargetProperties(hWnd,
      D2D1::SizeU(CLIENT_WIDTH, CLIENT_HEIGHT),
      D2D1_PRESENT_OPTIONS_IMMEDIATELY),
    &target);

  target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
  target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_ALIASED);
  target->CreateSolidColorBrush(D2D1::ColorF(0x9A9FA8, 1.0f), &brushes[ColorBrush::GRAY]);
  target->CreateSolidColorBrush(D2D1::ColorF(0x98DF50, 1.0f), &brushes[ColorBrush::GREEN]);
  target->CreateSolidColorBrush(D2D1::ColorF(0xFF0000, 0.5f), &brushes[ColorBrush::RED_HALF]);
  target->CreateSolidColorBrush(D2D1::ColorF(0x000000, 1.0f), &brushes[ColorBrush::BLACK]);

  IWICImagingFactory* pIWICFactory = 0;
  CoCreateInstance(CLSID_WICImagingFactory, NULL,
    CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
    reinterpret_cast<void**>(&pIWICFactory));

  if (pIWICFactory == 0)
  {
    printf("Failed to create the IWICImagingFactory\n");
    return;
  }

  std::vector<std::filesystem::path> sprites;
  FindFiles(L"assets/sprites", sprites);

  for (std::filesystem::path& sprite : sprites)
  {
    ID2D1Bitmap* bitmap;
    HRESULT hr = LoadBitmapFromFile(target, pIWICFactory, sprite.wstring().c_str(), &bitmap);
    if (SUCCEEDED(hr))
    {
      std::wstring name = sprite.stem().wstring();
      printf("Added sprite: %ls\n", name.c_str());
      sprite_map[name] = bitmap;
    }
  }
  //delete pIWICFactory;
}

bool ResourceLoader::GetFile(std::wstring& filename)
{
  char szFile[256];
  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;
  ofn.lpstrFile = szFile;
  ofn.lpstrFile[0] = '\0';
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter = "";
  ofn.nFilterIndex = 1;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = NULL;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
  GetOpenFileName(&ofn);
  std::string sfile(ofn.lpstrFile);
  filename = std::wstring(sfile.begin(), sfile.end());
  return !filename.empty();
}

void ResourceLoader::Initialize()
{
  runpath = std::filesystem::current_path();
}

std::filesystem::path ResourceLoader::GetMapPath()
{
  std::filesystem::path map_path;
  map_path = std::filesystem::path(runpath);
  map_path.append(MAP_DIRECTORY);
  return map_path;
}