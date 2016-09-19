//
// Created by nvt on 8/13/16.
//

#include "ArxString.h"

#include "core/Predef.h"

#include <Optional.h>

using namespace Bstrlib;
namespace Arx {


    String::String(const std::string &intern) :
            intern(intern.c_str()) {
    }

    bool String::startsWith(const std::string &str) const {
        return intern.find(str.c_str()) == 0;
    }

    bool String::contains(const std::string &str) const {
        return intern.find(str.c_str()) >= 0;
    }

    bool String::contains(char c) const {
        return intern.find(c) >= 0;
    }

    bool String::endsWith(const std::string& str) const {
        return intern.find(str.c_str()) == this->size() - str.length();
    }

    String::String(const char *raw) :intern(raw){

    }

    String::String(const Bstrlib::CBString intern) :intern(intern){

    }
    String::String() :intern(){

    }

    String String::takeWhile(std::function<bool(char)> predicate) const {
        String ret;
        for (int i = 0; i < intern.length(); ++i) {
            if (predicate(intern[i])) {
                ret.append(intern[i]);
            } else {
                break;
            }
        }
        return ret;
    }

    String String::takeRightWhile(std::function<bool(char)> predicate) const {
        String ret;
        for (int i = intern.length() - 1; i >= 0; --i) {
            if (predicate(intern[i])) {
                ret.append(intern[i]);
            } else {
                break;
            }
        }
        return ret.reversed();
    }

    String String::reversed() const {
        String ret;
        for (int i = size()-1; i >= 0; --i) {
            ret.append((*this)[i]);
        }
        return ret;
    }

    String String::trimmed() const {
        String ret(intern);
        ret.intern.trim();
        return ret;
    }
    void String::trim() {
        intern.trim();
    }

    void String::append(char c) {
        intern += c;
    }

    void String::append(const char *c) {
        intern += c;
    }

    void String::append(const Arx::String& str) {
        intern += str.raw();
    }

    void String::insert(const std::string &str, int atIndex) {
        intern.insert(atIndex, str.c_str());
    }


    Sequence<String> String::split(char on) const {
        CBStringList l;
        l.split(intern, (unsigned char) on);
        Sequence<String> ret;
        for (int i = 0; i < l.size(); ++i) {
            ret.add(l.at(i));
        }
        return ret;
    }

    size_t String::hash() const {
        unsigned long hash = 5381;

        for (int i = 0; i < intern.length(); ++i) {
            hash = ((hash << 5) + hash) + intern[i]; /* hash * 33 + c */
        }

        return hash;
    }

    bool String::operator==(const String &rhs) const {
        return intern == rhs.intern;
    }

    bool String::operator==(const char *&rhs) const {
        return intern == rhs;
    }

    bool String::operator!=(const String &rhs) const {
        return !(rhs == *this);
    }

    int String::size() const {
        return intern.length();
    }

    const Bstrlib::CBString &String::getIntern() const {
        return intern;
    }

    const char *String::raw() const {
        return intern;
    }

    String String::operator+(const char *other) const {
        String ret = *this;
        ret.append(other);
        return ret;
    }


    String String::operator+(const String &other) const {
        String ret = *this;
        ret.append(other.raw());
        return ret;
    }

    void String::clear() {
        intern.trunc(0);
    }

    String String::dropWhile(std::function<bool(char)> predicate) const {
        String ret;
        int i = 0;
        for (;i < size() && predicate((*this)[i]); ++i) {}
        for (;i < size(); ++i) {
            ret.append((*this)[i]);
        }
        return ret;
    }

    String String::dropRightWhile(std::function<bool(char)> predicate) const {
        String ret;
        int end = size()-1;
        for (;end > 0 && predicate((*this)[end]); --end) {}
        for (int i = 0; i <= end; ++i) {
            ret.append((*this)[i]);
        }
        return ret;
    }

    String String::drop(int n) const {
        String ret;
        for (int i = n; i < size(); ++i) {
            ret.append((*this)[i]);
        }
        return ret;
    }

    String String::dropRight(int n) const {
        String ret;
        for (int i = 0; i < size()-n; ++i) {
            ret.append((*this)[i]);
        }
        return ret;
    }

    bool String::isEmpty() const {
        return size() == 0;
    }

    bool String::nonEmpty() const {
        return ! isEmpty();
    }

    const String &String::operator=(const char *s) {
        intern = s;
        return *this;
    }

    Optional<int> String::intValue() const {
        try {
            return some(std::stoi(*this));
        } catch (const std::exception& e) {
            return none<int>();
        }
    }

    Optional<float> String::floatValue() const {
        try {
            return some(std::stof(*this));
        } catch (const std::exception& e) {
            return none<float>();
        }
    }

    Optional<bool> String::boolValue() const {
        if (intern.caselessEqual("true")) {
            return some(true);
        } else if (intern.caselessEqual("false")) {
            return some(false);
        } else {
            return none<bool>();
        }
    }

    Optional<double> String::doubleValue() const {
        try {
            return some(std::stod(*this));
        } catch (const std::exception& e) {
            return none<double>();
        }
    }

    Optional<long> String::longValue() const {
        try {
            return some(std::stol(*this));
        } catch (const std::exception& e) {
            return none<long>();
        }
    }

    String String::toLower() const {
        String ret = *this;
        ret.intern.tolower();
        return ret;
    }

    String String::toUpper() const {
        String ret = *this;
        ret.intern.toupper();
        return ret;
    }

    int String::find(const std::string &str) const {
        return intern.find(str.c_str());
    }

    int String::reverseFind(const std::string &str) const {
        return intern.reversefind(str.c_str(), (int) intern.length());
    }


    int String::reverseFind(char c) const {
        return intern.reversefind(c, (int)intern.length());
    }

    int String::find(char c) const {
        return intern.find(c);
    }

    String String::takeAfter(int start) {
        return takeBetween(start,size());
    }

    String String::takeBetween(int start, int end) {
        String ret;
        for (int i = start; i < end; ++i) {
            ret.append((*this)[i]);
        }
        return ret;
    }

    String String::takeBefore(int end) {
        return takeBetween(0,end);
    }



    std::ostream &operator<<(std::ostream &os, const Arx::String &string1) {
        os << string1.raw();
        return os;
    }
}