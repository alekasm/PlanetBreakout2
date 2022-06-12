#include "Menu.h"
#include <string>
#include "GameLoader.h"
#include "Constants.h"

LRESULT CALLBACK EditorWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  EditorMenu* pWnd = (EditorMenu*)GetWindowLongPtr(hWnd, GWL_USERDATA);
  switch (message)
  {
  case WM_NCCREATE:
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

void EditorMenu::Initialize(HINSTANCE hInstance)
{
  grfStyle = WS_VISIBLE | WS_CLIPCHILDREN | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
  grfExStyle = WS_EX_STATICEDGE;
  WindowRect = { 0, 0, EDITOR_WIDTH, EDITOR_HEIGHT };
  AdjustWindowRectEx(&WindowRect, grfStyle, FALSE, grfExStyle);
  
  WNDCLASSEX EditorClass = {};
  EditorClass.cbClsExtra = NULL;
  EditorClass.cbWndExtra = NULL;
  EditorClass.lpszMenuName = NULL;
  EditorClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  EditorClass.hbrBackground = CreateSolidBrush(COLORREF(0xf0f0f0));
  EditorClass.cbSize = sizeof(WNDCLASSEX);
  //EditorClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
  //EditorClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
  EditorClass.hInstance = hInstance;
  EditorClass.lpfnWndProc = EditorWndProc;
  EditorClass.lpszClassName = "PlanetBreakout2EditorClass";
  EditorClass.style = CS_HREDRAW | CS_VREDRAW;
  RegisterClassEx(&EditorClass);

  hWnd = CreateWindowEx(
    grfExStyle,
    EditorClass.lpszClassName,
    std::string("Planet Breakout 2 - Map Editor").c_str(),
    grfStyle,
    0, 0,
    WindowRect.right - WindowRect.left,
    WindowRect.bottom - WindowRect.top, NULL, NULL, hInstance, this);

  //ShowWindow(hWnd, FALSE);
}