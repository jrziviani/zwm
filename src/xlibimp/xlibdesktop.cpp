#include "xlibdesktop.h"
#include "helper.h"

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

XLibDesktop::XLibDesktop() :
    _display { XOpenDisplay(0x0), &XCloseDisplay }
{
    _handlers.insert(make_pair(MapRequest,    &XLibDesktop::mapRequest));
    _handlers.insert(make_pair(KeyPress,      &XLibDesktop::keyPress));
    _handlers.insert(make_pair(ButtonPress,   &XLibDesktop::buttonPress));
    _handlers.insert(make_pair(ButtonRelease, &XLibDesktop::buttonRelease));
    _handlers.insert(make_pair(MotionNotify,  &XLibDesktop::motionNotify));
}

int XLibDesktop::width(int screenNumber)
{
    assert(_display.get() != nullptr);
    assert(screenNumber >= 0 && screenNumber < this->getNumberOfScreens());

    return DisplayWidth(_display.get(), screenNumber);
}

int XLibDesktop::height(int screenNumber)
{
    assert(_display.get() != nullptr);
    assert(screenNumber >= 0 && screenNumber < this->getNumberOfScreens());

    return DisplayHeight(_display.get(), screenNumber);
}

int XLibDesktop::depth(int screenNumber)
{
    assert(_display.get() != nullptr);
    assert(screenNumber >= 0 && screenNumber < this->getNumberOfScreens());

    return DefaultDepth(_display.get(), screenNumber);
}

int XLibDesktop::getNumberOfScreens()
{
    assert(_display.get() != nullptr);

    return ScreenCount(_display.get());
}

int XLibDesktop::initRootWindow(int screenNumber)
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
    XGrabButton(_display.get(),
                1,
                Mod1Mask,
                DefaultRootWindow(_display.get()),
                True,
                ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
                GrabModeAsync,
                GrabModeAsync,
                None,
                None);

    XGrabButton(_display.get(),
                3,
                Mod1Mask,
                DefaultRootWindow(_display.get()),
                True,
                ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
                GrabModeAsync,
                GrabModeAsync,
                None,
                None);

    setStatusBar();
    setAccelKeys();
}

void XLibDesktop::setAccelKeys()
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

Window XLibDesktop::getWindowByPID(unsigned long pid)
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

void XLibDesktop::loop()
{
    std::cout << "main loop...\n";
    XEvent event;

    args_t args;
    args.buttonPressed = button_t::NONE;

    while (true) {

        XNextEvent(_display.get(), &event);

        // ignore the event if we don't have a mapped handler for it
        if (_handlers.find(event.type) == _handlers.end())
            continue;

        // call the handler based on the event type received
        (this->*_handlers[event.type])(event, args);
    }
}

void XLibDesktop::setStatusBar()
{
    XLibWindow statusbar(_display);
    statusbar.x(0);
    statusbar.y(0);
    statusbar.width(width(0) - 1);
    statusbar.height(height(0) / 50);
    statusbar.parent(_window);
    statusbar.create();
}

void XLibDesktop::mapRequest(XEvent &e, args_t &arg)
{
    std::cout << "e type: " << e.type << std::endl;
    std::cout << "window: " << e.xmaprequest.window << std::endl;
    XTextProperty title;
    XGetWMName(_display.get(), e.xmaprequest.window, &title);
    std::cout << "window name: " << title.value << std::endl;
    std::cout << "***************************************" << std::endl;

    XLibWindow wnd(_display);
    std::unique_ptr<XLibWindow> pWindow(new XLibWindow(_display));
    pWindow->x(0);
    pWindow->y(50);
    pWindow->width(200);
    pWindow->height(200);
    pWindow->parent(e.xmaprequest.parent);
    pWindow->window(e.xmaprequest.window);
    pWindow->redraw();
    _children.insert(make_pair(e.xmaprequest.window, std::move(pWindow)));
}

void XLibDesktop::keyPress(XEvent &e, args_t &arg)
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

void XLibDesktop::buttonPress(XEvent &e, args_t &arg)
{
    std::cout << "button press\n";

    if (e.xbutton.subwindow == _window ||
        e.xbutton.subwindow == None)
        return;

    XRaiseWindow(_display.get(), e.xbutton.subwindow);

    switch (e.xbutton.button)
    {
        case 1:
            arg.buttonPressed = button_t::LEFT;
            break;

        case 2:
            break;

        case 3:
            arg.buttonPressed = button_t::RIGHT;
            break;
    }

    arg.windowid         = e.xbutton.subwindow;
    arg.windowPosition.x = _children[arg.windowid]->x();
    arg.windowPosition.y = _children[arg.windowid]->y();
    arg.windowPosition.w = _children[arg.windowid]->width();
    arg.windowPosition.h = _children[arg.windowid]->height();
    arg.buttonPosition.x = e.xbutton.x_root;
    arg.buttonPosition.y = e.xbutton.y_root;
}

void XLibDesktop::buttonRelease(XEvent &e, args_t &arg)
{
    arg.buttonPressed = button_t::NONE;
}

void XLibDesktop::motionNotify(XEvent &e, args_t &arg)
{
    // get only the last move event, discarding the avalanche
    while(XCheckTypedEvent(_display.get(), MotionNotify, &e));

    if (arg.buttonPressed == button_t::NONE)
        return;

    assert(_children.find(arg.windowid) != _children.end());

    //std::cout << "mouse move\n";
    IWindow &rWindow = *_children[arg.windowid];

    int xdiff = e.xbutton.x_root - arg.buttonPosition.x;
    int ydiff = e.xbutton.y_root - arg.buttonPosition.y;

    if (arg.buttonPressed == button_t::LEFT)
    {
        rWindow.move(arg.windowPosition.x + xdiff,
                     arg.windowPosition.y + ydiff);
    }
    else if (arg.buttonPressed == button_t::RIGHT)
        rWindow.resize(arg.windowPosition.w + xdiff,
                       arg.windowPosition.h + ydiff);
}