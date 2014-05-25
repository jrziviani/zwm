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
    resize(width(), height());
    XMapWindow(_display.get(), window());
    XSelectInput(_display.get(), window(), EnterWindowMask);
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

    Pixmap pix = XCreatePixmap(_display.get(),
                               window(),
                               width(),
                               height()-2,
                               DefaultDepth(_display.get(),
                                            DefaultScreen(_display.get())));
    GC gc = XCreateGC(_display.get(), 
                      pix,
                      0, 
                      (XGCValues *)0);

    XFillRectangle(_display.get(), 
                   window(), 
                   gc, 
                   0, 
                   0, 
                   width(), 
                   height());

    Font f = XLoadFont(_display.get(), "-*-fixed-*-*-*-*-14-*-*-*-*-*-*-*");
    //Font f = XLoadFont(_display.get(), "xft:Ubuntu Mono:size=11:antialias=true");

    XSetForeground(_display.get(),
                   gc,
                   0x00ff9944);
    XTextItem ti;
    ti.chars = const_cast<char*>(status.c_str());
    ti.nchars = status.length();
    ti.delta = 0;
    ti.font = f;
    XDrawText(_display.get(), 
              window(), 
              gc, 
              5, 
              20, 
              &ti,
              1);
}
