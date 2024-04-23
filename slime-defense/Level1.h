#pragma once
#include "Scene.h"

class Level1 : public Scene {
public:
    // ————— CUSTOM ENTITIES ————— //
    Entity*& e_slime = m_state.entities[0];

    // ————— METHODS ————— //
    Level1(int cap);

    void initialise() override;
    void process_input() override;
    void process_event(SDL_Event event) override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

    Entity* get_player() const override { return nullptr; }
};