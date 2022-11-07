#pragma once
#include <Windows.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <Wincodec.h>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <xaudio2.h>
#include "Shlwapi.h"
#include "Shellapi.h"

#pragma comment(lib, "Windowscodecs")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Shell32.lib")

enum ColorBrush {
  GRAY = 0x9A9FA8,
  GREEN = 0x98DF50,

  BLACK = 0x000000,
  ORANGE = 0xdfa650,
  DARK_GREEN = 0x679e2f,
  DARK_GRAY = 0x121212,
  WHITE = 0xFFFFFF,
  RED = 0xFF0000,
  BLUE = 0x4287f5,
  PURPLE = 0xFF00FF,

  GREEN_HALF,
  RED_HALF,
  BLACK_HALF,

  GRADIENT_1,
  RADIAL_GRADIENT_1,
  GRADIENT_2,
  GRADIENT_BUTTON
};


enum TextFormat {
  LEFT_8F, LEFT_10F, LEFT_12F,
  LEFT_16F, LEFT_24F,
  CENTER_10F, CENTER_12F, CENTER_14F,
  CENTER_18F, CENTER_24F, CENTER_72F,
};

struct TextFormatData {
  IDWriteTextFormat* format = nullptr;
  float size;
  bool centerAlignment;
  TextFormatData(float size, bool centerAlignment) :
    size(size), centerAlignment(centerAlignment)
  {
  }
};

typedef std::unordered_map<TextFormat, TextFormatData> TextFormatMap;
typedef std::unordered_map<std::wstring, ID2D1Bitmap*> SpriteMap;

struct AudioData {
  WAVEFORMATEXTENSIBLE wfx = { 0 };
  XAUDIO2_BUFFER buffer = { 0 };
};
typedef std::unordered_map<std::wstring, AudioData> AudioMap;
struct ResourceLoader
{
  ResourceLoader() = delete;
  static HRESULT LoadAudioFiles();
  static void PlayAudio(std::wstring, bool loop = false);
  static void Initialize();
  static void InitializeClient(HWND);
  static void FindFiles(std::wstring directory, std::vector<std::filesystem::path>& out);
  static const SpriteMap& GetSpriteMap();
  static bool ContainsSprite(const std::wstring&);
  static bool GetFile(std::wstring& filename);
  static ID2D1HwndRenderTarget* GetHwndRenderTarget();
  static ID2D1Brush* GetBrush(ColorBrush);
  static IDWriteTextFormat* GetTextFormat(TextFormat);
  static const TextFormatData& GetTextFormatData(TextFormat);
  static ID2D1Factory* GetFactory();
  static std::filesystem::path GetLevelPath();
private:
  static std::filesystem::path runpath;
  static SpriteMap sprite_map;
  static ID2D1HwndRenderTarget* target;
  static ID2D1HwndRenderTarget* fullscreenTarget;
  static ID2D1Factory* factory;
  static IDWriteFactory* wfactory;
  //static IDWriteTextFormat* formats[];
  //static ID2D1Brush* brushes[];
  static std::unordered_map<ColorBrush, ID2D1Brush*> brushes;
  static AudioMap audioMap;
  static IXAudio2* pXAudio2;
  static IXAudio2MasteringVoice* pMasterVoice;
  static TextFormatMap textFormatMap;

};
