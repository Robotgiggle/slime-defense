#pragma once
#define LOG(argument) std::cout << argument << '\n'
#include "Map.h"

class Scene;

class Entity {
public:
    enum MotionType { NONE, TOP_DOWN, SIDE_ON, SIDE_ON_Y, PHYSICS };
    enum AnimType { MOVING, ALWAYS, CUSTOM };
    enum { LEFT, RIGHT, UP, DOWN };

private:
    Scene* m_scene;
    int m_array_index = -1;
    bool m_loaded = false;
    bool m_is_active = true;
    bool m_has_collision = true;
    float m_despawn_timer = -1;
    MotionType m_motion_type = NONE;

    // ––––– PHYSICS ––––– //
    float     m_angle;
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    // ————— TRANSFORMATIONS ————— //
    float     m_speed;
    float     m_rot_speed;
    float     m_rotation;
    glm::vec3 m_movement;
    glm::vec3 m_scale;
    glm::vec3 m_sprite_scale;
    glm::mat4 m_model_matrix;

public:
    // ————— ANIMATION ————— //
    int* m_walking[4] = { NULL };
    int* m_animation_indices = NULL;

    int m_animation_frames = 0,
        m_animation_index = 0,
        m_animation_cols = 0,
        m_animation_rows = 0,
        m_frames_per_second = 4;
    float m_animation_time = 0.0f;
    AnimType m_animation_type = MOVING;

    // ––––– PHYSICS (JUMPING) ––––– //
    bool  m_is_jumping = false;
    float m_jumping_power = 0;

    // ––––– PHYSICS (COLLISIONS) ––––– //
    bool m_collided_top = false;
    bool m_collided_bottom = false;
    bool m_collided_left = false;
    bool m_collided_right = false;

    GLuint m_texture_id;

    // ————— METHODS ————— //
    Entity(Scene* scene);
    virtual ~Entity();
    void despawn();

    void setup_anim(int cols = 0, int rows = 0, int frames = 0, int fps = 4,
        int type = 0, int index = 0, float time = 0.0f);
    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    bool const check_collision(Entity* other) const;
    void const check_solid_collision(Entity* solid_entities, int solid_entity_count);
    void const check_solid_collision(Map* map);

    virtual void update(float delta_time, Entity* collidable_entities, int collidable_entity_count, Map* map);
    virtual void render(ShaderProgram* program);

    void move_left() { m_movement.x = -1.0f; };
    void move_right() { m_movement.x = 1.0f; };
    void move_up() { m_movement.y = 1.0f; };
    void move_down() { m_movement.y = -1.0f; };

    void rotate_clockwise() { m_rotation = -1.0f; };
    void rotate_anticlockwise() { m_rotation = 1.0f; };

    // ————— GETTERS ————— //
    MotionType const get_motion_type()   const { return m_motion_type; };
    glm::vec3  const get_position()      const { return m_position; };
    glm::vec3  const get_velocity()      const { return m_velocity; };
    glm::vec3  const get_acceleration()  const { return m_acceleration; };
    glm::vec3  const get_movement()      const { return m_movement; };
    Scene*     const get_scene()         const { return m_scene; };
    float      const get_rotation()      const { return m_rotation; };
    float      const get_angle()         const { return m_angle; };
    float      const get_rot_speed()     const { return m_rot_speed; };
    float      const get_speed()         const { return m_speed; };
    float      const get_width()         const { return m_scale.x; };
    float      const get_height()        const { return m_scale.y; };
    float      const get_sprite_width()  const { return m_sprite_scale.x; };
    float      const get_sprite_height() const { return m_sprite_scale.y; };
    bool       const get_active()        const { return m_is_active; };
    bool       const get_collision()     const { return m_has_collision; };
    int        const get_array_index()   const { return m_array_index; };

    // ————— SETTERS ————— //
    void const set_motion_type(MotionType new_type) { m_motion_type = new_type; };
    void const set_position(glm::vec3 new_position) { m_position = new_position; };
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; };
    void const set_acceleration(glm::vec3 new_position) { m_acceleration = new_position; };
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; };
    void const set_scale(glm::vec3 new_scale) { m_scale = new_scale; };
    void const set_sprite_scale(glm::vec3 new_scale) { m_sprite_scale = new_scale; };
    void const set_rotation(float new_rotation) { m_rotation = new_rotation; };
    void const set_angle(float new_angle) { m_angle = new_angle; };
    void const set_rot_speed(float new_rot_speed) { m_rot_speed = new_rot_speed; };
    void const set_speed(float new_speed) { m_speed = new_speed; };
    void const set_width(float new_width) { m_scale.x = new_width; };
    void const set_height(float new_height) { m_scale.y = new_height; };
    void const set_sprite_width(float new_width) { m_sprite_scale.x = new_width; };
    void const set_sprite_height(float new_height) { m_sprite_scale.y = new_height; };
    void const set_active(bool new_active) { m_is_active = new_active; };
    void const set_collision(bool new_collision) { m_has_collision = new_collision; };
    void const set_array_index(int new_index) { if (m_array_index == -1) m_array_index = new_index; };
    void const set_lifetime(float lifetime) { m_despawn_timer = lifetime; };
};
