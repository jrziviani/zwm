#ifndef _LOG_H
#define _LOG_H

#include <sstream>
#include <iostream>
#include <fstream>

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

enum level_t { DEBUG, INFO, WARNING, ERROR, CRITICAL };

class logger
{
    public:

        logger(std::ostream& output = std::cout) : 
            _output(output)
        {
        }

        void log(std::string const& msg)
        {
            _output << msg << "\n";
            _output.flush();
        }

    private:

        std::ostream &_output;
};

#endif
