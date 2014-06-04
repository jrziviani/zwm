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
// CONSTANTS
//
const int MAX_VIRTUAL_DESKTOPS = 9;
const int MAX_WINDOW_PER_DESKTOP = 10;
const int LINE = 0;
const int COLUMN = 1;


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
            _logger   (logger),
            _desktops (MAX_VIRTUAL_DESKTOPS),
            _currentDesktop (0)
        {
            calarea[0][LINE]  = 0; calarea[0][COLUMN]  = 0;
            calarea[1][LINE]  = 1; calarea[1][COLUMN]  = 1;
            calarea[2][LINE]  = 2; calarea[2][COLUMN]  = 1;
            calarea[3][LINE]  = 2; calarea[3][COLUMN]  = 2;
            calarea[4][LINE]  = 2; calarea[4][COLUMN]  = 2;
            calarea[5][LINE]  = 3; calarea[5][COLUMN]  = 2;
            calarea[6][LINE]  = 3; calarea[6][COLUMN]  = 2;
            calarea[7][LINE]  = 3; calarea[7][COLUMN]  = 3;
            calarea[8][LINE]  = 3; calarea[8][COLUMN]  = 3;
            calarea[9][LINE]  = 3; calarea[9][COLUMN]  = 3;
            calarea[10][LINE] = 4; calarea[10][COLUMN] = 3;
        }

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

        // Given the screen area and the number of windows that
        // must be fit in it, this method returns a vector with all
        // windows positions and sizes
        std::vector<position_t> getAreas(int width, int height, int nwindows)
        {
            std::vector<position_t> ret;

            // no special reason to block MAX_WINDOW_PER_DESKTOP other
            // than a higher number of windows does not fit well in my
            // monitor...
            if (nwindows > MAX_WINDOW_PER_DESKTOP || nwindows <= 0)
                return ret;

            // calculate the max width possible per line and the
            // max height per column given the screen size passed
            int lin          = calarea[nwindows][LINE];
            int col          = calarea[nwindows][COLUMN];
            int widthPerLin  = width  / lin;
            int heightPerCol = height / col;

            // when there are more cells than windows to open we merge
            // some cells to make a bigger one, ie:
            // +----+----+
            // |    |    |
            // |    +----+
            // |    |    |
            // +----+----+
            int cellsToMerge = (lin * col) % nwindows;
            int skip = 0;

            for (int i = 0; i < lin; ++i)
            {
                int merged = 0;

                for (int j = cellsToMerge; j < col; ++j)
                {
                    position_t p;

                    // the next window starts after the previous width
                    p.x = (i == 0) 
                            ? 0 
                            : ret[(i - 1) * col].w * i;

                    // the next window starts after the previous height
                    // (discounting the merged cells)
                    p.y = (j == cellsToMerge) 
                            ? cellsToMerge 
                            : ret[i * col - skip + (j - 1)].h * (j - cellsToMerge);

                    // width is max width per line
                    p.w = widthPerLin;

                    //height is max height per column * cells merged
                    p.h = heightPerCol * (cellsToMerge - merged + 1);
                    std::cout << "P.H: " << p.h << std::endl;
                    
                    ret.push_back(p);

                    // discount the merged cell
                    if (cellsToMerge > 0)
                        skip = cellsToMerge;

                    merged = cellsToMerge;
                }

                // reset because all cells were merged already
                cellsToMerge = 0;
            }

            return ret;
        }

    protected:

        // Accel keys.
        KeyMaps _keyMaps;

        // Window handler.
        whandler _window;

        // Logger reference.
        logger &_logger;

        // Maps any window to its ID.
        using virtualDesktop = std::unordered_map<whandler, std::unique_ptr<IWindow>>;
        std::vector<virtualDesktop> _desktops;

        // Desktop currently active.
        int _currentDesktop;

        // Screen number
        int _screenNumber;

        // Size of the screen
        size _screenSize; 

    private:

        int calarea[MAX_WINDOW_PER_DESKTOP][2];
};

#endif
