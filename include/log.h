#ifndef _LOG_H
#define _LOG_H

//
// INCLUDES
//
#include <sstream>
#include <iostream>
#include <fstream>


//
// MACROS
//
#define LOG(LOGGER, LEVEL, MESSAGE)             \
    do {                                        \
        std::ostringstream o;                   \
        o << #LEVEL    << " - "                 \
          << __FILE__  << " - "                 \
          << __LINE__  << " - "                 \
          << __FUNCTION__  << " - "             \
          << MESSAGE;                           \
        LOGGER.log(o.str());                    \
    } while(0);

#define DEBUG(LOGGER, MESSAGE)                  \
    LOG(LOGGER, DEBUG, MESSAGE)

#define INFO(LOGGER, MESSAGE)                   \
    LOG(LOGGER, INFO, MESSAGE)

#define WARNING(LOGGER, MESSAGE)                \
    LOG(LOGGER, WARNING, MESSAGE)

#define ERROR(LOGGER, MESSAGE)                  \
    LOG(LOGGER, ERROR, MESSAGE)

#define CRITICAL(LOGGER, MESSAGE)               \
    LOG(LOGGER, CRITICAL, MESSAGE)


//
// DECLARATIONS
//

// log level - not used yet.
enum level_t { DEBUG, INFO, WARNING, ERROR, CRITICAL };

// Declares and implements a basic log system for this program. It's
// intended to have only one instance, created in the program's entry
// function and all clients would receveive a const reference of it.
//
// The log method shouldn't be used directly but through the MACROS
// created above. It gives the facility to append any type using '<<'
// operator since MACROS uses stringstream.
//
// Example:
//   logger myLog;
//   INFO(myLog, "Number: " << 42 << " - " << anyvar);
//   DEBUG(myLog, anyvar);
//
// TODO: Profile this class.
// TODO: Improve debug mode for speed.
class logger
{
    public:

        // Creates a logger using stdout or any output stream
        // desired by the user
        logger(std::ostream& output = std::cout) : 
            _output(output)
        {
        }

        // Prints the log message into the ostream
        void log(std::string const& msg)
        {
            _output << msg << "\n";
            _output.flush();
        }

    private:

        // References to the output stream desired by users.
        std::ostream &_output;
};

#endif
