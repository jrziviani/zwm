
#include "types.h"
#include "idesktop.h"
#include "xlibdesktop.h"
#include "helper.h"
#include "keymap.h"

#include <iostream>
#include <string>

/* TODO: accel keys should go to a config file */
KeyMaps maps {
    KeyMap { XStringToKeysym("F1"), Mod1Mask, 0, "xterm" },
    KeyMap { XStringToKeysym("F2"), Mod1Mask, 0, "gnome-calculator" },
    KeyMap { XStringToKeysym("F3"), Mod1Mask, 0, "gnome-terminal" }
};

int main(int argc, char *argv[])
{
    for (KeyMap m : maps)
    {
        std::cout << m.getProgram() << std::endl;
    }

    // TODO: implement a factory to create the real desktop
    std::unique_ptr<IDesktop> pDesktop(new XLibDesktop);
    pDesktop->setKeyMaps(maps);
    pDesktop->initRootWindow(0);
    pDesktop->loop();

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
