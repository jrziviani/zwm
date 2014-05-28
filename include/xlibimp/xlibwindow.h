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

/*********************************************************
 * TODO: ALIEN - this should be removed from here ASAP
 ********************************************************/
#include <X11/Xft/Xft.h>

#define RED    Xft::getColor(0xffff, 0,      0,      0xfff)
#define GREEN  Xft::getColor(0,      0xffff, 0,      0xfff)
#define BLUE   Xft::getColor(0,      0,      0xffff, 0xfff)
#define WHITE  Xft::getColor(0xffff, 0xffff, 0xffff, 0xfff)
#define BLACK  Xft::getColor(0,      0,      0,      0xfff)

class Xft
{
    using xlibpp_display = std::unique_ptr<Display, decltype(&XCloseDisplay)>;

    public:
        Xft(xlibpp_display &display,
            whandler window,
            Visual* visual,
            Colormap colormap) : _display (display),
                                 _window  (window),
                                 _draw    {XftDrawCreate(_display.get(),
                                                         _window,
                                                         visual,
                                                         colormap),
                                           &XftDrawDestroy}
    {
    }

    void drawRect(const XftColor &color,
                  int x,
                  int y,
                  int width,
                  int height)
    {
        XftDrawRect(_draw.get(),
                    &color,
                    x, y, width, height);
    }
                  
    void drawString(const XftColor &color,
                    XftFont  &font,
                    int x,
                    int y,
                    const std::string &text)
    {
        XftDrawString8(_draw.get(),
                       &color,
                       &font,
                       x,
                       y,
                       (const unsigned char*) text.c_str(),
                       text.length());
    }

    //
    // STATIC FUNCTIONS
    //
    static XftColor getColor(unsigned short red,
                             unsigned short green,
                             unsigned short blue,
                             unsigned short alpha)
    {
        XftColor c;
        c.pixel = 0;
        c.color.red   = red;
        c.color.green = green;
        c.color.blue  = blue;
        c.color.alpha = alpha;

        return c;
    }

    private:

        xlibpp_display &_display;
        whandler       _window;

        std::unique_ptr<XftDraw, decltype(&XftDrawDestroy)> _draw;
};


// Implements the IWindow interface by using XLib methods for X. Reference to
// iwindow.h for details.
class XLibWindow : public IWindow
{
    // C++11x - semantically the same as typedef however it's not a new type.
    // XLib display alias definition with custom deallocator
    using xlibpp_display = std::unique_ptr<Display, decltype(&XCloseDisplay)>;

    public:

        // Constructs a window using the same desktop display
        // TODO: const ref?
        XLibWindow(xlibpp_display &display);
        ~XLibWindow();

        // Minimizes a window.
        void minimize();

        // Maximizes a window.
        void maximize();

        // Brings a window to the top of the stack.
        void raiseTop();

        // Makes a window iconized.
        void iconfy();

        // Initializes the graphic context
        void initGraphic(int depth);

        // Refreshes the window.
        void redraw();

        // Creates a simple window.
        void create(int depth);

        // Attaches an existing window in this an instance of this class.
        void attach(pid_t pid);

        // Destroys the window.
        void destroy();

        // Moves the window.
        void move(int x, int y);

        // Resizes the window.
        void resize(int width , int height);

        // Sets the desktop display.
        // TODO: remove this method setDisplay.
        void setDisplay(xlibpp_display display);

        // Sets the status title
        void setStatusTitle(std::string status);

    private:

        // Sets the window non copyable and non moveable (std::move).
        XLibWindow(const XLibWindow&)              = delete;
        XLibWindow(XLibWindow&&)                   = delete;
        XLibWindow& operator=(const XLibWindow&) & = delete;
        XLibWindow& operator=(XLibWindow&&)      & = delete;

        // References to desktop display.
        xlibpp_display &_display;

        // Pixmap ID
        Pixmap _pixmap;

        std::unique_ptr<Xft> _xft;
};

#endif
