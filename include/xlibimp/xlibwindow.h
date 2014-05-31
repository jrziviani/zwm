#ifndef _XLIBWINDOW_H
#define _XLIBWINDOW_H


//
// INCLUDES
//
#include "iwindow.h"

#include <memory>
#include <X11/Xlib.h>


//
// DECLARATIONS
//
// Implements the IWindow interface by using XLib methods for X. Reference to
// iwindow.h for details.
class XLibWindow : public IWindow
{
    protected:

        // C++11x - semantically the same as typedef however it's not a new type.
        // XLib display alias definition with custom deallocator
        using xlibpp_display = std::unique_ptr<Display, decltype(&XCloseDisplay)>;

    public:

        // Constructs a window using the same desktop display
        // TODO: const ref?
        XLibWindow(xlibpp_display &display);
        virtual ~XLibWindow();

        // Minimizes a window.
        void minimize();

        // Maximizes a window.
        void maximize();

        // Brings a window to the top of the stack.
        void raiseTop();

        // Makes a window iconized.
        void iconfy();

        // Refreshes the window.
        void redraw();

        // Creates a simple window.
        void create(int depth);

        // Destroys the window.
        void destroy();

        // Moves the window.
        void move(int x, int y);

        // Resizes the window.
        void resize(int width , int height);

        // Sets the colormap
        void setColormap(Colormap colormap);

        // Sets the visual type
        void setVisual(Visual *visual);

    protected:

        // References to desktop display.
        xlibpp_display &_display;

        // Colormap.
        Colormap _colormap;

        // Visual type.
        Visual *_visual;

    private:

        // Sets the window non copyable and non moveable (std::move).
        XLibWindow(const XLibWindow&)              = delete;
        XLibWindow(XLibWindow&&)                   = delete;
        XLibWindow& operator=(const XLibWindow&) & = delete;
        XLibWindow& operator=(XLibWindow&&)      & = delete;
};

#endif
