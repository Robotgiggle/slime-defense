#pragma once
#include "../Entity.h"

class Level;

class SlimeEntity : public Entity {
private:
	enum AIState { MOVE_UP, MOVE_RIGHT, MOVE_DOWN, MOVE_LEFT };
	enum SlimeType { BASIC, REGEN, SPLIT, MULTIPLY, BOSS };
	AIState m_ai_state;
	SlimeType m_slime_type;
	Level* m_level;
	float m_max_health;
	float m_health;
	float m_base_length;
	float m_squish_factor = 0.0f;
	float m_turn_cooldown = 0.0f;
	int m_target_point_index = 0;

	const glm::vec3 m_turn_offsets[4] = {
		glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(-0.5f, -0.5f, 0.0f),
		glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 0.0f)
	};
public:
	SlimeEntity(Scene* scene, int type, float health, int dir);
	~SlimeEntity();

	// ————— CUSTOM UPDATE METHOD ————— //
	void update(float delta_time, Entity* collidable_entities, int collidable_entity_count, Map* map) override;

	// ————— GETTERS ————— //
	AIState   const get_ai_state()   const { return m_ai_state; };
	Level*    const get_level()      const { return m_level; };
	glm::vec3 const get_goal_point() const;
	int       const get_goal_index() const { return m_target_point_index; };

	// ————— SETTERS ————— //
	void const set_ai_state(AIState new_state) { m_ai_state = new_state; };
};
