#include "Menu.h"
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
}

void ClientMenu::PostInitialize()
{
  levelEditor.initialize();
  initialized = true;
}

inline void DrawButton(ID2D1HwndRenderTarget* target, Button* button)
{
  if (button->type == ButtonType::PRIMITIVE)
  {
    if (button->highlighted)
      target->FillRectangle(button->d2d1Rect, button->primitiveFill);
    if (button->selected)
      target->DrawRectangle(button->d2d1Rect, button->primitiveFill, button->primitiveStroke);
    else
      target->DrawRectangle(button->d2d1Rect, button->primitiveOutline, button->primitiveStroke);

    if (button->hasText)
    {
      std::wstring text = button->text.text;
      if (text.empty())
      {
        if (button->id == LevelEditorButton::AUTHOR_NAME)
          text = L"Author Name";
        else if (button->id == LevelEditorButton::MAP_NAME)
          text = L"Map Name";
      }
      target->DrawText(
        text.c_str(),
        text.length(),
        button->text.textFormat,
        button->text.textRect,
        button->text.textBrush);
    }

    if (button->hasIcon)
      target->DrawBitmap(button->icon.iconBitmap, button->icon.iconRect, 1.0f);
  }
}

inline void DrawEditor(ClientMenu* menu)
{
  ID2D1HwndRenderTarget* target = ResourceLoader::GetHwndRenderTarget();
  IDWriteTextFormat* format = ResourceLoader::GetTextFormat(TextFormat::LEFT_12F);
  ID2D1SolidColorBrush* brushes = ResourceLoader::GetBrush(ColorBrush::GRAY);
  ID2D1SolidColorBrush* greenBrush = ResourceLoader::GetBrush(ColorBrush::GREEN);
  ID2D1SolidColorBrush* blackBrush = ResourceLoader::GetBrush(ColorBrush::BLACK);
  target->BeginDraw();
  target->SetTransform(D2D1::Matrix3x2F::Identity());
  target->Clear();
  std::wstring text = L"Planet Breakout 2 - Map Editor";
  target->DrawText(text.c_str(), text.length(), format, D2D1::RectF(800, 10, 800 + 300, 25 + 20), brushes);
  target->DrawLine(D2D1::Point2F(GAME_WIDTH, 0.f), D2D1::Point2F(GAME_WIDTH, GAME_HEIGHT), brushes);
  GameLevel level = levelEditor.editorLevel;
  BrickMap::iterator map_it = level.brickMap.begin();
  for (; map_it != level.brickMap.end(); ++map_it)
  {
    if (map_it->second.empty()) continue;
    for (Brick brick : map_it->second)
    {
      D2D1_RECT_F rect1 = brick.d2d1Rect;
      rect1.right += 50;
      rect1.bottom += 50;
      target->DrawBitmap(ResourceLoader::GetSpriteMap().at(brick.sprite), brick.d2d1Rect, 1.0f);
    }
    if (levelEditor.showGrid)
    {
      std::wstring count = std::to_wstring(map_it->second.size());
      target->DrawTextA(count.c_str(), count.length(),
        ResourceLoader::GetTextFormat(TextFormat::LEFT_12F),
        map_it->second.at(0).d2d1Rect, blackBrush);
      target->DrawTextA(count.c_str(), count.length(),
        ResourceLoader::GetTextFormat(TextFormat::LEFT_12F),
        AdjustRect(map_it->second.at(0).d2d1Rect, 1.f, 1.f), greenBrush);
    }
  }


  Brick* currentBrick = levelEditor.currentBrick;
  if (currentBrick != nullptr)
  {
    POINT p = GameController::GetInstance()->mousePos;
    unsigned x = p.x / BRICK_WIDTH;
    unsigned y = p.y / BRICK_HEIGHT;
    if (IsInGameSceen(x, y))
    {
      if (IsReservedBrick(x, y))
      {
        target->FillRectangle(GetBrickRect(x, y), ResourceLoader::GetBrush(ColorBrush::RED_HALF));
      }
      else
      {
        target->DrawBitmap(ResourceLoader::GetSpriteMap().at(currentBrick->sprite),
          GetBrickRect(x, y), 0.5f);
      }
    }
  }

  for (Button* button : levelEditor.primaryButtons)
  {
    DrawButton(target, button);
  }


  unsigned start = levelEditor.BrickIndexStart();
  unsigned end = levelEditor.BrickIndexEnd();
  for (unsigned i = start; i < end; ++i)
  {
    DrawButton(target, levelEditor.brickButtons.at(i));
  }
  std::wstring text_page = L"Page ";
  text_page.append(std::to_wstring(levelEditor.currentBrickPage + 1));
  text_page.append(L" / ");
  text_page.append(std::to_wstring(levelEditor.brickPages + 1));
  //GAME_WIDTH + 10, 720, 100, 14
  target->DrawText(text_page.c_str(), text_page.length(), format,
    D2D1::RectF(GAME_WIDTH + 8, CLIENT_HEIGHT - 20, CLIENT_WIDTH - 4, CLIENT_HEIGHT - 1), brushes);


  if (levelEditor.showGrid)
  {
    for (unsigned x = 0; x < GAME_WIDTH; x += BRICK_WIDTH)
    {
      target->DrawLine(
        D2D1::Point2F(x, 0.f), D2D1::Point2F(x, GAME_HEIGHT),
        brushes);
    }

    for (unsigned y = 0; y < GAME_HEIGHT; y += BRICK_HEIGHT)
    {
      target->DrawLine(
        D2D1::Point2F(0.f, y), D2D1::Point2F(GAME_WIDTH, y),
        brushes);
    }
  }

  //POINT pos = GameController::GetInstance()->mousePos;
  //target->FillRectangle(D2D1::RectF(pos.x - 1, pos.y - 1, pos.x + 3, pos.y + 3),
  //  ResourceLoader::GetBrush(ColorBrush::GREEN));

  target->EndDraw();
}

inline void DrawGame(ClientMenu* menu)
{

}

LRESULT CALLBACK ClientWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  ClientMenu* pWnd = (ClientMenu*)GetWindowLongPtr(hWnd, GWL_USERDATA);

  switch (message)
  {
  case WM_NCCREATE:
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
    break;
  case WM_PAINT:
  {
    switch (GameController::GetInstance()->type)
    {
    case GAME_EDITOR:
      DrawEditor(pWnd);
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

    for (int i = levelEditor.BrickIndexStart();
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
    POINT p;
    p.x = GET_X_LPARAM(lParam);
    p.y = GET_Y_LPARAM(lParam);
    GameController::GetInstance()->mousePos = p;
    for (Button* button : levelEditor.primaryButtons)
      button->Update();
    for (int i = levelEditor.BrickIndexStart();
      i < levelEditor.BrickIndexEnd(); ++i)
    {
      levelEditor.brickButtons.at(i)->Update();
    }
  }
  break;
  case WM_KEYDOWN:
  {
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
  //MainClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(108));
  //MainClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(108));
  MainClass.hInstance = hInstance;
  MainClass.lpfnWndProc = ClientWndProc;
  MainClass.lpszClassName = "PlanetBreakout2ClientClass";
  MainClass.style = CS_HREDRAW | CS_VREDRAW;
  RegisterClassEx(&MainClass);

  hWnd = CreateWindowEx(
    grfExStyle,
    MainClass.lpszClassName, "Planet Breakout 2",
    grfStyle,
    0, 0, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, NULL, NULL, hInstance, this);
}