#ifndef _XLIBDESKTOP_H
#define _XLIBDESKTOP_H


//
// INCLUDES
//
#include "keymap.h"
#include "idesktop.h"
#include "xlibwindow.h"

#include <memory>
#include <vector>
#include <unordered_map>

#include <X11/Xlib.h>


//
// DECLARATIONS
//

// Implements the IDesktop interface using Xlib, reference to idesktop.h
// for details.
class XLibDesktop : public IDesktop
{
    // C++11x - semantically the same as typedef however it's not a new type.
    // XLib display alias definition with custom deallocator
    using xlibpp_display = std::unique_ptr<Display, decltype(&XCloseDisplay)>;

    // Event handler alias definition
    using handler        = void (XLibDesktop::*)(XEvent &, args_t &);

    public:

        XLibDesktop(logger &logger);

        // Executes the main entry (event handler) of the desktop.
        void loop();

        // Returns the desktop width.
        int width(int screenNumber);

        // Returns the desktop height.
        int height(int screenNumber);

        // Returns the desktop depth.
        int depth(int screenNumber);

        // Initializes the desktop.
        void initRootWindow(int screenNumber);

        // Returns the number of screens plugged.
        int getNumberOfScreens();

        // Returns the Window ID based on the Process ID.
        whandler getWindowByPID(unsigned long pid);

        // Draws the status bar int the desktop top position.
        void setStatusBar();

    private:

        // Defines the accel keys in lib
        void setAccelKeys();

        // Gets the colormap ID
        Colormap getColormap(int screenNumber);

        // Gets the visual type
        Visual *getVisual(int screenNumber);

        // Event handlers
        void mapRequest(XEvent &e   , args_t &arg);
        void keyPress(XEvent &e     , args_t &arg);
        void buttonPress(XEvent &e  , args_t &arg);
        void buttonRelease(XEvent &e, args_t &arg);
        void motionNotify(XEvent &e , args_t &arg);
        void mapNotify(XEvent &e    , args_t &arg);
        void enterNotify(XEvent &e  , args_t &arg);

    private:

        // XLib display instance.
        xlibpp_display _display;

        // Event handlers maps.
        std::unordered_map <int, handler> _handlers;

        XLibWindow _statusBar;
};

#endif
