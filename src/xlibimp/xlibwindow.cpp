#include "xlibwindow.h"

#include <cassert>

XLibWindow::XLibWindow(xlibpp_display &display) :
    _display(display)
{

}

XLibWindow::~XLibWindow()
{

}

void XLibWindow::minimize()
{

}

void XLibWindow::maximize()
{

}

void XLibWindow::raiseTop()
{
    XRaiseWindow(_display.get(), window());
}

void XLibWindow::iconfy()
{

}

void XLibWindow::redraw()
{
    assert(_display.get() != nullptr);

    move(x(), y());
    XMapWindow(_display.get(), window());
}

void XLibWindow::create()
{
    assert(_display.get() != nullptr);

    Window wnd = XCreateSimpleWindow(_display.get(),
                                     parent(),
                                     x(),
                                     y(),
                                     width(),
                                     height(),
                                     1,                              // TODO: configure border width
                                     0x00ff9944,                     // TODO: configure border color
                                     BlackPixel(_display.get(), 0)); // TODO: configure bg color
    window(wnd);
    XMapWindow(_display.get(), wnd);
}

void XLibWindow::attach(pid_t pid)
{

}

void XLibWindow::destroy()
{
    assert(_display.get() != nullptr);

    XDestroyWindow(_display.get(), window());
}

void XLibWindow::move(int x, int y)
{
    assert(_display.get() != nullptr);

    XMoveResizeWindow(_display.get(), 
                      window(),
                      x,
                      y,
                      width(),
                      height());
}

void XLibWindow::resize(int width, int height)
{
    assert(_display.get() != nullptr);

    XMoveResizeWindow(_display.get(), 
                      window(),
                      x(),
                      y(),
                      width,
                      height);
}
