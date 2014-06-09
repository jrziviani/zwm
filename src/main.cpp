
#include "types.h"
#include "log.h"
#include "idesktop.h"
#include "xlibdesktop.h"
#include "helper.h"
#include "keymap.h"

#include <iostream>
#include <string>
#include <fstream>

/* TODO: accel keys should go to a config file */
KeyMaps maps {
    KeyMap { XStringToKeysym("F1"), Mod1Mask, 0, "xterm" },
    KeyMap { XStringToKeysym("F2"), Mod1Mask, 0, "gnome-calculator" },
    KeyMap { XStringToKeysym("F3"), Mod1Mask, 0, "gnome-terminal" },
    KeyMap { XStringToKeysym("F4"), Mod1Mask, 0, "firefox" },
    KeyMap { XStringToKeysym("Q"), Mod1Mask, 0, "quit" }
};

int main(int argc, char *argv[])
{
    // set logger
    std::ofstream ofs ("build/zwm.log", std::ofstream::out);
    logger myLog(ofs);

    // TODO: implement a factory to create the real desktop
    IDesktop *pDesktop = new XLibDesktop(myLog);
    pDesktop->setKeyMaps(maps);
    pDesktop->loop();
    delete pDesktop;

    /*using namespace std;
    string o, e;

    cout << "---------------------\n";
    //cout << "r1: " << helper::callProgram("ls") << endl;
    cout << "r1: " << helper::callProgram("ls", {"-l", "-a", "--color"}, o, e ) << endl;
    cout << "Out: " << o << endl;
    cout << "Err: " << e << endl;

    cout << "---------------------\n";

    cout << "r2: " << helper::callProgram("lsd", o ,e) << endl;
    cout << "Out: " << o << endl;
    cout << "Err: " << e << endl;

    cout << "---------------------\n";

    cout << "r3: " << helper::callProgram("echo", {"Oi"}, o, e ) << endl;
    cout << "Out: " << o << endl;
    cout << "Err: " << e << endl;*/

    /*
    std::cout << "Screens: " << xlib.numberOfScreens() << std::endl;
    std::cout << "Resolution: " << xlib.width(0) << " x " << xlib.height(0) << std::endl;
    std::cout << "Depth: " << xlib.depth(0) << std::endl;
    std::cout << "Bye...\n";
    */

    return 0;
}
