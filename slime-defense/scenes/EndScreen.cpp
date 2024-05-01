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
#include "EndScreen.h"

// terrain map
const int LV1_WIDTH = 9,
LV1_HEIGHT = 7;
const int LV1_DATA[] = {
    12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12,
};

// constructor definition
EndScreen::EndScreen(int cap) : Scene(cap) {}

// other methods
void EndScreen::initialise() {
    // ————— BASICS ————— //
    Scene::initialise();
    m_next_scene_id = -1;

    // ————— FONT ————— //
    m_font_texture_id = Utility::load_texture("assets/display_font.png");

    // ————— TERRAIN ————— //
    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_map = new Map(LV1_WIDTH, LV1_HEIGHT, LV1_DATA, map_texture_id, 1.0f, 7, 7);

    // ————— EXIT BUTTON ————— //
    e_exit_button = new Entity(this);

    e_exit_button->set_position(glm::vec3(4.0f, 0.5f, 0.0f));
    e_exit_button->set_sprite_scale(glm::vec3(3.0f, 1.0f, 0.0f));
    e_exit_button->m_texture_id = Utility::load_texture("assets/menu_button.png");

    e_exit_button->update(0.0f, NULL, 0, m_map);
}

void EndScreen::process_event(SDL_Event event) {
    switch (event.type) {
    case SDL_MOUSEBUTTONDOWN:
        if (Utility::touching_entity(m_global_info->mousePos, e_exit_button, 0)) {
            m_global_info->gameIsRunning = false;
        }
        break;
    default:
        break;
    }
}

void EndScreen::process_input() {

}

void EndScreen::update(float delta_time) {
    Scene::update(delta_time);
}

void EndScreen::render(ShaderProgram* program) {
    Scene::render(program);

    int kills = m_global_info->slimesKilled;
    int hurts = m_global_info->livesLost;
    std::string killsDisplay = ((kills < 10)? "00" : (kills < 100)? "0" : "") + std::to_string(kills);
    std::string hurtsDisplay = ((hurts < 10)? "0" : "") + std::to_string(hurts);

    if (m_global_info->playerDied) {
        Utility::draw_text(program, m_font_texture_id, "YOU DIED!", 0.9f, 0.0f, glm::vec3(0.58f, 5.2f, 0.0f));
    } else {
        Utility::draw_text(program, m_font_texture_id, "YOU WIN!", 1.0f, 0.0f, glm::vec3(0.7f, 5.2f, 0.0f));
    }
    Utility::draw_text(program, m_font_texture_id, "SLIMES KILLED:"+killsDisplay, 0.4f, 0.0f, glm::vec3(0.8f, 3.5f, 0.0f));
    Utility::draw_text(program, m_font_texture_id, "LIVES LOST:"+hurtsDisplay, 0.45f, 0.0f, glm::vec3(1.3f, 2.3f, 0.0f));
    Utility::draw_text(program, m_font_texture_id, "EXIT", 0.42f, 0.0f, glm::vec3(3.38f, 0.5f, 0.0f));
}