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

const std::wstring& MainMenu::GetInfoDescription() const
{
  return infoDescription;
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

void MainMenu::initialize(Client* client)
{
  powerupDescriptions = {
   {PowerupType::HYPER_BALL, L"Hyper Energy: Balls and lasers destroy everything in their path"},
   {PowerupType::CREATOR_BALL, L"Creator Ball: Each ball will create another ball every few seconds"},
   {PowerupType::LASER_BAT, L"Laser Bat: Left-Click to shoot lasers"},
   {PowerupType::BONUS_POINTS, L"Rare Gems: Increases the point multiplier"},
   {PowerupType::BARRIER, L"Barrier: Creates an energy barrier under your bat for protection"},
   {PowerupType::EXTRA_LIFE, L"Extra Life: You will not lose a life if all the balls are lost"},
   {PowerupType::GHOST, L"Ghost: Space ghosts possess the balls, making them bound randomly off the bat"},
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
    button0->SetButtonHighlightType(ButtonHighlightType::BORDER,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button0->action = [this]() {
      state = MainMenuState::CAMPAIGN_SELECT;
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
    button0->SetButtonHighlightType(ButtonHighlightType::BORDER,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button0->action = [this]() {
      state = MainMenuState::INFO;
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
    button0->SetButtonHighlightType(ButtonHighlightType::BORDER,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    fullscreenButton = button0;
    button0->action = [this, button0, client]() {
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
    button0->SetButtonHighlightType(ButtonHighlightType::BORDER,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button0->action = [this]() {
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
    button0->SetButtonHighlightType(ButtonHighlightType::BORDER,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button0->action = [this]() {
      PostQuitMessage(0);
    };
    buttons[MainMenuState::MAIN].push_back(button0);
  }

  {
    Drawable button0draw(10, CLIENT_HEIGHT - 40, 300, 30);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.GetD2D1Rect(), L"Back");
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 2.f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button0->action = [this]() {
      state = MainMenuState::MAIN;
    };
    buttons[MainMenuState::CAMPAIGN_SELECT].push_back(button0);
    buttons[MainMenuState::INFO].push_back(button0);
  }

  if (!GameLoader::GetCampaigns().empty())
  {
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, (CLIENT_HEIGHT / 2) - 160, 300, 30);
    Button* button0 = new Button(button0draw);
    std::wstring name = GameLoader::GetCampaigns().begin()->second.name;
    Text button0Text(button0draw.GetD2D1Rect(), name);
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    ID2D1LinearGradientBrush* fillBrush;
    ResourceLoader::GetHwndRenderTarget()->CreateLinearGradientBrush(
      props, collection, &fillBrush);
    button0->SetButtonFill(fillBrush);
    button0->SetBorder(ResourceLoader::GetBrush(ColorBrush::GRAY), 2.f);
    button0->SetButtonHighlightType(ButtonHighlightType::FILL,
      ResourceLoader::GetBrush(ColorBrush::GREEN));
    button0->action = [this, client, button0]() {
      if (GameLoader::GetCampaigns().size() > campaign_page)
      { //Lots of checks just in case
        client->SetClientFocus(true);
        CampaignMap::iterator it = GameLoader::GetCampaigns().begin();
        std::advance(it, campaign_page);
        GameController::GetInstance()->CreateGame(it->second);
        state = MainMenuState::MAIN;
        campaign_page = 0;
        it = GameLoader::GetCampaigns().begin();
        button0->text.Update(it->second.name);
      }
    };
    buttons[MainMenuState::CAMPAIGN_SELECT].push_back(button0);

    Drawable button1draw((unsigned)button0draw.GetRect().right + 2,
      (unsigned)button0draw.GetRect().top, 30, 30);
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

    button1->action = [this, button0, button0draw]() {
      size_t campaign_count = GameLoader::GetCampaigns().size();
      if (campaign_count == 0) return;
      if (campaign_page + 1 < campaign_count)
        ++campaign_page;
      else
        campaign_page = 0;
      CampaignMap::iterator it = GameLoader::GetCampaigns().begin();
      std::advance(it, campaign_page);
      button0->text.Update(it->second.name);
    };
    buttons[MainMenuState::CAMPAIGN_SELECT].push_back(button1);

    Drawable button2draw((unsigned)button0draw.GetRect().left - 32,
      (unsigned)button0draw.GetRect().top, 30, 30);
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
    button2->action = [this, button0, button0draw]() {
      size_t campaign_count = GameLoader::GetCampaigns().size();
      if (campaign_count == 0) return;
      if (campaign_page == 0)
        campaign_page = campaign_count - 1;
      else
        --campaign_page;
      CampaignMap::iterator it = GameLoader::GetCampaigns().begin();
      std::advance(it, campaign_page);
      button0->text.Update(it->second.name);
    };
    buttons[MainMenuState::CAMPAIGN_SELECT].push_back(button2);
  }


}