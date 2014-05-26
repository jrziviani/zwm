#ifndef _KEYMAP_H
#define _KEYMAP_H


//
// INCLUDES
// 
#include "types.h"

#include <vector>
#include <string>


//
// DECLARATIONS
//

// Creates a map with the keycode (with modification keys)
// and the program this combination will executes.
// TODO: Make it more flexible to accept actions (like ALT+TAB)
//       instead of programs only.
class KeyMap
{
    public:
        explicit KeyMap (keysym key,
                         unsigned int modkey1,
                         unsigned int modkey2,
                         std::string program);

        // Gets the keycode.
        keysym getKey();

        // Gets the mod key 1.
        unsigned int getMod1();

        // Gets the mod key 2.
        unsigned int getMod2();

        // Gets the program full path to execute.
        std::string getProgram();

    private:

        // Key code.
        keysym _key;

        // Mod key 1
        unsigned int _modkey1;

        // Mod key 2
        unsigned int _modkey2;

        // Program's full path
        std::string _program;
};


//
// TYPEDEFS
//

// Defines the vector to receive all keymaps defined by the user
typedef std::vector<KeyMap> KeyMaps;

#endif
