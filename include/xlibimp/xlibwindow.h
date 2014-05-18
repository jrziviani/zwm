#ifndef _XLIBWINDOW_H
#define _XLIBWINDOW_H

#include "iwindow.h"
#include "xlibpp.h"

/* XLibWindow
 *
 * This class implements a window using the XLib API
 */ 
class XLibWindow : public IWindow
{
    public:

        XLibWindow(xlibpp_display &display);
        ~XLibWindow();

        /* actions */
        void minimize();
        void maximize();
        void raiseTop();

        void iconfy();
        void redraw();
        void create();
        void destroy();

        void move(int x, int y);
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
