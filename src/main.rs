pub extern crate bevy;

use bevy::{prelude::*, window::PresentMode};

use crate::{assets::{load_assets, Sprites}, game::inputs::ActionMap};


pub mod assets;
pub mod game;
pub mod ui;

#[derive(PartialEq, Eq, Debug, Default, States, Hash, Clone)]
pub enum GameState {
    #[default]
    Hub,
    Game,
}

#[derive(PartialEq, Eq, Debug, Default, States, Hash, Clone)]
pub enum MenuState {
    #[default]
    Closed,
    Settings,
}

fn camera(mut commands: Commands) {
    // Spawn 2D camera
    commands.spawn((Camera2d,));
}

fn main() {
    let mut app = App::new();

    app.add_plugins(
        DefaultPlugins
            .set(WindowPlugin {
                primary_window: Some(Window {
                    title: "Game".to_string(),
                    present_mode: PresentMode::AutoVsync,
                    resizable: false,
                    ..Default::default()
                }),
                ..Default::default()
            })
            .set(AssetPlugin {
                file_path: "assets/".to_string(),
                ..Default::default()
            })
            .set(ImagePlugin::default_nearest()),
    );

    app.init_resource::<Sprites>();
    app.init_resource::<ActionMap>();

    app.add_plugins((ui::plugin, game::plugin));

    app.add_systems(Startup, (load_assets, camera));

    app.init_state::<GameState>().init_state::<MenuState>();

    info!("Running application now");

    app.run();
}
