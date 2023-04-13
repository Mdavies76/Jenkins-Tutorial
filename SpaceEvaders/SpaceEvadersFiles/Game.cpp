#include <iostream>
#include <random>
#include <cmath>
#include "Game.h"

const int maxMeteors = 12;
const int wallThickness = 16;
const int playerWidth = 50;
const float WINDOW_WIDTH = 1200.0f;
const float WINDOW_HEIGHT = 650.0f;
const float WINDOW_X = 45.0f;
const float WINDOW_Y = 25.0f;
const float meteorSpawnRangeX = (WINDOW_WIDTH - (wallThickness * 2.0f)) / 4.0f; //using for spawn areas
const float meteorSpawnRangeY = WINDOW_HEIGHT - (wallThickness * 2.0f);
const int lowX = 50;
const int highX = 400;
const int lowY = 25;
const int highY = 300;

Meteor::Meteor() {
	ballTexture = nullptr;
}


void Meteor::setImage(const std::string image) {
	mBallImage = image;
}

std::string Meteor::getImage() const {
	return mBallImage;
}

void Meteor::setTexture(SDL_Texture* newTexture) {
	ballTexture = newTexture;
}

SDL_Texture* Meteor::getTexture() const {
	return ballTexture;
}

void Meteor::setYPosition(float newYPos) {
	mBallPos.y = newYPos;
}

void Meteor::setXPosition(float newXPos) {
	mBallPos.x = newXPos;
}

float Meteor::getYPosition() const {
	return mBallPos.y;
}

float Meteor::getXPosition() const {
	return mBallPos.x;
}

void Meteor::setYVelocity(float newYVel) {
	mBallVel.y = newYVel;
}

void Meteor::setXVelocity(float newXVel) {
	mBallVel.x = newXVel;
}

float Meteor::getYVelocity() const {
	return mBallVel.y;
}

float Meteor::getXVelocity() const {
	return mBallVel.x;
}

void Meteor::incrementYPosition(float yPosIncrement) {
	mBallPos.y += yPosIncrement;
}

void Meteor::incrementXPosition(float xPosIncrement) {
	mBallPos.x += xPosIncrement;
}

void Meteor::swapYDirection() {
	mBallVel.y *= -1.0f;
}

void Meteor::swapXDirection() {
	mBallVel.x *= -1.0f;
}

void Game::updateScore() {
	//update the score
	points++; //increase 'points' every frame (for dummy purposes)
	timeScore = points / 60;	//divide 'points' by 60 to get a point per second
	scoreText = "Score: " + std::to_string(timeScore);
	mSurface = TTF_RenderText_Solid(scoreFont, scoreText.c_str(), scoreColour);
	scoreTexture = SDL_CreateTextureFromSurface(mRenderer, mSurface);
	SDL_FreeSurface(mSurface);

	//update the highScore label if the player has acheived a new highscore
	if (timeScore > highScore) {
		scoreText = "HighScore: " + std::to_string(timeScore);
		mSurface = TTF_RenderText_Solid(scoreFont, scoreText.c_str(), scoreColour);
		highScoreTexture = SDL_CreateTextureFromSurface(mRenderer, mSurface);
		SDL_FreeSurface(mSurface);
	}
}

void Game::createTitleScreen(std::string image, int x, int y, float w, float h) {
	titleScreen = image;
	mSurface = IMG_Load(titleScreen.c_str());
	titleScreenTexture = SDL_CreateTextureFromSurface(mRenderer, mSurface);
	SDL_Rect title{ x, y, w, h };
	SDL_RenderCopyEx(mRenderer, titleScreenTexture, NULL, &title, 0, NULL, SDL_FLIP_NONE);
	SDL_RenderPresent(mRenderer);
	SDL_FreeSurface(mSurface);
}



Game::Game()
{
	mWindow = nullptr;
	mRenderer = nullptr;
	soundEffect = nullptr;
	mMusic = nullptr;
	playerTexture = nullptr;
	mSurface = nullptr;
	scoreTexture = nullptr;
	scoreFont = nullptr;
	highScoreTexture = nullptr;
	titleScreenTexture = nullptr;
	mTicksCount = 0;
	mIsRunning = true;
}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Space E-vaders", // Window title
		WINDOW_X,	// Top left x-coordinate of window
		WINDOW_Y,	// Top left y-coordinate of window
		WINDOW_WIDTH,	// Width of window
		WINDOW_HEIGHT,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	//create audio
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	//load sounds 
	soundEffect = Mix_LoadWAV("C:\\Users\\davie\\OneDrive\\Documents\\SCHOOL\\Fall 2022\\Game Development\\SpaceEvaders\\SpaceEvadersFiles\\arcadeExplosion.wav");
	//load background music
	mMusic = Mix_LoadMUS("C:\\Users\\davie\\OneDrive\\Documents\\SCHOOL\\Fall 2022\\Game Development\\SpaceEvaders\\SpaceEvadersFiles\\CelesteScatteredAndLost.wav");
	//Play the music
	Mix_PlayMusic(mMusic, 0);

	//Create the title screen
	createTitleScreen("C:\\Users\\davie\\OneDrive\\Documents\\SCHOOL\\Fall 2022\\Game Development\\SpaceEvaders\\SpaceEvadersFiles\\titlePage.png", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	SDL_Delay(5000);

	//Create player sprite
	mPlayerImage = "C:\\Users\\davie\\OneDrive\\Documents\\SCHOOL\\Fall 2022\\Game Development\\SpaceEvaders\\SpaceEvadersFiles\\spaceship.png";
	mSurface = IMG_Load(mPlayerImage.c_str());
	playerTexture = SDL_CreateTextureFromSurface(mRenderer, mSurface);
	SDL_FreeSurface(mSurface);

	//set player position and velocity
	mPlayerPos.x = WINDOW_WIDTH / 2.0f;
	mPlayerPos.y = WINDOW_HEIGHT / 2.0f;

	//Create meteor sprites
	for (int i = 0; i < maxMeteors; i++) {
		meteors[i] = new Meteor();
		meteors[i]->setImage("C:\\Users\\davie\\OneDrive\\Documents\\SCHOOL\\Fall 2022\\Game Development\\SpaceEvaders\\SpaceEvadersFiles\\meteor.png");
		mSurface = IMG_Load(meteors[i]->getImage().c_str());
		meteors[i]->setTexture(SDL_CreateTextureFromSurface(mRenderer, mSurface));
		SDL_FreeSurface(mSurface);
	}
	timeForNewMeteor = 10; //first extra meteor will join after 10 seconds
	bonusMeteorIndex = 4; //meteor #5 is the first bonus one to come on

	srand(time(NULL));
	//start with 4 meteors
	for (int i = 0; i < maxMeteors; i++) {
		//generating random (to a degree) spawn points and velocities for them meteors
		float randomXVel = lowX + static_cast<float>(rand()) * static_cast<float>(highX - lowX) / RAND_MAX;
		float randomYVel = lowY + static_cast<float>(rand()) * static_cast<float>(highY - lowY) / RAND_MAX;
		//dividing the screen into 4 areas where the meteors might spawn (1 meteor in each area)
		float randomXPos = (wallThickness + (meteorSpawnRangeX * i)) + static_cast<float>(rand()) *
							static_cast<float>(meteorSpawnRangeX * (i + 1) -
							(wallThickness + (meteorSpawnRangeX * i))) / RAND_MAX;
		float randomYPos = wallThickness + static_cast<float>(rand()) *
							static_cast<float>(meteorSpawnRangeY - wallThickness) / RAND_MAX;
		if (i < 4) { 
			//first 4 meteors spawn in the play area
			meteors[i]->setXPosition(randomXPos);
			meteors[i]->setYPosition(randomYPos);
		}
		else {	//rest will spawn on the top bar
			meteors[i]->setXPosition(WINDOW_WIDTH * (maxMeteors - i) / (maxMeteors - 3)); //dividing the screen to spawn
			meteors[i]->setYPosition(-3);													//the extra meteors
		}
		meteors[i]->setXVelocity(-randomXVel);
		meteors[i]->setYVelocity(randomYVel);
	}
	

	TTF_Init();
	//Create Score label
	scoreColour = { 0, 100, 255 };
	scoreText = "Score: 0";
	scoreFont = TTF_OpenFont("C:\\Users\\davie\\OneDrive\\Documents\\SCHOOL\\Fall 2022\\Game Development\\SpaceEvaders\\SpaceEvadersFiles\\scoreFont.ttf", 16);
	mSurface = TTF_RenderText_Solid(scoreFont, scoreText.c_str(), scoreColour);
	scoreTexture = SDL_CreateTextureFromSurface(mRenderer, mSurface);
	SDL_FreeSurface(mSurface);

	//set up highscore file
	highScoreSaver.open("C:\\Users\\davie\\OneDrive\\Documents\\SCHOOL\\Fall 2022\\Game Development\\SpaceEvaders\\SpaceEvadersFiles\\highscore.txt", std::ios::in);
	highScoreSaver >> highScore;	//read the previous score from the file
	highScoreSaver.close();

	//create highScore label
	highScoreColour = { 255, 255, 255 };
	scoreText = "HighScore: " + std::to_string(highScore);
	mSurface = TTF_RenderText_Solid(scoreFont, scoreText.c_str(), highScoreColour);
	highScoreTexture = SDL_CreateTextureFromSurface(mRenderer, mSurface);
	SDL_FreeSurface(mSurface);

	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		updateScore();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	//Pause and play background music with 'Z'
	if (state[SDL_SCANCODE_Z]) {
		//If the music is paused
		switch (Mix_PausedMusic())
		{
		case 1:
			//Resume the music
			Mix_ResumeMusic();
			break;
		//If the music is playing
		default:
			//Pause the music
			Mix_PauseMusic();
			break;
		}
	}

	//player is not moving if no movement keys are pressed
	mPlayerDir.y = 0;
	mPlayerDir.x = 0;
	// Update paddle direction based on W/S keys or up/down arrow
	bool up = state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP];
	bool down = state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN];
	bool left = state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT];
	bool right = state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT];
	if (up)
	{	//rotate diagonally if moving in 2 different directions
		if (left)
			playerRotation = 315;
		else if (right)
			playerRotation = 45;
		else //face upward 
			playerRotation = 0;
		mPlayerDir.y -= 1;
	}
	else if (down)
	{	//rotate diagonally if moving in 2 different directions
		if (left)
			playerRotation = 225;
		else if (right)
			playerRotation = 135;
		else //face downward 
			playerRotation = 180;
		mPlayerDir.y += 1;
	}
	if (left)
	{	//rotate diagonally if moving in 2 different directions
		if (up)
			playerRotation = 315;
		else if (down)
			playerRotation = 225;
		else //face left 
			playerRotation = 270;
		mPlayerDir.x -= 1;
	}
	else if (right)
	{	//rotate diagonally if moving in 2 different directions
		if (up)
			playerRotation = 45;
		else if (down)
			playerRotation = 135;
		else //face right 
			playerRotation = 90;
		mPlayerDir.x += 1;
	}
}

void Game::UpdateGame()
{

	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))	
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();
	
	// Update players position based on direction
	if (mPlayerDir.y != 0 || mPlayerDir.x != 0) //nothing to do if player isn't moving (user hasnt pressed movement key)
	{
		//if player is "moving", update its position accordingly
		mPlayerPos.y += mPlayerDir.y * 300.0f * deltaTime;
		mPlayerPos.x += mPlayerDir.x * 300.0f * deltaTime;

		// Make sure player doesn't move off screen!
		// Y wall detection
		if (mPlayerPos.y < (wallThickness * 1.5)) //top
		{
			mPlayerPos.y = wallThickness * 1.5;
		}
		else if (mPlayerPos.y > WINDOW_HEIGHT - wallThickness / 2 - playerWidth) //bottom
		{
			mPlayerPos.y = WINDOW_HEIGHT - wallThickness / 2 - playerWidth;
		}
		// X wall detection
		if (mPlayerPos.x < wallThickness) //left side
		{
			mPlayerPos.x = wallThickness;
		}
		else if (mPlayerPos.x > WINDOW_WIDTH - (wallThickness + playerWidth)) //right side
		{
			mPlayerPos.x = WINDOW_WIDTH - (wallThickness + playerWidth);
		}
	}

	//check if its time to add another meteor to the game
	if (timeScore == timeForNewMeteor && bonusMeteorIndex < maxMeteors) {
		timeForNewMeteor += 5 * (bonusMeteorIndex - 1);	//spawn times will be: (10, 25, 45, 70, 100, 135, 175, 220)
		meteors[bonusMeteorIndex++]->setYPosition(-2); //move meteor down into play area so it starts moving
	}																					
	
	for (int i = 0; i < maxMeteors; i++) {
		if (meteors[i]->getYPosition() > -3) {	//update meteors movement if they are not waiting at the top
			meteors[i]->incrementYPosition(meteors[i]->getYVelocity() * deltaTime);
			meteors[i]->incrementXPosition(meteors[i]->getXVelocity() * deltaTime);
		}
		//increase each meteors speed every 10 seconds 
		if (points % 1000 == 0) {
			meteors[i]->setXVelocity(meteors[i]->getXVelocity() * 1.02);
			meteors[i]->setYVelocity(meteors[i]->getYVelocity() * 1.02);
		}
	
		//Check for game over (collision between player and a meteor)
		//trying to fix a bug where collision happens too far away if the spaceship is to the right or on top of the meteor
		if (mPlayerPos.x > meteors[i]->getXPosition()) {
			collided[i] = abs(mPlayerPos.y - meteors[i]->getYPosition()) < playerWidth / 2.0f &&
							abs(mPlayerPos.x - meteors[i]->getXPosition()) < playerWidth / 3.5f;
		}
		else {
			//checking the distance between player & meteor, with a little extra leeway since the objects are techinacally squares
			collided[i] = abs(mPlayerPos.y - meteors[i]->getYPosition()) < playerWidth  / 2.0f &&
							abs(mPlayerPos.x - meteors[i]->getXPosition()) < playerWidth / 1.5f;
		}
		
		if (collided[i] && timeScore > 2) //2 second time buffer to give the user a chance to get settled 
		{
			//stop the music and play explosion sound
			Mix_PauseMusic();
			Mix_PlayChannel(-1, soundEffect, 0);
			//show game over screen for 5 seconds
			createTitleScreen("C:\\Users\\davie\\OneDrive\\Documents\\SCHOOL\\Fall 2022\\Game Development\\SpaceEvaders\\SpaceEvadersFiles\\gameOver.png", (WINDOW_WIDTH / 2) - (WINDOW_WIDTH / 8),	//centered in the window
											(WINDOW_HEIGHT / 2) - (WINDOW_HEIGHT / 8), 
											WINDOW_WIDTH / 4, 
											WINDOW_HEIGHT / 4);
			SDL_Delay(5000);
			//stop game
			mIsRunning = false;
			//check if most recent score was higher than previous record
			if (timeScore > highScore) {
				highScoreSaver.open("C:\\Users\\davie\\OneDrive\\Documents\\SCHOOL\\Fall 2022\\Game Development\\SpaceEvaders\\SpaceEvadersFiles\\highscore.txt", std::ios::out);
				highScoreSaver << timeScore;	//write the score to the .txt file if so
				highScoreSaver.close();
			}

		}
		// Did a meteor collide with the right wall?
		if (meteors[i]->getXPosition() >= (WINDOW_WIDTH - wallThickness) && meteors[i]->getXVelocity() > 0.0f)
		{
			meteors[i]->swapXDirection();
		}
		// Did the ball collide with the left wall?
		else if (meteors[i]->getXPosition() <= wallThickness && meteors[i]->getXVelocity() < 0.0f)
		{
			meteors[i]->swapXDirection();
		}

		// Did the ball collide with the top wall?
		else if (meteors[i]->getYPosition() <= wallThickness && meteors[i]->getYVelocity() < 0.0f)
		{
			meteors[i]->swapYDirection();
		}
		// Did the ball collide with the bottom wall?
		else if (meteors[i]->getYPosition() >= (WINDOW_HEIGHT - wallThickness * 2) && meteors[i]->getYVelocity() > 0.0f)
		{
			meteors[i]->swapYDirection();
		}
	}
}

void Game::GenerateOutput()
{

	// Set draw color to black for background (black)
	SDL_SetRenderDrawColor(mRenderer,0, 0, 0, 255);
	
	// Clear back buffer
	SDL_RenderClear(mRenderer);

	//Draw the player
	SDL_Rect player{
		static_cast<int>(mPlayerPos.x),
		static_cast<int>(mPlayerPos.y - wallThickness / 2),
		playerWidth,
		playerWidth
	};
	SDL_RenderCopyEx(mRenderer, playerTexture, NULL, &player, playerRotation, NULL, SDL_FLIP_NONE);
	
	//Draw the meteors 
	SDL_Rect meteorRects[maxMeteors];
	for (int i = 0; i < maxMeteors; i++) {
		meteorRects[i] = {
		static_cast<int>(meteors[i]->getXPosition()),
		static_cast<int>(meteors[i]->getYPosition()),
		playerWidth / 2,
		playerWidth / 2
		};
		SDL_RenderCopyEx(mRenderer, meteors[i]->getTexture(), NULL, &meteorRects[i], 0, NULL, SDL_FLIP_NONE);
	}

	SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);
	// Draw top wall
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		static_cast<int>(WINDOW_WIDTH),		// Width
		wallThickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw bottom wall
	wall.y = static_cast<int>(WINDOW_HEIGHT) - wallThickness;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw right wall
	wall.x = static_cast<int>(WINDOW_WIDTH) - wallThickness;
	wall.y = 0;
	wall.w = wallThickness;
	wall.h = static_cast<int>(WINDOW_HEIGHT);
	SDL_RenderFillRect(mRenderer, &wall);

	//draw left wall
	wall.x = 0;
	SDL_RenderFillRect(mRenderer, &wall);

	//Render the score
	SDL_Rect score{ WINDOW_WIDTH - 150, 40, 100, 40 };
	SDL_RenderCopyEx(mRenderer, scoreTexture, NULL , &score, 0, NULL, SDL_FLIP_NONE);

	//Render the high score
	SDL_Rect highScore{ WINDOW_WIDTH - 150, 10, 120, 40 };
	SDL_RenderCopyEx(mRenderer, highScoreTexture, NULL, &highScore, 0, NULL, SDL_FLIP_NONE);
	
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyTexture(playerTexture);
	playerTexture = nullptr;
	for (int i = 0; i < maxMeteors; i++) {
		SDL_DestroyTexture(meteors[i]->getTexture());
		meteors[i]->setTexture(nullptr);
	}
	Mix_FreeChunk(soundEffect);
	soundEffect = nullptr;
	Mix_FreeMusic(mMusic);
	mMusic = nullptr;
	TTF_CloseFont(scoreFont);
	SDL_DestroyTexture(scoreTexture);
	scoreTexture = nullptr;
	SDL_DestroyRenderer(mRenderer);
	mRenderer = nullptr;
	SDL_DestroyWindow(mWindow);
	mWindow = nullptr;
	SDL_Quit();
}