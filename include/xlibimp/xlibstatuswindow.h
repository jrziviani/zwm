#ifndef _XLIBSTATUSWINDOW_H_
#define _XLIBSTATUSWINDOW_H_


//
// INCLUDES
//
#include "types.h"
#include "xlibwindow.h"
#include "xft.h"

#include <thread>
#include <mutex>
#include <unordered_map>


//
// DECLARATIONS
//
// Specializes a generic window to the status bar window. It will be
// placed at the top on the desktop and will have configurable areas
// to display information about the system and the desktop itself.
//
// +-----------------------------------------------------------------+
// | active wnd title  #  system status  #  indicators  # time       |
// +-----------------------------------------------------------------+
class XLibStatusWindow : public XLibWindow
{
    public:

        enum widgets { TITLES, DESKTOPS, CLOCK, INDICATORS };
        enum alignment { LEFT, RIGHT };

        XLibStatusWindow(xlibpp_display &display);
        ~XLibStatusWindow();

        // Creates the window.
        void create(int depth);

        // Initializes the graphic context.
        void initGraphic(int depth);

        // Draws widgets.
        void drawWidgets(widgets widget, alignment align);

        // Draws the active window title in the status bar.
        void drawStatusTitle(const std::string &status, alignment align = LEFT);

        void drawVirtualDesktops(unsigned int current, alignment align = LEFT);

        // Draws the clock in the status bar.
        void drawClock(alignment align = RIGHT);

        // Sets the clock in the status bar
        void setClock(const std::string &format);

    private:

        // Updates the clock every second
        void updateClock();

    private:

        // Xft object to draw strings in a window.
        std::unique_ptr<Xft> _xft;

        // Thread to update the clock.
        std::thread _clockThread;

        // Last status string in status bar.
        std::string _lastStatus;

        // Position of each widget in the status bar.
        std::unordered_map<int, box> _positions;
        int _left;
        int _right;
};

#endif
