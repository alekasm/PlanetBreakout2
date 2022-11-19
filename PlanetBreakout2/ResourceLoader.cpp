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
ID2D1HwndRenderTarget* ResourceLoader::target;
ID2D1HwndRenderTarget* ResourceLoader::fullscreenTarget;
ID2D1Factory* ResourceLoader::factory;
IDWriteFactory* ResourceLoader::wfactory;
std::unordered_map<ColorBrush, ID2D1Brush*> ResourceLoader::brushes;
//IDWriteTextFormat* ResourceLoader::formats[16];
std::filesystem::path ResourceLoader::runpath;
IXAudio2* ResourceLoader::pXAudio2;
IXAudio2MasteringVoice* ResourceLoader::pMasterVoice;
IXAudio2SourceVoice* ResourceLoader::pSourceVoice = nullptr;
TextFormatMap ResourceLoader::textFormatMap = {
  {TextFormat::LEFT_8F, TextFormatData(8.f, false) },
  {TextFormat::LEFT_10F, TextFormatData(10.f, false) },
  {TextFormat::LEFT_12F, TextFormatData(12.f, false) },
  {TextFormat::LEFT_16F, TextFormatData(16.f, false) },
  {TextFormat::LEFT_24F, TextFormatData(24.f, false) },
  {TextFormat::CENTER_10F, TextFormatData(10.f, true) },
  {TextFormat::CENTER_12F, TextFormatData(12.f, true) },
  {TextFormat::CENTER_14F, TextFormatData(14.f, true) },
  {TextFormat::CENTER_18F, TextFormatData(18.f, true) },
  {TextFormat::CENTER_24F, TextFormatData(24.f, true) },
  {TextFormat::CENTER_72F, TextFormatData(72.f, true) }
};
AudioMap ResourceLoader::audioMap = {
  {L"brick.wav", {}},
  {L"lost.wav", {}},
  {L"laser.wav", {}},
  {L"click.wav", {}},
  {L"powerup.wav", {}}
};

ID2D1Factory* ResourceLoader::GetFactory()
{
  return factory;
}

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


ID2D1Brush* ResourceLoader::GetBrush(ColorBrush brush)
{
  return brushes[brush];
}

IDWriteTextFormat* ResourceLoader::GetTextFormat(TextFormat format)
{
  return textFormatMap.at(format).format;
}

const TextFormatData& ResourceLoader::GetTextFormatData(TextFormat format)
{
  return textFormatMap.at(format);
}

void ResourceLoader::InitializeClient(HWND hWnd)
{
  DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&wfactory));


  TextFormatMap::iterator it = textFormatMap.begin();
  for (; it != textFormatMap.end(); ++it)
  {
    wfactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_BOLD,
      DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
      it->second.size, L"en-us", &it->second.format);
    if(it->second.centerAlignment)
      it->second.format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
  }

  D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &factory);
  factory->CreateHwndRenderTarget(
    D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_HARDWARE),
    D2D1::HwndRenderTargetProperties(hWnd,
      //D2D1::SizeU(1440, 1080),
      D2D1::SizeU(CLIENT_WIDTH, CLIENT_HEIGHT),
      D2D1_PRESENT_OPTIONS_IMMEDIATELY),
    &target);


  //target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
  target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_ALIASED);
  target->CreateSolidColorBrush(D2D1::ColorF(ColorBrush::GRAY, 1.0f), (ID2D1SolidColorBrush**)&brushes[ColorBrush::GRAY]);
  target->CreateSolidColorBrush(D2D1::ColorF(ColorBrush::GREEN, 1.0f), (ID2D1SolidColorBrush**)&brushes[ColorBrush::GREEN]);
  target->CreateSolidColorBrush(D2D1::ColorF(ColorBrush::GREEN, 0.3f), (ID2D1SolidColorBrush**)&brushes[ColorBrush::GREEN_HALF]);
  target->CreateSolidColorBrush(D2D1::ColorF(ColorBrush::DARK_GREEN, 1.0f), (ID2D1SolidColorBrush**)&brushes[ColorBrush::DARK_GREEN]);
  target->CreateSolidColorBrush(D2D1::ColorF(ColorBrush::DARK_GRAY, 1.0f), (ID2D1SolidColorBrush**)&brushes[ColorBrush::DARK_GRAY]);
  target->CreateSolidColorBrush(D2D1::ColorF(ColorBrush::ORANGE, 1.0f), (ID2D1SolidColorBrush**)&brushes[ColorBrush::ORANGE]);

  target->CreateSolidColorBrush(D2D1::ColorF(ColorBrush::RED, 1.0f), (ID2D1SolidColorBrush**)&brushes[ColorBrush::RED]);
  target->CreateSolidColorBrush(D2D1::ColorF(ColorBrush::RED, 0.5f), (ID2D1SolidColorBrush**)&brushes[ColorBrush::RED_HALF]);

  target->CreateSolidColorBrush(D2D1::ColorF(ColorBrush::BLACK, 1.0f), (ID2D1SolidColorBrush**)&brushes[ColorBrush::BLACK]);
  target->CreateSolidColorBrush(D2D1::ColorF(ColorBrush::BLACK, 0.5f), (ID2D1SolidColorBrush**)&brushes[ColorBrush::BLACK_HALF]);

  target->CreateSolidColorBrush(D2D1::ColorF(ColorBrush::WHITE, 1.0f), (ID2D1SolidColorBrush**)&brushes[ColorBrush::WHITE]);

  target->CreateSolidColorBrush(D2D1::ColorF(ColorBrush::PURPLE, 1.0f), (ID2D1SolidColorBrush**)&brushes[ColorBrush::PURPLE]);
  target->CreateSolidColorBrush(D2D1::ColorF(ColorBrush::BLUE, 1.0f), (ID2D1SolidColorBrush**)&brushes[ColorBrush::BLUE]);

  {
    D2D1_GRADIENT_STOP stops[] =
    {
        { 0.0f, D2D1::ColorF(D2D1::ColorF::Cyan) },
        { 1.0f, D2D1::ColorF(D2D1::ColorF::DarkBlue) }
    };
    ID2D1GradientStopCollection* collection;
    target->CreateGradientStopCollection(stops, _countof(stops), &collection);
    D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES props = {};
    target->CreateLinearGradientBrush(props, collection,
      (ID2D1LinearGradientBrush**)&brushes[ColorBrush::GRADIENT_1]);
  }

  {
    D2D1_GRADIENT_STOP stops[] =
    {
        { 0.0f, D2D1::ColorF(D2D1::ColorF::Gold) },
        { 1.0f, D2D1::ColorF(D2D1::ColorF::Aquamarine) }
    };
    ID2D1GradientStopCollection* collection;
    target->CreateGradientStopCollection(stops, _countof(stops), &collection);
    D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES props = {};

    target->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(
      D2D1::Point2F(CLIENT_WIDTH / 2.f, 0.f),
      D2D1::Point2F(CLIENT_WIDTH / 2.f, CLIENT_HEIGHT)), collection,
      (ID2D1LinearGradientBrush**)&brushes[ColorBrush::GRADIENT_2]);
  }


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

std::filesystem::path ResourceLoader::GetLevelPath()
{
  std::filesystem::path map_path;
  map_path = std::filesystem::path(runpath);
  map_path.append(MAP_DIRECTORY);
  return map_path;
}


#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif
HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
{
  HRESULT hr = S_OK;
  if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
    return HRESULT_FROM_WIN32(GetLastError());

  DWORD dwChunkType;
  DWORD dwChunkDataSize;
  DWORD dwRIFFDataSize = 0;
  DWORD dwFileType;
  DWORD bytesRead = 0;
  DWORD dwOffset = 0;

  while (hr == S_OK)
  {
    DWORD dwRead;
    if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
      hr = HRESULT_FROM_WIN32(GetLastError());

    if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
      hr = HRESULT_FROM_WIN32(GetLastError());

    switch (dwChunkType)
    {
    case fourccRIFF:
      dwRIFFDataSize = dwChunkDataSize;
      dwChunkDataSize = 4;
      if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
        hr = HRESULT_FROM_WIN32(GetLastError());
      break;

    default:
      if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
        return HRESULT_FROM_WIN32(GetLastError());
    }

    dwOffset += sizeof(DWORD) * 2;

    if (dwChunkType == fourcc)
    {
      dwChunkSize = dwChunkDataSize;
      dwChunkDataPosition = dwOffset;
      return S_OK;
    }

    dwOffset += dwChunkDataSize;

    if (bytesRead >= dwRIFFDataSize) return S_FALSE;

  }

  return S_OK;

}
HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
  HRESULT hr = S_OK;
  if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
    return HRESULT_FROM_WIN32(GetLastError());
  DWORD dwRead;
  if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
    hr = HRESULT_FROM_WIN32(GetLastError());
  return hr;
}

void ResourceLoader::PlayAudio(std::wstring audio, bool loop)
{
  AudioMap::iterator it = audioMap.find(audio);
  if (it == audioMap.end())
    return;

  HRESULT hr;
  if (pSourceVoice != nullptr)
    pSourceVoice->DestroyVoice();
  if (FAILED(hr = pXAudio2->CreateSourceVoice(&pSourceVoice,
    (WAVEFORMATEX*)&it->second.wfx)))
  {
    return;
  }

  it->second.buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
  if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&it->second.buffer)))
    return;

  pSourceVoice->SetVolume(1.f);
  pSourceVoice->Start(0);
}

HRESULT ResourceLoader::LoadAudioFiles()
{ 
  HRESULT hr;
  pXAudio2 = nullptr;
  if (FAILED(hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
    return hr;

  pMasterVoice = nullptr;
  if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasterVoice)))
    return hr;

  AudioMap::iterator it = audioMap.begin();
  for (; it != audioMap.end(); ++it)
  {
    std::wstring filename = L"assets/audio/";
    filename.append(it->first);
    HANDLE hFile = CreateFileW(
      filename.c_str(),
      GENERIC_READ,
      FILE_SHARE_READ,
      NULL,
      OPEN_EXISTING,
      0,
      NULL);
    
    if (INVALID_HANDLE_VALUE == hFile)
    {
      wprintf(L"Failed to load audio file: %s\n", filename.c_str());
      return HRESULT_FROM_WIN32(GetLastError());
    }


    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
      return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwChunkSize;
    DWORD dwChunkPosition;
    //check the file type, should be fourccWAVE or 'XWMA'
    FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
    DWORD filetype;
    ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
    if (filetype != fourccWAVE)
      return S_FALSE;

    FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
    ReadChunkData(hFile, &it->second.wfx, dwChunkSize, dwChunkPosition);

    //fill out the audio data buffer with the contents of the fourccDATA chunk
    FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
    BYTE* pDataBuffer = new BYTE[dwChunkSize];
    ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

    it->second.buffer.AudioBytes = dwChunkSize;  //size of the audio buffer in bytes
    it->second.buffer.pAudioData = pDataBuffer;  //buffer containing audio data    
    // tell the source voice not to expect any data after this buffer
    it->second.buffer.Flags = XAUDIO2_END_OF_STREAM;
    wprintf(L"Loaded audio file: %s\n", it->first.c_str());
  }

  return S_OK;
}