#ifndef _XLIBPP_H
#define _XLIBPP_H

#include "keymap.h"

#include <memory>
#include <vector>
#include <X11/Xlib.h>
       
using xlibpp_display = std::unique_ptr<Display, decltype(&XCloseDisplay)>;

class xlibpp
{
    public:

        xlibpp();

        int width(int screenNumber);
        int height(int screenNumber);
        int depth(int screenNumber);
        int numberOfScreens();

        int initRootWindow(int screenNumber);
        void setMaps(const KeyMaps& keys);
        Window getWindowByPID(unsigned long pid);

        void loop();

        void drawBar();

    private:

        xlibpp_display _display;
        Window _window;
        std::vector<KeyMap> _keyMaps;
};

#endif
