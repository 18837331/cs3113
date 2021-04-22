#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"
#include "Map.h"
#include "Util.h"
#include "Scene.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Menu.h"






SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
glm::vec3 life_Vec, gameover_Vec;

GLuint fontTextureID;

int remain_Life;
Scene* currentScene;
Scene* scenes[4];

Mix_Music* music;
Mix_Chunk* bounce;

void SwitchToScene(Scene* scene) {
	currentScene = scene;
	currentScene->Initialize();
}


void Initialize() {
	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
	displayWindow = SDL_CreateWindow("Platformer!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);



#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);


	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

	fontTextureID = Util::LoadTexture("font1.png");

	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	glUseProgram(program.programID);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	scenes[0] = new Menu();
	scenes[1] = new Level1();
	scenes[2] = new Level2();
	scenes[3] = new Level3();
	SwitchToScene(scenes[0]);
	remain_Life = 3;
	int total_life = currentScene->state.player.life;

	Mix_OpenAudio(44100, MIX_DEFAULT_CHANNELS, 2, 4096);
	music = Mix_LoadMUS("dooblydoo.mp3");
	Mix_PlayMusic(music, -1);

	bounce= Mix_LoadWAV("bounce.wav");
}

void ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				currentScene->state.player.Jump();
				break;
			case SDLK_RETURN:
				if (currentScene->state.nextLevel == -4) {
					SwitchToScene(scenes[1]);
				}
				break;
			}
		}
	}

	currentScene->state.player.velocity.x = 0;

	// Check for pressed/held keys below
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_A])
	{
		currentScene->state.player.velocity.x = -3.0f;
	}
	else if (keys[SDL_SCANCODE_D])
	{
		currentScene->state.player.velocity.x = 3.0f;
	}
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;

	deltaTime += accumulator;
	if (deltaTime < FIXED_TIMESTEP) {
		accumulator = deltaTime;
		return;
	}

	while (deltaTime >= FIXED_TIMESTEP) {

		currentScene->Update(FIXED_TIMESTEP);

		deltaTime -= FIXED_TIMESTEP;
	}

	accumulator = deltaTime;


	viewMatrix = glm::mat4(1.0f);
	gameover_Vec = glm::vec3(3, -2, 0);
	life_Vec = glm::vec3(1, -0.5, 0);
	

	if (currentScene->state.player.position.x > 5) {
		viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player.position.x, 3.75, 0));
		life_Vec = glm::vec3(currentScene->state.player.position.x - 4, -0.5, 0);
		gameover_Vec = glm::vec3(currentScene->state.player.position.x - 1.5, -2, 0);
	}
	else {
		viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
	}


	remain_Life = currentScene->state.player.life;

	if (currentScene->state.player.dead) {
		Mix_PlayChannel(-1, bounce, 0);
	}

}

void Render() {
	glUseProgram(program.programID);
	program.SetViewMatrix(viewMatrix);
	glClear(GL_COLOR_BUFFER_BIT);

	
	
	Util::DrawText(&program, fontTextureID, "lives: "+ std::to_string(remain_Life), 0.5f, -0.3f, life_Vec);

	if (currentScene->state.player.win) {
		Util::DrawText(&program, fontTextureID, "You Win!", 1.0f, -0.5f, gameover_Vec);
	}
	if (currentScene->state.player.life == 0) {
		Util::DrawText(&program, fontTextureID, "You Lose!", 1.0f, -0.5f, gameover_Vec);
	}

	if (currentScene->state.nextLevel == -4) {
		Util::DrawText(&program, fontTextureID, "PLATFORMER", 1.0f, -0.5f, glm::vec3(2.5, -2, 0));
		Util::DrawText(&program, fontTextureID, "press enter to start", 1.0f, -0.62f, glm::vec3(2, -4, 0));
	}

	if (currentScene->state.nextLevel == -1 && currentScene->state.player.life > 0) {
		Util::DrawText(&program, fontTextureID, "LEVEL1", 1.0f, -0.5f, glm::vec3(2.5, -4, 0));
		Util::DrawText(&program, fontTextureID, "Next Level ->", 0.8f, -0.5f, glm::vec3(22, -3, 0));
	}
	if (currentScene->state.nextLevel == -2 && currentScene->state.player.life > 0) {
		Util::DrawText(&program, fontTextureID, "LEVEL2", 1.0f, -0.5f, glm::vec3(2.5, -4, 0));
		Util::DrawText(&program, fontTextureID, "Next Level ->", 0.8f, -0.5f, glm::vec3(22, -3, 0));
	}
	if (currentScene->state.nextLevel == -3 && currentScene->state.player.life > 0) {
		Util::DrawText(&program, fontTextureID, "LEVEL3", 1.0f, -0.5f, glm::vec3(2.5, -4, 0));
		Util::DrawText(&program, fontTextureID, "GOAL ->", 0.8f, -0.4f, glm::vec3(18, -3, 0));
	}

	currentScene->Render(&program);

	SDL_GL_SwapWindow(displayWindow);
}

int main(int argc, char* argv[]) {
	Initialize();

	while (gameIsRunning) {
		if (currentScene->state.nextLevel >= 0) {
			SwitchToScene(scenes[currentScene->state.nextLevel]);
			currentScene->state.player.life = remain_Life;
		}
		if ((!currentScene->state.player.dead) && (!currentScene->state.player.win)) {
			ProcessInput();
		}
		else {
			currentScene->state.player.velocity.x = 0;
		}
		Update();
		Render();
	}


	return 0;
}