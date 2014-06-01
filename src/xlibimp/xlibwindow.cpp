#include "xlibwindow.h"

#include <cassert>

XLibWindow::XLibWindow(xlibpp_display &display) :
    _display(display),
    _visual(nullptr)
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

void XLibWindow::setColormap(Colormap colormap)
{
    _colormap = colormap;
}

void XLibWindow::setVisual(Visual *visual)
{
    _visual = visual;
}

void XLibWindow::redraw()
{
    assert(_display.get() != nullptr);

    // updates the place if needed.
    move(x(), y());

    // updates the size if needed.
    resize(width(), height());

    // show the window.
    XMapWindow(_display.get(), window());

    // register the window to receive EnterWindow event, this is specially
    // important to let the desktop knows when this window object receives
    // the focus, so the status bar can be updated.
    // TODO: remove XSelectInput from here, a new method must be created.
    XSelectInput(_display.get(), window(), EnterWindowMask);
}

void XLibWindow::create(int depth)
{
    assert(_display.get() != nullptr);

    // create a simple window using the position and size
    // defined by the user.
    Window wnd = XCreateSimpleWindow(_display.get(),
                                     parent(),
                                     x(),
                                     y(),
                                     width(),
                                     height(),
                                     0,                              // TODO: configure border width
                                     0x00ff9944,                     // TODO: configure border color
                                     BlackPixel(_display.get(), 0)); // TODO: configure bg color

    // set the id to this object so the desktop will be able
    // to handle it whenever need.
    window(wnd);

    // show the window.
    XMapWindow(_display.get(), wnd);
}

void XLibWindow::destroy()
{
    assert(_display.get() != nullptr);

    XDestroyWindow(_display.get(), window());
}

void XLibWindow::move(int x, int y)
{
    assert(_display.get() != nullptr);

    this->x(x);
    this->y(y);

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

    this->width(width);
    this->height(height);

    XMoveResizeWindow(_display.get(), 
                      window(),
                      x(),
                      y(),
                      width,
                      height);
}
