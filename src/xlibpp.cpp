#include "xlibpp.h"
#include "helper.h"
#include "xlibwindow.h"

#include <algorithm>
#include <cassert>
#include <unistd.h>

#include <iostream>

#include <X11/cursorfont.h>
#include <X11/Xatom.h>

#include <stack>

using namespace std;

xlibpp::xlibpp() :
    _display { XOpenDisplay(0x0), &XCloseDisplay }
{
    std::cout << "constructing...\n";
}

int xlibpp::width(int screenNumber)
{
    assert(_display.get() != nullptr);
    assert(screenNumber >= 0 && screenNumber < this->numberOfScreens());

    return DisplayWidth(_display.get(), screenNumber);
}

int xlibpp::height(int screenNumber)
{
    assert(_display.get() != nullptr);
    assert(screenNumber >= 0 && screenNumber < this->numberOfScreens());

    return DisplayHeight(_display.get(), screenNumber);
}

int xlibpp::depth(int screenNumber)
{
    assert(_display.get() != nullptr);
    assert(screenNumber >= 0 && screenNumber < this->numberOfScreens());

    return DefaultDepth(_display.get(), screenNumber);
}

int xlibpp::numberOfScreens()
{
    assert(_display.get() != nullptr);

    return ScreenCount(_display.get());
}

int xlibpp::initRootWindow(int screenNumber)
{
    assert(_display.get() != nullptr);
    assert(screenNumber >= 0 && screenNumber < this->numberOfScreens());

    Cursor cursor, hand_cursor;

    // TODO: get the root window based on the screenNumber
    std::cout << "init root window...\n";
    std::cout << "n of screens: " << numberOfScreens() << std::endl;

    cursor = XCreateFontCursor(_display.get(), XC_left_ptr);
    hand_cursor = XCreateFontCursor(_display.get(), XC_hand2);

    _window = DefaultRootWindow(_display.get());
    XDefineCursor(_display.get(), _window, cursor);

    drawBar();
}

void xlibpp::setMaps(const std::vector<KeyMap>& keys)
{
    _keyMaps = keys;

    XSelectInput(_display.get(), _window, SubstructureRedirectMask |
                                          SubstructureNotifyMask |
                                          ButtonPressMask |
                                          EnterWindowMask |
                                          LeaveWindowMask |
                                          StructureNotifyMask |
                                          PropertyChangeMask |
                                          ExposureMask | 
                                          KeyPressMask);
    for (KeyMap k : _keyMaps)
    {
        XGrabKey(_display.get(),
                 XKeysymToKeycode(_display.get(), k.getKey()),
                 k.getMod1(),
                 _window,
                 True,
                 GrabModeAsync,
                 GrabModeAsync);
    }
}

Window xlibpp::getWindowByPID(unsigned long pid)
{
    Window root;
    Window parent;
    Window *children;
    Status status;
    unsigned int total;

    Atom type;
    int format;
    unsigned long nItems;
    unsigned long bytesAfter;
    unsigned char *pPid;

    std::stack<Window> windowStack;

    windowStack.push(_window);

    while (!windowStack.empty())
    {
        Window lookupWnd = windowStack.top();
        windowStack.pop();

        // get the PID of a given window
        /*XGetWindowProperty(_display.get(),
                           lookupWnd,
                           XInternAtom(_display.get(), "_NET_WM_PID", True),
                           0,
                           1,
                           False,
                           XA_CARDINAL,
                           &type,
                           &format,
                           &nItems,
                           &bytesAfter,
                           &pPid);


        // the pid from the given window is the one
        // required: return the window
        if (pid == *pPid)
        {
            XFree(pPid);
            return lookupWnd;
        }

        XFree(pPid);*/

        // search for all windows placed in the desktop
        status = XQueryTree(_display.get(),
                            lookupWnd,
                            &root,
                            &parent,
                            &children,
                            &total);

        // fill the stack with children windows to be
        // searched
        for (unsigned int i = 0; i < total; ++i)
        {
            windowStack.push(children[i]);
            XMapRaised(_display.get(), lookupWnd);
        }

        XFree(children);
    }
}

void xlibpp::loop()
{
    std::cout << "main loop...\n";
    XGrabButton(_display.get(), 1, Mod1Mask, DefaultRootWindow(_display.get()), True,
            ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
    XGrabButton(_display.get(), 3, Mod1Mask, DefaultRootWindow(_display.get()), True,
            ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);

    XEvent event;
    XWindowAttributes attr;
    XButtonEvent start;

    start.subwindow = None;
    while (true) {

        XNextEvent(_display.get(), &event);


        if (event.type == KeyPress)
        {
            for (KeyMap k : _keyMaps)
            {
                if (XKeysymToKeycode(_display.get(), k.getKey()) == event.xkey.keycode &&
                    k.getMod1() | k.getMod2() == event.xkey.state)
                {
                    pid_t pid = helper::callProgramBg(k.getProgram().c_str());
                    //Window wnd = getWindowByPID(pid);



                        /*char *win_name = NULL;
                        XFetchName(_display.get(), wnd, &win_name);
                       //XResizeWindow(_display.get(), children[i], 50, 50);
                        if (win_name == NULL)
                            continue;
                        std::cout << "child name: " << win_name << std::endl;*/
                    

                    break;
                }

            }
        }
        else if (event.type == CreateNotify)
        {
            std::cout << "CREATE\n";
        }
        else if (event.type == ButtonPress &&
                event.xbutton.subwindow != None)
        {
            std::cout << "button press\n";
            XRaiseWindow(_display.get(), event.xkey.subwindow);


            XGetWindowAttributes(_display.get(), 
                                 event.xbutton.subwindow, 
                                 &attr);

            //std::cout << "window: " << event.xbutton.subwindow << std::endl;
            //XUnmapWindow(_display.get(), event.xbutton.subwindow);
            start = event.xbutton;
        }

        else if (event.type == MotionNotify && 
                start.subwindow != None)
        {
            //std::cout << "mouse move\n";
            int xdiff = event.xbutton.x_root - start.x_root;
            int ydiff = event.xbutton.y_root - start.y_root;
            XMoveResizeWindow(_display.get(), start.subwindow,
                attr.x + (start.button==1 ? xdiff : 0),
                attr.y + (start.button==1 ? ydiff : 0),
                std::max(1, attr.width + (start.button==3 ? xdiff : 0)),
                std::max(1, attr.height + (start.button==3 ? ydiff : 0)));
        }
        else if(event.type == ButtonRelease)
        {
            //std::cout << "button release\n";
            start.subwindow = None;
        }
        else if (event.type == MappingNotify)
        {   std::cout << "event type: " << event.type << std::endl;
            std::cout << "event request: " << event.xmapping.request << std::endl;
            std::cout << "window: " << event.xmapping.window << std::endl;

            //XUnmapWindow(_display.get(), event.xmapping.window);

        }
        else if (event.type == MapRequest)
        {   std::cout << "event type: " << event.type << std::endl;
            std::cout << "window: " << event.xmaprequest.window << std::endl;

            XSetWindowBorderWidth(_display.get(), event.xmaprequest.window, 2);
            XSetWindowBorder(_display.get(), event.xmaprequest.window, 0x00ff9944);
            XMoveResizeWindow(_display.get(), event.xmaprequest.window, 0, 0, 800, 600);
            //XRaiseWindow(_display.get(), event.xmaprequest.window);
            XMapWindow(_display.get(), event.xmaprequest.window);

        }
        //std::cout << "event type [all]: " << event.type << std::endl;

    }
}

void xlibpp::drawBar()
{
    XLibWindow statusbar(_display);
    statusbar.x(0);
    statusbar.y(0);
    statusbar.width(width(0) - 1);
    statusbar.height(height(0) / 50);
    statusbar.parent(_window);
    statusbar.create();
}
