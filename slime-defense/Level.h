#pragma once
#include "Scene.h"

struct SlimeWave {
	int density;
	int basics;
	int regens;
	int splits;
	int elites;
	int bosses;
	int slimes_left() const {
		return basics + regens + splits + elites + bosses;
	}
};

class Level : public Scene {
public:
	// ————— CUSTOM ENTITIES ————— //
	Entity*& e_path_end = m_entities[0];
	Entity*& e_cursor_range = m_entities[1];
	Entity*& e_cursor_item = m_entities[2];
	Entity*& e_game_menu = m_entities[m_entity_cap-4];
	Entity*& e_gun_turret_button = m_entities[m_entity_cap - 3];
	Entity*& e_aoe_turret_button = m_entities[m_entity_cap - 2];
	Entity*& e_next_button = m_entities[m_entity_cap - 1];

	// ————— ATTRIBUTES ————— //
	// terrain
	glm::vec3 m_spawn_point = glm::vec3(0.0f);
	glm::vec3 m_turn_points[10] = { glm::vec3(0.0f) };
	int m_turn_point_count;
	int m_start_dir;
	// waves
	SlimeWave m_waves[10] = { SlimeWave() };
	int m_wave_count;
	int m_current_wave;
	// turrets
	enum HeldItem { NONE, GUN_TURRET, AOE_TURRET };
	HeldItem m_held_item;
	int m_turret_cost;
	int m_money;
	// other
	int m_slimes_alive;
	int m_lives;
	
	// ————— METHODS ————— //
	Level(int cap);

	void initialise() override;
	void process_input() override;
	void process_event(SDL_Event event) override;
	void update(float delta_time) override;
	void render(ShaderProgram* program) override;

	bool check_placement_validity();

	Entity* get_player() const override { return nullptr; }
};