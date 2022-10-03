#include "MainMenu.h"
#include "GameController.h"
#include "GameLoader.h"
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


void MainMenu::initialize(ClientMenu* client)
{
  powerupDescriptions = {
   {PowerupType::HYPER_BALL, L"Hyper Ball: Destroys all bricks in its path, applies to lasers too"},
   {PowerupType::CREATOR_BALL, L"Creator Ball: Each ball will create another ball every few seconds"},
   {PowerupType::LASER_BAT, L"Laser Bat: Left-Click to shoot lasers"},
   {PowerupType::BONUS_POINTS, L"Rare Gems: Increases the point multiplier"},
   {PowerupType::BARRIER, L"Barrier: Creates a barrier under your bat to prevent balls from escaping"},
   {PowerupType::EXTRA_LIFE, L"Extra Life: You will not lose a life if all the balls are lost"},
   {PowerupType::GHOST, L"Ghost: A mean space ghost absorbs the light which radiates from balls"},
   {PowerupType::BRICK_SHIELD, L"Brick Shield: All bricks gain a shield, destroying them doesn't yield any points"},
  };

  {
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, (CLIENT_HEIGHT / 2) - 50, 300, 30);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.d2d1Rect, L"Play");
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    button0->action = [this]() { 
      state = MainMenuState::CAMPAIGN_SELECT;
    };
    buttons[MainMenuState::MAIN].push_back(button0);
  }

  {
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, (CLIENT_HEIGHT / 2) + 0, 300, 30);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.d2d1Rect, L"Info");
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    button0->action = [this]() {
      state = MainMenuState::INFO;
    };
    buttons[MainMenuState::MAIN].push_back(button0);
  }

  {
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, (CLIENT_HEIGHT / 2) + 50, 300, 30);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.d2d1Rect, L"Level Editor");
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    button0->action = [this]() {
      GameController::GetInstance()->SetGameType(GameType::GAME_EDITOR);
    };
    buttons[MainMenuState::MAIN].push_back(button0);
  }

  {
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, (CLIENT_HEIGHT / 2) + 100, 300, 30);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.d2d1Rect, L"Exit");
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    button0->action = [this]() {
      PostQuitMessage(0);
    };
    buttons[MainMenuState::MAIN].push_back(button0);
  }

  {
    Drawable button0draw(10, CLIENT_HEIGHT - 40, 300, 30);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.d2d1Rect, L"Back");
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    button0->action = [this]() {
      state = MainMenuState::MAIN;
    };
    buttons[MainMenuState::CAMPAIGN_SELECT].push_back(button0);
    buttons[MainMenuState::INFO].push_back(button0);
  }

  if(!GameLoader::GetCampaigns().empty())
  {
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, (CLIENT_HEIGHT / 2) - 160, 300, 30);
    Button* button0 = new Button(button0draw);
    std::wstring name = GameLoader::GetCampaigns().begin()->second.name;
    Text button0Text(button0draw.d2d1Rect, name);
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
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

    Drawable button1draw(button0draw.d2d1Rect.right + 2.f, button0draw.d2d1Rect.top, 30.f, 30.f);
    Button* button1 = new Button(button1draw);
    Text button1Text(button1draw.d2d1Rect, L">");
    button1Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button1->SetText(button1Text);
    button1->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
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

    Drawable button2draw(button0draw.d2d1Rect.left - 32.f, button0draw.d2d1Rect.top, 30.f, 30.f);
    Button* button2 = new Button(button2draw);
    Text button2Text(button2draw.d2d1Rect, L"<");
    button2Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button2->SetText(button2Text);
    button2->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
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