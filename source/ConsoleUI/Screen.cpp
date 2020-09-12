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
        return *one < *two;
    });
    if(m_buttons.size() == 1) {
        m_buttonChoice = m_buttons.begin();
        selectButton(m_buttonChoice);
    }
    return elemPtr;
}

void Screen::removeText(const std::shared_ptr<Text>& txt)
{
    removeElement(std::static_pointer_cast<Element>(txt));
}

void Screen::removeButton(const std::shared_ptr<Button>& btn)
{
    removeElement(std::static_pointer_cast<Element>(btn));

    decltype(m_buttons)::iterator btnIt;
    if((btnIt = std::find(m_buttons.begin(), m_buttons.end(), btn)) != m_buttons.end())
        m_buttons.erase(btnIt);
}

void Screen::removeElement(const std::shared_ptr<Element>& elem)
{
    draw_info info = elem->getUpdates();
    info.changed = true;
    info.newVisible = false;
    m_extraDraw.push_back(info);

    decltype(m_elements)::iterator it;
    if((it = std::find(m_elements.begin(), m_elements.end(), elem)) != m_elements.end())
        m_elements.erase(it);
}

void Screen::update(int input, bool reset)
{
    if(reset)
        printf("\033[2J");

    for(std::shared_ptr<Element> ptr : m_elements) {
        draw_info info = ptr->getUpdates();
        if(reset)
            info.changed = true;
        draw(info);
    }

    auto it = m_extraDraw.end();
    while(it != m_extraDraw.begin()) {
        --it;
        draw(*it);
        m_extraDraw.erase(it);
    }

    handleInput(input);
}

void Screen::draw(const draw_info& info)
{
    if(info.changed) {
        if(info.oldVisible)
            printAtLocation(std::string(info.oldLen, ' '), info.oldStart);

        if(info.newVisible)
            printAtLocation(colorFormatString(info.newData, info.newBackgroundColor, info.newTextColor), info.newStart);
    }
}

void Screen::printAtLocation(const std::string& str, screen_pos pos)
{
    if(pos.x >= 0 && pos.x < m_dimensions.x && pos.y >= 0 && pos.y < m_dimensions.y) {
        printf("\033[%d;%dH%s", pos.y + 2, pos.x, str.c_str());
    }
}

std::string Screen::colorFormatString(const std::string& str, Color bg, Color fg) const
{
    std::string formatted;
    formatted.reserve(str.size() + 14);
    switch(bg) {
        case Color::BLACK:
            formatted += "\033[40m";
            break;
        case Color::WHITE:
            formatted += "\033[47m";
            break;
    }

    switch(fg) {
        case Color::BLACK:
            formatted += "\033[30m";
            break;
        case Color::WHITE:
            formatted += "\033[37m";
            break;
    }

    formatted += str;
    formatted += "\033[37m\033[40m";
    return formatted;
}

void Screen::handleInput(int input)
{
    if(m_buttons.size() > 0) {
        std::list<std::shared_ptr<Button>>::iterator btnIterator = m_buttonChoice;
        screen_pos currentStart = (*m_buttonChoice)->m_drawInfo.newStart;

        if(input & BUTTON_UP) {
            while(btnIterator != m_buttons.begin()) {
                --btnIterator;
                if((*btnIterator)->m_drawInfo.newStart.y < currentStart.y &&
                    (*btnIterator)->m_drawInfo.newVisible) {
                    selectButton(btnIterator);
                    break;
                }
            }
        } else if(input & BUTTON_DOWN) {
            while(btnIterator != --m_buttons.end()) {
                ++btnIterator;
                if((*btnIterator)->m_drawInfo.newStart.y > currentStart.y &&
                    (*btnIterator)->m_drawInfo.newVisible) {
                    selectButton(btnIterator);
                    break;
                }
            }
        } else if(input & BUTTON_LEFT) {
            while(btnIterator != m_buttons.begin()) {
                --btnIterator;
                if((*btnIterator)->m_drawInfo.newVisible) {
                    selectButton(btnIterator);
                    break;
                }
            }
        } else if(input & BUTTON_RIGHT) {
            while(btnIterator != --m_buttons.end()) {
                ++btnIterator;
                if((*btnIterator)->m_drawInfo.newVisible) {
                    selectButton(btnIterator);
                    break;
                }
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