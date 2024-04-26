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
	m_texture_id = Utility::load_texture("assets/test_slime.png");
	m_ai_state = static_cast<AIState>(dir);
	m_slime_type = static_cast<SlimeType>(type);
	m_level = static_cast<Level*>(scene);
	m_base_length = get_height();
	m_max_health = health;
	m_health = health;
}

SlimeEntity::~SlimeEntity() {

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

	// movment animation
	set_sprite_height(m_base_length * (1 + 0.08f*sin(m_squish_factor)));
	m_squish_factor += 8.5f * delta_time;
	if (m_squish_factor >= 360.0f) m_squish_factor -= 360.0f;

	// turning logic
	glm::vec3 turnPoint = m_level->m_turn_points[m_target_point_index];
	if (abs(get_position().x - turnPoint.x) <= 0.5 and abs(get_position().y - turnPoint.y) <= 0.5) {
		// calculate the corner to turn around
		glm::vec3 turnCorner = get_goal_point();
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

	// check for path end
	if (check_collision(m_level->e_path_end)) {
		m_level->m_lives -= 1;
		despawn();
		return;
	}

	// check other collisions
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
		if ((m_slime_type == SPLIT or m_slime_type == MULTIPLY) and m_max_health > 2.0f) {
			SlimeEntity* childSlime;
			for (int i = 0; i < 2; i++) {
				childSlime = m_level->spawn<SlimeEntity>(m_level, m_slime_type, floor(m_max_health / 2), m_ai_state);
				glm::vec3 spawnPos;
				while (true) {
					// find a valid location within 0.25 units of the parent
					spawnPos = get_position() + glm::vec3((rand() % 10 - 5) / 20.0f, (rand() % 10 - 5) / 20.0f, 0.0f);
					if (m_level->m_state.map->is_solid(spawnPos)) continue;
					break;
				}
				childSlime->set_position(spawnPos);
				childSlime->m_target_point_index = m_target_point_index;
				if (m_slime_type == MULTIPLY) childSlime->m_max_health = m_max_health;
			}
		}
		m_level->m_money += 1;
		despawn();
		return;
	}

	// health regeneration
	if ((m_slime_type == REGEN or m_slime_type == MULTIPLY) and m_health < m_max_health) {
		m_health += 0.005f;
	}

	Entity::update(delta_time, collidable_entities, collidable_entity_count, map);
}

glm::vec3 const SlimeEntity::get_goal_point() const {
	if (m_target_point_index > m_level->m_turn_point_count) {
		return m_level->e_path_end->get_position();
	} else {
		glm::vec3 turnPoint = m_level->m_turn_points[m_target_point_index];
		return turnPoint + m_turn_offsets[m_ai_state + int(turnPoint.z)];
	}
}