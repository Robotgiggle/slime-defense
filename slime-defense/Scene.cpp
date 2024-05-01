#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Scene.h"

Scene::Scene(int cap) : m_entities(new Entity*[cap]{}), m_entity_cap(cap) {}

Scene::~Scene() {
    for (int i = 0; i < m_entity_cap; i++) delete m_entities[i];
    delete[] m_entities;
    delete m_map;
}

void Scene::initialise() {
    for (int i = 0; i < m_entity_cap; i++) {
        delete m_entities[i];
        m_entities[i] = nullptr;
    }
}

void Scene::update(float delta_time) {
    if (m_timer > 0.0f) m_timer -= delta_time;
    for (int i = 0; i < m_entity_cap; i++) {
        if (m_entities[i]) {
            m_entities[i]->update(delta_time, NULL, 0, m_map);
        }
    }
}

void Scene::render(ShaderProgram* program) {
    m_map->render(program);
    for (int i = m_unordered_render_start; i < m_entity_cap; i++) {
        if (m_entities[i]) {
            m_entities[i]->render(program);
        }
    }
}