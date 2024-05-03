#pragma once
#include "../Level.h"

class Level2 : public Level {
public:
    // ————— METHODS ————— //
    Level2(int cap);

    void initialise() override;

    Entity* get_player() const override { return nullptr; }
};