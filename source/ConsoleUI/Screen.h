#pragma once

#include <list>
#include <memory>
#include <string>
#include "Base.h"
#include "Text.h"
#include "Button.h"

namespace ConsoleUI
{

class Screen {
    friend class Canvas;
    public:
        std::shared_ptr<Text> addText(const std::string& initial, screen_pos startPos, bool visible = true, Align alignment = Align::NONE);
        std::shared_ptr<Button> addButton(const std::string& initial, screen_pos startPos, bool visible = true,
            const std::function<void()>& cb = std::function<void()>(), Align alignment = Align::NONE);
        void removeText(const std::shared_ptr<Text>&);
        void removeButton(const std::shared_ptr<Button>&);
    private:
        Screen(screen_pos dimensions);
        void update(int input, bool reset);

        void removeElement(const std::shared_ptr<Element>&);
        void printAtLocation(const std::string&, screen_pos);
        std::string colorFormatString(const std::string& str, Color bg, Color fg) const;
        void handleInput(int);
        void selectButton(const std::list<std::shared_ptr<Button>>::iterator&);
        void draw(const draw_info&);

        std::list<std::shared_ptr<Element>> m_elements;
        std::list<std::shared_ptr<Button>> m_buttons;
        std::list<draw_info> m_extraDraw;
        std::list<std::shared_ptr<Button>>::iterator m_buttonChoice;
        screen_pos m_dimensions;
};

}