#include "Menu.h"
#define MAINMENU_WIDTH 14
#define MAINMENU_HEIGHT 8
unsigned int mainmenu_data[] =
{
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void Menu::Initialize() {
	GLuint mapTextureID = Util::LoadTexture("tileset.png");
	state.map = new Map(MAINMENU_WIDTH, MAINMENU_HEIGHT, mainmenu_data, mapTextureID, 1.0f, 4, 1);
	state.nextLevel = -4;
}
void Menu::Update(float deltaTime) {}
void Menu::Render(ShaderProgram* program) {
	state.map->Render(program);
}