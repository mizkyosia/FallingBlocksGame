use bevy::{color::palettes::css::PURPLE, math::bounding::Aabb2d, prelude::*};

use crate::game::GameRect;

pub fn setup_game_rect(
    mut commands: Commands,
    mut meshes: ResMut<Assets<Mesh>>,
    mut materials: ResMut<Assets<ColorMaterial>>,
) {
    let transform = Transform::from_scale(Vec3::splat(480.));

    commands.spawn((
        GameRect {
            bounds: Aabb2d::new(transform.translation.xy(), transform.scale.xy() / 2.),
        },
        Mesh2d(meshes.add(Rectangle::default())),
        MeshMaterial2d(materials.add(Color::from(PURPLE))),
        transform,
    ));
}
