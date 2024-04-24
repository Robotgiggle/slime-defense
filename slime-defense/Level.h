#pragma once
#include "Scene.h"

class Level : public Scene {
public:
	// ————— CUSTOM ENTITIES ————— //
	Entity*& e_path_end = m_state.entities[0];

	// ————— ATTRIBUTES ————— //
	glm::vec3 m_turn_points[10] = { glm::vec4(0.0f) };
	int m_turn_point_count = 0;
	int m_lives = 5;
	int m_money = 0;

	// ————— METHODS ————— //
	Level(int cap);
};