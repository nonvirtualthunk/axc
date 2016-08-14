//
// Created by nvt on 8/13/16.
//

#include <core/Predef.h>
#include "VBO.h"
#include "StandardAttributeProfiles.h"

const Arx::Sequence<Attribute> SimpleAttributeProfile::attributes =
        Seq({ Attribute(3,GL_FLOAT), Attribute(2,GL_FLOAT), Attribute(4,GL_UNSIGNED_BYTE) });