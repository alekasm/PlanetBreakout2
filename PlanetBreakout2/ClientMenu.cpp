#include "Menu.h"
#include "GameDraw.h"
#include <d2d1_1.h>
#include <Dwmapi.h>
#include <dwrite.h>
#include <string>
#include "ResourceLoader.h"
#include "GameLoader.h"
#include "Constants.h"
#include "GameController.h"
#include "Button.h"
#include "Windowsx.h"
#include "LevelEditor.h"
#include "LogicHelper.h"

#pragma comment(lib, "d2d1")
#pragma comment(lib, "Dwmapi")
#pragma comment(lib, "dwrite")

namespace
{
  LevelEditor levelEditor;
  bool initialized = false;
  HCURSOR cursor;
  POINT window_center;
  POINT mouse_pos;
  const RECT GameWindowRect = { 0, 0, GAME_WIDTH, GAME_HEIGHT };
  RECT GameClipRect;
  
  bool focused = false;
  //float window_cx = 0.f, window_cy = 0.f;
}

void ClientMenu::PostInitialize()
{
  levelEditor.initialize();
  initialized = true;
  cursor = LoadCursor(NULL, IDC_ARROW);
  UpdateClientWindow();
  window_center = POINT();
}

void ClientMenu::UpdateClientWindow()
{
  GameClipRect = GameWindowRect;
  ClientToScreen(hWnd, (LPPOINT)&GameClipRect.left);
  ClientToScreen(hWnd, (LPPOINT)&GameClipRect.right);
}


void SetClientFocus(bool value)
{
  focused = value;
  SetCursor(focused ? NULL : cursor);
  if (focused)
  {
    //GameController::GetInstance()->MouseUpdate(mouse_pos, true);
    ClipCursor(&GameClipRect);
  }
  else
  {
    ClipCursor(NULL);
  }
}

LRESULT CALLBACK ClientWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  ClientMenu* pWnd = (ClientMenu*)GetWindowLongPtr(hWnd, GWL_USERDATA);

  switch (message)
  {
  case WM_WINDOWPOSCHANGED:
    pWnd->UpdateClientWindow();
    //GameController::GetInstance()->MouseUpdate(mouse_pos, true);
  break;
  case WM_NCCREATE:
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
    break;
  case WM_SETCURSOR: //Patches an unsolved edge case with ShowCursor(FALSE)
    SetClientFocus(focused);
    break;

  case WM_PAINT:
  {
    switch (GameController::GetInstance()->type)
    {
    case GAME_EDITOR:
      DrawEditor(pWnd, levelEditor);
      break;
    default:
      DrawGame(pWnd);
    }
  }
  break;
  case WM_LBUTTONDOWN:
  {
    bool clicked = false;
    for (Button* button : levelEditor.primaryButtons)
      if (clicked = button->Click())
        break;
    if (clicked) break;

    for (unsigned int i = levelEditor.BrickIndexStart();
      i < levelEditor.BrickIndexEnd(); ++i)
    {
      if (clicked = levelEditor.brickButtons.at(i)->Click())
        break;
    }

    if (clicked) break;
    Brick* currentBrick = levelEditor.currentBrick;
    if (currentBrick != nullptr)
    {
      POINT p = GameController::GetInstance()->mousePos;
      unsigned x = p.x / BRICK_WIDTH;
      unsigned y = p.y / BRICK_HEIGHT;
      if (IsInGameSceen(x, y))
      {
        Brick newBrick(*currentBrick, x, y);
        levelEditor.editorLevel.AddBrick(newBrick);
      }
    }
  }
  break;
  case WM_RBUTTONDOWN:
  {
    POINT p = GameController::GetInstance()->mousePos;
    unsigned x = p.x / BRICK_WIDTH;
    unsigned y = p.y / BRICK_HEIGHT;
    if (IsInGameSceen(x, y))
    {
      uint32_t index = GetBrickIndex(x, y);
      std::vector<Brick>::reverse_iterator it = levelEditor.editorLevel.brickMap[index].rbegin();
      for (; it != levelEditor.editorLevel.brickMap[index].rend(); ++it)
      {
        Brick brick = (*it);
        if (brick.col == x && brick.row == y)
        {
          levelEditor.editorLevel.brickMap[index].erase(std::next(it).base());
          break;
        }
      }
    }
  }
  break;
  case WM_MOUSEMOVE:
  {
    mouse_pos.x = GET_X_LPARAM(lParam);
    mouse_pos.y = GET_Y_LPARAM(lParam);
    GameController::GetInstance()->MouseUpdate(mouse_pos);
    //pWnd->UpdateMousePosition();
    for (Button* button : levelEditor.primaryButtons)
      button->Update();
    for (unsigned int i = levelEditor.BrickIndexStart();
      i < levelEditor.BrickIndexEnd(); ++i)
    {
      levelEditor.brickButtons.at(i)->Update();
    }
  }
  break;
  case WM_KEYDOWN:
  {
    if (wParam == VK_F5)
    {
      if (GameController::GetInstance()->type == GAME_EDITOR)
      {
        Campaign campaign;
        campaign.levels.push_back(levelEditor.editorLevel);
        GameController::GetInstance()->CreateGame(campaign);
        SetClientFocus(true);
      }
      else
      {
        GameController::GetInstance()->EndGame();
        SetClientFocus(false);
      }
    }
    if (levelEditor.buttonTextSelect == nullptr)
      break;
    if (wParam == VK_BACK)
      levelEditor.buttonTextSelect->text.DeleteChar();
    else if (wParam == VK_RETURN)
      levelEditor.ClearSelected();
  }
  break;
  case WM_CHAR:
  {
    if (levelEditor.buttonTextSelect == nullptr)
      break;
    if (IsCharAlphaNumeric(wParam))
      levelEditor.buttonTextSelect->text.AddChar(wParam);
  }
  break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  case WM_ERASEBKGND:
    break; //Fixes screen-drag flicker 
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return pWnd == nullptr ? DefWindowProc(hWnd, message, wParam, lParam) : 0;

}

void ClientMenu::Initialize(HINSTANCE hInstance)
{
  grfStyle = WS_VISIBLE | WS_CLIPCHILDREN | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
  grfExStyle = WS_EX_STATICEDGE;
  WindowRect = { 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT };
  AdjustWindowRectEx(&WindowRect, grfStyle, FALSE, grfExStyle);

  WNDCLASSEX MainClass = {};
  MainClass.cbClsExtra = 0;
  MainClass.cbWndExtra = 0;
  MainClass.lpszMenuName = 0;
  MainClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  MainClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
  MainClass.cbSize = sizeof(WNDCLASSEX);
  MainClass.hInstance = hInstance;
  MainClass.lpfnWndProc = ClientWndProc;
  MainClass.lpszClassName = "PlanetBreakout2ClientClass";
  MainClass.style = CS_HREDRAW | CS_VREDRAW;
  RegisterClassEx(&MainClass);

  hWnd = CreateWindowEx(
    grfExStyle,
    MainClass.lpszClassName, "Planet Breakout 2",
    grfStyle,
    0, 0, WindowRect.right - WindowRect.left,
    WindowRect.bottom - WindowRect.top,
    NULL, NULL, hInstance, this);
}