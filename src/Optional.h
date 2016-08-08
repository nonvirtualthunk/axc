//
// Created by nvt on 8/3/16.
//

#ifndef TEST2_OPTIONAL_H
#define TEST2_OPTIONAL_H

#include <cstdint>
#include <cstring>
#include <functional>

template<typename T>
class Optional {
public:
    static Optional<T> some(const T &t) {
        return Optional(false, t);
    }

    static Optional<T> none() {
        return Optional();
    }

    bool isPresent() const { return !_isEmpty; }

    bool isEmpty() const { return _isEmpty; }

    const T& get() const { return *((T*)intern); }

    template<typename OP>
    const Optional<T>& ifPresent(OP op) const {
        if (isPresent()) {
            op(get());
        }
        return *this;
    }

    template<typename OP>
    const Optional<T>& ifEmpty(OP op) const {
        if (!isPresent()) {
            op();
        }
        return *this;
    }

    template<typename OP>
    const Optional<T>& orIfEmpty(OP op) const { return ifEmpty(op); }
    template<typename OP>
    const Optional<T>& orIfPresent(OP op) const { return ifPresent(op); }


    template<typename PRESENT_OP, typename ABSENT_OP>
    auto match(PRESENT_OP pop, ABSENT_OP aop) const -> decltype(ABSENT_OP()) {
        if (isPresent()) {
            return pop(get());
        } else {
            return aop();
        }
    }

    const T& orElse(const T& other) const {
        if (isPresent()) {
            return get();
        } else {
            return other;
        }
    }

    T orElse(const std::function<T()>& other) const {
        if (isPresent()) {
            return get();
        } else {
            return other();
        }
    }

    template<typename U>
    auto map (U (*func)(T)) -> Optional<U> {
        if (isPresent()) {
            return Optional<U>::some(func(get()));
        } else {
            return Optional<U>::none();
        }
    }
private:
    const bool _isEmpty;
    uint8_t intern[sizeof(T)];

    Optional(const bool isEmpty, const T value) : _isEmpty(isEmpty) {
        memcpy(intern,&value,sizeof(T));
    }
    Optional() : _isEmpty(true) {}

public:
    bool operator==(const Optional &rhs) const {
        if (_isEmpty && rhs._isEmpty) {
            return true;
        } else {
            return _isEmpty == rhs._isEmpty &&
                   get() == rhs.get();
        }
    }

    bool operator!=(const Optional &rhs) const {
        return !(rhs == *this);
    }
};



#endif //TEST2_OPTIONAL_H
