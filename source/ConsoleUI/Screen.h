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
    public:
        Screen(screen_pos dimensions);
        std::shared_ptr<Text> addText(const std::string& initial, screen_pos startPos, bool visible = true, Align alignment = Align::NONE);
        std::shared_ptr<Button> addButton(const std::string& initial, screen_pos startPos, bool visible = true,
            const std::function<void()>& cb = std::function<void()>(), Align alignment = Align::NONE);
        void update(int input);
    private:
        void printAtLocation(const std::string&, screen_pos);
        std::string colorFormatString(const std::string& str, Color bg, Color fg) const;
        void handleInput(int);
        void selectButton(const std::list<std::shared_ptr<Button>>::iterator&);

        std::list<std::shared_ptr<Element>> m_elements;
        std::list<std::shared_ptr<Button>> m_buttons;
        std::list<std::shared_ptr<Button>>::iterator m_buttonChoice;
        screen_pos m_dimensions;
};

}