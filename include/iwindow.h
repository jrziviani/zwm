#ifndef _IWINDOW_H
#define _IWINDOW_H

#include <string>

typedef unsigned long whandler;

/* IWindow
 *
 * This class is an interface (abstract class to be accurate) to
 * represent an window in the desktop.
 */ 
class IWindow
{
    public:
        virtual ~IWindow() {}

        /* actions */
        virtual void minimize() = 0;
        virtual void maximize() = 0;
        virtual void raiseTop() = 0;

        virtual void iconfy()   = 0;
        virtual void redraw()   = 0;
        virtual void create()   = 0;
        virtual void destroy()  = 0;

        virtual void move(int x, int y)             = 0;
        virtual void resize(int width , int height) = 0;
        
        /* getters */
        int x()             const { return _x; }
        int y()             const { return _y; }
        int width()         const { return _width; }
        int height()        const { return _height; }

        std::string name()  const { return _name; }
        std::string title() const { return _title; }

        pid_t pid()         const { return _pid; }
        whandler window()   const { return _windowid; }
        whandler parent()   const { return _parentid; }

        /* setters */
        void x(int pixels)           { _x = pixels; }
        void y(int pixels)           { _y = pixels; }
        void width(int pixels)       { _width = pixels; }
        void height(int pixels)      { _height = pixels; }

        void name(std::string name)  { _name = name; }
        void title(std::string name) { _name = name; }

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
