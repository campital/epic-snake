#pragma once

#include <memory>
#include "Base.h"
#include "Screen.h"

namespace ConsoleUI
{


class Canvas {
    public:
        Canvas(screen_pos dimensions);
        void update(int input);
        std::shared_ptr<Screen> addScreen();
        void setScreen(std::shared_ptr<Screen>);
    private:
        void clearScreen();

        std::shared_ptr<Screen> m_screen;
        screen_pos m_dimensions;
        bool m_reset;
};

}