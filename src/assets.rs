use bevy::{
    asset::{AssetServer, Handle},
    ecs::{
        resource::Resource,
        system::{Res, ResMut},
    },
    image::Image,
};

#[derive(Resource, Default)]
pub struct Sprites {
    pub logo: Handle<Image>,
}

pub fn load_assets(asset_server: Res<AssetServer>, mut sprites: ResMut<Sprites>) {
    sprites.logo = asset_server.load("logo.png");
}
