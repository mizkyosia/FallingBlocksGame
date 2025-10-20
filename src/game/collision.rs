use bevy::{math::bounding::Aabb2d, prelude::*};


#[derive(Clone)]
pub enum ColliderTrigger {
    None,
    Kill,
    Block,
}

#[derive(Component, Clone)]
pub struct Collider {
    pub trigger: ColliderTrigger,
    pub bounds: Aabb2d,
}

impl Collider {
    pub fn new(trigger: ColliderTrigger, center: Vec2, half_size: Vec2) -> Self {
        Self {
            trigger,
            bounds: Aabb2d::new(center, half_size),
        }
    }
}
