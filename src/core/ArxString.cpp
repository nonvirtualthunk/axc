//
// Created by nvt on 8/13/16.
//

#include "ArxString.h"

#include "core/Predef.h"


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

    String String::trimmed() const {
        String ret(intern);
        ret.intern.trim();
        return ret;
    }

    void String::append(char c) {
        intern += c;
    }

    void String::append(const char *c) {
        intern += c;
    }


    Sequence<String> String::split(char on) const {
        CBStringList l;
        l.split(intern,on);
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


    std::ostream &operator<<(std::ostream &os, const Arx::String &string1) {
        os << string1.raw();
        return os;
    }
}