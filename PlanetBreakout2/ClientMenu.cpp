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
#include "MainMenu.h"

#pragma comment(lib, "d2d1")
#pragma comment(lib, "Dwmapi")
#pragma comment(lib, "dwrite")

namespace
{
  LevelEditor levelEditor;
  MainMenu mainMenu;
  bool initialized = false;
  HCURSOR cursor;
  POINT window_center;
  POINT mouse_pos;
  //RECT FullscreenWindowRect;
  bool fullscreen = false;
  const RECT GameWindowRect = { 0, 0, GAME_WIDTH, GAME_HEIGHT };
  float mouse_scale = 1.f;

  float fs_scale_factor = 1.f;
  float fs_scale_factor_rel = 1.f;

  RECT ClientFullscreenRect;
  RECT ClientFullscreenGameRect;

  RECT GameClipRect;
  bool focused = false;
  RECT desktop;
}

bool ClientMenu::IsFocused()
{
  return focused;
}

void ClientMenu::PostInitialize()
{
  mainMenu.initialize(this);
  levelEditor.initialize(this);
  initialized = true;
  cursor = LoadCursor(NULL, IDC_ARROW);
  UpdateClientWindow();
  window_center = POINT();
}

void ClientMenu::UpdateClientWindow()
{
  if (!fullscreen)
  {
    GameClipRect = GameWindowRect;
    ClientToScreen(hWnd, (LPPOINT)&GameClipRect.left);
    ClientToScreen(hWnd, (LPPOINT)&GameClipRect.right);
  }
  else
  {
    GameClipRect = ClientFullscreenGameRect;
  }
}

void ClientMenu::SetWindowed()
{
  SetWindowLongPtr(containerhWnd, GWL_STYLE, grfStyle);
  SetWindowLongPtr(containerhWnd, GWL_EXSTYLE, grfExStyle);
  ShowWindow(containerhWnd, SW_SHOWNORMAL);
  SetWindowPos(containerhWnd, HWND_TOP,
    0, 0,
    WindowRect.right - WindowRect.left,
    WindowRect.bottom - WindowRect.top,
    SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
  SetWindowPos(hWnd, HWND_TOP,
    0, 0,CLIENT_WIDTH, CLIENT_HEIGHT,
    SWP_NOZORDER);
  UpdateClientWindow();
  mouse_scale = 1.f;
}

void ClientMenu::SetWindowedBorderless()
{
  SetWindowLongPtr(containerhWnd, GWL_STYLE, 0);
  SetWindowLongPtr(containerhWnd, GWL_EXSTYLE, WS_EX_TOPMOST);
  SetWindowPos(containerhWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
  ShowWindow(containerhWnd, SW_SHOWMAXIMIZED);
  SetWindowPos(hWnd, HWND_TOP,
    ClientFullscreenRect.left, ClientFullscreenRect.top,
    ClientFullscreenRect.right, ClientFullscreenRect.bottom,
    SWP_NOZORDER);

  UpdateClientWindow();
  //GetWindowRect(hWnd, &FullscreenWindowRect);
  mouse_scale = fs_scale_factor_rel;
}

void ClientMenu::SetClientFocus(bool value)
{
  focused = value;
  //SetCursor(NULL);
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

void LeftClickMainMenu()
{
  for (Button* button : mainMenu.GetButtons())
    if(button->Click())
      break;
}

void LeftClickLevelEditor()
{
  bool clicked = false;
  for (Button* button : levelEditor.primaryButtons)
    if (clicked = button->Click())
      break;
  if (clicked) return;

  for (unsigned int i = levelEditor.BrickIndexStart();
    i < levelEditor.BrickIndexEnd(); ++i)
  {
    if (clicked = levelEditor.brickButtons.at(i)->Click())
      break;
  }

  if (clicked) return;
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

void RightClickLevelEditor()
{
  POINT p = GameController::GetInstance()->mousePos;
  unsigned x = p.x / BRICK_WIDTH;
  unsigned y = p.y / BRICK_HEIGHT;
  if (IsInGameSceen(x, y))
  {
    uint32_t index = GetBrickIndex(x, y);
    levelEditor.editorLevel.GetBrickMap().Erase(index,
      PB2_BRICKMAP_ERASE_TOP | PB2_BRICKMAP_ERASE_ANY);
  }
}

void ClientMenu::LeftClickLevel()
{
  switch (GameController::GetInstance()->GetLevelState())
  {
    case LevelState::START:
      GameController::GetInstance()->Play();
    break;
    case LevelState::PAUSED:
      SetClientFocus(true);
      break;
    case LevelState::END:
      GameController::GetInstance()->NextLevel();
      break;
  }
}

void ClientMenu::RightClickLevel()
{
  switch (GameController::GetInstance()->GetLevelState())
  {
  case LevelState::ACTIVE:
    GameController::GetInstance()->Pause();
    SetClientFocus(false);
    break;
  case LevelState::PAUSED:
    //TODO if attempting to resume and mouse gets adjusted due to clip
    //the bat jumps
    SetClientFocus(true);
    Bat* bat = GameController::GetInstance()->GetBat();
    int x = bat->x + (BAT_WIDTH / 2);
    int y = bat->y;
    mouse_pos.x = GameClipRect.left + (x * fs_scale_factor);
    mouse_pos.y = GameClipRect.top + (y * fs_scale_factor);
    GameController::GetInstance()->MouseUpdate(mouse_pos);
    SetCursorPos(mouse_pos.x, mouse_pos.y);
    GameController::GetInstance()->Play();
    break;
  }
}

void ClientMenu::ProcessWM_CHAR(WPARAM wParam)
{
  if (!IsCharAlphaNumeric(wParam)) return;
  GameType game_type = GameController::GetInstance()->GetGameType();
  if (game_type == GameType::GAME_EDITOR)
  {
    if (levelEditor.buttonTextSelect == nullptr)
      return;
    levelEditor.buttonTextSelect->text.AddChar(wParam);
  }
  else if (game_type == GameType::GAME_NORMAL)
  {
    if (GameController::GetInstance()->GetLevelState() == LevelState::HIGHSCORE)
    {
      GameController::GetInstance()->GetHighscoreText().AddChar(wParam);
    }
  }
}

void ClientMenu::ProcessWM_KEYDOWN(WPARAM wParam)
{
  if (wParam == VK_F11)
  {
    GameController::GetInstance()->Pause();
    fullscreen ^= true;
    if (fullscreen)
    {
      SetWindowedBorderless();
    }
    else
    {
      SetWindowed();
    }
  }
  if (wParam == VK_ESCAPE)
  {
    GameController::GetInstance()->EndGame();
    SetClientFocus(false);
  }
  GameType game_type = GameController::GetInstance()->GetGameType();
  if (game_type == GameType::GAME_EDITOR)
  {
    if (levelEditor.buttonTextSelect == nullptr)
      return;
    if (wParam == VK_BACK)
      levelEditor.buttonTextSelect->text.DeleteChar();
    else if (wParam == VK_RETURN)
      levelEditor.ClearSelected();
  }
  else if (game_type == GameType::GAME_NORMAL)
  {
    if (GameController::GetInstance()->GetLevelState() == LevelState::HIGHSCORE)
    {
      if (wParam == VK_BACK)
        GameController::GetInstance()->GetHighscoreText().DeleteChar();
      else if (wParam == VK_RETURN)
      { //Seems dumb, but for now it allows the highscore text to be accessed
        if (!GameController::GetInstance()->GetHighscoreText().GetString().empty())
        {
          GameController::GetInstance()->SetHighscoreName(
            GameController::GetInstance()->GetHighscoreText().GetString());
          GameController::GetInstance()->GetHighscoreText().Clear();
          GameLoader::SaveCampaign(GameController::GetInstance()->campaign);
          SetClientFocus(false);
        }
      }
    }
  }
}

LRESULT CALLBACK ContainerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  ClientMenu* pWnd = (ClientMenu*)GetWindowLongPtr(hWnd, GWL_USERDATA);

  switch (message)
  {
  case WM_NCCREATE:
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
    break;
  case WM_WINDOWPOSCHANGED:
    pWnd->UpdateClientWindow();
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  case WM_KEYDOWN:
    pWnd->ProcessWM_KEYDOWN(wParam);
    break;
  case WM_CHAR:
    pWnd->ProcessWM_CHAR(wParam);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return pWnd == nullptr ? DefWindowProc(hWnd, message, wParam, lParam) : 0;
}

LRESULT CALLBACK ClientWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  ClientMenu* pWnd = (ClientMenu*)GetWindowLongPtr(hWnd, GWL_USERDATA);

  switch (message)
  {
  case WM_NCCREATE:
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
    break;
  case WM_SETCURSOR: //Patches an unsolved edge case with ShowCursor(FALSE)
    pWnd->SetClientFocus(focused);
    break;

  case WM_PAINT:
  {
    switch (GameController::GetInstance()->GetGameType())
    {
    case GameType::GAME_EDITOR:
      DrawEditor(pWnd, levelEditor);
      break;
    case GameType::MAIN_MENU:
      DrawMainMenu(pWnd, mainMenu);
      break;
    default:
      DrawGame(pWnd);
    }
  }
  break;
  case WM_LBUTTONDOWN:
  {
    switch (GameController::GetInstance()->GetGameType())
    {
    case GameType::GAME_EDITOR:
      LeftClickLevelEditor();
      break;
    case GameType::GAME_NORMAL:
      pWnd->LeftClickLevel();
      break;
    case GameType::MAIN_MENU:
      LeftClickMainMenu();
      break;
    }
  }
  break;
  case WM_RBUTTONDOWN:
  {
    switch (GameController::GetInstance()->GetGameType())
    {
    case GameType::GAME_EDITOR:
      RightClickLevelEditor();
      break;
    case GameType::GAME_NORMAL:
      pWnd->RightClickLevel();
      break;
    }
  }
  break;
  case WM_MOUSEMOVE:
  {
    mouse_pos.x = GET_X_LPARAM(lParam);
    mouse_pos.y = GET_Y_LPARAM(lParam);
    mouse_pos.x *= mouse_scale;
    mouse_pos.y *= mouse_scale;
    GameController::GetInstance()->MouseUpdate(mouse_pos);
    switch(GameController::GetInstance()->GetGameType())
    {
    case GameType::GAME_EDITOR:
      for (Button* button : levelEditor.primaryButtons)
        button->Update();
      for (unsigned int i = levelEditor.BrickIndexStart();
        i < levelEditor.BrickIndexEnd(); ++i)
      {
        levelEditor.brickButtons.at(i)->Update();
      }
      break;
    case GameType::MAIN_MENU:
      for (Button* button : mainMenu.GetButtons())
        button->Update();
      break;
    }
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

  WNDCLASSEX BaseClass = { 0 };
  BaseClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  BaseClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  BaseClass.cbSize = sizeof(WNDCLASSEX);
  BaseClass.hInstance = hInstance;
  BaseClass.style = CS_HREDRAW | CS_VREDRAW;

  WNDCLASSEX MainClass = BaseClass;
  MainClass.lpfnWndProc = ContainerWndProc;
  MainClass.lpszClassName = "PB2ContainerClass";
  RegisterClassEx(&MainClass);
  
  WNDCLASSEX MainClass2 = BaseClass;
  MainClass2.lpfnWndProc = ClientWndProc;
  MainClass2.lpszClassName = "PB2ClientClass";
  RegisterClassEx(&MainClass2);
  
  containerhWnd = CreateWindowEx(
    grfExStyle,
    MainClass.lpszClassName, "Planet Breakout 2",
    grfStyle, 0, 0,
    WindowRect.right - WindowRect.left,
    WindowRect.bottom - WindowRect.top,
    NULL, NULL, hInstance, this);

  hWnd = CreateWindow(
    MainClass2.lpszClassName,
    "", WS_VISIBLE | WS_CHILD,
    0, 0,
    CLIENT_WIDTH,
    CLIENT_HEIGHT,
    containerhWnd, NULL, hInstance, this);

  const HWND hDesktop = GetDesktopWindow();
  GetWindowRect(hDesktop, &desktop);

  float scale_factor = 1.f;
  if (desktop.bottom <= desktop.right)
    scale_factor = (float)desktop.bottom / CLIENT_HEIGHT;
  else
    scale_factor = (float)desktop.right / CLIENT_WIDTH;

  float modifier = std::fmodf(scale_factor, 0.1f);
  fs_scale_factor = scale_factor - modifier;
  fs_scale_factor_rel = 1.f / fs_scale_factor;

  ClientFullscreenRect.right = CLIENT_WIDTH * fs_scale_factor;
  ClientFullscreenRect.bottom = CLIENT_HEIGHT * fs_scale_factor;
  ClientFullscreenRect.top = (desktop.bottom - ClientFullscreenRect.bottom) / 2;
  ClientFullscreenRect.left = (desktop.right - ClientFullscreenRect.right) / 2;
  ClientFullscreenGameRect.left = ClientFullscreenRect.left;
  ClientFullscreenGameRect.top = ClientFullscreenRect.top;
  ClientFullscreenGameRect.bottom = ClientFullscreenGameRect.top + (GAME_HEIGHT * fs_scale_factor);
  ClientFullscreenGameRect.right = ClientFullscreenGameRect.left + (GAME_WIDTH * fs_scale_factor);
}