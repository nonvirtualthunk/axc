//
// Created by nvt on 8/3/16.
//

#ifndef TEST2_PREDEF_H
#define TEST2_PREDEF_H


#include <Optional.h>
#include <Combinators.h>
#include <containers/Seq.h>
#include <containers/ArxMap.h>
#include <containers/ArxSet.h>

template<typename T>
Optional<T> some(const T &t) {
    return Optional<T>::some(t);
}

template<typename T>
Optional<T> none() {
    return Optional<T>::none();
}

template <typename T>
Arx::Sequence<T> Seq( std::initializer_list<T> list )
{
    Arx::Sequence<T> nseq;
    for( auto elem : list ) {
        nseq.add(elem);
    }
    return nseq;
}


template <typename T>
Arx::Set<T> Set( std::initializer_list<T> list )
{
    Arx::Set<T> set;
    for( auto elem : list ) {
        set.add(elem);
    }
    return set;
}

template <typename T>
std::set<T> StdSet( std::initializer_list<T> list )
{
    std::set<T> set;
    for( auto elem : list ) {
        set.insert(elem);
    }
    return set;
}

template <typename K, typename V>
Arx::Map<K,V> Map() {
    return Arx::Map<K,V>();
};

template<typename Coll>
std::set<typename Coll::value_type> toStdSet(const Coll& coll) {
    return std::set<typename Coll::value_type>(coll.begin(), coll.end());
}


template <typename Coll>
Arx::Set<typename Coll::value_type> toSet(const Coll& coll) {
    Arx::Set<typename Coll::value_type> ret;
    auto iter = coll.begin();
    auto end = coll.end();
    while (iter != end) {
        ret.add(*iter);
        ++iter;
    }
    return ret;
}

class IdCounter {
public:
    static atomic_int& getIdCounter();
};

template<typename T>
int getTypeId() {
    static int id = IdCounter::getIdCounter().fetch_add(1);
    return id;
}



#include <iostream>
template<typename T>
static atomic_int& getTaggedIdCounter() {
    static atomic_int idCounter(0);
    return idCounter;
}

template<typename Tag, typename T>
int getNextTaggedId() {
    static int id = getTaggedIdCounter<Tag>().fetch_add(1);
    return id;
}


inline bool isBitSet(int combinedValue, int bit) {
    return (combinedValue & bit) == bit;
}

inline int
pow2roundup (int x)
{
    if (x < 0)
        return 0;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
}

long long int epochMillisSteady();
long long int epochMillisSystem();

#define DECLARE_METHOD_HASH(forType) \
namespace std {                                                                \
template<>                                                                     \
struct hash<forType> {                                                        \
  std::size_t operator()(const forType& object) const {                       \
    return object.hash();                                                      \
  }                                                                            \
};                                                                             \
}

#define fnv(body) []() { return body; }
#define fn(var,body) [](var) { return body; }
#define fni(body) [](int i) { return body; }
#define fnf(body) [](float f) { return body; }

#define snew(typeName) std::make_shared<typeName>




#define COUNTER_READ_CRUMB( TAG, RANK, ACC ) counter_crumb( TAG(), constant_index< RANK >(), constant_index< ACC >() )
#define COUNTER_READ( TAG ) COUNTER_READ_CRUMB( TAG, 1, COUNTER_READ_CRUMB( TAG, 2, COUNTER_READ_CRUMB( TAG, 4, COUNTER_READ_CRUMB( TAG, 8, \
    COUNTER_READ_CRUMB( TAG, 16, COUNTER_READ_CRUMB( TAG, 32, COUNTER_READ_CRUMB( TAG, 64, COUNTER_READ_CRUMB( TAG, 128, 0 ) ) ) ) ) ) ) )

#define COUNTER_INC( TAG ) \
constant_index< COUNTER_READ( TAG ) + 1 > \
constexpr counter_crumb( TAG, constant_index< ( COUNTER_READ( TAG ) + 1 ) & ~ COUNTER_READ( TAG ) >, \
          					constant_index< ( COUNTER_READ( TAG ) + 1 ) & COUNTER_READ( TAG ) > ) { return {}; }

#define COUNTER_LINK_NAMESPACE( NS ) using NS::counter_crumb;

#define REGISTERED_CLASS( TAG, KEY, NAME ) KEY NAME &register_type( TAG, decltype( COUNTER_READ( TAG ) ) ); \
	COUNTER_INC( TAG ) KEY NAME /* class definition follows */

#include <utility>

template< std::size_t n >
struct constant_index : std::integral_constant< std::size_t, n > {};

template< typename id, std::size_t rank, std::size_t acc >
constexpr constant_index< acc > counter_crumb( id, constant_index< rank >, constant_index< acc > ) { return {}; } // found by ADL via constant_index


#endif //TEST2_PREDEF_H
