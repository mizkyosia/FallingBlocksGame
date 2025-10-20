use bevy::prelude::*;

use crate::GameState;

pub mod blocks;
pub mod collision;
pub mod data;
pub mod game_rect;
pub mod inputs;
pub mod player;

pub use data::*;

pub fn plugin(app: &mut App) {
    app.add_systems(
        OnEnter(GameState::Game),
        (player::setup, game_rect::setup_game_rect),
    );

    app.add_systems(FixedUpdate, (player::movement, blocks::update));
}
