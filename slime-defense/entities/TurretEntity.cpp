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
#include "TurretEntity.h"

TurretEntity::TurretEntity(Scene* scene) : Entity(scene), m_gun_entity(scene) {
	// base setup
	set_scale(glm::vec3(0.5f, 0.5f, 0.0f));
	set_sprite_scale(glm::vec3(0.75f, 0.75f, 0.0f));
	m_texture_id = Utility::load_texture("assets/turret_base.png");
	m_level = static_cast<Level*>(scene);
	m_range = 1.6f;
	// gun setup
	m_gun_entity.set_position(scene->m_global_info->mousePos);
	m_gun_entity.set_sprite_scale(glm::vec3(0.75f, 0.75f, 0.0f));
	m_gun_entity.m_texture_id = Utility::load_texture("assets/turret_gun.png");
}

TurretEntity::~TurretEntity() {
	
}

void TurretEntity::update(float delta_time, Entity* collidable_entities, int collidable_entity_count, Map* map) {
	switch (m_ai_state) {
	case IDLE: {
		// scan for a target
		float lowestGoalDist = 50.0f;
		int furthestGoalIndex = 0;
		for (int i = 0; i < m_level->m_entity_cap; i++) {
			Entity* other = m_level->m_state.entities[i];
			if (!other) continue;
			if (typeid(*other) == typeid(SlimeEntity) and glm::distance(get_position(), other->get_position()) <= m_range) {
				SlimeEntity* slime = static_cast<SlimeEntity*>(other);
				// target whichever slime is furthest forward
				float distToGoal = glm::distance(slime->get_position(), slime->get_goal_corner());
				if (distToGoal < lowestGoalDist and slime->get_goal_index() >= furthestGoalIndex) {
					lowestGoalDist = distToGoal;
					furthestGoalIndex = slime->get_goal_index();
					m_target = slime;
					m_ai_state = TRACKING;
				}
			}
		}
		break; }
	case TRACKING: {
		if (glm::distance(get_position(), m_target->get_position()) > m_range) {
			// unlock if target moves out of range
			m_target = nullptr;
			m_ai_state = IDLE;
		}
		else {
			// aim at target entity
			float rangeFactor = glm::distance(get_position(), m_target->get_position()) / m_range;
			glm::vec3 predictedPos = m_target->get_position() + m_target->get_velocity() * rangeFactor * 0.3f;
			glm::vec3 targetDir = glm::normalize(predictedPos - get_position());
			m_gun_entity.set_angle(glm::degrees(atan2(targetDir.y, targetDir.x)));
			// if cooldown is done, shoot
			if (m_shot_cooldown <= 0.0f) {
				Entity* bullet = m_level->spawn<Entity>(m_level);
				bullet->set_lifetime(1.0f);
				bullet->set_collision(false);
				bullet->set_position(get_position());
				bullet->set_motion_type(Entity::TOP_DOWN);
				bullet->set_movement(targetDir);
				bullet->set_speed(2.5f);
				bullet->set_scale(glm::vec3(0.17f, 0.17f, 0.0f));
				bullet->set_sprite_scale(glm::vec3(0.17f, 0.17f, 0.0f));
				bullet->m_texture_id = Utility::load_texture("assets/bullet.png");
				m_shot_cooldown = 0.7f;
			}
		}
		break; }
	default:
		break;
	}
	m_shot_cooldown -= delta_time;

	Entity::update(delta_time, collidable_entities, collidable_entity_count, map);
	m_gun_entity.update(delta_time, collidable_entities, collidable_entity_count, map);
}

void TurretEntity::render(ShaderProgram* program) {
	Entity::render(program);
	m_gun_entity.render(program);
}