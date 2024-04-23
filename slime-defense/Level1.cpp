#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Level1.h"
#include "Utility.h"
#include "entities/SlimeEntity.h"

// terrain map
const int LV1_WIDTH = 9,
          LV1_HEIGHT = 7;
const int LV1_DATA[] = {
    0, 0, 0, 0, 0, 0, 0, 1, 1,
    0, 2, 0, 0, 2, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 2, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1,
};

// sprite filepaths
const char SPRITESHEET_FILEPATH[] = "assets/default_player.png",
           NPC_FILEPATH[] = "assets/default_npc.png",
           MAP_TILES_FILEPATH[] = "assets/default_platform.png";

// audio filepaths
const char MUSIC_FILEPATH[] = "assets/default_music.mp3";

// useful constants
const float ACC_OF_GRAVITY = -4.91f;

// constructor definition
Level1::Level1(int cap) : Scene(cap) {}

// other methods
void Level1::initialise() {
    Scene::initialise();

    // ����� TERRAIN ����� //
    GLuint map_texture_id = Utility::load_texture(MAP_TILES_FILEPATH);
    m_state.map = new Map(LV1_WIDTH, LV1_HEIGHT, LV1_DATA, map_texture_id, 1.0f, 3, 1);

    m_local_info.turnPoints[0] = glm::vec3(1.0f, 5.0f, 0.0f);
    m_local_info.turnPoints[1] = glm::vec3(4.0f, 5.0f, 0.0f);
    m_local_info.turnPoints[2] = glm::vec3(4.0f, 2.0f, 1.0f);
    m_local_info.turnPointCount = 3;

    // ����� TEST SLIME ����� //
    e_slime = new SlimeEntity(this);
    e_slime->set_position(glm::vec3(1.0f, 0.0f, 0.0f));

    SlimeEntity* newSlime = spawn<SlimeEntity>(this);
    newSlime->set_position(glm::vec3(0.7f, -0.7f, 0.0f));

    newSlime = spawn<SlimeEntity>(this);
    newSlime->set_position(glm::vec3(1.3f, -1.0f, 0.0f));

    // ����� AUDIO ����� //
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_state.bgm = Mix_LoadMUS(MUSIC_FILEPATH);
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 3);

    m_state.jumpSfx = Mix_LoadWAV("assets/default_jump.wav");
    Mix_VolumeChunk(m_state.jumpSfx, MIX_MAX_VOLUME / 2);
}

void Level1::process_event(SDL_Event event) {
    // process event triggers
    switch (event.type) {
    case SDL_KEYDOWN:
        // process keydown triggers specifically
        switch (event.key.keysym.sym) {
        case SDLK_r:
            initialise();
            break;
        default:
            break;
        }
    }
}

void Level1::process_input()
{
    // event triggers are *NOT* handled in this function, unlike before
    // see process_event() for event handling

    // process held keys
    const Uint8* key_state = SDL_GetKeyboardState(NULL);
}

void Level1::update(float delta_time) {
    Scene::update(delta_time);
}

void Level1::render(ShaderProgram* program) {
    Scene::render(program);
}