#ifndef _IDESKTOP_H
#define _IDESKTOP_H


//
// INCLUDES
//
#include "types.h"
#include "keymap.h"
#include "iwindow.h"

#include <memory>
#include <vector>
#include <unordered_map>


//
// DECLARATION
//

// Defines an interface (abstract class) for a desktop implementation,
// this will give the control of all windows within. It's library 
// agnostic and must have code generic enough to let the client
// handle the whole dekstop through this interface.
class IDesktop
{
    protected:

        // Represents a window position/size in the desktop.
        struct position_t
        {
            int x;
            int y;
            int w;
            int h;
        };

        // Represents the mouse button clicked.
        enum button_t
        {
            NONE,
            LEFT,
            MIDDLE,
            RIGHT
        };

        // Stores the current scenario of a window when
        // a button clicks in it.
        struct args_t
        {
            button_t   buttonPressed;
            position_t buttonPosition;
            position_t windowPosition;
            whandler   windowid;
        };

    public:
        virtual ~IDesktop() {}

        IDesktop(logger &logger) :
            _logger(logger) {}

        //
        // ABSTRACT CODE
        //

        // Executes the main entry (event handler) of the desktop.
        virtual void loop()                          = 0;

        // Returns the desktop width.
        virtual int width(int screenNumber)          = 0;

        // Returns the desktop height.
        virtual int height(int screenNumber)         = 0;

        // Returns the desktop depth.
        virtual int depth(int screenNumber)          = 0;

        // Initializes the desktop.
        virtual void initRootWindow(int screenNumber) = 0;

        // Returns the number of screens plugged.
        virtual int getNumberOfScreens()             = 0;

        // Draws the status bar int the desktop top position.
        // TODO: status bar should accept parameter to configure
        //       it with some customization like background color,
        //       font color, font size, etc.
        virtual void setStatusBar()                   = 0;

        // Returns the Window ID based on the Process ID.
        // NOTE: This information is not a standard, today it's
        //       WM responsibility to assign PID to a window.
        // TODO: Change this method for getWindowByAttributte to
        //       give it more flex.
        whandler getWindowByPID(unsigned long pid);

        //
        // GENERIC CODE
        //

        // Maps all defined accel keys.
        void setKeyMaps(const KeyMaps& keys) { _keyMaps = keys; }

    protected:

        // Accel keys
        KeyMaps _keyMaps;

        // Window handler
        whandler _window;

        // Logger reference
        logger &_logger;

        // Maps any window to its ID
        std::unordered_map <whandler, std::unique_ptr<IWindow>> _children;
};

#endif
