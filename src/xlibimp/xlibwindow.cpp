#include "xlibwindow.h"

#include <cassert>
#include <X11/Xft/Xft.h>

XLibWindow::XLibWindow(xlibpp_display &display) :
    _display(display),
    _pixmap(None)
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

void XLibWindow::redraw()
{
    assert(_display.get() != nullptr);

    move(x(), y());
    resize(width(), height());
    XMapWindow(_display.get(), window());
    XSelectInput(_display.get(), window(), EnterWindowMask);
}

void XLibWindow::initGraphic(int depth)
{
    assert(_display.get() != nullptr);

    // create a pixmap with the same window size as
    // a drawable for the graphic context
    _pixmap = XCreatePixmap(_display.get(),
                            window(),
                            width(),
                            height(),
                            depth);
    
    _xft = std::unique_ptr<Xft>(new Xft(_display, 
                                window(), 
                                // TODO: these should be passed from the dekstop
                                XDefaultVisual(_display.get(), 0),
                                XDefaultColormap(_display.get(), 0)));
}

void XLibWindow::create(int depth)
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
    initGraphic(depth);
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

    // create the graphic context
    GC gc = XCreateGC(_display.get(), 
                      _pixmap,
                      0,
                      (XGCValues *)0);


    // reset the background (clear the whole screen)
    /*XFillRectangle(_display.get(), 
                   window(), 
                   gc, 
                   0, 
                   0, 
                   width(), 
                   height());*/

    // set the font used to display the window name in the status
    // bar
    // TODO: font stly must be configured
    XftFont *font = XftFontOpenName (_display.get(),
                                     0,
                                     "xft:Ubuntu Mono:size=11:antialias=true");
    //Font f = XLoadFont(_display.get(), "-*-fixed-*-*-*-*-14-*-*-*-*-*-*-*");
    //Font f = XLoadFont(_display.get(), "xft:Ubuntu Mono:size=11:antialias=true");

    // set the font color
    // TODO: font color must be configured
  /*  XSetForeground(_display.get(),
                   gc,
                   0x00ff9944);*/

    /*XftDraw *draw = XftDrawCreate (_display.get(),
                                   window(),
                                   XDefaultVisual(_display.get(), 0),
                                   XDefaultColormap(_display.get(), 0));

    XftColor bcolor;
    bcolor.pixel = 0;
    bcolor.color.red=0;
    bcolor.color.green=0;
    bcolor.color.blue=0;
    bcolor.color.alpha=0xffff;

    // reset the background (clear the whole screen)
    XftDrawRect (draw,
                 &bcolor,
                 0,
                 0,
                 width(),
                 height());

    //XRenderColor render = {.red=0xffff, .green=0xffff, .blue=0, .alpha=0x50};
    XftColor color;
    color.pixel = 0;
    color.color.red=0xffff;
    color.color.green=0xffff;
    color.color.blue=0;
    color.color.alpha=0xffff;

    XftDrawString8 (draw,
                    &color,
                    font,
                    8,
                    10,
                    (const unsigned char*)status.c_str(),
                    status.length());*/


    _xft->drawRect(BLACK, 0, 0, width(), height());

    _xft->drawString(RED, *font, 5, 10, status);

    // draw the active screen name in the status bar
    /*XDrawString(_display.get(), 
                window(), 
                gc, 
                5,  // x
                10, // y
                status.c_str(),
                status.length());
*/
    XftFontClose(_display.get(), font);
    //XUnloadFont(_display.get(), f);

    //XFreeGC(_display.get(), gc);
}
