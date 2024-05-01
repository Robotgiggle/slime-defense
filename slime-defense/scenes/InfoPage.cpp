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
#include "InfoPage.h"

// terrain map
const int LV1_WIDTH = 9,
LV1_HEIGHT = 7;
const int LV1_DATA[] = {
    40, 46, 46, 46, 46, 46, 46, 46, 41,
    46, 46, 46, 46, 46, 46, 46, 46, 46,
    46, 46, 46, 46, 46, 46, 46, 46, 46,
    46, 46, 46, 46, 46, 46, 46, 46, 46,
    46, 46, 46, 46, 46, 46, 46, 46, 46,
    46, 46, 46, 46, 46, 46, 46, 46, 46,
    47, 46, 46, 46, 46, 46, 46, 46, 48,
};

// constructor definition
InfoPage::InfoPage(int cap) : Scene(cap) {}

// other methods
void InfoPage::initialise() {
    // ————— BASICS ————— //
    Scene::initialise();
    m_next_scene_id = 0;
    m_unordered_render_start = 5;

    // ————— FONT ————— //
    m_font_texture_id = Utility::load_texture("assets/display_font.png");

    // ————— TERRAIN ————— //
    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_state.map = new Map(LV1_WIDTH, LV1_HEIGHT, LV1_DATA, map_texture_id, 1.0f, 7, 7);

    // ————— INFO BUTTON ————— //
    e_back_button = new Entity(this);

    e_back_button->set_position(glm::vec3(4.0f, 0.32f, 0.0f));
    e_back_button->set_sprite_scale(glm::vec3(3.0f, 1.0f, 0.0f));
    e_back_button->m_texture_id = Utility::load_texture("assets/menu_button.png");

    e_back_button->update(0.0f, NULL, 0, m_state.map);

    // ————— FAKE SLIMES ————— //
    glm::vec3 slimePositions[5] = {
        glm::vec3(0.65f, 2.2f, 0.0f), glm::vec3(0.65f, 1.4f, 0.0f),
        glm::vec3(3.25f, 2.2f, 0.0f), glm::vec3(3.25f, 1.4f, 0.0f),
        glm::vec3(5.85f, 1.83f, 0.0f)
    };
    for (int i = 0; i < 5; i++) {
        Entity* fakeSlime = spawn<Entity>(this);

        fakeSlime->set_collision(false);
        fakeSlime->set_position(slimePositions[i]);
        if (i != 4) fakeSlime->set_sprite_scale(glm::vec3(0.42f, 0.56f, 0.0f));
        else fakeSlime->set_sprite_scale(glm::vec3(0.63f, 0.84f, 0.0f));
        fakeSlime->m_texture_id = Utility::load_texture("assets/base_slime.png");
    }
}

void InfoPage::process_event(SDL_Event event) {
    switch (event.type) {
    case SDL_MOUSEBUTTONDOWN:
        if (Utility::touching_entity(m_global_info->mousePos, e_back_button, 0)) {
            m_global_info->changeScenes = true;
        }
        break;
    default:
        break;
    }
}

void InfoPage::process_input() {

}

void InfoPage::update(float delta_time) {
    Scene::update(delta_time);
}

void InfoPage::render(ShaderProgram* program) {
    Scene::render(program);

    m_state.entities[0]->render_tinted(program, glm::vec3(0.4f, 0.9f, 0.1f));
    m_state.entities[1]->render_tinted(program, glm::vec3(1.0f, 0.35f, 0.35f));
    m_state.entities[2]->render_tinted(program, glm::vec3(0.05f, 0.6f, 1.0f));
    m_state.entities[3]->render_tinted(program, glm::vec3(0.90f, 0.54f, 0.96f));
    m_state.entities[4]->render_tinted(program, glm::vec3(1.0f, 0.68f, 0.15f));

    std::string lines[6] = {
        "PREVENT THE SLIMES FROM",
        "REACHING THE END OF THE",
        "PATH! DEFEAT THEM USING",
        "PLACED TOWERS. MONEY IS",
        "DROPPED BY EVERY SLIME,",
        "AND BUYS MORE TOWERS.  ",
    };
    for (int i = 0; i < 6; i++) {
        Utility::draw_text(program, m_font_texture_id, lines[i], 0.34f, 0.0f, glm::vec3(0.24f, 5.76f - i*0.56f, 0.0f));
    }
    Utility::draw_text(program, m_font_texture_id, "BASIC", 0.3f, 0.0f, glm::vec3(1.1f, 2.2f, 0.0f));
    Utility::draw_text(program, m_font_texture_id, "REGEN", 0.3f, 0.0f, glm::vec3(1.1f, 1.4f, 0.0f));
    Utility::draw_text(program, m_font_texture_id, "SPLIT", 0.3f, 0.0f, glm::vec3(3.7f, 2.2f, 0.0f));
    Utility::draw_text(program, m_font_texture_id, "ELITE", 0.3f, 0.0f, glm::vec3(3.7f, 1.4f, 0.0f));
    Utility::draw_text(program, m_font_texture_id, "BOSS", 0.33f, 0.0f, glm::vec3(6.43f, 1.83f, 0.0f));
    Utility::draw_text(program, m_font_texture_id, "BACK", 0.42f, 0.0f, glm::vec3(3.38f, 0.32f, 0.0f));
}