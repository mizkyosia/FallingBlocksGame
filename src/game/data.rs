use bevy::{
    ecs::component::Component,
    math::{Vec2, bounding::Aabb2d},
};

// Marker struct for the gameplay rect. The player and cubes cannot leave this rect
#[derive(Component)]
pub struct GameRect {
    pub bounds: Aabb2d,
}

pub const COYOTE_TIME: f32 = 0.05;
pub const JUMP_BUFFER_TIME: f32 = 0.05;
pub const JUMP_TIMER: f32 = 0.2;
pub const JUMP_FORCE: f32 = 200.;
pub const GRAVITY: f32 = -1000.0;

pub const BLOCK_SIZE: Vec2 = Vec2 { x: 16.0, y: 16.0 };
pub const BLOCK_HALF_SIZE: Vec2 = Vec2 { x: 8.0, y: 8.0 };

pub const MOVEMENT_SPEED: f32 = 150.;
