#ifndef _IWINDOW_H
#define _IWINDOW_H

#include "types.h"

#include <string>

/* IWindow
 *
 * This class is an interface (abstract class to be accurate) to
 * represent an window in the desktop.
 */ 
class IWindow
{
    public:
        virtual ~IWindow() {}

        /* ------------------
         * abstract code 
         * --------------- */

        /* minimizes the window */
        virtual void minimize()         = 0;

        /* maximizes the window */
        virtual void maximize()         = 0;

        /* brings the window to the top of the stack */
        virtual void raiseTop()         = 0;

        /* iconfies the window */
        virtual void iconfy()           = 0;

        /* refreshes the screen */
        virtual void redraw()           = 0;

        /* creates a simple window */
        /* TODO: give options for the window creation and improve
         *       the document about this */
        virtual void create()           = 0;

        /* attaches an existing window in this an instance of this class */
        virtual void attach(pid_t pid)  = 0;

        /* destroys the window */
        virtual void destroy()          = 0;

        /* moves the window */
        virtual void move(int x, int y) = 0;

        /* resizes the window */
        virtual void resize(int width , int height) = 0;
        
        /* ------------------
         * abstract code 
         * --------------- */

        /* returns window positions/sizes */
        int x()             const { return _x; }
        int y()             const { return _y; }
        int width()         const { return _width; }
        int height()        const { return _height; }

        /* returns window name/title */
        std::string name()  const { return _name; }
        std::string title() const { return _title; }

        /* returns window identifications */
        pid_t pid()         const { return _pid; }
        whandler window()   const { return _windowid; }
        whandler parent()   const { return _parentid; }

        /* sets window positions/sizes */
        void x(int pixels)           { _x = pixels; }
        void y(int pixels)           { _y = pixels; }
        void width(int pixels)       { _width = pixels; }
        void height(int pixels)      { _height = pixels; }

        /* sets window name/title */
        void name(std::string name)  { _name = name; }
        void title(std::string name) { _name = name; }

        /* sets window indentifications */
        void pid(pid_t pid)          { _pid = pid; }
        void window(whandler wid)    { _windowid = wid; }
        void parent(whandler wid)    { _parentid = wid; }

    private:
        int _x;
        int _y;
        int _width;
        int _height;

        pid_t _pid;
        whandler _windowid;
        whandler _parentid;

        std::string _name;
        std::string _title;
};

#endif
