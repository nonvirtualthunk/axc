//
// Created by nvt on 8/4/16.
//

#ifndef TEST2_NOTO_H
#define TEST2_NOTO_H


#include <string>
#include <spdlog/spdlog.h>

class Noto {
public:
    const static int DEBUG_LEVEL = 0;
    const static int FINE_LEVEL = 1;
    const static int INFO_LEVEL = 2;
    const static int WARN_LEVEL = 3;
    const static int ERROR_LEVEL = 4;

    static std::shared_ptr<spdlog::logger> getConsole();

    static int logLevel;

    template<typename... Args>
    static void info(const char *fmt, const Args &... args) {
        log(INFO_LEVEL,fmt,args...);
    }
    template<typename... Args>
    static void warn(const char *fmt, const Args &... args) {
        log(WARN_LEVEL,fmt,args...);
    }
    template<typename... Args>
    static void error(const char *fmt, const Args &... args) {
        log(ERROR_LEVEL,fmt,args...);
    }

    template<typename... Args>
    static void log(int level, const char *fmt, const Args &... args) {
        if (level == INFO_LEVEL) {
            getConsole()->info(fmt, args...);
        } else if (level == WARN_LEVEL) {
            getConsole()->warn(fmt, args...);
        } else if (level == ERROR_LEVEL) {
            getConsole()->error(fmt, args...);
        } else {
            getConsole()->error("Haven't supported log level %i yet",level);
        }
    }

    static bool checkLevel(int level) {
        return level >= logLevel;
    }
};


#endif //TEST2_NOTO_H
