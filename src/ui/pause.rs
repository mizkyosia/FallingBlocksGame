use bevy::prelude::*;

use crate::GameState;

pub fn setup(mut commands: Commands) {
    commands.spawn((
        DespawnOnExit(GameState::Game),
        Node {
            display: Display::Grid,
            padding: UiRect::all(percent(15.0)),
            width: percent(100.0),
            height: percent(100.0),

            grid_template_columns: vec![
                GridTrack::flex(1.0),
                GridTrack::px(200.),
                GridTrack::px(200.),
                GridTrack::flex(1.0),
            ],

            grid_template_rows: vec![
                GridTrack::flex(1.0),
                GridTrack::px(50.),
                GridTrack::px(50.),
                GridTrack::flex(1.0),
            ],
            ..Default::default()
        },
    ));
}
