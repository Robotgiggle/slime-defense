#pragma once
#include "Level.h"

class Level1 : public Level {
public:
    // ————— LEVEL-SPECIFIC ENTITIES ————— //
    

    // ————— METHODS ————— //
    Level1(int cap);

    void initialise() override;
    void process_input() override;
    void process_event(SDL_Event event) override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

    Entity* get_player() const override { return nullptr; }
};