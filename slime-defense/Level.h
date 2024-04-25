#pragma once
#include "Scene.h"

struct SlimeWave {
	int density;
	int basics;
	int regens;
	int splits;
	int multis;
	int bosses;
};

class Level : public Scene {
public:
	// ————— CUSTOM ENTITIES ————— //
	Entity*& e_path_end = m_state.entities[0];
	Entity*& e_game_menu = m_state.entities[m_entity_cap-2];
	Entity*& e_cursor_item = m_state.entities[m_entity_cap - 1];

	// ————— ATTRIBUTES ————— //
	enum HeldItem { NONE, TURRET };
	HeldItem m_held_item;
	glm::vec3 m_spawn_point = glm::vec3(0.0f);
	glm::vec3 m_turn_points[10] = { glm::vec3(0.0f) };
	SlimeWave m_waves[10] = { SlimeWave() };
	int m_turn_point_count = 0;
	int m_wave_count = 0;
	int m_current_wave = -1;
	int m_start_dir = 0;
	int m_lives = 5;
	int m_money = 0;
	
	// ————— METHODS ————— //
	Level(int cap);

	void initialise() override;
	void process_input() override;
	void process_event(SDL_Event event) override;
	void update(float delta_time) override;
	void render(ShaderProgram* program) override;

	bool is_empty(const SlimeWave& wave);

	Entity* get_player() const override { return nullptr; }
};