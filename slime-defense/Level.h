#pragma once
#include "Scene.h"

class Level : public Scene {
public:
	// ————— CUSTOM ENTITIES ————— //
	Entity*& e_path_end = m_state.entities[0];
	Entity*& e_game_menu = m_state.entities[m_entity_cap-2];
	Entity*& e_cursor_item = m_state.entities[m_entity_cap - 1];

	// ————— ATTRIBUTES ————— //
	enum HeldItem { NONE, TURRET };
	HeldItem m_held_item;
	glm::vec3 m_turn_points[10] = { glm::vec4(0.0f) };
	int m_turn_point_count = 0;
	int m_lives = 5;
	int m_money = 0;

	// ————— METHODS ————— //
	Level(int cap);

	void initialise() override;
	void process_input() override;
	void process_event(SDL_Event event) override;
	void update(float delta_time) override;
	void render(ShaderProgram* program) override;

	Entity* get_player() const override { return nullptr; }
};