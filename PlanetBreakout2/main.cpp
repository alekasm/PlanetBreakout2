#include <vector>
#include <ctime>
#include <thread>
#include <memory>
#include <fcntl.h>
#include <io.h>
#include <cstdio>
#include <string>
#include "ResourceLoader.h"
#include "GameLoader.h"
#include "GameController.h"
#include "Menu.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  CoInitializeEx(NULL, COINIT_MULTITHREADED);

  AllocConsole();
  FILE* p_file;
  freopen_s(&p_file, "CONIN$", "r", stdin);
  freopen_s(&p_file, "CONOUT$", "w", stdout);
  freopen_s(&p_file, "CONOUT$", "w", stderr);

  ResourceLoader::Initialize();
  InitializeMenus(hInstance);
  ResourceLoader::InitializeClient(Menus[MenuType::MENU_CLIENT]->hWnd);
  GameLoader::LoadAssets(L"assets/asset_map.cfg");

  Menus[MenuType::MENU_CLIENT]->PostInitialize();

  MSG msg;
  clock_t KeyTimer = clock();
  while (GetMessage(&msg, nullptr, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return (int)msg.wParam;
}