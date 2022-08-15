#include "MainMenu.h"
#include "GameController.h"

std::vector<Button*>& MainMenu::GetButtons()
{
  return buttons[state];
}
void MainMenu::initialize()
{

  {
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, (CLIENT_HEIGHT / 2) - 150, 300, 30);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.d2d1Rect, L"Play");
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    button0->action = [this]() { 
      GameController::GetInstance()->SetGameType(GameType::GAME_EDITOR);
    };
    buttons[MainMenuState::MAIN].push_back(button0);
  }

  {
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, (CLIENT_HEIGHT / 2) - 75 - 20, 300, 30);
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
    Drawable button0draw((CLIENT_WIDTH / 2) - 150, (CLIENT_HEIGHT / 2) + 0 - 40, 300, 30);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.d2d1Rect, L"Highscores");
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    button0->action = [this]() {
      state = MainMenuState::HIGHSCORE;
    };
    buttons[MainMenuState::MAIN].push_back(button0);
  }

  {
    Drawable button0draw((CLIENT_WIDTH / 2) - 100, CLIENT_HEIGHT - 200, 200, 30);
    Button* button0 = new Button(button0draw);
    Text button0Text(button0draw.d2d1Rect, L"Back");
    button0Text.FormatText(TextFormat::CENTER_24F, ColorBrush::WHITE);
    button0->SetText(button0Text);
    button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    button0->action = [this]() {
      state = MainMenuState::MAIN;
    };
    buttons[MainMenuState::HIGHSCORE].push_back(button0);
    buttons[MainMenuState::INFO].push_back(button0);
  }


}