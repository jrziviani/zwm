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

// TODO: fix all code regarding position/size to use class properties
//       instead of always poking XLib.
XLibDesktop::XLibDesktop(logger &logger) :
    IDesktop   (logger),
    _display   {XOpenDisplay(0x0), &XCloseDisplay},
    _statusBar (_display),
    _cursor    (None)
{
    _handlers.insert(make_pair(ButtonPress,   &XLibDesktop::buttonPress));
    _handlers.insert(make_pair(ButtonRelease, &XLibDesktop::buttonRelease));
    _handlers.insert(make_pair(MotionNotify,  &XLibDesktop::motionNotify));
    //_handlers.insert(make_pair(MapNotify,     &XLibDesktop::mapNotify));
    //_handlers.insert(make_pair(Expose,        &XLibDesktop::expose));
    //_handlers.insert(make_pair(LeaveNotify,   &XLibDesktop::enterNotify));
    //_handlers.insert(make_pair(EnterNotify,   &XLibDesktop::enterNotify));
    //_handlers.insert(make_pair(UnmapNotify,   &XLibDesktop::enterNotify));

    _handlers.insert(make_pair(MapRequest,       &XLibDesktop::mapRequest));
    _handlers.insert(make_pair(KeyPress,         &XLibDesktop::keyPress));
    _handlers.insert(make_pair(MappingNotify,    &XLibDesktop::mappingNotify));
    _handlers.insert(make_pair(ConfigureNotify,  &XLibDesktop::configureNotify));
    _handlers.insert(make_pair(ConfigureRequest, &XLibDesktop::configureRequest));
    _handlers.insert(make_pair(DestroyNotify,    &XLibDesktop::destroyNotify));
    //_handlers.insert(make_pair(ClientMessage,    &XLibDesktop::clientMessage));

    _screenNumber = 0;
    initRootWindow(_screenNumber);
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
    _cursor = XCreateFontCursor(_display.get(), XC_left_ptr);
    XDefineCursor(_display.get(), _window, _cursor);

    // choose the events we want to handle
    XSelectInput(_display.get(), _window, SubstructureRedirectMask |
                                          //FocusChangeMask |
                                          //VisibilityChangeMask |
                                          SubstructureNotifyMask |
                                          //ButtonPressMask |
                                          //EnterWindowMask |
                                          //LeaveWindowMask |
                                          //StructureNotifyMask |
                                          //PropertyChangeMask |
                                          //ExposureMask |
                                          KeyPressMask);

    setAccelKeys();
    setStatusBar();
}

unsigned int NumlockMask(Display *display)
{
	XModifierKeymap *modifierKeymap;
	modifierKeymap = XGetModifierMapping(display);

	unsigned int numlockMask = 0;
	for(int i = 0; i < 8; i++)
    {
		for(int j = 0; j < modifierKeymap->max_keypermod; j++)
        {
			if(modifierKeymap->modifiermap[i * modifierKeymap->max_keypermod + j]
			   == XKeysymToKeycode(display, XK_Num_Lock))
            {
				numlockMask = (1 << i);
            }
        }
    }
	XFreeModifiermap(modifierKeymap);

    return numlockMask;
}

void XLibDesktop::setAccelKeys()
{
    XUngrabButton(_display.get(), AnyButton, AnyModifier, _window);

    unsigned int numlockMask = NumlockMask(_display.get());
    unsigned int modifiers[] = {0, LockMask, numlockMask,
                                numlockMask | LockMask};
    unsigned int buttons[] = {Button1, Button3};

    for (KeyMap k : _keyMaps)
    {
        XGrabKey(_display.get(),
                 XKeysymToKeycode(_display.get(), k.getKey()),
                 AnyModifier,
                 _window,
                 True,
                 GrabModeAsync,
                 GrabModeAsync);
    }

    // grab the mouse left button
    XGrabButton(_display.get(),
                1,
                Mod1Mask,
                _window,
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
                _window,
                True,
                ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
                GrabModeAsync,
                GrabModeAsync,
                None,
                None);

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

    XSync(_display.get(), False);

    while (true) {

        XNextEvent(_display.get(), &event);

        std::cout << "Evt type: " << event.type << " - wnd: " <<  event.xany.window << std::endl;

        // ignore the event if we don't have a mapped handler for it
        if (_handlers.find(event.type) == _handlers.end())
            continue;

        // call the handler based on the event type received
        (this->*_handlers[event.type])(event, args);

        if (args.buttonPressed == button_t::MIDDLE)
            break;
    }
}

void XLibDesktop::tiling()
{
    unsigned int nonFloatingWindows = 0;

    using lbPair = std::pair<const long unsigned int, 
                             std::unique_ptr<IWindow>>;

    // couting only non-floating windows to calcule the areas
    // for tiling them.
    std::for_each(
            _desktops[_currentDesktop].begin(),
            _desktops[_currentDesktop].end(),

            // lambda - use nonFloatingWindows variable as
            // reference, it will be incresead if the window is
            // not floating
            [&nonFloatingWindows](lbPair& wnd)
                { 
                    if (!wnd.second->floating())
                        ++nonFloatingWindows;
                }
            );

    // get where the windows will be located and their size
    // in the screen based on how many windows must be displayed.
    std::vector<position_t> areas = getAreas(
        width(_screenNumber),
        height(_screenNumber),
        nonFloatingWindows);

    unsigned int i = 0;
    for (auto &kv : _desktops[_currentDesktop])
    {
        // floating windows will be mapped using its default size.
        if (kv.second->floating())
        {
            // get the window attributes for the floating window.
            XWindowAttributes attributes;
            XGetWindowAttributes(_display.get(),
                                 kv.first,
                                 &attributes);

            // set the window desired location/size
            kv.second->x      (attributes.x);
            kv.second->y      (attributes.y);
            kv.second->width  (attributes.width);
            kv.second->height (attributes.height);

            // bring it to upfront
            kv.second->redraw();
            continue;
        }

        // fix small rounding errors and discount the status
        // bar size.
        if (areas[i].y < 10)
            areas[i].y = 0;
        
        // set place/size and draw (map) the window.
        kv.second->x     (areas[i].x);
        kv.second->y     (areas[i].y);
        kv.second->width (areas[i].w);
        kv.second->height(areas[i].h);
        kv.second->redraw();

        ++i;
    }
}

void XLibDesktop::changeFocus(bool next)
{
    Window focusSearchWnd;
    int unused;

    // get the window that has the focus.
    XGetInputFocus(_display.get(), 
                   &focusSearchWnd,
                   &unused);

    // ignore if no window has focus.
    if (focusSearchWnd == None || focusSearchWnd == PointerRoot)
        return;

    // try to find the focused window in the map.
    virtualDesktop::const_iterator focused = 
        _desktops[_currentDesktop].find(focusSearchWnd);

    Window newFocusWnd;

    // set focus to first window if window not found
    if (focused == _desktops[_currentDesktop].end())
        newFocusWnd = _desktops[_currentDesktop].begin()->first;

    else
    {
        // iterate the map to the next window available
        ++focused;

        // set focus to first window if window not found
        if (focused == _desktops[_currentDesktop].end())
            newFocusWnd = _desktops[_currentDesktop].begin()->first;
    
        // or use the next one
        else
            newFocusWnd = focused->first;
    }

    // raise the window (just in case) and set the focus
    XMapRaised(_display.get(), newFocusWnd);

    XSetInputFocus(_display.get(),
                   newFocusWnd, 
                   RevertToNone, 
                   CurrentTime);
}

void XLibDesktop::setStatusBar()
{
    /*_statusBar.x(0);
    _statusBar.y(0);
    _statusBar.width(width(0) - 1);
    _statusBar.height(height(0) / 50);
    _statusBar.parent(_window);
    _statusBar.setColormap(getColormap(0));
    _statusBar.setVisual(getVisual(0));
    _statusBar.create(depth(0));
    _statusBar.setClock("");*/
}

void XLibDesktop::mapRequest(XEvent &e, args_t &arg)
{
    DEBUG(_logger, "Handling map request event");
    DEBUG(_logger, "Event type: " << e.type);
    DEBUG(_logger, "Window ID: " << e.xmaprequest.window);
    std::cout << e.xmaprequest.window << std::endl;

    // for now, only map from desktop.
    if (e.xmaprequest.parent != _window)
        return;

    XWindowAttributes attributes;

    // return when we cannot get window attributes.
    // TODO: investigate when this situation could happen (exit then?).
    if(!XGetWindowAttributes(_display.get(), 
                             e.xmaprequest.window, 
                             &attributes))
        return;

    // do not map windows that asked for it.
    if(attributes.override_redirect)
        return;
    
    // dont duplicate a window.
    if (_desktops[_currentDesktop].find(e.xmaprequest.window) 
            != _desktops[_currentDesktop].end())
        return;

    Window transient;
    XGetTransientForHint(_display.get(),
                         e.xmaprequest.window,
                         &transient);

    XLibWindow wnd(_display);
    std::unique_ptr<XLibWindow> pWindow(new XLibWindow(_display));

    // when opening a transient window we don't tile it but
    // simply open using its default setup.
    pWindow->floating(transient != None);

    // set parent and window id for controlling.
    pWindow->parent(e.xmaprequest.parent);
    pWindow->window(e.xmaprequest.window);

    // TODO: this setup should have been managed by pWindow.
    /*XSelectInput(_display.get(),
                 e.xmaprequest.window,
                 EnterWindowMask |
                 FocusChangeMask |
                 ButtonPressMask |
                 StructureNotifyMask);*/

    // set the border width.
    XSetWindowBorderWidth(_display.get(),  e.xmaprequest.window, 1);

    // add the window in the current virtual desktop.
    _desktops[_currentDesktop].insert(
            make_pair(
                e.xmaprequest.window, 
                std::move(pWindow)
                )
            );

    // not force configure event event for floating windows
    if (transient != None)
    {
        // map the window and bring it to the top of the stack.
        XRaiseWindow(_display.get(), e.xmaprequest.window);

        return;
    }

    // disable request processing for now.
    XGrabServer(_display.get());

    // map the window and bring it to the top of the stack.
    XRaiseWindow(_display.get(), e.xmaprequest.window);

    // flush the x buffer (discards all events in queue).
    XSync(_display.get(), True);

    // as soon as we map the window we need to force windows to be
    // tiled, this is done by ConfigureModify event (triggered by this
    // ConfigureRequest).
    XEvent configureEvent;
    configureEvent.type = ConfigureRequest;
    configureEvent.xconfigurerequest.window = e.xmaprequest.window;
    configureEvent.xconfigurerequest.width = 1;
    configureEvent.xconfigurerequest.height = 1;
    configureEvent.xconfigurerequest.value_mask = CWWidth | CWHeight;

    XSendEvent(_display.get(), 
               _window, 
               SubstructureRedirectMask,
               True,
               &configureEvent);

    // flush the buffer but now we want our events untouched
    XSync(_display.get(), False);

    // release the request processing
    XUngrabServer(_display.get());
}

void XLibDesktop::keyPress(XEvent &e, args_t &arg)
{
    DEBUG(_logger, "Handling key press event");

    std::cout << "Wnd pressed: " << e.xkey.subwindow << std::endl;

    if (e.type == KeyRelease) {
        return;
    }

    if (e.xkey.subwindow != None)
    {
        std::cout << "here...\n";
        XRaiseWindow(_display.get(), e.xkey.subwindow);
    }

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

            if (k.getProgram() == "rotate")
            {
                changeFocus();
                return;
            }

            // dont open more than MAX_WINDOW_PER_DESKTOP windows per virtual
            // desktop
            if (_desktops[_currentDesktop].size() >= MAX_WINDOW_PER_DESKTOP - 1)
                return;

            INFO(_logger, "Starting program " << k.getProgram());
            std::cout << k.getProgram() << std::endl;
            helper::callProgramBg(k.getProgram().c_str());
            return;
        }
    }
}

void XLibDesktop::buttonPress(XEvent &e, args_t &arg)
{
    DEBUG(_logger, "Handling button press event");

    // get current active desktop
    virtualDesktop &desktop = _desktops[_currentDesktop];

    // only handle registered windows
    if (desktop.find(e.xbutton.subwindow) == desktop.end())
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
    arg.windowPosition.x = desktop[arg.windowid]->x();
    arg.windowPosition.y = desktop[arg.windowid]->y();
    arg.windowPosition.w = desktop[arg.windowid]->width();
    arg.windowPosition.h = desktop[arg.windowid]->height();
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
    /*if (e.xcrossing.window == None)
    {
        ERROR(_logger, "window none");
        return;
    }

    XTextProperty title;
    XGetWMName(_display.get(), e.xcrossing.window, &title);

    if (!title.value)
    {
        _statusBar.drawStatusTitle("");
        return;
    }

    std::string tmp((char*) title.value);
    INFO(_logger, tmp);
    _statusBar.drawStatusTitle(tmp);
    XFree((char*)(title.value));*/
}

void XLibDesktop::expose(XEvent &e, args_t &arg)
{
    // while(XCheckTypedEvent(_display.get(), MotionNotify, &e));

    //_statusBar.drawClock();*/
}

void XLibDesktop::mapNotify(XEvent &e, args_t &arg)
{
    /*DEBUG(_logger, "Handling map notify event");
    XTextProperty title;
    XGetWMName(_display.get(), e.xmap.window, &title);

    if (!title.value)
        return;

    std::string tmp((char*) title.value);
    INFO(_logger, tmp);
    _statusBar.drawStatusTitle(tmp);
    XFree((char*)(title.value));
    */
}

void XLibDesktop::motionNotify(XEvent &e, args_t &arg)
{
    // get only the last move event, discarding the avalanche
    while(XCheckTypedEvent(_display.get(), MotionNotify, &e));

    if (arg.buttonPressed == button_t::NONE)
        return;

    DEBUG(_logger, "Handling motion event");

    // get current active desktop
    virtualDesktop &desktop = _desktops[_currentDesktop];

    assert(desktop.find(arg.windowid) != desktop.end());

    //std::cout << "mouse move\n";
    IWindow &rWindow = *desktop[arg.windowid];

    if (!rWindow.floating())
        return;

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

void XLibDesktop::mappingNotify(XEvent &e, args_t &arg)
{
    std::cout << "mapping notify\n";

    XRefreshKeyboardMapping(&e.xmapping);

    if(e.xmapping.request == MappingKeyboard)
    {
        std::cout << "accel keys...\n";
        setAccelKeys();
    }
}

void XLibDesktop::destroyNotify(XEvent &e, args_t &arg)
{
    // ignore event if the window destroyed is not mapped.
    virtualDesktop &desktop = _desktops[_currentDesktop];
    if (desktop.find(e.xdestroywindow.window) == desktop.end())
        return;

    // remove the window from the array.
    desktop.erase(e.xdestroywindow.window);

    // organize the windows
    tiling();
}

void XLibDesktop::clientMessage(XEvent &e, args_t &arg)
{
    /*
    std::cout << "---------------------\n";
    std::cout << "Client Message" << std::endl;
    std::cout << "atom: " << XGetAtomName(_display.get(), e.xclient.message_type) << std::endl;
    std::cout << "send: " << e.xclient.send_event << std::endl;
    std::cout << "wnd: " << e.xclient.window << std::endl;
    std::cout << "format: " << e.xclient.format << std::endl;
    std::cout << "data: " << e.xclient.data.l[0] << std::endl;
    std::cout << "data: " << XGetAtomName(_display.get(), e.xclient.data.l[1]) << std::endl;
    std::cout << "data: " << e.xclient.data.l[2] << std::endl;
    std::cout << "data: " << e.xclient.data.l[3] << std::endl;
    std::cout << "data: " << e.xclient.data.l[4] << std::endl;
    std::cout << "====================" << std::endl;
    */
}

void XLibDesktop::configureNotify(XEvent &e, args_t &arg)
{
    // compress repeated events.
    while(XCheckTypedWindowEvent(_display.get(), e.xconfigure.window, ConfigureNotify, &e));

    // ignore events for windows not mapped yet.
    virtualDesktop &desktop = _desktops[_currentDesktop];
    if (desktop.find(e.xconfigure.window) == desktop.end())
    {
        DEBUG(_logger, "Window is not mapped yet");
        return;
    }

    // ignore configure notify not fired from the desktop.
    if (e.xconfigure.event != _window)
        return;

    // organize the windows.
    tiling();

    // set the focus in the window notified.
    XSetInputFocus(_display.get(),
                   e.xconfigure.window,
                   RevertToParent, 
                   CurrentTime);

}

void XLibDesktop::configureRequest(XEvent &e, args_t &arg)
{
    XWindowChanges changes;
    changes.x            = e.xconfigurerequest.x;
    changes.y            = e.xconfigurerequest.y;
    changes.width        = e.xconfigurerequest.width;
    changes.height       = e.xconfigurerequest.height;
    changes.border_width = e.xconfigurerequest.border_width;
    changes.sibling      = e.xconfigurerequest.above;
    changes.stack_mode   = e.xconfigurerequest.detail;

    XConfigureWindow(_display.get(),
                     e.xconfigurerequest.window,
                     e.xconfigurerequest.value_mask, 
                     &changes);
}
