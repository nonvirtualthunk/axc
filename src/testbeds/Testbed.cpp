//
// Created by nvt on 8/13/16.
//


#include <string>
#include <containers/ArxMap.h>
#include "Testbed.h"

Arx::Map<std::string,Testbed*>& Testbed::getTestbedsByName() {
    static Arx::Map<std::string,Testbed*> testbeds;
    return testbeds;
};

void registerTestbed(const std::string& name, Testbed* testbed) {
    Testbed::getTestbedsByName().put(name,testbed);
}


Testbed::Testbed(const std::string &name) : name(name) {
    registerTestbed(name,this);
}