#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Entity.h"

#define TILE_COUNT 20

bool win;

struct GameState {
    Entity* player;
    Entity* tiles;

};

GameState state;


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;

glm::mat4 viewMatrix, modelMatrix, projectionMatrix;



GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}

bool Gameover();

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("PONG!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 0.0f, 1.0f);

    glUseProgram(program.programID);


    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    state.player = new Entity();
    state.player->position = glm::vec3(0, 4, 0);
    state.player->accerlation = glm::vec3(0.0f, -0.3f, 0.0f);
    state.player->speed = 1.0f;
    state.player->textureID= LoadTexture("jump.png");

    state.tiles=new Entity[TILE_COUNT];

    GLuint tileTextureId = LoadTexture("tileset.png");

    state.tiles[0].textureID = LoadTexture("goal.png");;
    state.tiles[0].position = glm::vec3(-1.0f, -3.25f, 0.0f); //goal
    
    state.tiles[1].textureID = tileTextureId;
    state.tiles[1].position = glm::vec3(-2.0f, -3.25f, 0.0f); 

    state.tiles[2].textureID = tileTextureId;
    state.tiles[2].position = glm::vec3(-3.0f, -3.25f, 0.0f);

    state.tiles[3].textureID = tileTextureId;
    state.tiles[3].position = glm::vec3(-4.0f, -3.25f, 0.0f);

    state.tiles[4].textureID = tileTextureId;
    state.tiles[4].position = glm::vec3(-5.0f, -3.25f, 0.0f);

    state.tiles[5].textureID = tileTextureId;
    state.tiles[5].position = glm::vec3(1.0f, -3.25f, 0.0f);

    state.tiles[6].textureID = tileTextureId;
    state.tiles[6].position = glm::vec3(2.0f, -3.25f, 0.0f);

    state.tiles[7].textureID = tileTextureId;
    state.tiles[7].position = glm::vec3(3.0f, -3.25f, 0.0f);

    state.tiles[8].textureID = tileTextureId;
    state.tiles[8].position = glm::vec3(4.0f, -3.25f, 0.0f);

    state.tiles[9].textureID = tileTextureId;
    state.tiles[9].position = glm::vec3(5.0f, -3.25f, 0.0f);

    state.tiles[10].textureID = tileTextureId;
    state.tiles[10].position = glm::vec3(0.0f, -3.25f, 0.0f);

    state.tiles[11].textureID = tileTextureId;
    state.tiles[11].position = glm::vec3(-1.0f, 0.25f, 0.0f);

    state.tiles[12].textureID = tileTextureId;
    state.tiles[12].position = glm::vec3(-2.0f, 0.25f, 0.0f);

    state.tiles[13].textureID = tileTextureId;
    state.tiles[13].position = glm::vec3(-3.0f, 0.25f, 0.0f);

    state.tiles[14].textureID = tileTextureId;
    state.tiles[14].position = glm::vec3(-4.0f, 0.25f, 0.0f);

    state.tiles[15].textureID = tileTextureId;
    state.tiles[15].position = glm::vec3(-5.0f, 0.25f, 0.0f);


    state.tiles[16].textureID = tileTextureId;
    state.tiles[16].position = glm::vec3(0.0f, 0.25f, 0.0f);
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break; // SDL_KEYDOWN        
        }
    }
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {       
        state.player->accerlation.x = -1.0f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->accerlation.x = 1.0f;
       
    }
}

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position) {

    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;

    std::vector <float> vertices;
    std::vector <float> texCoords;

    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];

        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;

        texCoords.insert(texCoords.end(), { u,v + height, u + width, v + height, u + width, v, u, v + height, u + width, v, u, v });

        float offset = (size + spacing) * i;
        vertices.insert(vertices.end(), { offset + (-0.5f * size), (-0.5f * size),
            offset + (0.5f * size), (-0.5f * size),
            offset + (0.5f * size), (0.5f * size),
            offset + (-0.5f * size), (-0.5f * size),
            offset + (0.5f * size), (0.5f * size),
            offset + (-0.5f * size), (0.5f * size),
            });
    }

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);

    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 2.0f);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);

}


float lastTicks = 0.0f;
void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;// Add (direction * units per second * elapsed time)
    if (!Gameover()) {
        state.player->Update(deltaTime);
        for (int i = 0; i < 20; i++) {
            state.tiles[i].Update(deltaTime);
        }
    }

    else {
        state.player->accerlation = glm::vec3(0);
        state.player->velocity = glm::vec3(0);
    }
    

}



void Render() {
   
    glClear(GL_COLOR_BUFFER_BIT);

    state.player->Render(&program);

    for (int i = 0; i < 20; i++) {
        state.tiles[i].Render(&program);
    }

    if (Gameover()) {
        if (win) {
            DrawText(&program, LoadTexture("font1.png"), "success", 0.5f, -0.25f, glm::vec3(0.0, 0.0, 0));
        }
        else {
            DrawText(&program, LoadTexture("font1.png"), "fail", 0.5f, -0.25f, glm::vec3(0.0, 0.0, 0));
        }
    }
    SDL_GL_SwapWindow(displayWindow);
}

bool Gameover() {
    if (state.player->position.y < -2.25) {
        if (state.player->position.x >-1.5 && state.player->position.x<-0.5) {
            win = true;
        }
        else {
            win = false;
        }
        return true;
    }

    if (state.player->position.y < 1.25&& state.player->position.y>0.25) {
        if (state.player->position.x<0.8) {
            win = false;
            return true;
        }
        
    }


    
    return false;
}

void Shutdown() {
    SDL_Quit();
}



int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();

    }

    Shutdown();
    return 0;
}

