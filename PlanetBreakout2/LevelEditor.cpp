#include "LevelEditor.h"
#include "GameLoader.h"
#include "GameController.h"
#include "Client.h"

Button* CreateButton(size_t index)
{
  unsigned buttonOnPage = index % EDITOR_BUTTONS_PER_PAGE;
  unsigned x = GAME_WIDTH + 10;
  unsigned y = 305 + (buttonOnPage * (EDITOR_BUTTON_HEIGHT + 15));
  if (buttonOnPage > (EDITOR_BUTTONS_PER_PAGE / 2) - 1)
  {
    x += 125;
    y -= (((EDITOR_BUTTONS_PER_PAGE / 2) + 0) * (EDITOR_BUTTON_HEIGHT + 15));
  }
  Drawable button1draw(x, y, EDITOR_BUTTON_WIDTH, EDITOR_BUTTON_HEIGHT);
  Button* button = new Button(button1draw);
  button->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 2.f);
  button->SetButtonHighlightType(ButtonHighlightType::BORDER,
    ResourceLoader::GetBrush(ColorBrush::GREEN));
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
    buttonTextSelect->SetSelected(false);
    buttonTextSelect = nullptr;
  }
  editorLevel.author = buttonAuthorName->text.GetString();
  editorLevel.map_name = buttonMapName->text.GetString();
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

void LevelEditor::initialize(Client* client)
{

  D2D1_GRADIENT_STOP stops[] =
  {
        { 0.0f, D2D1::ColorF(0x525557) },
        { 1.0f, D2D1::ColorF(0x323436) }
  };
  ID2D1GradientStopCollection* collection = nullptr;
  ResourceLoader::GetHwndRenderTarget()->CreateGradientStopCollection(
    stops, _countof(stops), &collection);
  D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES props = {};
  if (collection == nullptr)
    return;

  {
    Drawable button0draw(GAME_WIDTH + 20, 20 + (14 + 22) * 3, 100, 24);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"New");
    button0Text.AlignCenter();
    button0->SetText(button0Text);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 1.f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->onClick = [this]() {
      editorLevel.clear();
      buttonMapName->text.Clear();
      buttonAuthorName->text.Clear();
    };
    primaryButtons.push_back(button0);
  }

  {
    Drawable button0draw(GAME_WIDTH + 110 + 20, 20 + (14 + 22) * 3, 100, 24);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"Save");
    button0Text.AlignCenter();
    button0->SetText(button0Text);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 1.f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->onClick = [this]() {
      ClearSelected();
      std::wstring mapname;
      if (GameLoader::SaveMap(editorLevel, mapname))
      {
        buttonMapName->text.Update(mapname);
      }
    };
    primaryButtons.push_back(button0);
  }

  {
    Drawable button0draw(GAME_WIDTH + 20, 20 + (14 + 22) * 4, 100, 24);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"Load");
    button0Text.AlignCenter();
    button0->SetText(button0Text);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 1.f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->onClick = [this]() {
      std::wstring loadmap;
      if (ResourceLoader::GetFile(loadmap))
      {
        if (GameLoader::LoadMap(loadmap, editorLevel))
        {
          buttonMapName->text.Update(editorLevel.map_name);
          buttonAuthorName->text.Update(editorLevel.author);
        }
      }
    };
    primaryButtons.push_back(button0);
  }

  {
    Drawable button0draw(GAME_WIDTH + 110 + 20, 20 + (14 + 22) * 4, 100, 24);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"Toggle Grid");
    button0Text.AlignCenter();
    button0->SetText(button0Text);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 1.f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->onClick = [this]() { showGrid ^= true; };
    primaryButtons.push_back(button0);
  }


  {
    Drawable button0draw(GAME_WIDTH + 20, 20 + (14 + 22) * 1, 210, 24);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"Main Menu");
    button0Text.AlignCenter();
    button0->SetText(button0Text);
    buttonMapName = button0;
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 1.f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->onClick = [this, button0]() {
      GameController::GetInstance()->SetGameType(GameType::MAIN_MENU);
    };
    primaryButtons.push_back(button0);
  }

  {
    Drawable button0draw(GAME_WIDTH + 20, 20 + (14 + 22) * 2, 210, 24);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"Test Level");
    button0Text.AlignCenter();
    button0->SetText(button0Text);
    buttonMapName = button0;
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 1.f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->onClick = [this, button0, client]() {
      Campaign* campaign = new Campaign();
      campaign->SetTestMode();
      GameLevel level = editorLevel;
      if (level.author.empty())
        level.author = L"Unknown";
      if (level.map_name.empty())
        level.map_name = L"Unknown";
      campaign->levels.push_back(level);
      GameController::GetInstance()->CreateGame(*campaign);
      client->SetClientFocus(true);
    };
    primaryButtons.push_back(button0);
  }

  {
    Drawable button0draw(GAME_WIDTH + 20, 20 + (14 + 22) * 5, 210, 24);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"");
    button0Text.AlignCenter();
    button0->SetPriority(MAP_NAME);
    button0->SetText(button0Text);
    buttonMapName = button0;
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 2.f);
    button0->SetButtonHighlightType(ButtonHighlightType::BORDER,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button0->onClick = [this, button0]() {
      if (buttonTextSelect == button0)
      {
        buttonTextSelect->SetSelected(false);
        buttonTextSelect = nullptr;
      }
      else
      {
        if (buttonTextSelect != nullptr)
          buttonTextSelect->SetSelected(false);
        buttonTextSelect = button0;
        buttonTextSelect->SetSelected(true);
      }
    };
    primaryButtons.push_back(button0);
  }

  {
    Drawable button0draw(GAME_WIDTH + 20, 20 + (14 + 22) * 6, 210, 24);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"");
    button0Text.AlignCenter();
    button0->SetPriority(AUTHOR_NAME);
    button0->SetText(button0Text);
    buttonAuthorName = button0;
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 2.f);
    button0->SetButtonHighlightType(ButtonHighlightType::BORDER,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button0->onClick = [this, button0]() {
      if (buttonTextSelect == button0)
      {
        buttonTextSelect->SetSelected(false);
        buttonTextSelect = nullptr;
      }
      else
      {
        if (buttonTextSelect != nullptr)
          buttonTextSelect->SetSelected(false);
        buttonTextSelect = button0;
        buttonTextSelect->SetSelected(true);
      }
    };
    primaryButtons.push_back(button0);
  }


  {
    Drawable button0draw(GAME_WIDTH + 22, 22 + (14 + 22) * 7, 16, 16);
    Button* button0 = new Button(button0draw);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 2.f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button0->onClick = [this, button0]() {
      invincibleBrick ^= true;
      button0->SetSelected(invincibleBrick);
    };
    primaryButtons.push_back(button0);
  }

  {
    Drawable button0draw(GAME_WIDTH + 10, 710, 100, 24);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"Previous");
    button0Text.AlignCenter();
    button0->SetText(button0Text);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 1.f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->onClick = [this]() {
      if (currentBrickPage > 0)
        currentBrickPage--;
    };
    primaryButtons.push_back(button0);
  }

  {
    Drawable button0draw(CLIENT_WIDTH - 110, 710, 100, 24);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"Next");
    button0Text.AlignCenter();
    button0->SetText(button0Text);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 1.f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->onClick = [this]() {
      if (currentBrickPage < brickPages)
        currentBrickPage++;
    };
    primaryButtons.push_back(button0);
  }

  unsigned brickCount = ResourceLoader::GetBrickSprites().size();
  if(brickCount > 0)
    brickPages = (brickCount - 1) / EDITOR_BUTTONS_PER_PAGE;
  for (size_t i = 0; i < brickCount; ++i)
  {
    std::wstring brickSprite = ResourceLoader::GetBrickSprites().at(i);
    Button* button1 = CreateButton(i);

    Icon button1icon(D2D1::RectF(
      (FLOAT)button1->x,
      (FLOAT)button1->y,
      (FLOAT)button1->x + BRICK_WIDTH,
      (FLOAT)button1->y + BRICK_HEIGHT),
      brickSprite);
    button1icon.CenterX(button1->GetD2D1Rect());
    button1icon.CenterY(button1->GetD2D1Rect());
    //button1icon.AlignIcon(0.f, 1.0f);
    button1->SetIcon(button1icon);
    button1->onClick = [this](void) {
      for (size_t i = 0; i < brickButtons.size(); ++i)
      {
        if (brickButtons.at(i)->IsHighlighted())
        {
          currentBrick = ResourceLoader::GetBrickSprites().at(i);
          brickButtons.at(i)->SetSelected(true);
        }
        else
        {
          brickButtons.at(i)->SetSelected(false);
        }
      }
    };
    brickButtons.push_back(button1);
  }
}