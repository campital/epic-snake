#include "Text.h"

namespace ConsoleUI
{

Text::Text(const std::string& initial, screen_pos startPos, bool visible, screen_pos dimensions, Align alignment) : m_dimensions(dimensions)
{
    m_drawInfo.newVisible = visible;
    setText(initial);
    move(startPos, alignment);
}

void Text::setText(const std::string& text)
{
    updateDrawInfo();
    m_drawInfo.newData = text;
}

std::string Text::text()
{
    return m_drawInfo.newData;
}

void Text::move(screen_pos location, Align alignment)
{
    updateDrawInfo();
    if(alignment == Align::CENTER_H || alignment == Align::CENTER_ALL)
        location.x += m_dimensions.x / 2 - m_drawInfo.newData.size() / 2;

    if(alignment == Align::CENTER_V || alignment == Align::CENTER_ALL)
        location.y += m_dimensions.y / 2 - 3;
    m_drawInfo.newStart = location;
}

void Text::setVisible(bool visible)
{
    if(visible != m_drawInfo.newVisible)
        updateDrawInfo();
    m_drawInfo.newVisible = visible;
}

draw_info Text::getUpdates()
{
    m_drawInfo.changed = m_changed;
    m_changed = false;
    return m_drawInfo;
}

void Text::updateDrawInfo()
{
    if(!m_changed) {
        m_drawInfo.oldStart = m_drawInfo.newStart;
        m_drawInfo.oldLen = m_drawInfo.newData.size();
        m_drawInfo.oldVisible = m_drawInfo.newVisible;
        m_changed = true;
    }
}

Text::~Text()
{
    setVisible(false);
}

}