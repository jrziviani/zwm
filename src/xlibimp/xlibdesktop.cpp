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

XLibDesktop::XLibDesktop(logger &logger) :
    IDesktop   (logger),
    _display   {XOpenDisplay(0x0), &XCloseDisplay},
    _statusBar (_display),
    _cursor    (None)
{
    _handlers.insert(make_pair(MapRequest,    &XLibDesktop::mapRequest));
    _handlers.insert(make_pair(KeyPress,      &XLibDesktop::keyPress));
    _handlers.insert(make_pair(ButtonPress,   &XLibDesktop::buttonPress));
    _handlers.insert(make_pair(ButtonRelease, &XLibDesktop::buttonRelease));
    _handlers.insert(make_pair(MotionNotify,  &XLibDesktop::motionNotify));
    _handlers.insert(make_pair(MapNotify,     &XLibDesktop::mapNotify));
    _handlers.insert(make_pair(LeaveNotify,   &XLibDesktop::enterNotify));
    _handlers.insert(make_pair(EnterNotify,   &XLibDesktop::enterNotify));

    initRootWindow(0);
}

XLibDesktop::~XLibDesktop()
{
    XUndefineCursor(_display.get(), _window);

    if (_cursor != None)
        XFreeCursor(_display.get(), _cursor);
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

Colormap XLibDesktop::getColormap(int screenNumber)
{
    assert(_display.get() != nullptr);

    return DefaultColormap(_display.get(), screenNumber);
}

Visual *XLibDesktop::getVisual(int screenNumber)
{
    assert(_display.get() != nullptr);

    return DefaultVisual(_display.get(), screenNumber);
}

void XLibDesktop::initRootWindow(int screenNumber)
{
    assert(_display.get() != nullptr);
    assert(screenNumber >= 0 && screenNumber < this->getNumberOfScreens());

    // TODO: get the root window based on the screenNumber
    DEBUG(_logger, "Number of screens: " << getNumberOfScreens());

    _window = XDefaultRootWindow(_display.get());

    // set the cursor used in desktop
    //_cursor = XCreateFontCursor(_display.get(), XC_left_ptr);
    //XDefineCursor(_display.get(), _window, _cursor);

    // choose the events we want to handle
    XSelectInput(_display.get(), _window, SubstructureRedirectMask |
                                          FocusChangeMask |
                                          VisibilityChangeMask |
                                          SubstructureNotifyMask |
                                          ButtonPressMask |
                                          EnterWindowMask |
                                          LeaveWindowMask |
                                          StructureNotifyMask |
                                          PropertyChangeMask |
                                          ExposureMask |
                                          KeyPressMask);

    // grab the mouse left button
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

    // grab the mouse right button
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
        XQueryTree(_display.get(),
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

    return None;
}

void XLibDesktop::loop()
{
    DEBUG(_logger, "Main loop");
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

        if (args.buttonPressed == button_t::MIDDLE)
            break;
    }
}

void XLibDesktop::setStatusBar()
{
    _statusBar.x(0);
    _statusBar.y(0);
    _statusBar.width(width(0) - 1);
    _statusBar.height(height(0) / 50);
    _statusBar.parent(_window);
    _statusBar.setColormap(getColormap(0));
    _statusBar.setVisual(getVisual(0));
    _statusBar.create(depth(0));
    _statusBar.setClock("");
}

void XLibDesktop::mapRequest(XEvent &e, args_t &arg)
{
    DEBUG(_logger, "Handling map request event");
    DEBUG(_logger, "Event type: " << e.type);
    DEBUG(_logger, "Window ID: " << e.xmaprequest.window);

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
    DEBUG(_logger, "Exiting mapRequest..");
}

void XLibDesktop::keyPress(XEvent &e, args_t &arg)
{
    DEBUG(_logger, "Handling key press event");

    for (KeyMap k : _keyMaps)
    {
        if (XKeysymToKeycode(_display.get(), k.getKey()) == e.xkey.keycode &&
            (k.getMod1() | k.getMod2()) == e.xkey.state)
        {
            if (k.getProgram() == "quit")
            {
                arg.buttonPressed = button_t::MIDDLE;
                return;
            }
            INFO(_logger, "Starting program " << k.getProgram());
            helper::callProgramBg(k.getProgram().c_str());
        }
    }
}

void XLibDesktop::buttonPress(XEvent &e, args_t &arg)
{
    DEBUG(_logger, "Handling button press event");

    // only handle registered windows
    if (_children.find(e.xbutton.subwindow) == _children.end())
    {
        ERROR(_logger, "Window " 
                      << e.xbutton.subwindow
                      << " can't be changed");
        return;
    }

    XRaiseWindow(_display.get(), e.xbutton.subwindow);

    switch (e.xbutton.button)
    {
        case 1:
            arg.buttonPressed = button_t::LEFT;
            DEBUG(_logger, "LEFT button pressed");
            break;

        case 2:
            break;

        case 3:
            arg.buttonPressed = button_t::RIGHT;
            DEBUG(_logger, "RIGHT button pressed");
            break;
    }

    arg.windowid         = e.xbutton.subwindow;
    arg.windowPosition.x = _children[arg.windowid]->x();
    arg.windowPosition.y = _children[arg.windowid]->y();
    arg.windowPosition.w = _children[arg.windowid]->width();
    arg.windowPosition.h = _children[arg.windowid]->height();
    arg.buttonPosition.x = e.xbutton.x_root;
    arg.buttonPosition.y = e.xbutton.y_root;
    DEBUG(_logger, "exit button press");
}

void XLibDesktop::buttonRelease(XEvent &e, args_t &arg)
{
    DEBUG(_logger, "Handling button release event");
    DEBUG(_logger, "Event type: " << e.type);
    DEBUG(_logger, "Window ID: " << e.xbutton.window);
    arg.buttonPressed = button_t::NONE;
}

void XLibDesktop::enterNotify(XEvent &e, args_t &arg)
{
    DEBUG(_logger, "Handling (focus)enter notify event: " << e.xcrossing.root);
    DEBUG(_logger, "Handling (focus)enter notify event: " << e.xcrossing.window);
    DEBUG(_logger, "Handling (focus)enter notify event: " << e.xcrossing.subwindow);

    if (e.xcrossing.window == None)
    {
        ERROR(_logger, "window none");
        return;
    }
    XTextProperty title;
    XGetWMName(_display.get(), e.xcrossing.window, &title);

    if (!title.value)
        return;

    std::string tmp((char*) title.value);
    INFO(_logger, tmp);
    _statusBar.setStatusTitle(tmp);
    XFree((char*)(title.value));

}

void XLibDesktop::mapNotify(XEvent &e, args_t &arg)
{
    DEBUG(_logger, "Handling map notify event");
    XTextProperty title;
    XGetWMName(_display.get(), e.xmap.window, &title);

    if (!title.value)
        return;

    std::string tmp((char*) title.value);
    INFO(_logger, tmp);
    _statusBar.setStatusTitle(tmp);
    XFree((char*)(title.value));
}

void XLibDesktop::motionNotify(XEvent &e, args_t &arg)
{
    // get only the last move event, discarding the avalanche
    while(XCheckTypedEvent(_display.get(), MotionNotify, &e));

    if (arg.buttonPressed == button_t::NONE)
        return;

    DEBUG(_logger, "Handling motion event");

    assert(_children.find(arg.windowid) != _children.end());

    //std::cout << "mouse move\n";
    IWindow &rWindow = *_children[arg.windowid];

    int xdiff = e.xbutton.x_root - arg.buttonPosition.x;
    int ydiff = e.xbutton.y_root - arg.buttonPosition.y;

    if (arg.buttonPressed == button_t::LEFT)
    {
        rWindow.move(arg.windowPosition.x + xdiff,
                     arg.windowPosition.y + ydiff);

        DEBUG(_logger, "Wnd: " << arg.windowid
                       << " - moved to ("
                       << arg.windowPosition.x + xdiff
                       << " x "
                       << arg.buttonPosition.y + ydiff
                       << ")");
    }
    else if (arg.buttonPressed == button_t::RIGHT)
    {
        rWindow.resize(arg.windowPosition.w + xdiff,
                       arg.windowPosition.h + ydiff);

        DEBUG(_logger, "Wnd: " << arg.windowid
                       << " - resized ("
                       << arg.windowPosition.x + xdiff
                       << " x "
                       << arg.buttonPosition.y + ydiff
                       << ")");
    }
}
