#pragma once
#include "../Entity.h"

class PulseEntity : public Entity {
private:
	enum AIState { GROW, SHRINK };
	AIState m_ai_state = GROW;
	glm::vec3 m_tint;
	Level* m_level;
	float m_damage;
	float m_max_radius;
	float m_current_radius = 0.0f;
public:
	PulseEntity(Scene* scene, float radius, float damage, glm::vec3 tint);
	~PulseEntity();

	// ————— CUSTOM UPDATE METHOD ————— //
	void update(float delta_time, Entity* collidable_entities, int collidable_entity_count, Map* map) override;
	void render(ShaderProgram* program) override;

	// ————— GETTERS ————— //
	AIState const get_ai_state() const { return m_ai_state; };

	// ————— SETTERS ————— //
	void const set_ai_state(AIState new_state) { m_ai_state = new_state; };
};