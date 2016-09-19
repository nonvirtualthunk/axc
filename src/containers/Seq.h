//
// Created by nvt on 8/4/16.
//

#ifndef TEST2_SEQ_H
#define TEST2_SEQ_H

#include <vector>
#include <Combinators.h>
#include <deque>

using namespace std;

namespace Arx {
    template<typename T, typename Container, typename Self>
    class RawSequence {
    public:
        typedef T value_type;
        typedef typename Container::const_iterator const_iterator;
        typedef typename Container::iterator iterator;


    protected:
        Container intern;
    public:
        RawSequence() {}
        template<typename OtherContainer>
        RawSequence(const OtherContainer &from) {
            for (const T& t : from) {
                intern.push_back(t);
            }
        }

        void add(const T& t) {
            intern.push_back(t);
        }
        template<typename Coll>
        void addAll(const Coll& coll) {
            for (const T& t : coll) {
                add(t);
            }
        }

        void push(const T& t) {
            intern.push_back(t);
        }
        void pop() {
            intern.pop_back();
        }

        void pushFront(const T& t) {
            intern.insert(intern.begin(),t);
        }
        void popFront() {
            intern.erase(intern.begin());
        }
        void popBack() {
            intern.pop_back();
        }
        void pushBack() {
            intern.push_back();
        }

        /**
         * Pops the back element from this RawSequence if it is equal to the given expected value,
         * otherwise leaves the RawSequence unchanged.
         * @param expected the value that is expected to be at the back
         * @return true if the expected was found and removed, false otherwise
         */
        bool pop(const T& expected) {
            bool ret = intern.back() == expected;
            if (ret) intern.pop_back();
            return ret;
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
        Self filter(Op op) const {
            return Self(comb::filter(*this, op));
        }

        template<typename Op>
        Self filterNot(Op op) const {
            return Self(comb::filterNot(*this, op));
        }

        template<typename Op>
        void foreach(Op op) const {
            for (const T& t : intern) {
                op(t);
            }
        }

		bool forall(std::function<bool(const T&)> predicate) {
			for (const T& t : intern) {
				if (!predicate(t)) {
					return false;
				}
			}
			return true;
		}


        bool contains(const T& needle) const {
            for (const T& t : intern) {
                if (t == needle) {
                    return true;
                }
            }
            return false;
        }

        unsigned long size() const {
            return intern.size();
        }
        bool isEmpty() const {
            return size() == 0;
        }
        bool nonEmpty() const {
            return ! isEmpty();
        }

        const T& head() const {
            return intern[0];
        }
        const T& last() const {
            return intern[intern.size()-1];
        }
		T& head() {
			return intern[0];
		}
		T& last() {
			return intern[intern.size()-1];
		}
		Optional<T> headOpt() const {
			return size() > 0 ? Optional<T>::some(head()) : Optional<T>::none();
		}

        Self take(int n) const {
            Self ret;
            for (int i = 0; i < n && i < size(); ++i) {
                ret.add((*this)[i]);
            }
            return ret;
        }

        Self takeRight(int n) const {
            Self ret;
            for (int i = std::max(int(size() - n),0); i < size(); ++i) {
                ret.add((*this)[i]);
            }
            return ret;
        }

		void clear() {
			intern.clear();
		}

        const_iterator begin() const { return intern.begin(); }
        const_iterator end() const { return intern.end(); }
        iterator begin() { return intern.begin(); }
        iterator end() { return intern.end(); }

        bool operator==(const RawSequence &rhs) const {
            return intern == rhs.intern;
        }

        bool operator!=(const RawSequence &rhs) const {
            return !(rhs == *this);
        }
    };

    template<typename T>
    class Sequence : public RawSequence<T,deque<T>, Sequence<T>> {
    public:
//        template<typename Op>
//        auto map(Op op) const -> Sequence<decltype(op(*((T*)nullptr)))> {
//            return Sequence(comb::fmap(*this, op));
//        }
        template<typename U>
        Sequence<U> map(std::function<U(T)> func) {
            Sequence<U> ret;
            for (const T& t : *this) {
                ret.add( func(t) );
            }
            return ret;
        }
    };
};
#endif //TEST2_SEQ_H
