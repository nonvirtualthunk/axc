//
// Created by nvt on 8/3/16.
//

#include <core/Predef.h>
#include "ArxEnum.h"
#include <Noto.h>

ArxEnum::ArxEnum(const std::string &name, int typeId) :
        name(name),
        typeId(typeId),
        ordinal(ArxEnumData::nextOrdinalFor(typeId))
{
    Noto::info("Instantiating enum type({}), name({}), ordinal({})",typeId,name,ordinal);
}

bool ArxEnum::operator==(const ArxEnum &rhs) const {
    return typeId == rhs.typeId &&
           ordinal == rhs.ordinal;
}

bool ArxEnum::operator!=(const ArxEnum &rhs) const {
    return !(rhs == *this);
}

bool ArxEnum::operator<(const ArxEnum &rhs) const {
    if (typeId < rhs.typeId)
        return true;
    if (rhs.typeId < typeId)
        return false;
    return ordinal < rhs.ordinal;
}

Arx::Map<int,ArxEnumData*> ArxEnumData::enumData = Map<int,ArxEnumData*>();
std::mutex ArxEnumData::mutex;