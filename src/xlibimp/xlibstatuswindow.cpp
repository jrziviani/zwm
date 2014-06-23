#include "xlibstatuswindow.h"

#include <unistd.h>
#include <ctime>
#include <cassert>
#include <mutex>


XLibStatusWindow::XLibStatusWindow(xlibpp_display &display) :
    XLibWindow(display),
    _left(0),
    _right(0)
{
}

XLibStatusWindow::~XLibStatusWindow()
{
    _clockThread.~thread();
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
                                "xft:Ubuntu Mono:size=10:antialias=true",
                                _colormap));
}

void XLibStatusWindow::drawWidgets(widgets widget, alignment align)
{
    switch (widget)
    {
        case TITLES:
            drawStatusTitle("This is a title", align);
            break;

        case DESKTOPS:
            drawVirtualDesktops(1, align);
            break;
    }
}

void XLibStatusWindow::drawStatusTitle(const std::string &status, alignment align)
{
    if (_lastStatus.length() > 0 && _lastStatus != status)
    {
        // 'clear' the screen drawing the string.
        XClearArea(_display.get(), 
                   window(), 
                   _positions[TITLES].pos.x, 
                   _positions[TITLES].pos.y, 
                   _positions[TITLES].sz.width, 
                   height(),
                   False);
    }

    // get the string (pixels used to draw) size
    size stringSize = _xft->getStringSize(status);

    if (_positions.find(TITLES) == _positions.end())
    {
        box bx;
        bx.pos.x = (align == LEFT) ? _left : _right;
        bx.pos.y = 0;
        bx.sz.width = stringSize.width;
        bx.sz.height = height() - 5;
        _positions.insert(std::make_pair(TITLES, bx));

        if (align == LEFT)
            _left += stringSize.width + 3;

        else
            _right += stringSize.width + 3;

    }

    // draws the status bar in the window.
    _xft->drawString(RED,
                    _positions[TITLES].pos.x, 
                    height() - 5, 
                    status);

   _lastStatus = status;
}

void XLibStatusWindow::drawVirtualDesktops(unsigned int current, alignment align)
{
    std::string virtDesktops = "[1] 2 3 4 5 6 7 8 9";

    // get the string (pixels used to draw) size
    size stringSize = _xft->getStringSize(virtDesktops);

    if (_positions.find(DESKTOPS) == _positions.end())
    {
        box bx;
        bx.pos.x = (align == LEFT) ? _left : _right;
        bx.pos.y = 0;
        bx.sz.width = stringSize.width;
        bx.sz.height = height() - 5;
        _positions.insert(std::make_pair(DESKTOPS, bx));

        if (align == LEFT)
            _left += stringSize.width + 3;

        else
            _right += stringSize.width + 3;
    }

    // draws the status bar in the window.
    _xft->drawString(WHITE,
                    _positions[DESKTOPS].pos.x, 
                    height() - 5, 
                    virtDesktops);

    // get the string (pixels used to draw) size
    //size s = _xft->getStringSize(virtDesktops);

    /*XClearArea(_display.get(),
               window(),
                _positions[1],
                0,
                s.width,
                height(),
                False);

    // draws the status bar in the window.
    _xft->drawString(RED, _positions[0], height() - 5, virtDesktops);

    _positions[1] = _positions[0] + s.width + 2;*/
}

void XLibStatusWindow::drawClock(alignment align)
{
    time_t rawtime;
    tm *timeinfo;

    // format time
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    std::string stime(asctime(timeinfo));

    // get the string (pixels used to draw) size
    size s = _xft->getStringSize(stime);

    // 'clear' the screen drawing the string.
    XClearArea(_display.get(), 
               window(), 
               width() - s.width - 5, 
               0, 
               width(), 
               height(), 
               False);

    // draws the status bar in the window.
    _xft->drawString(GREEN, width() - s.width, height() - 5, stime);
}

void XLibStatusWindow::updateClock()
{
    // Xlib doesn't handle concurrency properly, so it's
    // recommended to create a different display structure
    // to don't let threads mess up the main display.
    Display *tmpDisplay = XOpenDisplay(0x0);
    XEvent evt;

    while (true)
    {
        // initialize the event
        memset(&evt, 0, sizeof(evt));

        // define the event data
        evt.type               = Expose;
        evt.xexpose.window     = window();
	    evt.xexpose.send_event = True;

        // send the event to be handled
        XSendEvent(tmpDisplay, /*parent()*/window(), True, ExposureMask, &evt);
        XFlush(tmpDisplay);

        usleep(500000);
    }

    XCloseDisplay(tmpDisplay);
}

void XLibStatusWindow::setClock(const std::string &format)
{

    _clockThread = std::thread(&XLibStatusWindow::updateClock, this);
    _clockThread.detach();
}

