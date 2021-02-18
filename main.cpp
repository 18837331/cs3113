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

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;

glm::mat4 viewMatrix, modelMatrixMove, modelMatrixRotate, projectionMatrix;

float player_x = 0;
float player_rotate = 0;

GLuint MoveTexture;
GLuint RotateTexture;


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

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrixMove = glm::mat4(1.0f);
    modelMatrixMove = glm::translate(modelMatrixMove, glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrixRotate = glm::mat4(1.0f);
    modelMatrixRotate = glm::translate(modelMatrixRotate, glm::vec3(-1.0f, 0.0f, 0.0f));
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 0.0f, 1.0f);
    
    
    MoveTexture = LoadTexture("ctg.png");
    RotateTexture = LoadTexture("Image.png");

    glUseProgram(program.programID);

    
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f;


void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks=ticks;

    player_x += 1.0f*deltaTime;
    player_rotate -= 90 * deltaTime;
    
    modelMatrixMove = glm::mat4(1.0f);
    modelMatrixMove = glm::translate(modelMatrixMove, glm::vec3(player_x, 0.0f, 0.0f));

    modelMatrixRotate = glm::mat4(1.0f);
    modelMatrixRotate = glm::rotate(modelMatrixRotate, glm::radians(player_rotate), glm::vec3(0.0f, 0.0f, 1.0f));

    //modelMatrix = glm::scale(modelMatrix, glm::vec3(1.001f, 1.0f, 1.0f));
}

void Render() { 
     

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };    
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 }; 

    glClear(GL_COLOR_BUFFER_BIT);    

    program.SetModelMatrix(modelMatrixMove); 

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices); 
    glEnableVertexAttribArray(program.positionAttribute); 
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords); 
    glEnableVertexAttribArray(program.texCoordAttribute); 

    glBindTexture(GL_TEXTURE_2D, MoveTexture); glDrawArrays(GL_TRIANGLES, 0, 6);    
    glDisableVertexAttribArray(program.positionAttribute);    
    glDisableVertexAttribArray(program.texCoordAttribute); 


    glBindTexture(GL_TEXTURE_2D, MoveTexture); glDrawArrays(GL_TRIANGLES, 0, 6);    
    glDisableVertexAttribArray(program.positionAttribute);    
    glDisableVertexAttribArray(program.texCoordAttribute); 

    program.SetModelMatrix(modelMatrixRotate);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, RotateTexture); glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);


    glBindTexture(GL_TEXTURE_2D, RotateTexture); glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    SDL_GL_SwapWindow(displayWindow); 
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