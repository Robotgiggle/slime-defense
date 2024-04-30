#pragma once
#include "../Scene.h"

class MainMenu : public Scene {
public:
    // ————— CUSTOM ENTITIES ————— //
    Entity*& e_play_button = m_state.entities[0];
    Entity*& e_info_button = m_state.entities[1];

    // ————— FONT ————— //
    GLuint m_font_texture_id;

    // ————— METHODS ————— //
    MainMenu(int cap);

    void initialise() override;
    void process_input() override;
    void process_event(SDL_Event event) override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

    Entity* get_player() const override { return nullptr; }
};