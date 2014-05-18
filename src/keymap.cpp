#include "keymap.h"

KeyMap::KeyMap(KeySym key,
               unsigned int modkey1,
               unsigned int modkey2,
               std::string program)
    : _key(key), _modkey1(modkey1), 
      _modkey2(modkey2), _program(program)
{

}

KeySym KeyMap::getKey()
{
    return _key;
}

unsigned int KeyMap::getMod1()
{
    return _modkey1;
}

unsigned int KeyMap::getMod2()
{
    return _modkey2;
}

std::string KeyMap::getProgram()
{
    return _program;
}
