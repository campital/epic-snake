#pragma once

#include <string>

namespace ConsoleUI
{

enum class Align {
    NONE,
    CENTER_H,
    CENTER_V,
    CENTER_ALL
};

enum class Color {
    BLACK,
    WHITE
};

struct screen_pos {
    int x;
    int y;
};

constexpr int BUTTON_UP = 0x0800;
constexpr int BUTTON_DOWN = 0x0400;
constexpr int BUTTON_LEFT = 0x0100;
constexpr int BUTTON_RIGHT = 0x0200;
constexpr int BUTTON_CLICK = 0x0008;

struct draw_info {
    screen_pos oldStart { 0, 0 };
    size_t oldLen { 0 };
    bool oldVisible { false };

    screen_pos newStart { 0, 0 };
    std::string newData;
    Color newTextColor { Color::WHITE } ;
    Color newBackgroundColor { Color::BLACK };
    bool newVisible { false };
    bool changed { true };
};

class Element {
    friend class Screen;
    public:
        virtual void setVisible(bool) = 0;
        virtual ~Element() {};
    protected:
        virtual draw_info getUpdates() = 0;
};

}