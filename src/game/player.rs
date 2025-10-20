use bevy::{
    color::palettes::css::{GREEN, ORANGE},
    math::bounding::{Aabb2d, BoundingVolume},
    prelude::*,
};

use crate::{
    GameState,
    game::{
        GameRect,
        blocks::Block,
        inputs::{ActionMap, GameAction},
    },
};

#[derive(Debug, Component)]
pub struct Player {
    pub velocity: Vec2,
    pub is_jumping: bool,
    pub on_ground: bool,
    // Used for jump buffering
    pub jump_buffer_ts: f32,
    // Used for coyote time
    pub last_ground_ts: f32,
    // Timestamp for the actual start of the jump (in case of coyote time or jump buffering)
    pub real_jump_start_ts: f32,
    pub alive: bool,
    pub bounding_box: Aabb2d,
}

impl Player {}

const PLAYER_HALF_SIZE: Vec2 = Vec2::splat(16.);

pub fn setup(
    mut commands: Commands,
    mut meshes: ResMut<Assets<Mesh>>,
    mut materials: ResMut<Assets<ColorMaterial>>,
) {
    commands.spawn((
        Mesh2d(meshes.add(Rectangle::default())),
        MeshMaterial2d(materials.add(Color::from(ORANGE))),
        Transform::default().with_scale(vec3(PLAYER_HALF_SIZE.x * 2., PLAYER_HALF_SIZE.y * 2., 1.)),
        DespawnOnExit(GameState::Game),
        Player {
            bounding_box: Aabb2d::new(Vec2::ZERO, Vec2::ZERO),
            velocity: Vec2::ZERO,
            is_jumping: false,
            on_ground: false,
            jump_buffer_ts: 0.,
            last_ground_ts: 0.,
            real_jump_start_ts: 0.,
            alive: true,
        },
    ));
}

pub fn movement(
    mut player_query: Query<(&mut Transform, &mut Player)>,
    mut blocks_query: Query<(&mut Block, &Transform), Without<Player>>,
    game_rect: Single<&GameRect>,
    actions: Res<ActionMap>,
    inputs: Res<ButtonInput<KeyCode>>,
    time: Res<Time<Fixed>>,
    mut gizmos: Gizmos,
) {
    let (mut transform, mut player) = player_query.single_mut().unwrap();

    player.is_jumping |= actions.pressed(GameAction::Jump, &inputs);

    // Is a jump supposed to start this frame ? regardless of player position
    let jumping_start = if actions.just_pressed(GameAction::Jump, &inputs) {
        // If the player is on ground or in coyote time
        if player.on_ground || (time.elapsed_secs() - player.last_ground_ts) <= super::COYOTE_TIME {
            true
        } else {
            // Register jump buffer timestamp
            player.jump_buffer_ts = time.elapsed_secs();
            false
        }
    } else {
        // Takes jump buffering into account
        (time.elapsed_secs() - player.jump_buffer_ts) <= super::JUMP_BUFFER_TIME
    };

    if player.is_jumping
        && (jumping_start || (time.elapsed_secs() - player.real_jump_start_ts) < super::JUMP_TIMER)
    {
        // If the jump starts this frame, mark it
        if jumping_start {
            player.real_jump_start_ts = time.elapsed_secs();
        }
        player.velocity.y = super::JUMP_FORCE;
    }

    if actions.pressed(GameAction::Left, &inputs) {
        player.velocity.x = -super::MOVEMENT_SPEED;
    } else if actions.pressed(GameAction::Right, &inputs) {
        player.velocity.x = super::MOVEMENT_SPEED;
    } else {
        player.velocity.x = 0.;
    }

    // Apply gravity
    player.velocity.y += super::GRAVITY * time.delta_secs();

    // Move the player
    let delta = vec2(player.velocity.x, player.velocity.y) * time.delta_secs();

    let mut box_x = player.bounding_box.clone();
    box_x.max.x += delta.x;
    box_x.min.x += delta.x;

    let mut box_y = player.bounding_box.clone();
    box_y.max.y += delta.y;
    box_y.min.y += delta.y;

    // Game rect collision
    if game_rect.bounds.min.x > box_x.min.x || game_rect.bounds.max.x < box_x.max.x {
        box_x = player.bounding_box;
    }

    player.on_ground = false;

    if game_rect.bounds.min.y > box_x.min.y && player.velocity.y <= 0. {
        box_y = player.bounding_box;
        player.velocity.y = 0.;
        player.on_ground = true;
    } else if game_rect.bounds.max.y < box_x.max.y {
        box_y = player.bounding_box;
    }

    // Block collisions
    // for (mut block, block_transform) in blocks_query.iter_mut() {
    //     block.collide(
    //         block_transform.translation.xy(),
    //         &mut player,
    //         &mut box_x,
    //         &mut box_y,
    //     );
    // }

    info!(
        "Player info : {:?}, {:?}, {:?}",
        player.on_ground, player.velocity, jumping_start
    );

    gizmos.rect_2d(
        vec2(box_x.center().x, box_y.center().y),
        PLAYER_HALF_SIZE * 2.,
        GREEN,
    );

    transform.translation = vec3(box_x.center().x, box_y.center().y, 0.);
    player.bounding_box = Aabb2d::new(transform.translation.xy(), PLAYER_HALF_SIZE);
}
