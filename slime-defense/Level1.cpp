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
#include "Utility.h"
#include "Level1.h"
#include "entities/SlimeEntity.h"
#include "entities/TurretEntity.h"

// terrain map
const int LV1_WIDTH = 9,
          LV1_HEIGHT = 7;
const int LV1_DATA[] = {
     1, 19, 19, 19, 19,  2, 12,  0,  0,
    13,  0,  0,  0,  0, 11, 12,  0,  0,
    13,  0,  4,  6,  0, 11, 12,  0,  0,
    13,  0, 11, 13,  0, 18, 19,  0,  0,
    13,  0, 11, 13,  0,  0,  0,  0,  0,
    13,  0, 11,  8,  5,  5,  5,  0,  0,
    13,  0, 11, 12, 12, 12, 12,  0,  0,
};

// sprite filepaths
const char PLACEHOLDER_FILEPATH[] = "assets/placeholder.png",
           MAP_TILES_FILEPATH[] = "assets/tileset.png";

// audio filepaths
const char MUSIC_FILEPATH[] = "assets/default_music.mp3";

// useful constants
const float ACC_OF_GRAVITY = -4.91f;

// constructor definition
Level1::Level1(int cap) : Level(cap) {}

// other methods
void Level1::initialise() {
    // ————— BASICS ————— //
    Level::initialise();
    m_next_scene_id = 1;

    // ————— TERRAIN ————— //
    GLuint map_texture_id = Utility::load_texture(MAP_TILES_FILEPATH);
    m_state.map = new Map(LV1_WIDTH, LV1_HEIGHT, LV1_DATA, map_texture_id, 1.0f, 7, 3);

    m_start_dir = 0;
    m_spawn_point = glm::vec3(1.0f, -1.5f, 0.0f);
    m_turn_points[0] = glm::vec3(1.0f, 5.0f, 0.0f);
    m_turn_points[1] = glm::vec3(4.0f, 5.0f, 0.0f);
    m_turn_points[2] = glm::vec3(4.0f, 2.0f, 1.0f);
    m_turn_point_count = 3;

    // ————— WAVES ————— //
    m_waves[0] = SlimeWave{ 1, 6, 0, 0, 0, 0 };
    m_waves[1] = SlimeWave{ 1, 7, 2, 2, 0, 0 };
    m_waves[2] = SlimeWave{ 2, 6, 3, 3, 0, 0 };
    m_waves[3] = SlimeWave{ 2, 6, 3, 2, 2, 0 };
    m_waves[4] = SlimeWave{ 3,12, 0, 0, 0, 1 };
    m_wave_count = 5;

    // ————— PATH END ————— //
    e_path_end = new Entity(this);

    e_path_end->set_position(glm::vec3(7.3f, 2.0f, 0.0f));
    e_path_end->set_scale(glm::vec3(0.4f, 2.0f, 0.0f));

    // ————— AUDIO ————— //
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_state.bgm = Mix_LoadMUS(MUSIC_FILEPATH);
    //Mix_PlayMusic(m_state.bgm, -1);
    //Mix_VolumeMusic(MIX_MAX_VOLUME / 3);

    m_state.jumpSfx = Mix_LoadWAV("assets/default_jump.wav");
    Mix_VolumeChunk(m_state.jumpSfx, MIX_MAX_VOLUME / 2);
}

//void Level1::update(float delta_time) {
//    Level::update(delta_time);
//}
//
//void Level1::render(ShaderProgram* program) {
//    Level::render(program);
//}