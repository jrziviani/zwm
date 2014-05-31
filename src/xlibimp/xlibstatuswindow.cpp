#include "xlibstatuswindow.h"

#include <ctime>
#include <cassert>

XLibStatusWindow::XLibStatusWindow(xlibpp_display &display) :
    XLibWindow(display)
{
}

XLibStatusWindow::~XLibStatusWindow()
{
}

void XLibStatusWindow::create(int depth)
{
    // let the parent create this window
    XLibWindow::create(depth);

    // initialize the graphic context to display strings and
    // others visual components.
    initGraphic(depth);
}

void XLibStatusWindow::initGraphic(int depth)
{
    assert(_display.get() != nullptr);
    assert(_visual != nullptr);

    // create a Xft object specially to use modern X11 fonts
    // in the window status bar, this gives more flexibilty,
    // more options and bring some more beauty to it.
    _xft = std::unique_ptr<Xft>(new Xft(_display, 
                                window(), 
                                _visual,
                                "xft:Ubuntu Mono:size=11:antialias=true",
                                _colormap));
}

void XLibStatusWindow::setStatusTitle(const std::string &status)
{
    // 'clear' the screen drawing the string.
    //_xft->drawRect(BLACK, 0, 0, width(), height());

    // draws the status bar in the window.
    //_xft->drawString(RED, 5, 10, status);
}

void XLibStatusWindow::setClock(const std::string &format)
{
    time_t rawtime;
    tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // 'clear' the screen drawing the string.
    _xft->drawRect(BLACK, 200, 10, width(), height());

    // draws the status bar in the window.
    _xft->drawString(GREEN, 200, 10, std::string(asctime(timeinfo)));
}

