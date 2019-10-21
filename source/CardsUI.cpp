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
	int QuestionNumber = 0;
	vector <string> Questions = vector <string>(0);
	vector <string> Answers = vector <string>(0);
	int CardCount = 0;
	int CardNum = 0;
	TTF_Font *CardFont;
	TTF_Font *FooterFont;
	bool IsAnswer = true;
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
	string CurrentText = "Flash cards NX";
};

CardsUI::CardsUI()
{
	CardFont = TTF_OpenFont("romfs:/font.ttf", 128); //Load card font
	FooterFont = TTF_OpenFont("romfs:/font.ttf", 64); //Load font
}

void CardsUI::DrawUI()
{
	//Scan input
	while (SDL_PollEvent(Event))
		{
            switch (Event->type)
			{
				//Touch screen is pressed. Flip card.
				case SDL_FINGERDOWN:
					FlipCard();
				break;
				//Button pressed on joycon
                case SDL_JOYBUTTONDOWN:
                    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L52
                    // seek for joystick #0
                    if (Event->jbutton.which == 0)
					{
						//Plus pressed
						if (Event->jbutton.button == 10)
						{
							//Reset some vars and display the selection menu
							Questions.clear();
							Answers.clear();
							QuestionNumber = 0;
							CardNum = 0;
							IsAnswer = true;
                            *WindowState = 0;
                        }
						//A pressed
						else if(Event->jbutton.button == 0)
						{
							FlipCard();
						}
                    }
                    break;
            }
        }
		
		
	//Get one perecent of the window height to do more math later
	int HeightOnePercent = (*Height / 100);
		
	//Draw the card background
	int BackgroundWidth = *Width;
	int BackgroundHeight = HeightOnePercent * 80;
	SDL_Rect CardTop = {0,0, BackgroundWidth, BackgroundHeight};
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	SDL_RenderFillRect(renderer, &CardTop);
	
	//Draw the card text
	SDL_Color TextColour = {0, 0, 0}; 
	SDL_Surface* CardTextSurface = TTF_RenderUTF8_Blended_Wrapped(CardFont, CurrentText.c_str(), TextColour, BackgroundWidth);
	SDL_Texture* CardTextTexture = SDL_CreateTextureFromSurface(renderer, CardTextSurface);
	//make new rect size of the text and centered in the background
	SDL_Rect CardRect = {(BackgroundWidth - CardTextSurface->w) / 2, (BackgroundHeight - CardTextSurface->h) / 2, CardTextSurface->w, CardTextSurface->h};
	SDL_RenderCopy(renderer, CardTextTexture, NULL, &CardRect);
	//Clean up
	SDL_DestroyTexture(CardTextTexture);
	SDL_FreeSurface(CardTextSurface);
	
	//Draw the footer background
	int FooterHeight = HeightOnePercent * 20;
	SDL_Rect Footer = {0, BackgroundHeight, BackgroundWidth, FooterHeight};
	SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
	SDL_RenderFillRect(renderer, &Footer);
	
	//Draw the footer text
	SDL_Surface* FooterTextSurface = TTF_RenderUTF8_Blended_Wrapped(FooterFont, "A = Next card\n+ = Exit", TextColour, BackgroundWidth);
	SDL_Texture* FooterTextTexture = SDL_CreateTextureFromSurface(renderer, FooterTextSurface);
	//make new rect the size of the text and centered vertically in the footer
	SDL_Rect FooterTextRect = {0, BackgroundHeight + (FooterHeight - FooterTextSurface->h) / 2, FooterTextSurface->w, FooterTextSurface->h};
	SDL_RenderCopy(renderer, FooterTextTexture, NULL, &FooterTextRect);
	//clean up
	SDL_DestroyTexture(FooterTextTexture);
	SDL_FreeSurface(FooterTextSurface);
	//Draw the completion text
	string CompleteCardsString = "Complete: " + to_string(CardNum) + " / " + to_string(CardCount);
	SDL_Surface* CompleteTextSurface = TTF_RenderUTF8_Blended_Wrapped(FooterFont, CompleteCardsString.c_str(), TextColour, BackgroundWidth);
	SDL_Texture* CompleteTextTexture = SDL_CreateTextureFromSurface(renderer, CompleteTextSurface);
	//make new rect the size of the text and centered vertically in the footer
	FooterTextRect = {BackgroundWidth - CompleteTextSurface->w, BackgroundHeight + (FooterHeight - CompleteTextSurface->h) / 2, CompleteTextSurface->w, CompleteTextSurface->h};
	SDL_RenderCopy(renderer, CompleteTextTexture, NULL, &FooterTextRect);
	//clean up
	SDL_DestroyTexture(CompleteTextTexture);
	SDL_FreeSurface(CompleteTextSurface);
}

void CardsUI::PopulateVectors()
{
	CardCount = 0; //Reset card count
	//Loads cards from text file
	string CardPath = "sdmc:/config/cards/" + SelectedCardPath;
	ifstream CardStream(CardPath.c_str());
	//Check the file is able to be opened otherwise return
	if(!CardStream)
	{
		Questions.push_back("Error opening cards.");
		Answers.push_back("Error opening cards.");
		return;
	}
	//For each two lines load them in to the questions and answers vectors
	string CurrentLine = "";
	while(!CardStream.eof())
	{
		getline(CardStream, CurrentLine);
		Questions.push_back(CurrentLine);
		//Odd number of lines
		if(CardStream.eof())
		{
			Questions.pop_back();
			break;
		}
		getline(CardStream, CurrentLine);
		Answers.push_back(CurrentLine);
		CardCount++;
	}
}

void CardsUI::FlipCard()
{
		//Next card
		if(IsAnswer)
		{
			//Pick card at random
			srand(time(0));
			QuestionNumber = rand() % Answers.size();
			//Check if empty
			if(Answers.empty())
			{
			//Show complete
			CurrentText = "All questions shown. Press A to reset."; //Display the complete message.
			IsAnswer = !IsAnswer; //Next card should show the question.
			PopulateVectors(); //Reload the question and answer vectors.
			CardNum = 0;
			}
			else
			{
				//Show new card
				CurrentText = Questions.at(QuestionNumber);
				Questions.erase(Questions.begin() + QuestionNumber);
				CardNum++;
			}	
		}
		//Show answer
		else
		{
			//Check if empty
			if(Answers.empty())
			{
				//Show complete
				CurrentText = "All answers shown. If you see this text something has gone wrong.";
			}
			else
			{
				//Show answer
				CurrentText = Answers.at(QuestionNumber);
				Answers.erase(Answers.begin() + QuestionNumber);
			}
		}
	IsAnswer = !IsAnswer;
}