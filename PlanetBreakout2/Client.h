#pragma once
#include <Windows.h>
#include <iostream>
#include "Constants.h"


struct Client
{
  void Initialize(HINSTANCE);
  void PostInitialize();
  void UpdateClientWindow();
  bool IsFocused();
  void SetClientFocus(bool);
  void RightClickLevel();
  void LeftClickLevel();
  void ProcessWM_CHAR(WPARAM);
  void ProcessWM_KEYDOWN(WPARAM);
  void SetWindowedBorderless();
  void SetWindowed();
  bool IsFullScreen();
  void ToggleFullScreen();
  float GetFullScreenScale();
  HWND GetHWND();
  void SetShowCursor(bool);
  bool GetShowCursor();
private:
  bool showCursor = false;
  HWND hWnd;
  RECT WindowRect;
  DWORD grfStyle, grfExStyle;
};