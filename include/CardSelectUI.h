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