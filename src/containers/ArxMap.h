//
// Created by nvt on 8/4/16.
//

#ifndef TEST2_ARXMAP_H
#define TEST2_ARXMAP_H

#include <unordered_map>
#include <Optional.h>
#include <type_traits>

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


    template<typename K, typename V>
    class Map {
    protected:
        std::unordered_map<K, V, DelegatingHasher<K>> map;

    public:

        const Optional<V> get(const K &k) const {
            const auto res = map.find(k);
            if (res == map.end()) {
                return Optional<V>::none();
            } else {
                return Optional<V>::some(res->second);
            }
        }

        V &getOrElseUpdate(const K &k, std::function<V()> vfunc) {
            typename std::unordered_map<K, V>::iterator res = map.find(k);
            if (res == map.end()) {
                put(k, vfunc());
                return map[k];
            } else {
                return res->second;
            }
        }

        V getOrElse(const K& k, std::function<V()> vfunc) {
            return get(k).orElse(vfunc);
        }

        V getOrElse(const K& k, const V& v) {
            return get(k).orElse(v);
        }

        bool contains(const K &k) const {
            return map.find(k) == map.end();
        }

        void put(const K &k, const V &v) {
            auto res = map.emplace(k,v);
            if (!res.second) {
                res.first->second = v;
            }
        }
    };
}
#endif //TEST2_ARXMAP_H
