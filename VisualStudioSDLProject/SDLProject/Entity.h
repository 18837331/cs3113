#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

class Entity {
public:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 accerlation;
	GLuint textureID;

	float speed;
	glm::mat4 modelMatrix;
	Entity();
	
	void Update(float deltaTime);
	void Render(ShaderProgram* program);
	
};