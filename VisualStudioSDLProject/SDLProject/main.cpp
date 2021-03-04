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

glm::mat4 viewMatrix, modelMatrix_R, modelMatrix_L, modelMatrix_B, projectionMatrix;

GLuint MoveTexture_R, MoveTexture_L, MoveTexture_B;

glm::vec3 R_position = glm::vec3(0, 0, 0);
glm::vec3 L_position = glm::vec3(0, 0, 0);
glm::vec3 R_movement = glm::vec3(0, 0, 0);
glm::vec3 L_movement = glm::vec3(0, 0, 0);

float player_speed = 3.0f;
glm::vec3 B_position = glm::vec3(0, 0, 0);
glm::vec3 B_movement = glm::vec3(0, 0, 0);

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
    displayWindow = SDL_CreateWindow("PONG!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix_R = glm::mat4(1.0f);
    modelMatrix_L = glm::mat4(1.0f);
    modelMatrix_B = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 0.0f, 1.0f);


    MoveTexture_R = LoadTexture("ctg.png");
    MoveTexture_L = LoadTexture("ctg.png");
    MoveTexture_B = LoadTexture("ctg.png");

    glUseProgram(program.programID);


    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

}

void ProcessInput() {
    R_movement = glm::vec3(0);
    L_movement = glm::vec3(0);
    SDL_Event event;    
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:            
            case SDL_WINDOWEVENT_CLOSE:                
                gameIsRunning = false;                
                break;            
        
        
            break; // SDL_KEYDOWN        
        }    
    }
    const Uint8* keys = SDL_GetKeyboardState(NULL); 

    if (keys[SDL_SCANCODE_UP]) {
        if (R_position.y < 3.75f) {
            R_movement.y = 1.0f;
        }
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        if (R_position.y > -3.75f) {
            R_movement.y = -1.0f;
        }
    }

    if (keys[SDL_SCANCODE_W]) {
        if (L_position.y < 3.75f) {
            L_movement.y = 1.0f;
        }
    }
    else if (keys[SDL_SCANCODE_S]) {
        if (L_position.y > -3.75f) {
            L_movement.y = -1.0f;
        }
    }

    if (keys[SDL_SCANCODE_SPACE]) {
        if (B_movement.x == 0.0f && B_movement.y == 0.0f) {
            B_movement.x = 1.0f;
            B_movement.y = 1.0f;
        }
    }

    if (glm::length(R_movement) > 1.0f) {
        R_movement = glm::normalize(R_movement);
    }
}

float lastTicks = 0.0f;


void Col_detection() {
    if ((B_position.y >= 3.75f && B_movement.y > 0.0f) || (B_position.y <= -3.75f && B_movement.y < 0.0f)) {
        B_movement.y = -B_movement.y;
    }
    if (B_position.x >= 4.9f) {
        if (abs(B_position.y - R_position.y) <= 1.0f && B_movement.x>0.0f) {
            B_movement.x = -B_movement.x;
        }
    }

    if (B_position.x <= -4.9f && B_movement.x < 0.0f) {

        if (abs(B_position.y - L_position.y) <= 1.0f) {
            B_movement.x = -B_movement.x;
        }
    }
    
}

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f; 
    float deltaTime = ticks - lastTicks; 
    lastTicks = ticks;// Add (direction * units per second * elapsed time)
    R_position += R_movement * player_speed * deltaTime;
    L_position += L_movement * player_speed * deltaTime;

    Col_detection();
    
    B_position += B_movement * player_speed * deltaTime;
    
    modelMatrix_R = glm::mat4(1.0f);
    modelMatrix_R = glm::translate(modelMatrix_R, R_position);
    modelMatrix_L = glm::mat4(1.0f);
    modelMatrix_L = glm::translate(modelMatrix_L, L_position);

    modelMatrix_B = glm::mat4(1.0f);
    modelMatrix_B = glm::translate(modelMatrix_B, B_position);
}

void Render() {
    modelMatrix_R = glm::translate(modelMatrix_R, glm::vec3(5.0f, 0.0f, 0.0f));
    modelMatrix_L = glm::translate(modelMatrix_L, glm::vec3(-5.0f, 0.0f, 0.0f));

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glClear(GL_COLOR_BUFFER_BIT);

    program.SetModelMatrix(modelMatrix_R);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, MoveTexture_R); glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);


    glBindTexture(GL_TEXTURE_2D, MoveTexture_R); glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    program.SetModelMatrix(modelMatrix_L);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, MoveTexture_L); glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);


    glBindTexture(GL_TEXTURE_2D, MoveTexture_L); glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    program.SetModelMatrix(modelMatrix_B);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, MoveTexture_B); glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);


    glBindTexture(GL_TEXTURE_2D, MoveTexture_B); glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);


    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

bool Gameover() {
    if (B_position.x >5.0f) {
        return true;
    }

    if (B_position.x <-5.0f) {
        return true;
    }
    return false;
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
        if (Gameover()) {
            break;
        }
    }

    Shutdown();
    return 0;
}
