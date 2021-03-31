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


enum  EntityType { PLAYER, TILE, ENEMY };
enum AIState { CHASING, Flying, PATROLLING };

class Entity {
public:
	bool isActive;
	EntityType entityType;
	AIState aiState;

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 accerlation;
	GLuint textureID;

	bool CheckCollision(Entity other);

	float width;
	float height;
	float speed;
	Entity();

	void Jump();
	
	void Update(float deltaTime, Entity* player, Entity* tiles, int tileCount, Entity* enemies, int enemyCount);
	void Render(ShaderProgram* program);

	void AI(Entity* player);

	void CheckCollisionsX(Entity* objects, int objectCount);
	void CheckCollisionsY(Entity* objects, int objectCount);

	bool collidedTop;
	bool collidedBottom;
	bool collidedLeft;
	bool collidedRight;
	
};