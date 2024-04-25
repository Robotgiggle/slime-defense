#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Level.h"
#include "entities/SlimeEntity.h"
#include "entities/TurretEntity.h"

Level::Level(int cap) : Scene(cap) {}

void Level::initialise() {
	// ����� BASICS ����� //
	Scene::initialise();

	// ����� HELD ITEM ����� //
	e_cursor_item = new Entity(this);

	e_cursor_item->set_scale(glm::vec3(0.5f, 0.5f, 0.0f));
	e_cursor_item->set_sprite_scale(glm::vec3(0.5f, 0.5f, 0.0f));
	e_cursor_item->set_collision(false);
	e_cursor_item->set_active(false);

	e_cursor_item->m_texture_id = Utility::load_texture("assets/placeholder.png");

	// ����� GAME MENU ����� //
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
				if (e_cursor_item->check_collision(other)) {
					LOG("other turret here");
					validPlacement = false;
				}
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
	
	Scene::update(delta_time);
}

void Level::render(ShaderProgram* program) {
	Scene::render(program);
}