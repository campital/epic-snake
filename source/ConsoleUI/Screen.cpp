#include "Screen.h"
#include <algorithm>

namespace ConsoleUI
{

Screen::Screen(screen_pos dimensions) : m_dimensions(dimensions)
{}

std::shared_ptr<Text> Screen::addText(const std::string& initial, screen_pos startPos, bool visible, Align alignment)
{
    // explicit `new` is necessary
    std::shared_ptr<Text> elemPtr(new Text(initial, startPos, visible, m_dimensions, alignment));
    m_elements.push_back(std::static_pointer_cast<Element>(elemPtr));
    return elemPtr;
}

std::shared_ptr<Button> Screen::addButton(const std::string& initial, screen_pos startPos, bool visible,
    const std::function<void()>& cb, Align alignment)
{
    std::shared_ptr<Button> elemPtr(new Button(initial, startPos, visible, m_dimensions, alignment));
    elemPtr->setCallback(cb);
    m_elements.push_back(std::static_pointer_cast<Element>(elemPtr));
    m_buttons.push_back(elemPtr);
    m_buttons.sort([](const std::shared_ptr<Button>& one, const std::shared_ptr<Button>& two) -> bool {
        return *two < *one;
    });
    if(m_buttons.size() == 1) {
        m_buttonChoice = m_buttons.begin();
        selectButton(m_buttonChoice);
    }
    return elemPtr;
}

void Screen::update(int input)
{
    for(std::shared_ptr<Element> ptr : m_elements) {
        draw_info currInfo = ptr->getUpdates();
        if(currInfo.changed) {
            if(currInfo.oldVisible) {
                printAtLocation(std::string(currInfo.oldLen, ' '), currInfo.oldStart);
            }
            if(currInfo.newVisible) {
                printAtLocation(colorFormatString(currInfo.newData, currInfo.newBackgroundColor, currInfo.newTextColor), currInfo.newStart);
            }
        }
    }
    handleInput(input);
}

void Screen::printAtLocation(const std::string& str, screen_pos pos)
{
    if(pos.x >= 0 && pos.x < m_dimensions.x && pos.y >= 0 && pos.y < m_dimensions.y) {
        printf("\x1b[%d;%dH%s", pos.y + 2, pos.x, str.c_str());
    }
}

std::string Screen::colorFormatString(const std::string& str, Color bg, Color fg) const
{
    std::string formatted;
    formatted.reserve(str.size() + 14);
    switch(bg) {
        case Color::BLACK:
            formatted += "\x1b[40m";
            break;
        case Color::WHITE:
            formatted += "\x1b[47m";
            break;
    }

    switch(fg) {
        case Color::BLACK:
            formatted += "\x1b[30m";
            break;
        case Color::WHITE:
            formatted += "\x1b[37m";
            break;
    }

    formatted += str;
    formatted += "\x1b[37m\x1b[40m";
    return formatted;
}

void Screen::handleInput(int input)
{
    if(m_buttons.size() > 0) {
        std::list<std::shared_ptr<Button>>::iterator btnIterator = m_buttonChoice;
        bool looped = true;
        screen_pos currentStart = (*m_buttonChoice)->m_drawInfo.newStart;

        if(input & BUTTON_UP) {
            while(btnIterator != m_buttons.begin()) {
                --btnIterator;
                if((*btnIterator)->m_drawInfo.newStart.y < currentStart.y) {
                    selectButton(btnIterator);
                    looped = false;
                }
            }
            if(looped) {
                selectButton(--m_buttons.end());
            }
        } else if(input & BUTTON_DOWN) {
            while(btnIterator != --m_buttons.end()) {
                ++btnIterator;
                if((*btnIterator)->m_drawInfo.newStart.y > currentStart.y) {
                    selectButton(btnIterator);
                    looped = false;
                }
            }
            if(looped) {
                selectButton(m_buttons.begin());
            }
        } else if(input & BUTTON_LEFT) {
            if(btnIterator != m_buttons.begin()) {
                selectButton(--btnIterator);
            } else {
                selectButton(--m_buttons.end());
            }
        } else if(input & BUTTON_RIGHT) {
            if(btnIterator != --m_buttons.end()) {
                selectButton(++btnIterator);
            } else {
                selectButton(m_buttons.begin());
            }
        }

        if(input & BUTTON_CLICK) {
            (*m_buttonChoice)->click();
        }
    }
}

void Screen::selectButton(const std::list<std::shared_ptr<Button>>::iterator& newBtn)
{
    (*m_buttonChoice)->setSelected(false);
    (*newBtn)->setSelected(true);
    m_buttonChoice = newBtn;
}

}