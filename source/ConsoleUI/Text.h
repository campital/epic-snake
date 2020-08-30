#pragma once

#include <string>
#include "Base.h"

namespace ConsoleUI
{

class Text : public Element {
    friend class Screen;
    public:
        void setText(const std::string&);
        std::string text();
        void move(screen_pos, Align alignment = Align::NONE);
        void setVisible(bool);
        ~Text();
    protected:
        Text(const std::string& initial, screen_pos startPos, bool visible = true,
            screen_pos dimensions = { 0, 0 }, Align alignment = Align::NONE);
        virtual draw_info getUpdates();
        void updateDrawInfo();

        draw_info m_drawInfo;
    private:
        screen_pos m_dimensions;
        bool m_changed;
};

}