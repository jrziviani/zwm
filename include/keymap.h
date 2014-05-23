#ifndef _KEYMAP_H
#define _KEYMAP_H

#include "types.h"

#include <vector>
#include <string>
#include <X11/Xlib.h>

class KeyMap
{
    public:
        explicit KeyMap (keysym key,
                         unsigned int modkey1,
                         unsigned int modkey2,
                         std::string program);

        keysym getKey();
        unsigned int getMod1();
        unsigned int getMod2();
        std::string getProgram();

    private:
        keysym _key;
        unsigned int _modkey1;
        unsigned int _modkey2;
        std::string _program;
};

typedef std::vector<KeyMap> KeyMaps;

#endif
