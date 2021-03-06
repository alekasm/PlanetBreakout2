#include "LevelEditor.h"
#include "GameLoader.h"
#include "GameController.h"

Button* CreateButton(size_t index)
{
  unsigned buttonOnPage = index % EDITOR_BUTTONS_PER_PAGE;
  unsigned x = GAME_WIDTH + 10;
  unsigned y = 225 + (buttonOnPage * (EDITOR_BUTTON_HEIGHT + 15));
  if (buttonOnPage > (EDITOR_BUTTONS_PER_PAGE / 2) - 1)
  {
    x += 125;
    y -= (((EDITOR_BUTTONS_PER_PAGE / 2) + 0) * (EDITOR_BUTTON_HEIGHT + 15));
  }
  Drawable button1draw(x, y, EDITOR_BUTTON_WIDTH, EDITOR_BUTTON_HEIGHT);
  Button* button = new Button(button1draw);
  button->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
  return button;
}

LevelEditor::~LevelEditor()
{
  for (Button* brickButton : brickButtons)
    delete brickButton;
  for (Button* bgButton : bgButtons)
    delete bgButton;
  for (Button* primaryButton : primaryButtons)
    delete primaryButton;
  brickButtons.clear();
  bgButtons.clear();
  primaryButtons.clear();
}

const unsigned LevelEditor::BrickIndexStart()
{
  return currentBrickPage * EDITOR_BUTTONS_PER_PAGE;
}

void LevelEditor::ClearSelected()
{
  if (buttonTextSelect != nullptr)
  {
    buttonTextSelect->selected = false;
    buttonTextSelect = nullptr;
  }
  editorLevel.author = buttonAuthorName->text.text;
  editorLevel.map_name = buttonMapName->text.text;
}

const unsigned LevelEditor::BrickIndexEnd()
{
  unsigned end = BrickIndexStart();
  unsigned remaining = brickButtons.size() - end;
  if (remaining < EDITOR_BUTTONS_PER_PAGE)
    end += remaining;
  else
    end += EDITOR_BUTTONS_PER_PAGE;
  return end;
}

void LevelEditor::initialize()
{
  {
    Drawable button0draw(GAME_WIDTH + 110 + 20, 50 + (14 + 10), 100, 14);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.d2d1Rect, L"Toggle Grid");
    button0->SetText(button0Text);
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    button0->action = [this]() { showGrid ^= true; };
    primaryButtons.push_back(button0);
  }

  {
    Drawable button0draw(GAME_WIDTH + 110 + 20, 50, 100, 14);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.d2d1Rect, L"Save");
    button0->SetText(button0Text);
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    button0->action = [this]() {
      editorLevel.validate();
      ClearSelected();
      std::wstring mapname;
      if (GameLoader::SaveMap(editorLevel, mapname))
      {
        buttonMapName->text.text = mapname;
      }
    };
    primaryButtons.push_back(button0);
  }

  {
    Drawable button0draw(GAME_WIDTH + 20, 50 + (14 + 10), 100, 14);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.d2d1Rect, L"Load");
    button0->SetText(button0Text);
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    button0->action = [this]() {
      std::wstring loadmap;
      if (ResourceLoader::GetFile(loadmap))
      {
        if (GameLoader::LoadMap(loadmap, editorLevel))
        {
          editorLevel.validate();
          buttonMapName->text.text = editorLevel.map_name;
          buttonAuthorName->text.text = editorLevel.author;
        }
      }
    };
    primaryButtons.push_back(button0);
  }

  {
    Drawable button0draw(GAME_WIDTH + 20, 50, 100, 14);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.d2d1Rect, L"New");
    button0->SetText(button0Text);
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    button0->action = [this]() {
      editorLevel.clear();
    };
    primaryButtons.push_back(button0);
  }

  {
    Drawable button0draw(GAME_WIDTH + 20, 20 + (14 + 22) * 3, 210, 22);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.d2d1Rect, L"");
    button0->id = MAP_NAME;
    button0->SetText(button0Text);
    buttonMapName = button0;
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    button0->action = [this, button0]() {
      if (buttonTextSelect == button0)
      {
        buttonTextSelect->selected = false;
        buttonTextSelect = nullptr;
      }
      else
      {
        if (buttonTextSelect != nullptr)
          buttonTextSelect->selected = false;
        buttonTextSelect = button0;
        buttonTextSelect->selected = true;
      }
    };
    primaryButtons.push_back(button0);
  }

  {
    Drawable button0draw(GAME_WIDTH + 20, 20 + (14 + 22) * 4, 210, 22);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.d2d1Rect, L"");
    button0->id = AUTHOR_NAME;
    button0->SetText(button0Text);
    buttonAuthorName = button0;
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    button0->action = [this, button0]() {
      if (buttonTextSelect == button0)
      {
        buttonTextSelect->selected = false;
        buttonTextSelect = nullptr;
      }
      else
      {
        if (buttonTextSelect != nullptr)
          buttonTextSelect->selected = false;
        buttonTextSelect = button0;
        buttonTextSelect->selected = true;
      }
    };
    primaryButtons.push_back(button0);
  }

  {
    Drawable button0draw(GAME_WIDTH + 10, 720, 100, 14);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.d2d1Rect, L"Previous");
    button0->SetText(button0Text);
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    button0->action = [this]() {
      if (currentBrickPage > 0)
        currentBrickPage--;
    };
    primaryButtons.push_back(button0);
  }

  {
    Drawable button0draw(CLIENT_WIDTH - 110, 720, 100, 14);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.d2d1Rect, L"Next");
    button0->SetText(button0Text);
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    button0->action = [this]() {
      if (currentBrickPage < brickPages)
        currentBrickPage++;
    };
    primaryButtons.push_back(button0);
  }

  unsigned brickCount = GameLoader::GetAssetBricks().size();
  if(brickCount > 0)
    brickPages = (brickCount - 1) / EDITOR_BUTTONS_PER_PAGE;
  for (size_t i = 0; i < brickCount; ++i)
  {
    Brick brick = GameLoader::GetAssetBricks().at(i);
    Button* button1 = CreateButton(i);

    std::wstring desc = brick.subtype ==
      BrickType::NORMAL_BRICK ?
      L"Breakable" : L"Invincible";

    Text button1Text(button1->d2d1Rect, desc);
    button1Text.AlignText(0.f, BRICK_HEIGHT + 2);
    button1->SetText(button1Text);

    Icon button1icon(D2D1::RectF(
      (FLOAT)button1->x,
      (FLOAT)button1->y,
      (FLOAT)button1->x + BRICK_WIDTH,
      (FLOAT)button1->y + BRICK_HEIGHT),
      brick.sprite);
    button1icon.CenterX(button1->d2d1Rect);
    button1icon.AlignIcon(0.f, 1.0f);
    button1->SetIcon(button1icon);
    button1->action = [this](void) {
      for (size_t i = 0; i < brickButtons.size(); ++i)
      {
        if (brickButtons.at(i)->highlighted)
        {
          currentBrick = &GameLoader::GetAssetBricks().at(i);
          brickButtons.at(i)->selected = true;
        }
        else
        {
          brickButtons.at(i)->selected = false;
        }
      }
    };
    brickButtons.push_back(button1);
  }
}