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
#include "MainMenu.h"

// terrain map
const int LV1_WIDTH = 9,
LV1_HEIGHT = 7;
const int LV1_DATA[] = {
     0, 11, 12, 12, 12, 12, 12, 13,  0,
     0, 11, 12, 12, 12, 12, 12, 13,  0,
     0, 11, 12, 12, 12, 12, 12, 13,  0,
     0, 11, 12, 12, 12, 12, 12, 13,  0,
     0, 11, 12, 12, 12, 12, 12, 13,  0,
     0, 11, 12, 12, 12, 12, 12, 13,  0,
     0, 11, 12, 12, 12, 12, 12, 13,  0,
};

// constructor definition
MainMenu::MainMenu(int cap) : Scene(cap) {}

// other methods
void MainMenu::initialise() {
    // ————— BASICS ————— //
    Scene::initialise();
    m_next_scene_id = 2;
    m_unordered_render_start = 16;

    // ————— FONT ————— //
    m_font_texture_id = Utility::load_texture("assets/display_font.png");

    // ————— TERRAIN ————— //
    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_map = new Map(LV1_WIDTH, LV1_HEIGHT, LV1_DATA, map_texture_id, 1.0f, 7, 7);

    // ————— PLAY BUTTON ————— //
    e_play_button = new Entity(this);

    e_play_button->set_position(glm::vec3(4.0f, 2.45f, 0.0f));
    e_play_button->set_sprite_scale(glm::vec3(3.9f, 1.3f, 0.0f));
    e_play_button->m_texture_id = Utility::load_texture("assets/menu_button.png");

    e_play_button->update(0.0f, NULL, 0, m_map);

    // ————— INFO BUTTON ————— //
    e_info_button = new Entity(this);

    e_info_button->set_position(glm::vec3(4.0f, 0.65f, 0.0f));
    e_info_button->set_sprite_scale(glm::vec3(3.9f, 1.3f, 0.0f));
    e_info_button->m_texture_id = Utility::load_texture("assets/menu_button.png");

    e_info_button->update(0.0f, NULL, 0, m_map);

    // ————— FAKE SLIMES ————— //
    for (int i = 0; i < 16; i++) {
        Entity* fakeSlime = spawn<Entity>(this);

        fakeSlime->set_collision(false);
        fakeSlime->set_motion_type(Entity::TOP_DOWN);
        fakeSlime->set_sprite_scale(glm::vec3(0.42f, 0.56f, 0.0f));
        fakeSlime->set_speed(1.0f);
        fakeSlime->m_texture_id = Utility::load_texture("assets/base_slime.png");

        float xOffset = (rand() % 11 - 5) / 20.0f;
        if (i < 8) {
            fakeSlime->set_position(glm::vec3(7.95f + xOffset, float(i), 0.0f));
        } else {
            fakeSlime->set_position(glm::vec3(0.05f + xOffset, 6.0f - float(i-8), 0.0f));
        }
    }
}

void MainMenu::process_event(SDL_Event event) {
    switch (event.type) {
    case SDL_MOUSEBUTTONDOWN:
        if (Utility::touching_entity(m_global_info->mousePos, e_play_button, 0)) {
            Mix_PlayChannel(-1, m_global_info->clickSfx, 0);
            m_global_info->changeScenes = true;
        } else if (Utility::touching_entity(m_global_info->mousePos, e_info_button, 0)) {
            Mix_PlayChannel(-1, m_global_info->clickSfx, 0);
            m_next_scene_id = 1;
            m_global_info->changeScenes = true;
        }
        break;
    default:
        break;
    }
}

void MainMenu::process_input() {

}

void MainMenu::update(float delta_time) {
    for (int i = 0; i < 16; i++) {
        Entity* fakeSlime = m_entities[i];
        glm::vec3 pos = fakeSlime->get_position();

        float xOffset = (rand() % 11 - 5) / 20.0f;
        if (pos.x > 7.0f) {
            if (pos.y <= -1.0f) {
                fakeSlime->set_position(glm::vec3(0.05f + xOffset,-1.0f,0.0f));
                fakeSlime->move_up();
            } else {
                fakeSlime->move_down();
            }
        } else {
            if (pos.y >= 7.0f) {
                fakeSlime->set_position(glm::vec3(7.95f + xOffset, 7.0f, 0.0f));
                fakeSlime->move_down();
            }
            else {
                fakeSlime->move_up();
            }
        }
    }
    
    Scene::update(delta_time);
}

void MainMenu::render(ShaderProgram* program) {
    Scene::render(program);

    program->set_tint(glm::vec3(0.4f, 0.9f, 0.1f));
    for (int i = 0; i < 16; i++) {
        m_entities[i]->render(program);
    }
    program->no_tint();

    Utility::draw_text(program, m_font_texture_id, "SLIME", 1.0f, 0.0f, glm::vec3(2.0f, 5.3f, 0.0f));
    Utility::draw_text(program, m_font_texture_id, "DEFENSE", 0.8f, 0.0f, glm::vec3(1.62f, 4.1f, 0.0f));
    Utility::draw_text(program, m_font_texture_id, "PLAY", 0.55f, 0.0f, glm::vec3(3.18f, 2.45f, 0.0f));
    Utility::draw_text(program, m_font_texture_id, "INFO", 0.55f, 0.0f, glm::vec3(3.18f, 0.65f, 0.0f));
}