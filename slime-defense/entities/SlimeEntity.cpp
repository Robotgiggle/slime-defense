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
#include "../Scene.h"
#include "../Utility.h"
#include "SlimeEntity.h"

SlimeEntity::SlimeEntity(Scene* scene) : Entity(scene) {
	set_motion_type(TOP_DOWN);
	set_speed(1.0f);
	set_scale(glm::vec3(0.3f, 0.5f, 0.0f));
	set_sprite_scale(glm::vec3(0.3f, 0.5f, 0.0f));
	set_collision(false);
	m_texture_id = Utility::load_texture("assets/placeholder.png");
	m_ai_state = MOVE_UP;
	m_slime_type = BASIC;
	m_health = 10.0f;
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

	// turning logic
	for (int i = 0; i < get_scene()->m_local_info.turnPointCount; i++) {
		// check if we're near a turn point
		glm::vec3 turnPoint = get_scene()->m_local_info.turnPoints[i];
		if (abs(get_position().x - turnPoint.x) > 0.5 or abs(get_position().y - turnPoint.y) > 0.5) continue;
		
		// calculate the corner to turn around
		glm::vec3 turnCorner = turnPoint + m_turn_offsets[m_ai_state + int(turnPoint.z)];
		float xDist = abs(get_position().x - turnCorner.x);
		float yDist = abs(get_position().y - turnCorner.y);
		
		// if we're positioned correctly, make the turn
		if (abs(xDist - yDist) < 0.01f and m_turn_cooldown <= 0.0f) {
			m_turn_cooldown = 1.0f;
			m_ai_state = static_cast<AIState>((m_ai_state + 4 + (turnPoint.z ? -1 : 1)) % 4);
		}
	}

	// decrement turn cooldown
	if (m_turn_cooldown > 0) m_turn_cooldown -= delta_time;

	// check for death trigger
	if (m_health <= 0) {
		if (m_slime_type == SPLIT or m_slime_type == MULTIPLY) {
			// spawn new slimes
		}
		// reward points
		despawn();
	}

	// health regeneration
	if (m_slime_type == REGEN or m_slime_type == MULTIPLY) {
		m_health += 0.1f;
	}

	Entity::update(delta_time, collidable_entities, collidable_entity_count, map);
}