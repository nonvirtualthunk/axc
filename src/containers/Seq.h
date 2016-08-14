//
// Created by nvt on 8/4/16.
//

#ifndef TEST2_SEQ_H
#define TEST2_SEQ_H

#include <vector>
#include <Combinators.h>

using namespace std;

namespace Arx {
    template<typename T>
    class Sequence {
    public:
        typedef T value_type;
        typedef typename vector<T>::const_iterator const_iterator;
        typedef typename vector<T>::iterator iterator;


    protected:
        vector<T> intern;
    public:
        Sequence() {}
        Sequence(const vector<T> &intern) : intern(intern) {}

        void add(const T& t) {
            intern.push_back(t);
        }
        void insert(const_iterator iter, const T& t) {
            intern.insert(iter,t);
        }

        T& operator[](int i) {
            return intern[i];
        }
        const T& operator[](int i) const {
            return intern[i];
        }
        T& operator()(int i) {
            return intern[i];
        }
        const T& operator()(int i) const {
            return intern[i];
        }

        template<typename Op>
        Sequence<T> filter(Op op) const {
            return Sequence(comb::filter(*this, op));
        }

        template<typename Op>
        Sequence<T> filterNot(Op op) const {
            return Sequence(comb::filterNot(*this, op));
        }

        template<typename Op>
        Sequence<T> map(Op op) const {
            return Sequence(comb::fmap(*this, op));
        }

        unsigned long size() const {
            return intern.size();
        }

        const_iterator begin() const { return intern.begin(); }
        const_iterator end() const { return intern.end(); }
        iterator begin() { return intern.begin(); }
        iterator end() { return intern.end(); }

        bool operator==(const Sequence &rhs) const {
            return intern == rhs.intern;
        }

        bool operator!=(const Sequence &rhs) const {
            return !(rhs == *this);
        }
    };
}
#endif //TEST2_SEQ_H
