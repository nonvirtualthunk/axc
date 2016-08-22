//
// Created by nvt on 8/13/16.
//

#ifndef TEST2_ARXSTRING_H
#define TEST2_ARXSTRING_H


#include <string>
#include <bstrlib/bstrwrap.h>
#include <functional>
#include <ostream>
#include "containers/Seq.h"

namespace Arx {

    class String {
        Bstrlib::CBString intern;

    public:
        String(const std::string &intern);
        String(const char *raw);
        String(const Bstrlib::CBString intern);
        String();
        operator std::string() const { return std::string(intern); }
        operator const char *() const { return intern; }

        char operator[](int i) const {
            return intern.character(i);
        }

        bool startsWith(const std::string& str) const;
        bool endsWith(const std::string& str) const;
        bool contains(const std::string& str) const;
        String takeWhile(std::function<bool(char)> predicate) const;
        String trimmed() const;
        void append(char c);
        void append(const char *c);
        Sequence<String> split(char on) const;


        const char * raw() const;
        const Bstrlib::CBString& getIntern() const;
        int size() const;
        size_t hash() const;
        bool operator==(const String &rhs) const;
        bool operator!=(const String &rhs) const;
        String operator+(const char * other) const;
    };

    std::ostream &operator<<(std::ostream &os, const Arx::String &string1);
}

#endif //TEST2_ARXSTRING_H
