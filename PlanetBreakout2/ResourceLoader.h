#pragma once
#include <Windows.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <Wincodec.h>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include "Shlwapi.h"
#include "Shellapi.h"

#pragma comment(lib, "Windowscodecs")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Shell32.lib")



enum ColorBrush { GRAY, GREEN, RED_HALF, BLACK, ORANGE, DARK_GREEN, DARK_GRAY, WHITE, BLACK_HALF, GRADIENT_1 };
enum TextFormat {
  LEFT_12F, CENTER_12F, LEFT_10F, LEFT_8F,
  CENTER_24F, CENTER_18F, CENTER_10F, LEFT_16F, CENTER_14F,
  CENTER_72F, LEFT_24F};
typedef std::unordered_map<std::wstring, ID2D1Bitmap*> SpriteMap;
struct ResourceLoader
{
  ResourceLoader() = delete;
  static void Initialize();
  static void InitializeClient(HWND);
  static void FindFiles(std::wstring directory, std::vector<std::filesystem::path>& out);
  static const SpriteMap& GetSpriteMap();
  static bool ContainsSprite(const std::wstring&);
  static bool GetFile(std::wstring& filename);
  static ID2D1HwndRenderTarget* GetHwndRenderTarget();
  static ID2D1Brush* GetBrush(ColorBrush);
  static IDWriteTextFormat* GetTextFormat(TextFormat);
  static std::filesystem::path GetLevelPath();
private:
  static std::filesystem::path runpath;
  static SpriteMap sprite_map;
  static ID2D1HwndRenderTarget* target;
  static ID2D1Factory* factory;
  static IDWriteFactory* wfactory;
  static ID2D1Brush* brushes[];
  static IDWriteTextFormat* formats[];
};
