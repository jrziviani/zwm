#include "xlibstatuswindow.h"

#include <unistd.h>
#include <ctime>
#include <cassert>
#include <mutex>


XLibStatusWindow::XLibStatusWindow(xlibpp_display &display) :
    XLibWindow(display)
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

void XLibStatusWindow::drawStatusTitle(const std::string &status)
{
    if (_lastStatus.length() > 0)
    {
        // get the string (pixels used to draw) size
        size s = _xft->getStringSize(_lastStatus);

        // 'clear' the screen drawing the string.
        XClearArea(_display.get(), 
                   window(), 
                   0, 
                   0, 
                   s.width, 
                   height(), 
                   False);
    }

    // draws the status bar in the window.
    _xft->drawString(RED, 0, height() - 5, status);

   _lastStatus = status;
}

void XLibStatusWindow::drawClock()
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

