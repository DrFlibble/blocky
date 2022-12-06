//
// Created by Ian Parker on 12/10/2022.
//

#include "menu.h"
#include "blocky/blocky.h"

#include <frontier/widgets/button.h>
#include <frontier/widgets/frame.h>

using namespace Frontier;

MenuOverlay::MenuOverlay(Blocky* blocky) : Overlay(blocky)
{
}

void MenuOverlay::init()
{
    m_window = new Frontier::FrontierWindow(m_blocky->getApp(), L"Menu", Frontier::WINDOW_NORMAL);

    Frame* mainFrame = new VFrame(m_blocky->getApp());

    mainFrame->add(new Label(m_blocky->getApp(), L"Blocky!"));
    Button* button = new Button(m_blocky->getApp(), L"Quit");
    mainFrame->add(button);
    m_window->setContent(mainFrame);

    m_window->show();
}

void MenuOverlay::draw()
{
    m_window->update();

    Geek::Gfx::Surface* surface = m_window->getSurface();
    resize(surface->getWidth(), surface->getHeight());
    m_overlaySurface->blit(0, 0, surface);
    Overlay::draw(OVERLAY_CENTRE, OVERLAY_CENTRE);
}
