#include "Canvas.h"

namespace ConsoleUI
{

Canvas::Canvas(screen_pos dimensions) : m_dimensions(dimensions)
{}

void Canvas::update(int input)
{
    bool before = m_reset;
    m_screen->update(input, m_reset);
    if(before == m_reset)
        m_reset = false;
}

std::shared_ptr<Screen> Canvas::addScreen()
{
    return std::shared_ptr<Screen>(new Screen(m_dimensions));
}

void Canvas::setScreen(std::shared_ptr<Screen> screen)
{
    m_reset = true;
    m_screen = screen;
}

}