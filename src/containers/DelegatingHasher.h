//
// Created by Sam Bock on 8/12/16.
//

#ifndef TEST2_DELEGATINGHASHER_H
#define TEST2_DELEGATINGHASHER_H


#include <cstddef>
#include <functional>

namespace Arx {
    template<typename K>
    class DelegatingHasher {
    public:
        template<typename U = K>
        size_t operator()(const typename std::enable_if<!std::is_class<U>::value, U>::type& k) const {
            return std::hash<K>()(k);
        }

        template<typename U = K>
        auto operator()(const typename std::enable_if<std::is_class<U>::value, U>::type& k) const -> decltype(k.hash()){
            return k.hash();
        }


        template<typename U = K>
        auto operator()(const typename std::enable_if<std::is_class<U>::value, U>::type& k) const -> decltype(std::hash<U>()(k)) {
            return std::hash<K>()(k);
        }
    };
}

#endif //TEST2_DELEGATINGHASHER_H
