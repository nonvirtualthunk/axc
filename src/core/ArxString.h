//
// Created by nvt on 8/13/16.
//

#ifndef TEST2_ARXSTRING_H
#define TEST2_ARXSTRING_H


#include <string>
#include <bstrlib/bstrwrap.h>
#include <functional>
#include <ostream>
#include <Optional.h>
#include <containers/Seq.h>

namespace Arx {

    class String {
        Bstrlib::CBString intern;

    public:
        String(const std::string &intern);
        String(const char *raw);
        String(const Bstrlib::CBString intern);
        String();

        const String& operator = (const char *s);


        operator std::string() const { return std::string(intern); }
        explicit operator const char *() const { return intern; }

        char operator[](int i) const {
            return intern.character(i);
        }

        bool startsWith(const std::string& str) const;
        bool endsWith(const std::string& str) const;
        bool contains(const std::string& str) const;
        bool contains(char c) const;
        String takeWhile(std::function<bool(char)> predicate) const;
        String takeRightWhile(std::function<bool(char)> predicate) const;
        String dropWhile(std::function<bool(char)> predicate) const;
        String dropRightWhile(std::function<bool(char)> predicate) const;
        String drop(int n) const;
        String dropRight(int n) const;
        String trimmed() const;
        String toLower() const;
        String toUpper() const;
        String reversed() const;
        String takeAfter(int start);
        String takeBetween(int start, int end);
        String takeBefore(int end);
        void trim();
        void append(char c);
        void append(const char *c);
        void append(const String& str);
        void insert(const std::string& str, int atIndex);
        void clear();
        Sequence<String> split(char on) const;

        int reverseFind(const std::string& str) const;
        int reverseFind(char c) const;
        int find(const std::string& str) const;
        int find(char c) const;

        Optional<int> intValue() const;
        Optional<float> floatValue() const;
        Optional<bool> boolValue() const;
        Optional<double> doubleValue() const;
        Optional<long> longValue() const;

        bool isEmpty() const;
        bool nonEmpty() const;

        const char * raw() const;
        const Bstrlib::CBString& getIntern() const;
        int size() const;
        size_t hash() const;
        bool operator==(const String &rhs) const;
        bool operator==(const char * &rhs) const;
        bool operator!=(const String &rhs) const;
        String operator+(const char * other) const;
        String operator+(const String& other) const;

        String toString() const { return *this; }
    };

    std::ostream &operator<<(std::ostream &os, const Arx::String &string1);
}

#endif //TEST2_ARXSTRING_H
