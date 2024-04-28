#define LOG(argument) std::cout << argument << '\n'
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Scene.h"
#include "Level1.h"

// ����� CONSTANTS ����� //

// window size
const int WINDOW_WIDTH = 648,
          WINDOW_HEIGHT = 504;
          //WINDOW_HEIGHT = 648;

// camera size
const float CAMERA_WIDTH = 9.0f,
            CAMERA_HEIGHT = 7.0f;
            //CAMERA_HEIGHT = 9.0f;

// background color
const float BG_RED = 0.50f,
            BG_BLUE = 0.50f,
            BG_GREEN = 0.50f,
            BG_OPACITY = 1.0f;

// viewport position & size
const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

// shader filepaths
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/tinted_fragment_textured.glsl";

// time constants
const float MILLISECONDS_IN_SECOND = 1000.0;
const float FIXED_TIMESTEP = 0.0166666f;

// scenes
const int NUM_OF_SCENES = 1;
Scene* const ALL_SCENES[] = {
    new Level1(100)
};

// ������VARIABLES ����� //

// core globals
SDL_Window* g_displayWindow;
ShaderProgram g_shaderProgram;
glm::mat4 g_viewMatrix, g_projectionMatrix;
Scene* g_currentScene;

// global values visible to scenes (defined in Scene.h)
GlobalInfo g_globalInfo;

// times
float g_previousTicks = 0.0f;
float g_timeAccumulator = 0.0f;

// ���� GENERAL FUNCTIONS ���� //
void startup_scene(int index) {
    g_currentScene = ALL_SCENES[index];
    g_currentScene->initialise();
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_displayWindow = SDL_CreateWindow("Slime Defense",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_displayWindow);
    SDL_GL_MakeCurrent(g_displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shaderProgram.load(V_SHADER_PATH, F_SHADER_PATH);

    g_viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, -3.0f, 0.0f));
    g_projectionMatrix = glm::ortho(-CAMERA_WIDTH/2, CAMERA_WIDTH/2, -CAMERA_HEIGHT/2, CAMERA_HEIGHT/2, -1.0f, 1.0f);

    g_shaderProgram.set_projection_matrix(g_projectionMatrix);
    g_shaderProgram.set_view_matrix(g_viewMatrix);
    g_shaderProgram.no_tint();

    glUseProgram(g_shaderProgram.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    for (Scene* scene : ALL_SCENES) scene->m_global_info = &g_globalInfo;
    startup_scene(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    // process event triggers, this loop needs to be done at the top level
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_globalInfo.gameIsRunning = false;
            break;
        default:
            // send other unprocessed events to the current scene
            g_currentScene->process_event(event);
            break;
        }
    }

    // process other stuff (like key holds)
    g_currentScene->process_input();

    // track mouse position
    int x, y;
    SDL_GetMouseState(&x, &y);
    g_globalInfo.mousePos.x = ((float(x) / WINDOW_WIDTH) * CAMERA_WIDTH) - 0.5f;
    g_globalInfo.mousePos.y = (((WINDOW_HEIGHT - float(y)) / WINDOW_HEIGHT) * CAMERA_HEIGHT) - 0.5f;
}

void update()
{
    // ����� DELTA TIME ����� //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
    float delta_time = ticks - g_previousTicks; // the delta time is the difference from the last frame
    g_previousTicks = ticks;

    // ����� FIXED TIMESTEP ����� //
    g_timeAccumulator += delta_time;
    if (g_timeAccumulator < FIXED_TIMESTEP) return;
    while (g_timeAccumulator >= FIXED_TIMESTEP)
    {
        // update scene, unless the game is paused
        if (!g_globalInfo.gamePaused) g_currentScene->update(FIXED_TIMESTEP);

        // if changeScene is set, move to the next scene
        if (g_globalInfo.changeScenes) startup_scene(g_currentScene->m_next_scene_id);

        g_timeAccumulator -= FIXED_TIMESTEP;
    }
}

void render()
{
    g_shaderProgram.set_view_matrix(g_viewMatrix);
    glClear(GL_COLOR_BUFFER_BIT);

    g_currentScene->render(&g_shaderProgram);

    SDL_GL_SwapWindow(g_displayWindow);
}

void shutdown() { 
    SDL_Quit();
    for (int i = 0; i < NUM_OF_SCENES; i++) delete ALL_SCENES[i];
}

// ������DRIVER GAME LOOP ����� /
int main(int argc, char* argv[])
{
    initialise();

    while (g_globalInfo.gameIsRunning)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}