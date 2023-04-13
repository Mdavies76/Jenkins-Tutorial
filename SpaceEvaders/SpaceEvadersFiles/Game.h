#pragma once
#include <string>
#include <fstream>
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"

struct Vector2
{
	//x and y coordinates to keep track of ball & paddle position
	float x;
	float y;
};

class Meteor
{
public:
	Meteor();
	void setImage(const std::string);
	std::string getImage() const;
	void setTexture(SDL_Texture*);
	SDL_Texture* getTexture() const;
	void setYPosition(float);
	void setXPosition(float);
	float getYPosition() const;
	float getXPosition() const;
	void setYVelocity(float);
	void setXVelocity(float);
	float getYVelocity() const;
	float getXVelocity() const;
	void incrementYPosition(float);
	void incrementXPosition(float);
	void swapXDirection();
	void swapYDirection();

private:
	Vector2 mBallPos;
	Vector2 mBallVel;
	std::string mBallImage;
	SDL_Texture* ballTexture;
};

class Game
{
public:
	Game();	//default constructor, considering an empty game with everything set to null
	bool Initialize();	//here is where we will initialize all of our attributes (window, surface, texutres, sounds etc)
	void RunLoop();	//will call ProcessInput(), UpdateGame(), and GenerateOutput(). This is the main game loop
	void Shutdown();	//delete everything and free memory
private:
	void ProcessInput();	//any code for inputs like quit, keyboard interactions etc...
	void UpdateGame();	//frame rate limitation, moving the ball and paddle, collision detection etc...
	void GenerateOutput();	//creating the paddle, ball, walls, score and showing on the screen
	void updateScore();
	void createTitleScreen(std::string, int x, int y, float w, float h);

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	SDL_Surface* mSurface;
	SDL_Texture* playerTexture;
	SDL_Texture* scoreTexture;
	SDL_Texture* highScoreTexture;
	SDL_Texture* titleScreenTexture;
	TTF_Font* scoreFont;
	Mix_Chunk* soundEffect; 
	Mix_Music* mMusic;
	Uint32 mTicksCount;
	SDL_Color scoreColour;
	SDL_Color highScoreColour;

	int timeForNewMeteor;
	int bonusMeteorIndex;
	
	std::string titleScreen;
	bool mIsRunning;
	bool collided[12];	//array of booleans to check if each meteor haas hit the ship or not
	int points;	//helper variable to increment points every frame
	int timeScore; //dividing points by FPS to get an acurate 'timer', so this will be the actual 'score'
	int highScore;
	int playerRotation;
	std::fstream highScoreSaver;
	std::string mPlayerImage;
	std::string scoreText;
	Vector2 mPlayerDir;
	Vector2 mPlayerPos;
	Meteor* meteors[12];
};
