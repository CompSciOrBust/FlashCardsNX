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

class CardsUI
{
	private:
	int QuestionNumber;
	vector <string> Questions;
	vector <string> Answers;
	int CardCount = 0;
	int CardNum = 0;
	TTF_Font *CardFont;
	TTF_Font *FooterFont;
	bool IsAnswer;
	public:
	CardsUI();
	void DrawUI();
	void PopulateVectors();
	void FlipCard();
	string SelectedCardPath;
	SDL_Event *Event;
	int *WindowState;
	SDL_Renderer *renderer;
	int *Width;
	int *Height;
	string CurrentText;
};