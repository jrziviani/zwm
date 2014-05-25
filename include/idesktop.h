#ifndef _IDESKTOP_H
#define _IDESKTOP_H

#include "types.h"
#include "keymap.h"
#include "iwindow.h"

#include <memory>
#include <vector>
#include <unordered_map>
       
class IDesktop
{
    protected:

        struct position_t
        {
            int x;
            int y;
            int w;
            int h;
        };

        enum button_t
        {
            NONE,
            LEFT,
            MIDDLE,
            RIGHT
        };

        struct args_t
        {
            button_t   buttonPressed;
            position_t buttonPosition;
            position_t windowPosition;
            whandler   windowid;
        };

    public:
        virtual ~IDesktop() {}

        /* ------------------
         * abstract code 
         * --------------- */

        /* main loop */
        virtual void loop()                          = 0;

        /* returns the desktop width */
        virtual int width(int screenNumber)          = 0;

        /* returns the desktop height (considering the top bar) */
        virtual int height(int screenNumber)         = 0;

        /* returns the desktop depth resolution */
        virtual int depth(int screenNumber)          = 0;

        /* inits the root desktop window */
        virtual int initRootWindow(int screenNumber) = 0;

        /* returns the number os screens found */
        virtual int getNumberOfScreens()             = 0;

        /* draws the status bar on the top of the window */
        /* TODO: accept some configuration options here */
        virtual void setStatusBar()                   = 0;

        /* returns the window id given a process id */
        /* TODO: maybe whandler should become an union if other lib
         *       implementations use something different from ulong */
        whandler getWindowByPID(unsigned long pid);

        /* ------------------
         * generic code
         * --------------- */

        /* sets the accel keys for programs launch and controlling */
        void setKeyMaps(const KeyMaps& keys) { _keyMaps = keys; }

    protected:

        KeyMaps _keyMaps;
        whandler _window;
        std::unordered_map <whandler, std::unique_ptr<IWindow>> _children;
};

#endif
