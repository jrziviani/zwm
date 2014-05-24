#include "xlibpp.h"
#include "helper.h"
#include "xlibwindow.h"

#include <unordered_map>
#include <algorithm>
#include <stack>
#include <iostream>
#include <cassert>

#include <unistd.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

using namespace std;

XLibpp::XLibpp() :
    _display { XOpenDisplay(0x0), &XCloseDisplay }
{
    _handlers.insert(make_pair(MapRequest,    &XLibpp::mapRequest));
    _handlers.insert(make_pair(KeyPress,      &XLibpp::keyPress));
    _handlers.insert(make_pair(ButtonPress, &XLibpp::buttonPress));
    _handlers.insert(make_pair(ButtonRelease, &XLibpp::buttonRelease));
    _handlers.insert(make_pair(MotionNotify,  &XLibpp::motionNotify));
}

int XLibpp::width(int screenNumber)
{
    assert(_display.get() != nullptr);
    assert(screenNumber >= 0 && screenNumber < this->getNumberOfScreens());

    return DisplayWidth(_display.get(), screenNumber);
}

int XLibpp::height(int screenNumber)
{
    assert(_display.get() != nullptr);
    assert(screenNumber >= 0 && screenNumber < this->getNumberOfScreens());

    return DisplayHeight(_display.get(), screenNumber);
}

int XLibpp::depth(int screenNumber)
{
    assert(_display.get() != nullptr);
    assert(screenNumber >= 0 && screenNumber < this->getNumberOfScreens());

    return DefaultDepth(_display.get(), screenNumber);
}

int XLibpp::getNumberOfScreens()
{
    assert(_display.get() != nullptr);

    return ScreenCount(_display.get());
}

int XLibpp::initRootWindow(int screenNumber)
{
    assert(_display.get() != nullptr);
    assert(screenNumber >= 0 && screenNumber < this->getNumberOfScreens());

    Cursor cursor, hand_cursor;

    // TODO: get the root window based on the screenNumber
    std::cout << "init root window...\n";
    std::cout << "n of screens: " << getNumberOfScreens() << std::endl;

    cursor = XCreateFontCursor(_display.get(), XC_left_ptr);
    hand_cursor = XCreateFontCursor(_display.get(), XC_hand2);

    _window = DefaultRootWindow(_display.get());
    XDefineCursor(_display.get(), _window, cursor);

    XSelectInput(_display.get(), _window, SubstructureRedirectMask |
                                          SubstructureNotifyMask |
                                          ButtonPressMask |
                                          EnterWindowMask |
                                          LeaveWindowMask |
                                          StructureNotifyMask |
                                          PropertyChangeMask |
                                          ExposureMask | 
                                          KeyPressMask);
    setStatusBar();
    setAccelKeys();
}

void XLibpp::setAccelKeys()
{
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

Window XLibpp::getWindowByPID(unsigned long pid)
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
        XGetWindowProperty(_display.get(),
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

        XFree(pPid);

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

void XLibpp::loop()
{
    std::cout << "main loop...\n";
    XGrabButton(_display.get(), 1, Mod1Mask, DefaultRootWindow(_display.get()), True,
            ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
    XGrabButton(_display.get(), 3, Mod1Mask, DefaultRootWindow(_display.get()), True,
            ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);

    XEvent event;
    //XWindowAttributes attr;
    //XButtonEvent start;

    //start.subwindow = None;
    while (true) {

        XNextEvent(_display.get(), &event);

        // ignore the event if we don't have a mapped handler for it
        if (_handlers.find(event.type) == _handlers.end())
            continue;

        // call the handler based on the event type received
        (this->*_handlers[event.type])(event);
    }
}

void XLibpp::setStatusBar()
{
    XLibWindow statusbar(_display);
    statusbar.x(0);
    statusbar.y(0);
    statusbar.width(width(0) - 1);
    statusbar.height(height(0) / 50);
    statusbar.parent(_window);
    statusbar.create();
}

void XLibpp::mapRequest(XEvent &e)
{
    std::cout << "e type: " << e.type << std::endl;
    std::cout << "window: " << e.xmaprequest.window << std::endl;
    XTextProperty title;
    XGetWMName(_display.get(), e.xmaprequest.window, &title);
    std::cout << "window name: " << title.value << std::endl;
    std::cout << "***************************************" << std::endl;

    XLibWindow wnd(_display);
    wnd.x(0);
    wnd.y(50);
    wnd.width(200);
    wnd.width(200);
    wnd.parent(e.xmaprequest.parent);
    wnd.window(e.xmaprequest.window);

    //XSetWindowBorderWidth(_display.get(), e.xmaprequest.window, 2);
    //XSetWindowBorder(_display.get(), e.xmaprequest.window, 0x00ff9944);
    XMoveResizeWindow(_display.get(), e.xmaprequest.window, 0, 0, 800, 600);
    //XRaiseWindow(_display.get(), e.xmaprequest.window);
    XMapWindow(_display.get(), e.xmaprequest.window);
}

void XLibpp::keyPress(XEvent &e)
{
    for (KeyMap k : _keyMaps)
    {
        if (XKeysymToKeycode(_display.get(), k.getKey()) == e.xkey.keycode &&
            k.getMod1() | k.getMod2() == e.xkey.state)
        {
            helper::callProgramBg(k.getProgram().c_str());
            break;
        }

    }
}

void XLibpp::buttonPress(XEvent &e)
{
    std::cout << "button press\n";
    /*XRaiseWindow(_display.get(), e.xkey.subwindow);


    XGetWindowAttributes(_display.get(), 
                         e.xbutton.subwindow, 
                         &attr);

    //std::cout << "window: " << event.xbutton.subwindow << std::endl;
    //XUnmapWindow(_display.get(), event.xbutton.subwindow);
    start = e.xbutton;*/
}

void XLibpp::buttonRelease(XEvent &e)
{
    //start.subwindow = None;
}

void XLibpp::motionNotify(XEvent &e)
{
    //std::cout << "mouse move\n";
    /*int xdiff = e.xbutton.x_root - start.x_root;
    int ydiff = e.xbutton.y_root - start.y_root;
    XMoveResizeWindow(_display.get(), start.subwindow,
        attr.x + (start.button==1 ? xdiff : 0),
        attr.y + (start.button==1 ? ydiff : 0),
        std::max(1, attr.width + (start.button==3 ? xdiff : 0)),
        std::max(1, attr.height + (start.button==3 ? ydiff : 0)));*/
}
