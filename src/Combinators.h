//
// Created by nvt on 8/3/16.
//

#ifndef TEST2_COMBINATORS_H_H
#define TEST2_COMBINATORS_H_H

#include <algorithm>
#include <set>
#include <vector>

namespace comb {
    template <typename Collection,typename unop>
    Collection fmap(Collection col,unop op) {
      std::transform(col.begin(),col.end(),col.begin(),op);
      return col;
    }

    template<typename Coll, typename Op>
    void foreach(Coll coll, Op op) {
        auto last = coll.end();
        for (auto iter = coll.begin();iter != last;++iter) {
            op(*iter);
        }
    };

    template<typename Coll, typename Pred>
    Coll filter (const Coll& coll, Pred pred) {
        Coll newColl;
        for (const typename Coll::value_type& v : coll) {
            if (pred(v)) {
                newColl.insert(newColl.end(), v);
            }
        }
        return newColl;
    };

    template<typename Coll, typename Pred>
    Coll filterNot (const Coll& coll, Pred pred) {
        return filter(coll,[pred](const typename Coll::value_type& v){return !pred(v);});
    }

    template<class T>
    bool contains(const std::set<T>& set, const T& key) {
        return set.find(key) != set.end();
    }
}

#endif //TEST2_COMBINATORS_H_H
