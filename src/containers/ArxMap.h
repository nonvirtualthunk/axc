//
// Created by nvt on 8/4/16.
//

#ifndef TEST2_ARXMAP_H
#define TEST2_ARXMAP_H

#include <unordered_map>
#include <Optional.h>
#include <type_traits>
#include "containers/DelegatingHasher.h"

namespace Arx {


    template<typename K, typename V>
    class Map {
    protected:
        typedef std::unordered_map<K, V, DelegatingHasher<K>> MapType;
        MapType map;

    public:
        typedef typename MapType::iterator IteratorType;
        typedef typename MapType::const_iterator ConstIteratorType;

        const Optional<V> get(const K &k) const {
            const auto res = map.find(k);
            if (res == map.end()) {
                return Optional<V>::none();
            } else {
                return Optional<V>::some(res->second);
            }
        }

        V &getOrElseUpdate(const K &k, std::function<V()> vfunc) {
            typename MapType::iterator res = map.find(k);
            if (res == map.end()) {
                put(k, vfunc());
                return map[k];
            } else {
                return res->second;
            }
        }

        V getOrElse(const K& k, std::function<V()> vfunc) const {
            return get(k).orElse(vfunc);
        }

        V getOrElse(const K& k, const V& v) const {
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

        void foreach(std::function<void(K,V)> func) {
            auto iter = map.begin();
            auto end = map.end();
            while (iter != end) {
                func(iter->first, iter->second);
                iter++;
            }
        }

        size_t size() const {
            return map.size();
        }

        ConstIteratorType begin() const {
            return map.begin();
        }
        ConstIteratorType end() const {
            return map.end();
        }
    };
}
#endif //TEST2_ARXMAP_H
