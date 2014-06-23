#include <iostream>
#include <string>
#include <memory>

#include <X11/Xlib.h>
#include "xlibstatuswindow.h"

/*
g++ -std=c++11 -g3 -O0 -I/usr/include/freetype2 -Iinclude -Iinclude/xlibimp src/statusbar.cpp src/xlibimp/xlibwindow.o src/xlibimp/xlibstatuswindow.o -lX11 -lXft -L/usr/lib/x86_64-linux-gnu -lfreetype -lz -o status
*/

int main()
{
    using xlibpp_display = std::unique_ptr<Display, decltype(&XCloseDisplay)>;

    xlibpp_display dis = xlibpp_display(XOpenDisplay(0x0), &XCloseDisplay);

    Window root = XDefaultRootWindow(dis.get());

    XLibStatusWindow status(dis);


    
    status.x(0);
    status.y(0);
    status.width(1000);
    status.height(20);
    status.parent(root);
    status.setColormap(DefaultColormap(dis.get(), 0));
    status.setVisual(DefaultVisual(dis.get(), 0));
    status.create(DefaultDepth(dis.get(), 0));
    //status.setClock("");

    XSelectInput (dis.get(), status.window(), ExposureMask | KeyPressMask | ButtonPressMask);

    //status.drawClock();

    XEvent ev;

    while (true)
    {
        XNextEvent(dis.get(), &ev);

        switch (ev.type) {

            case Expose:
                status.drawWidgets(XLibStatusWindow::DESKTOPS, XLibStatusWindow::LEFT);
                status.drawWidgets(XLibStatusWindow::TITLES, XLibStatusWindow::LEFT);
                //status.drawVirtualDesktops(1);
                //status.drawStatusTitle("This is a test");
                //status.drawClock();
                break;

            /*case ButtonPress:
                XDestroyWindow(dis.get(), status.window());
                break;*/

        }

    }

    return 0;
}
