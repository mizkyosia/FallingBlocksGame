use std::collections::BTreeMap;

use bevy::{platform::collections::HashSet, prelude::*};
use serde::{Deserialize, Serialize};

macro_rules! count {
    () => (0usize);
    ( $x:tt $($xs:tt)* ) => (1usize + count!($($xs)*));
}

macro_rules! hash_set {
    () => (crate::bevy::platform::collections::HashSet::new());
    ($($value:expr),+ $(,)?) => {{
        let mut set = $crate::bevy::platform::collections::HashSet::with_capacity(
            const { count!($($value)*) }
        );

        $( set.insert($value); )*

        set
    }}
}

/// Enumerates all actions done throught keyboard in the game
#[derive(Serialize, Deserialize, Hash, PartialEq, Eq, PartialOrd, Ord)]
pub enum GameAction {
    Jump,
    Left,
    Right,
    Up,
    Down,
    Object,
}

#[derive(Resource)]
pub struct ActionMap {
    map: BTreeMap<GameAction, HashSet<KeyCode>>,
}

impl ActionMap {
    pub fn pressed(&self, action: GameAction, inputs: &ButtonInput<KeyCode>) -> bool {
        self.map
            .get(&action)
            .unwrap()
            .iter()
            .any(|k| inputs.pressed(*k))
    }

    pub fn just_pressed(&self, action: GameAction, inputs: &ButtonInput<KeyCode>) -> bool {
        self.map
            .get(&action)
            .unwrap()
            .iter()
            .any(|k| inputs.just_pressed(*k))
    }

    pub fn just_released(&self, action: GameAction, inputs: &ButtonInput<KeyCode>) -> bool {
        self.map
            .get(&action)
            .unwrap()
            .iter()
            .any(|k| inputs.just_released(*k))
    }
}

impl Default for ActionMap {
    fn default() -> Self {
        let mut map = BTreeMap::new();

        map.insert(GameAction::Up, hash_set![KeyCode::ArrowUp, KeyCode::KeyZ]);
        map.insert(GameAction::Down, hash_set![KeyCode::ArrowDown, KeyCode::KeyS]);
        map.insert(GameAction::Left, hash_set![KeyCode::ArrowLeft, KeyCode::KeyQ]);
        map.insert(GameAction::Right, hash_set![KeyCode::ArrowRight, KeyCode::KeyD]);
        map.insert(GameAction::Jump, hash_set![KeyCode::Space]);
        map.insert(GameAction::Object, hash_set![KeyCode::KeyE]);

        Self { map }
    }
}
