pub mod data;
pub mod home;
pub mod pause;
pub mod widgets;

pub use data::*;
pub use widgets::*;

pub use bevy::prelude::*;

use crate::{GameState, MenuState};

pub fn plugin(app: &mut App) {
    app.add_systems(OnEnter(GameState::Hub), (home::setup, pause::setup));

    app.add_systems(Update, base_button_system);
}

pub fn base_button_system(
    mut interaction_query: Query<
        (
            &Interaction,
            &mut BackgroundColor,
            &mut BorderColor,
            &ButtonData,
        ),
        Changed<Interaction>,
    >,
    mut game_state: ResMut<NextState<GameState>>,
    mut menu_state: ResMut<NextState<MenuState>>,
    mut ev_quit: MessageWriter<AppExit>,
) {
    for (interaction, mut bg, mut border, button) in &mut interaction_query {
        match *interaction {
            Interaction::Pressed => match button.action {
                UiButtonAction::Play => game_state.set(GameState::Game),
                UiButtonAction::Settings => menu_state.set(MenuState::Settings),
                UiButtonAction::Home => game_state.set(GameState::Hub),
                UiButtonAction::Quit => {
                    ev_quit.write(AppExit::Success);
                }
            },
            Interaction::Hovered => {
                bg.0 = button.hover_bg_color;
                border.set_all(button.hover_border_color);
            }
            Interaction::None => {
                bg.0 = button.base_bg_color;
                border.set_all(button.base_border_color);
            }
        }
    }
}
