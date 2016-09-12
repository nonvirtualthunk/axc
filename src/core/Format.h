//
// Created by Sam Bock on 9/6/16.
//

#ifndef TEST2_FORMAT_H
#define TEST2_FORMAT_H

#include <sstream>
#include "ArxString.h"

class Format {
public:
    template<typename T>
    static auto subFormat(const T& t) -> decltype(t.toString()){
        return t.toString();
    }

    template<typename T>
    static Arx::String subFormat(const Arx::Sequence<T>& seq) {
        Arx::String ret("Seq(");
        seq.foreach([&](const T& t) { ret.append(subFormat<T>(t)); });
        ret.append(")");
        return ret;
    }

    static Arx::String subFormat(const std::string& t) {
        return t;
    }

    template<typename T, typename std::enable_if<std::is_fundamental<T>::value>::type* = nullptr>
    static Arx::String subFormat(const T& t) {
        std::ostringstream stream;
        stream << t;
        std::string str = stream.str();
        return str;
    }

    static void internalFormat(const char *fmt, Arx::String& formatted) {
        formatted.append(fmt);
    }

    template<typename First, typename...Args>
    static void internalFormat(const char *fmt, Arx::String& formatted, const First& first, const Args& ... args) {
        for (const char * ptr = fmt; ; ptr++) {
            char c = *ptr;
            if (c == '{') {
                char n = *(ptr+1);
                if (n == '}') {
                    formatted.append(subFormat(first));
                    internalFormat(ptr+2,formatted,args...);
                    return;
                } else {
                    formatted.append(c);
                }
            } else if (c == '\0') {
                break;
            } else {
                formatted.append(c);
            }
        }
    };

    template<typename...Args>
    static Arx::String format(const char *fmt, const Args& ... args) {
        Arx::String formatted;
        internalFormat(fmt,formatted,args...);
        return formatted;
    };
};

#define fmt(fmtStr,...) Format::format(fmtStr,__VA_ARGS__)


#endif //TEST2_FORMAT_H
