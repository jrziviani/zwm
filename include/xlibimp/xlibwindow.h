#ifndef _XLIBWINDOW_H
#define _XLIBWINDOW_H

#include "iwindow.h"

#include <memory>
#include <X11/Xlib.h>

/* XLibWindow
 *
 * This class implements a window using the XLib API
 */ 
class XLibWindow : public IWindow
{
    using xlibpp_display = std::unique_ptr<Display, decltype(&XCloseDisplay)>;

    public:

        XLibWindow(xlibpp_display &display);
        ~XLibWindow();

        /* minimizes the window */
         void minimize();

        /* maximizes the window */
         void maximize();

        /* brings the window to the top of the stack */
         void raiseTop();

        /* iconfies the window */
         void iconfy();

        /* refreshes the screen */
         void redraw();

        /* creates a simple window */
         void create();

        /* attaches an existing window in this an instance of this class */
         void attach(pid_t pid);

        /* destroys the window */
         void destroy();

        /* moves the window */
         void move(int x, int y);

        /* resizes the window */
         void resize(int width , int height);

        /* specific to Xlib implementation */
        void setDisplay(xlibpp_display display);

    private:

        /* non copyable, non moveable */
        XLibWindow(const XLibWindow&)              = delete;
        XLibWindow(XLibWindow&&)                   = delete;
        XLibWindow& operator=(const XLibWindow&) & = delete;
        XLibWindow& operator=(XLibWindow&&)      & = delete;

        xlibpp_display &_display;
};

#endif
