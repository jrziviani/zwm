#ifndef _XLIBPP_H
#define _XLIBPP_H

#include "keymap.h"
#include "idesktop.h"

#include <memory>
#include <vector>
#include <X11/Xlib.h>
       
using xlibpp_display = std::unique_ptr<Display, decltype(&XCloseDisplay)>;

class XLibpp : public IDesktop
{
    public:

        XLibpp();

        /* main loop */
        void loop();

        /* returns the desktop width */
        int width(int screenNumber);

        /* returns the desktop height (considering the top bar) */
        int height(int screenNumber);

        /* returns the desktop depth resolution */
        int depth(int screenNumber);

        /* inits the root desktop window */
        int initRootWindow(int screenNumber);

        /* returns the number os screens found */
        int getNumberOfScreens(); 

        /* returns the window id given a process id */
        whandler getWindowByPID(unsigned long pid);

        /* draws the status bar on the top of the window */
        void setStatusBar();

    private:

        /* configure the keymaps as real accel keys */
        void setAccelKeys();

    private:

        xlibpp_display _display;
};

#endif
