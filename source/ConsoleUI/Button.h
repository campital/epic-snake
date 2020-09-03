#pragma once

#include "Text.h"
#include <functional>

namespace ConsoleUI
{

class Button : public Text {
    friend class Screen;
    public:
        void setCallback(const std::function<void()>&);
        bool operator<(const Button&) const;
    private:
        Button(const std::string& initial, screen_pos startPos, bool visible = true,
            screen_pos dimensions = { 0, 0 }, Align alignment = Align::NONE);
        draw_info getUpdates();
        void setSelected(bool);
        void click();

        int m_updateMod = 0;
        bool m_selected = false;
        std::function<void()> m_callback;
};

}