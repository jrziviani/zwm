#ifndef _IWINDOW_H
#define _IWINDOW_H


//
// INCLUDES
//
#include "types.h"

#include <string>


//
// DECLARATIONS
//

// Defines an interface for a window specification. Everything related
// to a window (properties and methods) should be declared here and
// implemented in the specialized classes.
//
// An object window should be created in two forms:
//  1) Defining its basic parameters (size, position) and calling
//     create() method to create that window in the desktop;
//  2) Attaching a window created somewhere else (i.e: terminal) by
//     simply setting both its Window ID and its Parent ID in MapRequest
//     event handler;
class IWindow
{
    public:
        virtual ~IWindow() {}

        //
        // ABSTRACT CODE
        //

        // Minimizes a window.
        virtual void minimize()         = 0;

        // Maximizes a window.
        virtual void maximize()         = 0;

        // Brings a window to the top of the stack.
        virtual void raiseTop()         = 0;

        // Makes a window iconized.
        virtual void iconfy()           = 0;

        // Refreshes the window.
        virtual void redraw()           = 0;

        // Initializes the graphic context
        virtual void initGraphic(int depth)  = 0;

        // Creates a simple window.
        // TODO: this method should receive parameters to be customized.
        virtual void create(int depth)  = 0;

        // Attaches an existing window in this an instance of this class.
        // TODO: remove attach(pid) - it won't be used.
        virtual void attach(pid_t pid)  = 0;

        // Destroys the window.
        virtual void destroy()          = 0;

        // Moves the window.
        virtual void move(int x, int y) = 0;

        // Resizes the window.
        virtual void resize(int width , int height) = 0;

        // Sets the status title
        // TODO: this should possible be moved to a plugin
        virtual void setStatusTitle(std::string status) = 0;
        
        //
        // GENERIC CODE
        //

        // Returns window's size/position.
        int x()             const { return _x; }
        int y()             const { return _y; }
        int width()         const { return _width; }
        int height()        const { return _height; }

        // Returns window's name/title.
        std::string name()  const { return _name; }
        std::string title() const { return _title; }

        // Returns window's identification.
        pid_t pid()         const { return _pid; }
        whandler window()   const { return _windowid; }
        whandler parent()   const { return _parentid; }

        // Sets window positions/sizes.
        void x(int pixels)           { _x = pixels; }
        void y(int pixels)           { _y = pixels; }
        void width(int pixels)       { _width = pixels; }
        void height(int pixels)      { _height = pixels; }

        // Sets window name/title.
        void name(std::string name)  { _name = name; }
        void title(std::string name) { _name = name; }

        // Sets window indentifications.
        void pid(pid_t pid)          { _pid = pid; }
        void window(whandler wid)    { _windowid = wid; }
        void parent(whandler wid)    { _parentid = wid; }

    private:
        // x coordinate (pixels).
        int _x;

        // y coordinate (pixels).
        int _y;

        // Window width (pixels).
        int _width;

        // Window height (pixels).
        int _height;

        // Process PID.
        pid_t _pid;

        // Window ID.
        whandler _windowid;

        // Parent window ID.
        whandler _parentid;

        // Window name
        std::string _name;

        // Window title
        std::string _title;
};

#endif
