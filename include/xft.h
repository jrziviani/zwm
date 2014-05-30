#ifndef _XFT_H
#define _XFT_H


//
// INCLUDES
//
#include <memory>
#include <X11/Xft/Xft.h>


//
// MACROS
//
#define RED    Xft::getColor(0xffff, 0,      0,      0xfff)
#define GREEN  Xft::getColor(0,      0xffff, 0,      0xfff)
#define BLUE   Xft::getColor(0,      0,      0xffff, 0xfff)
#define WHITE  Xft::getColor(0xffff, 0xffff, 0xffff, 0xfff)
#define BLACK  Xft::getColor(0,      0,      0,      0xfff)


//
// DECLARATIONS
//
class Xft
{
    // C++11x - semantically the same as typedef however it's not a new type.
    // XLib display alias definition with custom deallocator
    using xlibpp_display = std::unique_ptr<Display, decltype(&XCloseDisplay)>;

    public:
        Xft(xlibpp_display &display,
            const whandler window,
            Visual* visual,
            const Colormap colormap) : _display (display),
                                       _draw    {XftDrawCreate(_display.get(),
                                                         window,
                                                         visual,
                                                         colormap),
                                                 &XftDrawDestroy}
    {
    }

    // Draws a simple rectangle in the window.
    void drawRect(const XftColor &color,
                  const int x,
                  const int y,
                  const int width,
                  const int height) const
    {
        XftDrawRect(_draw.get(),
                    &color,
                    x,
                    y,
                    width,
                    height);
    }

    // Draws a string in the string using moderns
    // X11 fonts.
    void drawString(const XftColor &color,
                    XftFont  &font,
                    const int x,
                    const int y,
                    const std::string &text) const
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
    // Returns a xft color structure based on the color selected
    // by the user
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

        // References to desktop display.
        xlibpp_display &_display;

        // Handles the xft scope
        std::unique_ptr<XftDraw, decltype(&XftDrawDestroy)> _draw;
};

#endif
