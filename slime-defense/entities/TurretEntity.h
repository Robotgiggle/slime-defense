#pragma once
#include "../Entity.h"

class TurretEntity : public Entity {
private:
	enum AIState { IDLE, TRACKING };
	enum TurretType { GUN, AOE };
	AIState m_ai_state = IDLE;
	TurretType m_turret_type;
	SlimeEntity* m_target = nullptr;
	Entity m_range_entity;
	Entity m_head_entity;
	Level* m_level;
	float m_range;
	float m_shot_cooldown = 0.0f;
public:
	TurretEntity(Scene* scene, bool aoe);
	~TurretEntity();

	//  CUSTOM UPDATE METHOD  //
	void update(float delta_time, Entity* collidable_entities, int collidable_entity_count, Map* map) override;
	void render(ShaderProgram* program) override;

	//  GETTERS  //
	AIState const get_ai_state() const { return m_ai_state; };

	//  SETTERS  //
	void const set_ai_state(AIState new_state) { m_ai_state = new_state; };
};