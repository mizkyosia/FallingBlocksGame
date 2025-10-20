use bevy::{
    math::bounding::{Aabb2d, IntersectsVolume},
    prelude::*,
};

use crate::game::{BLOCK_HALF_SIZE, player::Player};

#[derive(Component, Clone)]
#[require(Transform)]
pub enum Block {
    Metal,
    Crate { hits_left: u8 },
}

impl Block {
    pub fn collide(
        &mut self,
        position: Vec2,
        player: &mut Player,
        box_x: &mut Aabb2d,
        box_y: &mut Aabb2d,
    ) {
        match self {
            Self::Metal => Self::block_collision(&position, player, box_x, box_y),
            Self::Crate { hits_left } => {
                Self::block_collision(&position, player, box_x, box_y);

                let collider = Aabb2d::new(position + BLOCK_HALF_SIZE.with_x(0.), vec2(12., 4.));

                if collider.intersects(box_x) || collider.intersects(box_y) {
                    *hits_left -= 1;
                }
            }
        }
    }

    fn block_collision(position: &Vec2, player: &Player, box_x: &mut Aabb2d, box_y: &mut Aabb2d) {
        let collider = Aabb2d::new(*position, super::BLOCK_HALF_SIZE);

        if collider.intersects(box_x) {
            box_x.min.x = player.bounding_box.min.x;
            box_x.max.x = player.bounding_box.max.x;
        }
        if collider.intersects(box_y) {
            box_x.min.y = player.bounding_box.min.y;
            box_x.max.y = player.bounding_box.max.y;
        }
    }
}

pub fn update(
    mut commands: Commands,
    mut blocks_query: Query<(Entity, &mut Block, &mut Transform)>,
    time: Res<Time<Fixed>>,
    mut spawned: Local<bool>,
) {
    if !*spawned {
        let translation = Vec3::ZERO.with_y(200.);

        commands.spawn((
            Transform::default().with_translation(translation),
            Block::Metal,
        ));
        *spawned = true;
    }

    for (entity, block, mut transform) in blocks_query.iter_mut() {
        transform.translation.y += super::GRAVITY
            * time.delta_secs()
            * match *block {
                Block::Crate { hits_left } => {
                    if hits_left == 0 {
                        commands.entity(entity).despawn();
                    }
                    1.0
                }
                Block::Metal => 1.5,
            };
    }
}
