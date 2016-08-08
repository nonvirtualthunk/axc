//
// Created by nvt on 8/4/16.
//


#include <catch.hpp>
#include <core/Predef.h>
#include <ArxEnum.h>

class KeyEnum : public ArxEnum {
public:
    KeyEnum(const string &name) : ArxEnum(name, getTypeId<KeyEnum>()) {}
};

KeyEnum key1("key1");
KeyEnum key2("key2");
KeyEnum key3("key3");

TEST_CASE("Map insertion test","[containers][core]") {
    auto map = Map<std::string,int>();

    map.put("hello",3);
    map.put("foo", -1);

    REQUIRE(map.get("hello") == some(3));

    map.put("hello",2);

    REQUIRE(map.get("hello") == some(2));

    REQUIRE(map.getOrElse("blah",3) == 3);
    REQUIRE(map.getOrElse("booo",fnv(4)) == 4);
}

TEST_CASE("Map key type test","[containers][core]") {
    auto strMap = Map<std::string,std::string>();

    strMap.put("a","b");
    strMap.put("b","c");

    REQUIRE(strMap.get("a") == some((std::string)"b"));
    REQUIRE(strMap.get("c") == none<std::string>());

    auto intMap = Map<int,std::string>();

    intMap.put(1,"a");
    intMap.put(7,"b");

    REQUIRE(intMap.get(7) == some((std::string)"b"));
    REQUIRE(intMap.get(2) == none<std::string>());

    auto custMap = Map<KeyEnum,std::string>();

    custMap.put(key1,"a");
    custMap.put(key2,"b");

    REQUIRE(custMap.get(key1) == some((std::string)"a"));
    REQUIRE(custMap.get(key3) == none<std::string>());

    custMap.put(key1,"c");

    REQUIRE(custMap.get(key1) == some((std::string)"c"));
}