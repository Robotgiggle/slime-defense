#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include <vector>
#include "../glm/mat4x4.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../ShaderProgram.h"
#include "../Level.h"
#include "../Utility.h"
#include "SlimeEntity.h"
#include "PulseEntity.h"

PulseEntity::PulseEntity(Scene* scene, float radius, float damage, glm::vec3 tint) : Entity(scene) {
	set_collision(false);
	m_texture_id = Utility::load_texture("assets/range_circle.png");
	m_level = static_cast<Level*>(scene);
	m_tint = tint;
	m_damage = damage;
	m_max_radius = radius;
}

PulseEntity::~PulseEntity() {

}

void PulseEntity::update(float delta_time, Entity* collidable_entities, int collidable_entity_count, Map* map) {
	switch (m_ai_state) {
	case GROW:
		m_current_radius += 4.3f * delta_time;
		for (int i = 0; i < m_level->m_entity_cap; i++) {
			Entity* other = m_level->m_state.entities[i];
			if (!other) continue;
			if (typeid(*other) != typeid(SlimeEntity)) continue;
			SlimeEntity* slime = static_cast<SlimeEntity*>(other);
			// if the slime is in range and hasn't been damaged yet, damage it
			if (glm::distance(get_position(), other->get_position()) <= m_current_radius
				and std::find(m_damaged.begin(),m_damaged.end(),slime) == m_damaged.end()) {
				slime->change_health(-m_damage);
				m_damaged.push_back(slime);
			}
		}
		if (m_current_radius >= m_max_radius) {
			m_ai_state = SHRINK;
		}
		break;
	case SHRINK:
		m_current_radius -= 3.7f * delta_time;
		if (m_current_radius <= 0) {
			m_ai_state = SHRINK;
			despawn();
			return;
		}
		break;
	default:
		break;
	}

	set_sprite_scale(glm::vec3(2 * m_current_radius, 2 * m_current_radius, 0.0f));

	Entity::update(delta_time, collidable_entities, collidable_entity_count, map);
}

void PulseEntity::render(ShaderProgram* program) {
	program->set_tint(m_tint);
	Entity::render(program);
	program->no_tint();
}