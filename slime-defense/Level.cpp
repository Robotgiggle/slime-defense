#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include <vector>
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

	// ————— HELD ITEM ————— //
	e_cursor_item = new Entity(this);

	e_cursor_item->set_scale(glm::vec3(0.5f, 0.5f, 0.0f));
	e_cursor_item->set_sprite_scale(glm::vec3(0.5f, 0.5f, 0.0f));
	e_cursor_item->set_collision(false);
	e_cursor_item->set_active(false);

	e_cursor_item->m_texture_id = Utility::load_texture("assets/placeholder.png");

	// ————— GAME MENU ————— //
	e_game_menu = new Entity(this);

	e_game_menu->set_position(glm::vec3(7.5f, 3.0f, 0.0f));
	e_game_menu->set_sprite_scale(glm::vec3(2.0f, 7.0f, 0.0f));
	e_game_menu->set_collision(false);

	e_game_menu->m_texture_id = Utility::load_texture("assets/placeholder.png");
}

void Level::process_event(SDL_Event event) {
	// process event triggers
	switch (event.type) {
	case SDL_MOUSEBUTTONDOWN:
		// process click triggers
		if (m_held_item == NONE) {
			if (Utility::touching_entity(m_global_info->mousePos, e_game_menu, 0)) {
				// show a turret in the cursor
				m_held_item = TURRET;
				e_cursor_item->set_position(m_global_info->mousePos);
			}
		} else {
			bool validPlacement = true;
			// check if any of the corners are outside placeable terrain
			float dzRadius = 0.41f;
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
			if (is_empty(m_waves[m_current_wave])) m_current_wave++;
			break;
		case SDLK_r:
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
		if (!is_empty(wave) and spawnDensity < wave.density) {
			// pick a spawn location and make sure it's not overlapping any existing slimes
			glm::vec3 offset = glm::vec3((rand() % 70 - 35) / 100.0f, (rand() % 70 - 35) / 100.0f, 0.0f);
			bool overlap = false;
			for (Entity* slime : slimes) {
				if (glm::distance(m_spawn_point + offset, slime->get_position()) <= 0.5f) overlap = true;
			}
			if (!overlap) {
				// pick a slime type from the ones remaining in the wave, and spawn it
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
						if (!wave.multis) continue;
						newSlime = spawn<SlimeEntity>(this, 3, 5.0f, m_start_dir);
						newSlime->set_position(m_spawn_point + offset);
						wave.multis--;
					}
					else {
						if (!wave.bosses) continue;
						newSlime = spawn<SlimeEntity>(this, 4, 10.0f, m_start_dir);
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
}

bool Level::is_empty(const SlimeWave& wave) {
	return (!wave.basics and !wave.regens and !wave.splits and !wave.multis and !wave.bosses);
}