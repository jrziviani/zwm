//
// INCLUDES
//
#include "helper.h"

#include <memory>
#include <iostream>

#include <cstring>
#include <unistd.h>
#include <sys/wait.h>


//
// CONSTANTS
//
char *const ENVIRONMENT[] = {
    const_cast<char*>("/bin"),
    const_cast<char*>("/usr/bin"),
    const_cast<char*>("DISPLAY=:1"),
    NULL
};


//
// IMPLEMENTATION
// 
// using 'const char*' instead of std::string due to the nature
// of fork. std::string allocates memory from the heap, when child
// exits (by calling _exit()) the std::string destructor will not
// be called, letting memory leak behind - const char* doesn't have
// this issue.
int helper::callProgram(const char *program)
{
    std::vector<const char *> tmp;
    return helper::callProgram(program, tmp);
}


int helper::callProgramBg(const char *program)
{
    std::vector<const char *> vtmp;
    std::string stmp;
    return helper::callProgram(program, vtmp, stmp, stmp, true);
}

int helper::callProgram(const char *program,
                        std::string &output,
                        std::string &error)
{
    std::vector<const char *> tmp;
    return helper::callProgram(program, tmp, output, error);
}

int helper::callProgram(const char *program,
                        const std::vector<const char *> &params)
{
    std::vector<const char *> vtmp;
    std::string stmp;
    return helper::callProgram(program, vtmp, stmp, stmp);
}

int helper::callProgram(const char *program,
                        const std::vector<const char *> &params,
                        std::string &output,
                        std::string &error,
                        bool background)
{
    int status;
    int pipefdOut[2];
    int pipefdErr[2];
    int programLen = strlen(program);

    // clear the strings to store the data.
    output.clear();
    error.clear();

    // create a pipe to handle programs output.
    if (pipe(pipefdOut) == -1)
        return -1;

    if (pipe(pipefdErr) == -1)
        return -1;

    // create a child process.
    pid_t pid = fork();

    // failed to fork: return.
    if (pid < 0)
        return -2;

    // child process.
    if (pid == 0)
    {
        // start a new session (become a leader of this new session).
        if (setsid() == -1)
            return -3;

        int result = 0;

        // close the read descriptor because this child will only
        // write in the pipe.
        close(pipefdOut[0]);
        close(pipefdErr[0]);

        // make pipe's write descriptor the stdout for this child
        // process then close the pipefd because we don't want to
        // have 2 copies of the same descriptor.
        dup2(pipefdOut[1], 1);
        close(pipefdOut[1]);

        // make pipe's write descriptor the stderr for this child
        // process then close the pipefd because we don't want to
        // have 2 copies of the same descriptor.
        dup2(pipefdErr[1], 2);
        close(pipefdErr[1]);

        // convert vector to old c string array to deal with unistd lib
        // plus two new spaces:
        //   1 - for programs name (argv[0])
        //   2 - end of paramters array (NULL in the last field) for
        //       execvpe
        char **cparams = new char*[ params.size() + 2 ];

        // intializes array.
        unsigned int i;
        for (i = 0; i < params.size() + 2; ++i)
            cparams[i] = nullptr;

        // copy programs name (argv[0]) into the array.
        cparams[0] = new char[programLen + 1];
        strncpy(cparams[0], program, programLen);
        cparams[0][programLen] = '\0';

        i = 1;
        // copy all required parameters into the array.
        for (const char *param : params)
        {
            int paramLen = strlen(param);
            cparams[i] = new char[paramLen + 1];
            strncpy(cparams[i], param, paramLen);
            cparams[i][paramLen] = '\0';
            ++i;
        }

        // try to execute the program passed.
        if (execvpe(program, cparams, ENVIRONMENT) < 0)
        {
            std::cerr << "Command \"" << program << "\" not found" << std::endl;
            result = 2;
        }

        // clean the memory used case execvpe fails to execute the
        // program passed.
        for (i = 0; i < params.size() + 2; ++i)
        {
            if (cparams[i] == nullptr)
                continue;

            delete[] cparams[i];
            cparams[i] = nullptr;
        }
        delete[] cparams;
        cparams = nullptr;

        // _exit() must be called instead of exit() because the later
        // cleanup static storage and might flush stdio buffers twice.
        _exit(result);
    }

    // parent execution
    else 
    {
        char buffer;

        // close the write descriptor because the parent will only
        // read from the pipe.
        close(pipefdOut[1]);
        close(pipefdErr[1]);

        // don't wait if we want programs running
        // in background, simple return with the child pid.
        if (background)
            return pid;

        // wait for the command finishes.
        waitpid(pid, &status, 0);

        // save the output results.
        while (read(pipefdOut[0], &buffer, 1) > 0)
            output += buffer;

        // save the error results.
        while (read(pipefdErr[0], &buffer, 1) > 0)
            error += buffer;

        std::cout << output << std::endl;
        std::cout << error << std::endl;
    }

    return WEXITSTATUS(status);
}
