#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include <vector>
#include <string>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Level.h"
#include "entities/SlimeEntity.h"
#include "entities/TurretEntity.h"

Level::Level(int cap) : Scene(cap) {}

void Level::initialise() {
	// ————— BASICS ————— //
	Scene::initialise();
	m_current_wave = -1;
	m_turret_cost = 2;
	m_lives = 10;
	m_money = 5;
	m_font_texture_id = Utility::load_texture("assets/display_font.png");

	// ————— HELD ITEM ————— //
	e_cursor_item = new Entity(this);

	e_cursor_item->set_scale(glm::vec3(0.5f, 0.5f, 0.0f));
	e_cursor_item->set_sprite_scale(glm::vec3(0.75f, 0.75f, 0.0f));
	e_cursor_item->set_collision(false);
	e_cursor_item->set_active(false);

	e_cursor_item->m_texture_id = Utility::load_texture("assets/turret_full.png");

	// ————— GAME MENU ————— //
	e_game_menu = new Entity(this);

	e_game_menu->set_position(glm::vec3(7.5f, 3.0f, 0.0f));
	e_game_menu->set_sprite_scale(glm::vec3(2.0f, 7.0f, 0.0f));
	e_game_menu->set_collision(false);

	e_game_menu->m_texture_id = Utility::load_texture("assets/game_menu.png");

	// ————— TURRET BUTTON ————— //
	e_turret_button = new Entity(this);

	e_turret_button->set_position(glm::vec3(7.16f, 3.66f, 0.0f));
	e_turret_button->set_sprite_scale(glm::vec3(0.85f, 0.85f, 0.0f));
	e_turret_button->set_collision(false);

	e_turret_button->m_texture_id = Utility::load_texture("assets/turret_button.png");
	e_turret_button->m_animation_indices = new int[2] { 0, 1 };
	e_turret_button->setup_anim(1, 2, 2, 0, 2, 1);

	// ————— NEXT-WAVE BUTTON ————— //
	e_next_button = new Entity(this);

	e_next_button->set_position(glm::vec3(7.5f, 0.25f, 0.0f));
	e_next_button->set_sprite_scale(glm::vec3(1.5f, 0.75f, 0.0f));
	e_next_button->set_collision(false);

	e_next_button->m_texture_id = Utility::load_texture("assets/next_button.png");
	e_next_button->m_animation_indices = new int[3] { 0, 1, 2 };
	e_next_button->setup_anim(1, 3, 3, 0, 2, 1);
}

void Level::process_event(SDL_Event event) {
	// process event triggers
	switch (event.type) {
	case SDL_MOUSEBUTTONDOWN:
		// process click triggers
		if (m_held_item == NONE) {
			if (Utility::touching_entity(m_global_info->mousePos, e_next_button, 0)) {
				// if the last wave is done, move to the next level
				if (m_current_wave == m_wave_count - 1) {
					m_global_info->changeScenes = true;
				}
				// start the next wave
				if (m_current_wave < 0 or m_waves[m_current_wave].slimes_left() == 0) {
					e_next_button->m_animation_index = 0;
					m_current_wave++;
				}
			} else if (Utility::touching_entity(m_global_info->mousePos, e_turret_button, 0) and m_money >= m_turret_cost) {
				// show a turret in the cursor
				m_held_item = TURRET;
				e_cursor_item->set_position(m_global_info->mousePos);
			} 
		} else {
			bool validPlacement = true;
			// check if any of the corners are outside placeable terrain
			float dzRadius = 0.44f;
			glm::vec3 dropZone[4] = {
				glm::vec3(-dzRadius, -dzRadius, 0.0f), glm::vec3(dzRadius, -dzRadius, 0.0f),
				glm::vec3(-dzRadius, dzRadius, 0.0f), glm::vec3(dzRadius, dzRadius, 0.0f),
			};
			for (const glm::vec3& corner : dropZone) {
				if (!m_state.map->is_solid(m_global_info->mousePos + corner)) validPlacement = false;
			}
			// check if you're trying to place on another turret
			for (int i = 0; i < m_entity_cap; i++) {
				Entity* other = m_state.entities[i];
				if (!other) continue;
				if (typeid(*other) != typeid(TurretEntity)) continue;
				if (e_cursor_item->check_collision(other)) validPlacement = false;
			}
			// if placement is still valid, spawn a new turret
			if (validPlacement) {
				m_money -= m_turret_cost;
				m_turret_cost = glm::min(++m_turret_cost, 9);
				if (m_money < m_turret_cost) e_turret_button->m_animation_index = 0;
				TurretEntity* newTurret = spawn<TurretEntity>(this);
				newTurret->set_position(m_global_info->mousePos);
			}
			// hide the cursor item
			m_held_item = NONE;
			e_cursor_item->set_active(false);
		}
		
		break;
	case SDL_KEYDOWN:
		// process keydown triggers
		switch (event.key.keysym.sym) {
		case SDLK_SPACE:
			// next button hotkey
			if (m_current_wave == m_wave_count - 1) {
				m_global_info->changeScenes = true;
			} else if (m_current_wave < 0 or m_waves[m_current_wave].slimes_left() == 0) {
				e_next_button->m_animation_index = 0;
				m_current_wave++;
			}
		case SDLK_ESCAPE:
			// cancel turret purchase
			m_held_item = NONE;
			e_cursor_item->set_active(false);
			break;
		case SDLK_r:
			// debug level reset
			this->initialise();
			break;
		default:
			break;
		}
	}
}

void Level::process_input()
{
	// event triggers are *NOT* handled in this function, unlike before
	// see process_event() for event handling

	// process held keys
	const Uint8* key_state = SDL_GetKeyboardState(NULL);
}

void Level::update(float delta_time) {
	// turn point safety
	if (m_turn_points[m_turn_point_count] == glm::vec3(0.0f)) {
		for (int i = m_turn_point_count; i < 10; i++) {
			m_turn_points[i] = e_path_end->get_position();
		}
	}

	// held item cursor tracking
	if (m_held_item != NONE) {
		e_cursor_item->set_active(true);
		e_cursor_item->set_position(m_global_info->mousePos);
	}
	
	// slime wave spawning
	if (m_current_wave > -1) {
		// count slimes in spawn zone
		int spawnDensity = 0;
		std::vector<Entity*> slimes;
		for (int i = 0; i < m_entity_cap; i++) {
			Entity* entity = m_state.entities[i];
			if (!entity) continue;
			if (typeid(*entity) != typeid(SlimeEntity)) continue;
			if (glm::distance(entity->get_position(), m_spawn_point) <= 0.8f) {
				slimes.push_back(entity);
				spawnDensity += 1;
			}
		}
	
		// if current density < wave density, spawn more slimes
		SlimeWave& wave = m_waves[m_current_wave];
		if (wave.slimes_left() > 0 and spawnDensity < wave.density) {
			// pick a spawn location and make sure it's not overlapping any existing slimes
			glm::vec3 offset = glm::vec3((rand() % 80 - 40) / 100.0f, (rand() % 70 - 35) / 100.0f, 0.0f);
			bool overlap = false;
			for (Entity* slime : slimes) {
				if (glm::distance(m_spawn_point + offset, slime->get_position()) <= 0.4f) overlap = true;
			}
			if (!overlap) {
				// pick a slime type from the ones remaining in the wave, and spawn it
				m_slimes_alive++;
				SlimeEntity* newSlime;
				while (true) {
					int type = rand() % 20;
					if (type < 6) {
						if (!wave.basics) continue;
						newSlime = spawn<SlimeEntity>(this, 0, 5.0f, m_start_dir);
						newSlime->set_position(m_spawn_point + offset);
						wave.basics--;
					}
					else if (type < 10) {
						if (!wave.regens) continue;
						newSlime = spawn<SlimeEntity>(this, 1, 5.0f, m_start_dir);
						newSlime->set_position(m_spawn_point + offset);
						wave.regens--;
					}
					else if (type < 14) {
						if (!wave.splits) continue;
						newSlime = spawn<SlimeEntity>(this, 2, 5.0f, m_start_dir);
						newSlime->set_position(m_spawn_point + offset);
						wave.splits--;
					}
					else if (type < 18) {
						if (!wave.elites) continue;
						newSlime = spawn<SlimeEntity>(this, 3, 5.0f, m_start_dir);
						newSlime->set_position(m_spawn_point + offset);
						wave.elites--;
					}
					else {
						if (!wave.bosses) continue;
						newSlime = spawn<SlimeEntity>(this, 4, 12.0f, m_start_dir);
						newSlime->set_position(m_spawn_point + offset);
						wave.bosses--;
					}
					break;
				}
			}
		}
	}

	Scene::update(delta_time);
}

void Level::render(ShaderProgram* program) {
	Scene::render(program);
	
	std::string livesDisplay = ((m_lives < 10) ? "%0" : "%") + std::to_string(std::max(0,m_lives));
	std::string moneyDisplay = ((m_money < 10) ? "$0" : "$") + std::to_string(m_money);
	std::string slimeDisplay = ((m_slimes_alive < 10) ? "&0" : "&") + std::to_string(m_slimes_alive);
	std::string turretCost = "$" + std::to_string(m_turret_cost);
	Utility::draw_text(program, m_font_texture_id, livesDisplay, 0.5f, -0.04f, glm::vec3(7.05f, 5.8f, 0.0f));
	Utility::draw_text(program, m_font_texture_id, moneyDisplay, 0.5f, -0.04f, glm::vec3(7.05f, 5.0f, 0.0f));
	Utility::draw_text(program, m_font_texture_id, slimeDisplay, 0.5f, -0.04f, glm::vec3(7.0f, 1.1f, 0.0f));
	Utility::draw_text(program, m_font_texture_id, turretCost, 0.35f, -0.03f, glm::vec3(7.81f, 3.66f, 0.0f));
}