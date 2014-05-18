#ifndef _HELPER_H
#define _HELPER_H

#include <string>
#include <vector>

namespace helper
{
    // using 'const char*' instead of std::string due to the nature
    // of fork. std::string allocates memory from the heap, when child
    // exits (by calling _exit()) the std::string destructor will not
    // be called, letting memory leak behind - const char* doesn't have
    // this issue.
    int callProgram(const char *program);

    int callProgramBg(const char *program);

    int callProgram(const char *program,
                    std::string &output,
                    std::string &error);

    int callProgram(const char *program,
                    const std::vector<const char *> &params);

    int callProgram(const char *program,
                    const std::vector<const char *> &params,
                    std::string &output,
                    std::string &error,
                    bool background = false);
}

#endif
