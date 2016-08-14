//
// Created by nvt on 8/5/16.
//

#ifndef TEST2_AUXDATA_H
#define TEST2_AUXDATA_H

#include <core/Predef.h>

template<typename Base,typename Assoc>
class AuxDataType {
public:
    typedef Assoc associated_type;
    typedef Base base_type;
    const int id;

    AuxDataType() : id(getNextTaggedId<Base,Assoc>()) {}
};

template<typename Assoc>
class WorldAuxData : AuxDataType<WorldAuxData<void>,Assoc>{};

template<typename Assoc>
class EntityAuxData : AuxDataType<EntityAuxData<void>,Assoc>{};

template<typename Assoc>
class GraphicsAuxData : AuxDataType<GraphicsAuxData<void>,Assoc>{};

template<typename ADT>
class AuxDataContainer {
protected:
    uint8_t auxDataCount = 0;
    void **auxData;

public:
    template<typename T>
    typename T::associated_type &operator()(T adt) {
        static_assert(std::is_same<ADT,typename T::base_type>::value,
                      "AuxDataContainer given request for incorrect aux data type");
        int index = adt.id;
        if (auxDataCount <= index) {
            int oldCount = auxDataCount;
            auxDataCount = (uint8_t) (index + 1);
            void **newArr = new void *[auxDataCount];
            memcpy(newArr, auxData, sizeof(void *) * oldCount);
            memset(newArr + oldCount, 0, (size_t) (auxDataCount - oldCount) * sizeof(void*));
            auxData = newArr;
        }
        void *&cur = auxData[index];
        if (cur == nullptr) {
            cur = new typename T::associated_type();
        }
        return *((typename T::associated_type *) cur);
    }
};

template<typename ADT>
class HasAuxData {
protected:
    AuxDataContainer<ADT> dataContainer;

public:
    template<typename T>
    typename T::associated_type &operator()(T adt) {
        return dataContainer(adt);
    }
};


#define DECL_AUX_DATA(BROAD_TYPE, TNAME) \
class TNAME ## _t; \
const static BROAD_TYPE<TNAME ## _t>& TNAME = BROAD_TYPE<TNAME ## _t>(); \
class TNAME ## _t

#endif //TEST2_AUXDATA_H
