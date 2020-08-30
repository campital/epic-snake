#include "Button.h"

namespace ConsoleUI
{

Button::Button(const std::string& initial, screen_pos startPos, bool visible,
            screen_pos dimensions, Align alignment) : Text(initial, startPos, visible, dimensions, alignment)
{}

void Button::setCallback(const std::function<void()>& cb)
{
    m_callback = cb;
}

bool Button::operator<(const Button& other) const
{
    // 512 is arbitrary -- it gives the y value a greater weight
    return (m_drawInfo.newStart.y * 512 + m_drawInfo.newStart.x < other.m_drawInfo.newStart.y * 512 + other.m_drawInfo.newStart.x);
}

draw_info Button::getUpdates()
{
    if(m_selected && m_drawInfo.newVisible) {
        updateDrawInfo();
        if(m_updateMod++ % 2 == 0) {
            m_drawInfo.newBackgroundColor = Color::BLACK;
            m_drawInfo.newTextColor = Color::WHITE;
        } else {
            m_drawInfo.newBackgroundColor = Color::WHITE;
            m_drawInfo.newTextColor = Color::BLACK;
        }
    }
    return Text::getUpdates();
}

bool Button::setSelected(bool selected)
{
    if(!m_drawInfo.newVisible) {
        return false;
    }
    if(!selected) {
        updateDrawInfo();
        m_drawInfo.newBackgroundColor = Color::BLACK;
        m_drawInfo.newTextColor = Color::WHITE;
    }
    m_selected = selected;
    return true;
}

void Button::click()
{
    if(m_callback && m_drawInfo.newVisible) {
        m_callback();
    }
}

}