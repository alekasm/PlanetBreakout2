#include <vector>
#include <ctime>
#include <thread>
#include <memory>
#include <fcntl.h>
#include <io.h>
#include <cstdio>
#include <string>
#include "ResourceLoader.h"
#include "GameLoader.h"
#include "GameController.h"
#include "Client.h"
//#pragma comment(lib, "xaudio2")

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  CoInitializeEx(nullptr, COINIT_MULTITHREADED);

  AllocConsole();
  FILE* p_file;
  freopen_s(&p_file, "CONIN$", "r", stdin); 
  freopen_s(&p_file, "CONOUT$", "w", stdout);
  freopen_s(&p_file, "CONOUT$", "w", stderr);

  ResourceLoader::Initialize();
  Client* client = new Client();
  client->Initialize(hInstance);
  ResourceLoader::InitializeClient(client->GetHWND());
  if (ResourceLoader::LoadAudioFiles() != S_OK)
  {
    printf("Error loading audio files\n");
  }
  GameLoader::LoadAssets(L"assets/assets.cfg");
  GameLoader::LoadCampaigns();
  client->PostInitialize();
  //ResourceLoader::PlayAudio(L"moon.wav", true);

  MSG msg;
  clock_t KeyTimer = clock();
  while (GetMessage(&msg, nullptr, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  delete client;
  return (int)msg.wParam;
}