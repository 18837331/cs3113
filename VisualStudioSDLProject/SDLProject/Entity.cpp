#include "Entity.h"

Entity::Entity()
{
	isActive = true;
	position = glm::vec3(0);
	velocity = glm::vec3(0);
	speed = 0;
	width = 1;
	height = 1;
}

bool Entity::CheckCollision(Entity other)
{
	if (isActive == false || other.isActive == false) return false;

	float xdist = fabs(position.x - other.position.x) - ((width + other.width) / 2.0f);
	float ydist = fabs(position.y - other.position.y) - ((height + other.height) / 2.0f);

	if (xdist < 0 && ydist < 0)
	{
		return true;
	}

	return false;
}



void Entity::Jump()
{
	if (collidedBottom)
	{
		velocity.y = 9.0f;
	}
}

void Entity::CheckCollisionsY(Entity* tiles, int tileCount)
{
	for (int i = 0; i < tileCount; i++)
	{
		Entity object = tiles[i];

		if (CheckCollision(object))
		{
			float ydist = fabs(position.y - object.position.y);
			float penetrationY = fabs(ydist - (height / 2) - (object.height / 2));
			if (velocity.y >= 0) {
				position.y -= penetrationY;
				velocity.y = 0;
				collidedTop = true;
				if (entityType == PLAYER && tiles[i].entityType == ENEMY)
				{
					isActive = false;
				}
			}
			else if (velocity.y <= 0) {
				position.y += penetrationY;
				velocity.y = 0;
				collidedBottom = true;
				if (entityType == PLAYER && tiles[i].entityType == ENEMY)
				{
					tiles[i].isActive = false;
				}
			}


		}
	}
}

void Entity::CheckCollisionsX(Entity* tiles, int tileCount)
{
	for (int i = 0; i < tileCount; i++)
	{
		Entity object = tiles[i];

		if (CheckCollision(object))
		{
			float xdist = fabs(position.x - object.position.x);
			float penetrationX = fabs(xdist - (width / 2) - (object.width / 2));
			if (velocity.x >= 0) {
				position.x -= penetrationX;
				velocity.x = 0;
				collidedRight = true;
				if (entityType == PLAYER && tiles[i].entityType == ENEMY)
				{
					isActive = false;
				}
			}
			else if (velocity.x <= 0) {
				position.x += penetrationX;
				velocity.x = 0;
				collidedLeft = true;
				if (entityType == PLAYER && tiles[i].entityType == ENEMY)
				{
					isActive = false;
				}
			}
		}
	}
}

void Entity::AI(Entity* player) {
	switch (aiState) {
	case CHASING :
		if (player->position.x > position.x) {
			velocity.x = 1.0f;
		}
		else {
			velocity.x = -1.0f;
		}

		if (collidedBottom) {
			Jump();
		}
		break;
	case Flying:
		if (player->position.y > position.y) {
			velocity.y = 1.0f;
		}
		else {
			velocity.y = -1.0f;
		}
		if (player->position.x > position.x) {
			velocity.x = 1.0f;
		}
		else {
			velocity.x = -1.0f;
		}
		break;
	case PATROLLING:
		if (position.x < -1.4) {
			velocity.x = 1.0f;
		}
		else if (position.x > 0.3) {
			velocity.x = -1.0f;
		}
		break;
	}

}


void Entity::Update(float deltaTime, Entity* player, Entity* tiles, int tileCount, Entity* enemies, int enemyCount)
{

	collidedTop = false;
	collidedBottom = false;
	collidedLeft = false;
	collidedRight = false;

	velocity += accerlation * deltaTime;

	if (entityType == ENEMY) {
		AI(player);
	}

	position.y += velocity.y * deltaTime;      
	CheckCollisionsY(tiles, tileCount);    

	if (entityType == PLAYER) {
		CheckCollisionsY(enemies, enemyCount);
	}

	position.x += velocity.x * deltaTime;      
	CheckCollisionsX(tiles, tileCount);    

	if (entityType == PLAYER) {
		CheckCollisionsX(enemies, enemyCount);
	}
}


void Entity::Render(ShaderProgram* program) {
	if (isActive) {
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, position);
		program->SetModelMatrix(modelMatrix);

		float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

		glBindTexture(GL_TEXTURE_2D, textureID);

		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);

		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program->texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
	}
}
