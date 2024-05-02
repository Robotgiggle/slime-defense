#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "../glm/mat4x4.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../ShaderProgram.h"
#include "../Utility.h"
#include "../entities/SlimeEntity.h"
#include "../entities/TurretEntity.h"
#include "Level3.h"

// terrain map
const int LV1_WIDTH = 7,
          LV1_HEIGHT = 7;
const int LV1_DATA[] = {
    12, 13,  0, 11, 12, 12, 12,
    12, 13,  0, 18, 19, 19,  2,
    12, 13,  0,  0,  0,  0, 11,
    19, 30, 24,  5,  6,  0, 11,
     0,  0, 18, 19, 20,  0, 11,
     6,  0,  0,  0,  0,  0, 11,
     8,  5,  5,  5,  5,  5,  9,
};

// constructor definition
Level3::Level3(int cap) : Level(cap) {}

// other methods
void Level3::initialise() {
    // ————— BASICS ————— //
    Level::initialise();
    m_next_scene_id = 5;

    // ————— TERRAIN ————— //
    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_map = new Map(LV1_WIDTH, LV1_HEIGHT, LV1_DATA, map_texture_id, 1.0f, 7, 7);

    m_start_dir = 1;
    m_spawn_point = glm::vec3(-1.5f, 2.0f, 0.0f);
    m_turn_points[0] = glm::vec3(1.0f, 2.0f, 0.0f);
    m_turn_points[1] = glm::vec3(1.0f, 1.0f, 1.0f);
    m_turn_points[2] = glm::vec3(5.0f, 1.0f, 1.0f);
    m_turn_points[3] = glm::vec3(5.0f, 4.0f, 1.0f);
    m_turn_points[4] = glm::vec3(2.0f, 4.0f, 0.0f);
    m_turn_point_count = 5;

    // ————— WAVES ————— //
    m_waves[0] = SlimeWave{ 1, 6, 0, 0, 0, 0 };
    m_waves[1] = SlimeWave{ 1, 7, 2, 2, 0, 0 };
    m_waves[2] = SlimeWave{ 2, 6, 3, 3, 0, 0 };
    m_waves[3] = SlimeWave{ 2, 6, 3, 2, 2, 0 };
    m_waves[4] = SlimeWave{ 3,12, 0, 0, 0, 1 };
    m_wave_count = 5;

    // ————— PATH END ————— //
    e_path_end = new Entity(this);

    e_path_end->set_position(glm::vec3(2.0f, 7.3f, 0.0f));
    e_path_end->set_scale(glm::vec3(2.0f, 0.4f, 0.0f));
}