#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "../glm/mat4x4.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../ShaderProgram.h"
#include "../Level.h"
#include "../Utility.h"
#include "SlimeEntity.h"

SlimeEntity::SlimeEntity(Scene* scene, int type, float health, int dir) : Entity(scene) {
	set_motion_type(TOP_DOWN);
	set_speed(0.8f);
	set_scale(glm::vec3(0.36f, 0.48f, 0.0f) * (health/10.0f + 0.5f));
	set_sprite_scale(glm::vec3(0.36f, 0.48f, 0.0f) * (health/10.0f + 0.5f));
	set_collision(false);
	m_texture_id = Utility::load_texture("assets/base_slime.png");
	m_ai_state = static_cast<AIState>(dir);
	m_slime_type = static_cast<SlimeType>(type);
	m_level = static_cast<Level*>(scene);
	m_max_health = health;
	m_health = health;
	switch (m_slime_type) {
	case BASIC: m_tint    = glm::vec3(0.4f, 0.9f, 0.1f); break;
	case REGEN: m_tint    = glm::vec3(1.0f, 0.35f, 0.35f); break;
	case SPLIT: m_tint    = glm::vec3(0.05f, 0.6f, 1.0f); break;
	case ELITE: m_tint    = glm::vec3(0.90f, 0.54f, 0.96f); break;
	case BOSS: m_tint     = glm::vec3(1.0f, 0.68f, 0.15f); break;
	default: m_tint       = glm::vec3(0.65f, 0.65f, 0.65f); break;
	}
}

SlimeEntity::~SlimeEntity() {
	// decrement slime count
	m_level->m_slimes_alive--;
	// if this was the last slime, enable the next-wave or next-level button
	if (m_level->m_slimes_alive == 0) {
		if (m_level->m_current_wave == m_level->m_wave_count - 1) {
			m_level->e_next_button->m_animation_index = 1;
		} else {
			m_level->e_next_button->m_animation_index = 0;
		}
	}
}

void SlimeEntity::update(float delta_time, Entity* collidable_entities, int collidable_entity_count, Map* map) {
	// basic motion
	set_movement(glm::vec3(0.0f));
	switch (m_ai_state) {
	case MOVE_UP:
		set_angle(0);
		move_up();
		break;
	case MOVE_RIGHT:
		set_angle(-90);
		move_right();
		break;
	case MOVE_DOWN:
		set_angle(180);
		move_down();
		break;
	case MOVE_LEFT:
		set_angle(90);
		move_left();
		break;
	default:
		break;
	}

	// course correction
	glm::vec3 turnPoint = m_level->m_turn_points[m_target_point_index];
	if (m_ai_state == MOVE_UP or m_ai_state == MOVE_DOWN) {
		if (get_position().x < turnPoint.x - 0.5f) move_right();
		else if (get_position().x > turnPoint.x + 0.5f) move_left();
	} else {
		if (get_position().y < turnPoint.y - 0.5f) move_up();
		else if (get_position().y > turnPoint.y + 0.5f) move_down();
	}

	// check for path end
	if (check_collision(m_level->e_path_end)) {
		m_level->m_lives -= 1;
		despawn();
		return;
	}

	// turning logic
	if (abs(get_position().x - turnPoint.x) <= 0.5 and abs(get_position().y - turnPoint.y) <= 0.5) {
		// calculate the corner to turn around
		glm::vec3 turnCorner = get_goal_corner();
		float xDist = abs(get_position().x - turnCorner.x);
		float yDist = abs(get_position().y - turnCorner.y);

		// if we're positioned correctly, make the turn
		if (abs(xDist - yDist) < 0.01f and m_turn_cooldown <= 0.0f) {
			m_turn_cooldown = 1.0f;
			m_target_point_index++;
			m_ai_state = static_cast<AIState>((m_ai_state + 4 + (turnCorner.z ? -1 : 1)) % 4);
		}
	}
	if (m_turn_cooldown > 0) m_turn_cooldown -= delta_time;

	// check for bullet collision
	for (int i = 0; i < m_level->m_entity_cap; i++) {
		Entity* other = m_level->m_state.entities[i];
		if (!other) continue;

		if (check_collision(other) and typeid(*other) != typeid(SlimeEntity) and glm::length(other->get_movement()) > 0) {
			// if it's not a slime and it's moving, it's a bullet
			m_health -= 1;
			other->despawn();
		}
	}

	// check for death trigger
	if (m_health <= 0) {
		// if this was a splitter or an elite, spawn new slimes
		if ((m_slime_type == SPLIT or m_slime_type == ELITE) and m_max_health > 2.0f) {
			bool elite = m_slime_type == ELITE;
			SlimeEntity* childSlime;
			for (int i = 0; i < m_slime_type; i++) {
				glm::vec3 spawnPos;
				while (true) {
					// find a valid location within 0.3 units of the parent
					spawnPos = get_position() + glm::vec3((rand() % 10 - 5) / 17.0f, (rand() % 10 - 5) / 17.0f, 0.0f);
					if (m_level->m_state.map->is_solid(spawnPos)) continue;
					break;
				}
				if (elite) {
					childSlime = m_level->spawn<SlimeEntity>(m_level, REGEN, floor(m_max_health / 2), m_ai_state);
					childSlime->m_max_health = m_max_health;
					childSlime->m_tint = m_tint;
				} else {
					childSlime = m_level->spawn<SlimeEntity>(m_level, SPLIT, floor(m_max_health / 2), m_ai_state);
				}
				childSlime->set_position(spawnPos);
				childSlime->m_target_point_index = m_target_point_index;
				m_level->m_slimes_alive++;
			}
		}
		// basic death effects
		m_level->m_money++;
		despawn();
		return;
	}

	// health regeneration
	if (m_slime_type != BASIC and m_slime_type != SPLIT and m_health < m_max_health) {
		if (m_slime_type == BOSS) m_health += 0.04f;
		else m_health += 0.005f;
	}

	// adjust size based on health
	float healthFactor = (m_health / 10.0f + 0.5f);
	set_scale(glm::vec3(0.36f, 0.48f, 0.0f)* healthFactor);
	set_sprite_scale(glm::vec3(0.36f, 0.48f, 0.0f)* healthFactor);

	// movment animation
	set_sprite_height(get_sprite_height()* (1 + 0.08f * sin(m_squish_factor)));
	m_squish_factor += 8.5f * delta_time;
	if (m_squish_factor >= 360.0f) m_squish_factor -= 360.0f;

	Entity::update(delta_time, collidable_entities, collidable_entity_count, map);
}

void SlimeEntity::render(ShaderProgram* program) {
	program->set_tint(m_tint);
	Entity::render(program);
	program->no_tint();
}

glm::vec3 const SlimeEntity::get_goal_corner() const {
	glm::vec3 turnPoint = m_level->m_turn_points[m_target_point_index];
	return turnPoint + m_turn_offsets[m_ai_state + int(turnPoint.z)];
}