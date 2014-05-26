#ifndef _HELPER_H
#define _HELPER_H

//
// INCLUDES
//
#include <string>
#include <vector>


//
// DECLARATIONS
//

// helper is an attempt to organize functions that makes no sense in have
// its particular class, all spawn functions are implemented here.
namespace helper
{
    // Calls an arbitrary program in a separate process. It returns the
    // output (and error) from the program into output and error. Also
    // returns the program's return code.
    //
    // Note: program running in background mode will not give back the
    // output and error, only its pid will be returned.
    //
    // Example:
    // string out, err;
    // int rcode = callProgram("ls", {"-l", "-a", "--color"}, out, err);
    //
    // pid_t pid = callProgramBg("xterm");
    //
    // Using 'const char*' instead of std::string due to the nature
    // of fork. std::string allocates memory from the heap, when child
    // exits (by calling _exit()) the std::string destructor will not
    // be called, letting memory leak behind - const char* doesn't have
    // this issue.
    int callProgram(const char *program,
                    const std::vector<const char *> &params,
                    std::string &output,
                    std::string &error,
                    bool background = false);

    // Alias for callProgram in foreground mode without expecting outputs.
    int callProgram(const char *program);

    // Alias for callProgram in foreground mode.
    int callProgram(const char *program,
                    std::string &output,
                    std::string &error);

    // Alias for callProgram with extra params.
    int callProgram(const char *program,
                    const std::vector<const char *> &params);

    // Alias for callProgram in background mode.
    // TODO: fix the return type to pid.
    int callProgramBg(const char *program);
}

#endif
