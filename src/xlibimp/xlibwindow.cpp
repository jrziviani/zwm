#include "xlibwindow.h"

#include <cassert>

XLibWindow::XLibWindow(xlibpp_display &display) :
    _display(display),
    _pixmap(None),
    _visual(nullptr)
{
}

XLibWindow::~XLibWindow()
{
    if (_pixmap != None)
        XFreePixmap(_display.get(), _pixmap);
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

void XLibWindow::initGraphic(int depth)
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
                                     1,                              // TODO: configure border width
                                     0x00ff9944,                     // TODO: configure border color
                                     BlackPixel(_display.get(), 0)); // TODO: configure bg color

    // set the id to this object so the desktop will be able
    // to handle it whenever need.
    window(wnd);

    // show the window.
    XMapWindow(_display.get(), wnd);

    // initialize the graphic context to display strings and
    // others visual components.
    initGraphic(depth);
}

void XLibWindow::attach(pid_t pid)
{
    // TODO: attach pid won't be used, should be removed
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

void XLibWindow::setStatusTitle(std::string status)
{
    assert(_display.get() != nullptr);

    // set the font used to display the window name in the status
    // bar, as we use xft we have more power to choose better and
    // modern fonts, including antialising.
    // TODO: font stile must be configured by the user.
    //XftFont *font = XftFontOpenName (_display.get(),
    //                                 0,
    //                                 "xft:Ubuntu Mono:size=11:antialias=true");

    // 'clear' the screen drawing the string.
    _xft->drawRect(BLACK, 0, 0, width(), height());

    // draws the status bar in the window.
    _xft->drawString(RED, 5, 10, status);

    // release the font resource.
    //XftFontClose(_display.get(), font);
}
