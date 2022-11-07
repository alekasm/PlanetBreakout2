#include "MainMenu.h"
#include "GameController.h"
#include "GameLoader.h"
#include "Client.h"
#define _USE_MATH_DEFINES
#include <math.h>

std::vector<Button*>& MainMenu::GetButtons()
{
  return buttons[state];
}

MainMenuState MainMenu::GetState()
{
  return state;
}

uint32_t MainMenu::GetCampaignPage()
{
  return campaign_page;
}

const std::wstring& MainMenu::GetDescription(PowerupType type) const
{
  return powerupDescriptions.at(type);
}

const std::wstring& MainMenu::GetVersion() const
{
  return versionString;
}

const std::wstring& MainMenu::GetCurrentCampaignNameSelection() const
{
  return currentCampaignNameSelected;
}

const std::wstring& MainMenu::GetCurrentDifficultySelection() const
{
  return currentDifficultySelected;
}

void MainMenu::RefreshFullscreenButton(Client* client)
{
  if (fullscreenButton == nullptr)
    return;
  if (client->IsFullScreen())
    fullscreenButton->text.Update(L"Windowed");
  else
    fullscreenButton->text.Update(L"Fullscreen");
}

PowerupType MainMenu::GetPowerUpSelection()
{
  return currentPowerupSelected;
}

void MainMenu::SwitchState(MainMenuState newState)
{
  for (Button* b : buttons.at(state))
  {
    b->SetHighlighted(false);
  }
  state = newState;
}

void MainMenu::initialize(Client* client)
{
  powerupDescriptions = {
   {PowerupType::HYPER_BALL, L"Hyper Energy: Balls and lasers destroy everything in their path"},
   {PowerupType::CREATOR_BALL, L"Creator Ball: Each ball will create another ball every few seconds"},
   {PowerupType::LASER_BAT, L"Laser Bat: Left-Click to shoot lasers"},
   {PowerupType::BONUS_POINTS, L"Rare Gems: Increases the point multiplier"},
   {PowerupType::BARRIER, L"Barrier: Creates an energy barrier under your bat for protection"},
   {PowerupType::EXTRA_LIFE, L"Extra Life: You will not lose a life if all the balls are lost"},
   {PowerupType::GHOST, L"Ghost: Space ghosts possess the balls, making them bounce randomly off the bat"},
   {PowerupType::BRICK_SHIELD, L"Brick Shield: All bricks gain a plasma shield, they don't give points"},
   {PowerupType::PORTAL, L"Time Portal: All balls travel back in time to the start position"},
   {PowerupType::DRONE, L"Drone: Transforms balls into drones that shoot lasers, which can also kill you"},
   {PowerupType::STRIKE, L"Brick Strike: Galactic artillery strike that fires at up to 50 bricks"},
   {PowerupType::EMP, L"EMP Bomb: Clears an entire brick shield and barrier"},
  };

  D2D1_GRADIENT_STOP stops[] =
  {
        { 0.0f, D2D1::ColorF(0x140394) },
        { 1.0f, D2D1::ColorF(0xF07012E) }
  };
  ID2D1GradientStopCollection* collection;
  ResourceLoader::GetHwndRenderTarget()->CreateGradientStopCollection(
    stops, _countof(stops), &collection);
  D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES props = {};

  {
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, (CLIENT_HEIGHT / 2) - 100, 300, 30);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"Play");
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::BLUE), 1.5f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button0->onClick = [this]() {
      SwitchState(MainMenuState::CAMPAIGN_SELECT);
    };
    buttons[MainMenuState::MAIN].push_back(button0);
  }

  {
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, (CLIENT_HEIGHT / 2) - 50, 300, 30);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"Info");
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::BLUE), 1.5f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button0->onClick = [this]() {
      SwitchState(MainMenuState::INFO);
    };
    buttons[MainMenuState::MAIN].push_back(button0);
  }

  {
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, (CLIENT_HEIGHT / 2) + 0, 300, 30);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"Fullscreen");
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::BLUE), 1.5f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    fullscreenButton = button0;
    button0->onClick = [this, button0, client]() {
      client->ToggleFullScreen();
      RefreshFullscreenButton(client);
    };
    buttons[MainMenuState::MAIN].push_back(button0);

  }

  {
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, (CLIENT_HEIGHT / 2) + 50, 300, 30);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"Level Editor");
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::BLUE), 1.5f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));;
    button0->onClick = [this, button0]() {
      button0->SetHighlighted(false);
      GameController::GetInstance()->SetGameType(GameType::GAME_EDITOR);
    };
    buttons[MainMenuState::MAIN].push_back(button0);
  }

  {
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, (CLIENT_HEIGHT / 2) + 100, 300, 30);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"Exit");
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::BLUE), 1.5f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button0->onClick = [this]() {
      PostQuitMessage(0);
    };
    buttons[MainMenuState::MAIN].push_back(button0);
  }

  {
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, CLIENT_HEIGHT - 40, 300, 30);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"Back");
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::BLUE), 1.5f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button0->onClick = [this]() {
      SwitchState(MainMenuState::MAIN);
    };
    buttons[MainMenuState::CAMPAIGN_SELECT].push_back(button0);
    buttons[MainMenuState::INFO].push_back(button0);
  }

  const GamePowerUpMap& pwr_map = GameController::GetInstance()->GetGamePowerUpMap();
  GamePowerUpMap::const_iterator pwr_it = pwr_map.begin();
  size_t index = 0;
  for (; pwr_it != pwr_map.end(); ++pwr_it)
  {
    float row = (index / 6);
    float col = (index % 6);

    //(width / 2) - (row + padding / 2) = 343.f
    float x = (343.f + (col * 48.f)) + (col * 12.f);
    float y = (300.f + (row * 48.f)) + (row * 12.f);


    Drawable button0draw(x, y, 48, 48);
    Button* button0 = new Button(button0draw);
    Icon button1icon(D2D1::RectF(
      (FLOAT)button0->x,
      (FLOAT)button0->y,
      (FLOAT)button0->x + 16.f,
      (FLOAT)button0->y + 16.f),
      pwr_it->second.GetIcon());

    button1icon.CenterX(button0->GetD2D1Rect());
    button1icon.CenterY(button0->GetD2D1Rect());
    button0->SetIcon(button1icon);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 2.f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button0->onClick = [this, index]() {
      for (size_t i = 0; i < powerupButtons.size(); ++i)
      {
        if (powerupButtons.at(i)->IsHighlighted())
        {
          const GamePowerUpMap& map = GameController::GetInstance()->GetGamePowerUpMap();
          GamePowerUpMap::const_iterator it = map.cbegin();
          std::advance(it, index);
          currentPowerupSelected = it->first;
          powerupButtons.at(i)->SetSelected(true);
        }
        else
        {
          powerupButtons.at(i)->SetSelected(false);
        }
      }
    };
    if (index == 0)
    {
      button0->SetSelected(true);
      currentPowerupSelected = pwr_it->first;
    }

    ++index;
    powerupButtons.push_back(button0);
    buttons[MainMenuState::INFO].push_back(button0);
  }

  /*
  {
    Drawable button0draw(10, CLIENT_HEIGHT - 60, 42, 42);
    Button* button0 = new Button(button0draw);
    Icon button1icon(D2D1::RectF(
      (FLOAT)button0->x,
      (FLOAT)button0->y,
      (FLOAT)button0->x + 32.f,
      (FLOAT)button0->y + 32.f),
      L"maximize");
    button1icon.CenterX(button0->GetD2D1Rect());
    button1icon.CenterY(button0->GetD2D1Rect());
    button0->SetIcon(button1icon);

    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::BLUE), 1.5f);
    button0->SetButtonHighlightType(ButtonHighlightType::BORDER,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button0->action = [this, button0, client]() {
      client->ToggleFullScreen();
      RefreshFullscreenButton(client);
      if (client->IsFullScreen())
        button0->icon.UpdateBitmap(L"minimize");
      else
        button0->icon.UpdateBitmap(L"maximize");
    };
    buttons[MainMenuState::MAIN].push_back(button0);
  }
  */


  if (!GameLoader::GetCampaigns().empty())
  {
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, (CLIENT_HEIGHT / 2) - 35, 300, 30);
    Button* button0 = new Button(button0draw);
    currentCampaignNameSelected = GameLoader::GetCampaigns().begin()->second.name;
    Text button0Text(button0draw.GetD2D1Rect(), L"Start Mission");
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 2.f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button0->onClick = [this, client]() {
      if (GameLoader::GetCampaigns().size() > campaign_page)
      { //Lots of checks just in case
        client->SetClientFocus(true);
        CampaignMap::iterator it = GameLoader::GetCampaigns().begin();
        std::advance(it, campaign_page);
        GameController::GetInstance()->CreateGame(it->second);
        SwitchState(MainMenuState::MAIN);
        campaign_page = 0;
        //it = GameLoader::GetCampaigns().begin();
        //button0->text.Update(it->second.name);
      }
    };
    buttons[MainMenuState::CAMPAIGN_SELECT].push_back(button0);

    Drawable button1draw((unsigned)button0draw.GetRect().right + 2,
      (CLIENT_HEIGHT / 2) - 160, 30, 30);
    Button* button1 = new Button(button1draw);
    Text button1Text(button1draw.GetD2D1Rect(), L">");
    button1Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button1->SetText(button1Text);

    ID2D1LinearGradientBrush* fillBrusha;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrusha);
    button1->SetButtonFill(fillBrusha);
    button1->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 1.f);
    button1->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));

    button1->onClick = [this]() {
      size_t campaign_count = GameLoader::GetCampaigns().size();
      if (campaign_count == 0) return;
      if (campaign_page + 1 < campaign_count)
        ++campaign_page;
      else
        campaign_page = 0;
      CampaignMap::iterator it = GameLoader::GetCampaigns().begin();
      std::advance(it, campaign_page);
      currentCampaignNameSelected = it->second.name;
      //button0->text.Update(it->second.name);
    };
    buttons[MainMenuState::CAMPAIGN_SELECT].push_back(button1);

    Drawable button2draw((unsigned)button0draw.GetRect().left - 32,
      (CLIENT_HEIGHT / 2) - 160, 30, 30);
    Button* button2 = new Button(button2draw);
    Text button2Text(button2draw.GetD2D1Rect(), L"<");
    button2Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button2->SetText(button2Text);
    ID2D1LinearGradientBrush* fillBrushb;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrushb);
    button2->SetButtonFill(fillBrushb);
    button2->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 1.f);
    button2->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button2->onClick = [this]() {
      size_t campaign_count = GameLoader::GetCampaigns().size();
      if (campaign_count == 0) return;
      if (campaign_page == 0)
        campaign_page = campaign_count - 1;
      else
        --campaign_page;
      CampaignMap::iterator it = GameLoader::GetCampaigns().begin();
      std::advance(it, campaign_page);
      //button0->text.Update(it->second.name);
      currentCampaignNameSelected = it->second.name;
    };
    buttons[MainMenuState::CAMPAIGN_SELECT].push_back(button2);
  }

  {
    currentDifficultySelected = GameController::GetInstance()->GetDifficulty().name;
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, (CLIENT_HEIGHT / 2) - 120, 300, 30);
    Drawable button1draw((unsigned)button0draw.GetRect().right + 2,
      (CLIENT_HEIGHT / 2) - 110, 30, 30);
    Button* button1 = new Button(button1draw);
    Text button1Text(button1draw.GetD2D1Rect(), L">");
    button1Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button1->SetText(button1Text);
    ID2D1LinearGradientBrush* fillBrusha;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrusha);
    button1->SetButtonFill(fillBrusha);
    button1->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 1.f);
    button1->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));

    button1->onClick = [this]() {
      size_t difficulty_count = GameController::GetInstance()->GetDifficultyMap().size();
      if (difficulty_count == 0) return;
      if (difficulty_page + 1 < difficulty_count)
        ++difficulty_page;
      else
        difficulty_page = 0;
      DifficultyMap::const_iterator it =
        GameController::GetInstance()->GetDifficultyMap().begin();
      std::advance(it, difficulty_page);
      currentDifficultySelected = it->second.name;
      GameController::GetInstance()->SetDifficulty(it->first);
    };
    buttons[MainMenuState::CAMPAIGN_SELECT].push_back(button1);

    Drawable button2draw((unsigned)button0draw.GetRect().left - 32,
      (CLIENT_HEIGHT / 2) - 110, 30, 30);
    Button* button2 = new Button(button2draw);
    Text button2Text(button2draw.GetD2D1Rect(), L"<");
    button2Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button2->SetText(button2Text);
    ID2D1LinearGradientBrush* fillBrushb;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrushb);
    button2->SetButtonFill(fillBrushb);
    button2->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 1.f);
    button2->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button2->onClick = [this]() {
      size_t difficulty_count = GameController::GetInstance()->GetDifficultyMap().size();
      if (difficulty_count == 0) return;
      if (difficulty_page == 0)
        difficulty_page = difficulty_count - 1;
      else
        --difficulty_page;
      DifficultyMap::const_iterator it =
        GameController::GetInstance()->GetDifficultyMap().begin();
      std::advance(it, difficulty_page);
      currentDifficultySelected = it->second.name;
      GameController::GetInstance()->SetDifficulty(it->first);
    };
    buttons[MainMenuState::CAMPAIGN_SELECT].push_back(button2);
  }

}