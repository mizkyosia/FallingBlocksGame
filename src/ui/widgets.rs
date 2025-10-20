use bevy::prelude::*;

use crate::ui::UiButtonAction;

#[derive(Component)]
pub struct ButtonData {
    pub base_bg_color: Color,
    pub hover_bg_color: Color,
    pub base_border_color: Color,
    pub hover_border_color: Color,
    pub action: UiButtonAction,
}

pub fn button(
    text: &str,
    border: impl Into<Color>,
    background: impl Into<Color>,
    action: UiButtonAction,
) -> impl Bundle {
    let bg: Color = background.into();
    let brd: Color = border.into();

    (
        Button,
        Node {
            width: px(250),
            height: px(65),
            border: UiRect::all(px(5)),
            // horizontally center child text
            justify_content: JustifyContent::Center,
            // vertically center child text
            align_items: AlignItems::Center,
            ..default()
        },
        BorderColor::all(brd),
        BorderRadius::all(px(10)),
        BackgroundColor(bg),
        children![(
            Text::new(text),
            TextFont {
                font_size: 33.0,
                ..default()
            },
            TextColor(Color::BLACK),
        )],
        ButtonData {
            base_bg_color: bg,
            hover_bg_color: bg.lighter(0.08),
            base_border_color: brd,
            hover_border_color: brd.lighter(0.08),
            action,
        },
    )
}
