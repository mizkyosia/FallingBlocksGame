use bevy::color::palettes::css;
use bevy::prelude::*;

use crate::GameState;
use crate::assets::Sprites;
use crate::ui::{UiButtonAction, button};

#[derive(Debug, Component)]
pub struct UIMarker;

#[derive(Debug, Component)]
pub enum HubButton {
    Play,
    Settings,
}

pub fn setup(mut commands: Commands, sprites: Res<Sprites>) {
    commands.spawn((
        Node {
            display: Display::Grid,
            width: percent(100.0),
            height: percent(100.0),
            padding: UiRect {
                left: percent(10.),
                right: percent(10.),
                top: percent(10.),
                bottom: percent(10.),
            },

            grid_template_columns: vec![GridTrack::flex(1.0), GridTrack::flex(1.0)],

            grid_template_rows: vec![GridTrack::flex(2.0), GridTrack::flex(1.0)],
            column_gap: percent(5.0),
            row_gap: percent(5.0),
            ..Default::default()
        },
        BackgroundColor(Color::Srgba(css::GRAY)),
        DespawnOnExit(GameState::Hub),
        children![
            (
                ImageNode {
                    image: sprites.logo.clone(),
                    ..Default::default()
                },
                Node {
                    width: percent(20.0),
                    height: auto(),
                    grid_column: GridPlacement::start_span(1, 2),
                    ..Default::default()
                },
            ),
            button("Play", css::DARK_RED, css::RED, UiButtonAction::Play),
            button(
                "Settings",
                css::DARK_BLUE,
                css::BLUE,
                UiButtonAction::Settings
            ),
        ],
    ));
}
