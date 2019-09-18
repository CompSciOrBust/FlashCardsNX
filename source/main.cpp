#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

#include <SDL.h>
#include <stdlib.h>
#include <SDL2/SDL_ttf.h>
#include <switch.h>
#include <time.h>
#include <CardSelectUI.h>
#include <CardsUI.h>
using namespace std;

//vars
int UIState = 0;
string CardsPath = "";
int WindowWidth = 1280, WindowHeight = 720;

int main(int argc, char *argv[])
{
	//Vars
	SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;
	int IsDone = 0;
	
	//Init
	romfsInit(); //Init romfs to load font
	TTF_Init(); //Init the font
	
	// mandatory at least on switch, else gfx is not properly closed
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        SDL_Log("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }
	
	// create an SDL window (OpenGL ES2 always enabled)
    // when SDL_FULLSCREEN flag is not set, viewport is automatically handled by SDL (use SDL_SetWindowSize to "change resolution")
    // available switch SDL2 video modes :
    // 1920 x 1080 @ 32 bpp (SDL_PIXELFORMAT_RGBA8888)
    // 1280 x 720 @ 32 bpp (SDL_PIXELFORMAT_RGBA8888)
    window = SDL_CreateWindow("sdl2_gles2", 0, 0, WindowWidth, WindowHeight, 0);
    if (!window)
	{
        SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
	
	// create a renderer (OpenGL ES2)
    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
	{
        SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
	
	// open CONTROLLER_PLAYER_1 and CONTROLLER_PLAYER_2
    // when railed, both joycons are mapped to joystick #0,
    // else joycons are individually mapped to joystick #0, joystick #1, ...
    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L45
    for (int i = 0; i < 2; i++)
	{
        if (SDL_JoystickOpen(i) == NULL)
		{
            SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
            SDL_Quit();
            return -1;
        }
    }
	
	//Selection UI set up
	CardSelectUI *SelectionUI = new CardSelectUI();
	SelectionUI->Width = &WindowWidth;
	SelectionUI->Height = &WindowHeight;
	SelectionUI->renderer = renderer;
	SelectionUI->WindowState = &UIState;
	SelectionUI->Event = &event;
	SelectionUI->SelectedCardPath = CardsPath;
	SelectionUI->IsDone = &IsDone;
	
	//Cards UI set up
	CardsUI *FlashCardsUI = new CardsUI();
	FlashCardsUI->Width = &WindowWidth;
	FlashCardsUI->Height = &WindowHeight;
	FlashCardsUI->renderer = renderer;
	FlashCardsUI->WindowState = &UIState;
	FlashCardsUI->Event = &event;
	//Keep track of which UI state was used last frame
	int LastUIState = 0;
	//Main loop
	while(!IsDone)
	{
		//Clear the frame
		SDL_RenderClear(renderer);
		//Draw the UIState
		switch(UIState)
		{
			//Draw the card selection UI
			case 0:
			{
				LastUIState = UIState;
				SelectionUI->DrawUI();
			}
			break;
			//Draw the main flash card UI
			case 1:
			{
				//Newly selected cards so populate vectors
				if(UIState != LastUIState)
				{
					FlashCardsUI->SelectedCardPath = SelectionUI->SelectedCardPath;
					FlashCardsUI->PopulateVectors();
					FlashCardsUI->CurrentText = "Flash cards NX";
				}
				LastUIState = UIState;
				FlashCardsUI->DrawUI();
			}
			break;
		}
		
		//Render the frame
		SDL_RenderPresent(renderer);
	}
	
	SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	
	return 0;
}
