#pragma once
#include <SDL_mixer.h>
#include "Entity.h"
#include "Map.h"

struct GlobalInfo {
    bool gameIsRunning = true;
    bool changeScenes = false;
    bool playerDied = false;
    bool gamePaused = false;
    int slimesKilled = 0;
    int livesLost = 0;
    glm::vec3 mousePos;
    Mix_Chunk* clickSfx;
    Mix_Chunk* placeSfx;
    Mix_Chunk* ouchSfx;
    Mix_Chunk* killSfx;
};

class Scene {
protected:
    // enforces abstractness without having any pure virtuals
    Scene(int cap);
public:
    // ————— ATTRIBUTES ————— //
    GLuint m_font_texture_id;
    GlobalInfo* m_global_info;
    Entity** m_entities;
    Map* m_map;
    const int m_entity_cap;
    int m_next_scene_id;
    int m_unordered_render_start = 0;
    float m_timer = 0.0f;

    // ————— VIRTUAL METHODS ————— //
    virtual void initialise();
    virtual void process_input() { return; }
    virtual void process_event(SDL_Event event) { return; }
    virtual void update(float delta_time);
    virtual void render(ShaderProgram* program);

    // ————— CONCRETE METHODS ————— //
    ~Scene();
    template <class EntityType, class... SpawnArgs>
    EntityType* spawn(Scene* scene, SpawnArgs... args) {
        for (int i = 0; i < m_entity_cap; i++) {
            if (m_entities[i]) continue;
            EntityType* newEntity = new EntityType(scene, args...);
            newEntity->set_array_index(i);
            m_entities[i] = newEntity;
            return newEntity;
        }
        std::cout << "Spawn failed, cap of " << m_entity_cap << " entities is full!" << std::endl;
        return nullptr;
    }

    // ————— GETTERS ————— //
    virtual Entity* get_player() const = 0;
};