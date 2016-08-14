//
// Created by Sam Bock on 8/12/16.
//

#ifndef TEST2_ARXSET_H
#define TEST2_ARXSET_H

#include <unordered_set>
#include "DelegatingHasher.h"

namespace Arx {

    template<typename T>
    class Set {
    protected:
        typedef std::unordered_set<T,DelegatingHasher<T>> SetType;
        SetType intern;

    public:
        typedef typename SetType::iterator IteratorType;
        typedef typename SetType::const_iterator ConstIteratorType;

        bool contains(const T& t) const {
            return intern.find(t) != intern.end();
        }

        void add(const T& t) {
            intern.insert(t);
        }

        void remove(const T& t) {
            intern.erase(t);
        }


        size_t size() const {
            return intern.size();
        }

        ConstIteratorType begin() const {
            return intern.begin();
        }
        ConstIteratorType end() const {
            return intern.end();
        }

        Set<T> filter(std::function<bool(const T&)> func) {
            Set<T> ret;
            for (const T& t : *this) {
                if (func(t)) {
                    ret.add(t);
                }
            }
            return ret;
        }
    };
}

#endif //TEST2_ARXSET_H
