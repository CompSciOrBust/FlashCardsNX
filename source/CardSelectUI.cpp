#include <stdlib.h>
#include <SDL.h>
#include <SDL2/SDL_ttf.h>
#include <dirent.h>
#include <string>
#include <switch.h>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

//Vars
vector <dirent> Files(0);
int SelectedIndex = 0;
int TouchY = -1;

class CardSelectUI
{
	private:
	void GetCards();
	TTF_Font *SelectionFont;
	public:
	CardSelectUI();
	void DrawUI();
	string SelectedCardPath;
	SDL_Event *Event;
	int *WindowState;
	SDL_Renderer *renderer;
	int *Width;
	int *Height;
	int *IsDone;
};

CardSelectUI::CardSelectUI()
{
	SelectionFont = TTF_OpenFont("romfs:/font.ttf", 48); //Load the options font
	GetCards();
}

void CardSelectUI::DrawUI()
{
	//If there is only one file we don't need to ask which file to load
	if(Files.size() == 1)
	{
		//If selected card path isn't empty then the user has pressed plus so we should exit
		if(!SelectedCardPath.empty())
		{
			*IsDone = 1;
		}
		SelectedCardPath = Files.at(0).d_name;
		*WindowState = 1;
		return;
	}
	
	//Scan input
	while (SDL_PollEvent(Event))
		{
            switch (Event->type)
			{
				//Touch screen
				case SDL_FINGERDOWN:
				TouchY = Event->tfinger.y * *Height;
				break;
				//Joycon button pressed
                case SDL_JOYBUTTONDOWN:
                    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L52
                    // seek for joystick #0
                    if (Event->jbutton.which == 0)
					{
						//Plus pressed
						if (Event->jbutton.button == 10)
						{
                            *IsDone = 1;
                        }
						//Up pressed
						else if(Event->jbutton.button == 13)
						{
							SelectedIndex--;
						}
						//Down pressed
						else if(Event->jbutton.button == 15)
						{
							SelectedIndex++;
						}
						//A pressed
						else if(Event->jbutton.button == 0)
						{
							SelectedCardPath = Files.at(SelectedIndex).d_name;
							*WindowState = 1;
						}
                    }
                    break;
            }
        }
	
	//Check selected index is with in the bounds
	if(SelectedIndex < 0)
	{
		SelectedIndex = Files.size()-1;
	}
	else if(SelectedIndex > Files.size()-1)
	{
		SelectedIndex = 0;
	}
	
	//Draw the options
	int ItemHeight = *Height / Files.size(); //Note: We don't check if files contains anything but we probably should.
	for(int i = 0; i < Files.size(); i++)
	{
		//Check if user touched the option
		if(TouchY > i * ItemHeight && TouchY < i * ItemHeight + ItemHeight)
		{
			SelectedIndex = i;
			SelectedCardPath = Files.at(SelectedIndex).d_name;
			*WindowState = 1;
			TouchY = -1;
		}
		//Set the background color
		SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
		//Check if this is the highlighted file
		if(i == SelectedIndex)
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		}
		SDL_Rect MenuItem = {0, (i * ItemHeight), *Width, ItemHeight};
		SDL_RenderFillRect(renderer, &MenuItem);
		//Draw card names
		SDL_Color TextColour = {0, 0, 0};
		SDL_Surface* CardsNameSurface = TTF_RenderUTF8_Blended_Wrapped(SelectionFont, Files.at(i).d_name, TextColour, *Width);
		SDL_Texture* CardsNameTexture = SDL_CreateTextureFromSurface(renderer, CardsNameSurface);
		SDL_Rect CardsNameRect = {0, (i * ItemHeight) + ((ItemHeight - CardsNameSurface->h) / 2), CardsNameSurface->w, CardsNameSurface->h};
		SDL_RenderCopy(renderer, CardsNameTexture, NULL, &CardsNameRect);
		//Clean up
		SDL_DestroyTexture(CardsNameTexture);
		SDL_FreeSurface(CardsNameSurface);
	}
}

void CardSelectUI::GetCards()
{
	DIR* dir;
	struct dirent* ent;
	dir = opendir("sdmc:/config/cards/");
	Files.clear();
	while ((ent = readdir(dir)))
	{
		Files.push_back(*ent);
	}
	closedir(dir);
}