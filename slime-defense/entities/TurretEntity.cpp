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
#include "../Level.h"
#include "../Utility.h"
#include "SlimeEntity.h"
#include "TurretEntity.h"

TurretEntity::TurretEntity(Scene* scene) : Entity(scene) {
	set_scale(glm::vec3(0.5f, 0.5f, 0.0f));
	set_sprite_scale(glm::vec3(0.5f, 0.5f, 0.0f));
	m_texture_id = Utility::load_texture("assets/placeholder.png");
	m_range = 1.5f;
}

TurretEntity::~TurretEntity() {

}

void TurretEntity::update(float delta_time, Entity* collidable_entities, int collidable_entity_count, Map* map) {
	switch (m_ai_state) {
	case IDLE:
		// scan for a target
		for (int i = 0; i < get_scene()->m_entity_cap; i++) {
			Entity* other = get_scene()->m_state.entities[i];
			if (!other) continue;
			if (typeid(*other) == typeid(SlimeEntity) and glm::distance(get_position(), other->get_position()) <= m_range) {
				// lock on
				m_target = static_cast<SlimeEntity*>(other);
				m_ai_state = TRACKING;
			}
		}
		break;
	case TRACKING:
		if (glm::distance(get_position(), m_target->get_position()) > m_range) {
			// unlock if target moves out of range
			m_target = nullptr;
			m_ai_state = IDLE;
		} else if (m_shot_cooldown <= 0.0f) {
			// calculate shot direction
			float rangeFactor = glm::distance(get_position(), m_target->get_position()) / m_range;
			glm::vec3 predictedPos = m_target->get_position() + m_target->get_velocity() * rangeFactor * 0.28f;
			glm::vec3 targetDir = glm::normalize(predictedPos - get_position());
			// shoot a bullet
			Entity* bullet = get_scene()->spawn<Entity>(get_scene());
			bullet->set_lifetime(1.0f);
			bullet->set_collision(false);
			bullet->set_position(get_position());
			bullet->set_motion_type(Entity::TOP_DOWN);
			bullet->set_movement(targetDir);
			bullet->set_speed(2.5f);
			bullet->set_scale(glm::vec3(0.22f, 0.22f, 0.0f));
			bullet->set_sprite_scale(glm::vec3(0.22f, 0.22f, 0.0f));
			bullet->m_texture_id = Utility::load_texture("assets/placeholder.png");
			m_shot_cooldown = 0.5f;
		}
		break;
	default:
		break;
	}
	m_shot_cooldown -= delta_time;

	Entity::update(delta_time, collidable_entities, collidable_entity_count, map);
}