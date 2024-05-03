#pragma once
#include "../Level.h"

class Level3 : public Level {
public:
    // ————— METHODS ————— //
    Level3(int cap);

    void initialise() override;

    Entity* get_player() const override { return nullptr; }
};