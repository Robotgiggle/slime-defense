#pragma once
#include "../Level.h"

class Level1 : public Level {
public:
    // ����� LEVEL-SPECIFIC ENTITIES ����� //


    // ����� METHODS ����� //
    Level1(int cap);

    void initialise() override;

    Entity* get_player() const override { return nullptr; }
};