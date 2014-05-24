#ifndef _XLIBPP_H
#define _XLIBPP_H

#include "keymap.h"
#include "idesktop.h"

#include <memory>
#include <vector>
#include <unordered_map>

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
        /* TODO: remove this method that simply doesn't work, there is
         *       no feasible way to get the window pid. All control must
         *       be done using the window id */
        whandler getWindowByPID(unsigned long pid);

        /* draws the status bar on the top of the window */
        void setStatusBar();

    private:

        /* configure the keymaps as real accel keys */
        void setAccelKeys();

        /* event handlers */
        void mapRequest(XEvent &e);
        void keyPress(XEvent &e);
        void buttonPress(XEvent &e);
        void buttonRelease(XEvent &e);
        void motionNotify(XEvent &e);

    private:

        typedef void (XLibpp::*handler)(XEvent &);

        xlibpp_display _display;
        std::unordered_map <int, handler> _handlers;
};

#endif
