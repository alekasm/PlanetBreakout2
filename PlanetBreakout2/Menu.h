#pragma once
#include <Windows.h>
#include <iostream>
#include "Constants.h"

struct Menu
{
  virtual void Initialize(HINSTANCE) = 0;
  virtual void PostInitialize() {}
  HWND hWnd;
  RECT WindowRect;
  DWORD grfStyle, grfExStyle;
};

struct ClientMenu : Menu
{
  void Initialize(HINSTANCE) override;
  void PostInitialize() override;
  void UpdateClientWindow();
  void UpdateMousePosition();
  bool IsFocused();
};

struct EditorSaveMenu : Menu
{
  void Initialize(HINSTANCE) override;
};

enum MenuType { MENU_CLIENT, MENU_EDITORSAVE };
namespace 
{
  Menu* Menus[MENU_COUNT];
  void InitializeMenus(HINSTANCE hInstance)
  {
    Menus[MenuType::MENU_CLIENT] = new ClientMenu();
    for (int i = 0; i < MENU_COUNT; ++i)
    {
      printf("Initializing Menu: %d\n", i);
      Menus[i]->Initialize(hInstance);
    }
  }
};
