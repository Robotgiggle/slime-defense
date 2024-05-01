#pragma once
#include "../Scene.h"

class EndScreen : public Scene {
public:
    // ————— CUSTOM ENTITIES ————— //
    Entity*& e_exit_button = m_state.entities[0];

    // ————— ATTRIBUTES ————— //
    bool m_win;

    // ————— METHODS ————— //
    EndScreen(int cap, bool win);

    void initialise() override;
    void process_input() override;
    void process_event(SDL_Event event) override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

    Entity* get_player() const override { return nullptr; }
};