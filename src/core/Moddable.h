//
// Created by Sam Bock on 8/22/16.
//

#ifndef TEST2_MODDABLE_H
#define TEST2_MODDABLE_H


#include <functional>

//enum class ModdableType {
//    mValue,
//    mFunction
//};
//
//template<typename T>
//class Moddable {
//protected:
//    T value;
//    std::function<T()> func;
//    ModdableType type = ModdableType::mValue;
//
//public:
//    virtual T resolve() {
//        switch(type) {
//            case ModdableType::mValue: return value;
//            case ModdableType::mFunction: return func;
//        }
//    }
//
//    Moddable(T value) : value(value), type(ModdableType::mValue) {}
//    Moddable(const std::function<T()> &func) : func(func), type(ModdableType::mFunction) {}
//};
//
//template<typename T>
//static Moddable<T> M(const T& val) {
//
//}


template<typename T>
class ModdableBase {
public:
    virtual T resolve() = 0;
};


template<typename T>
class ValueModdable : public ModdableBase<T> {
    const T value;
public:
    ValueModdable(const T value) : value(value) {}

    virtual T resolve() override {
        return value;
    }
};

template<typename T>
class FunctionModdable : public ModdableBase<T> {
    const std::function<T()> func;
public:
    FunctionModdable(const std::function<T()> &func) : func(func) {}

    virtual T resolve() override {
        return func();
    }
};


template<typename T>
class Moddable {
protected:
    std::shared_ptr<ModdableBase<T>> intern;

public:
    Moddable(const T& t) :
            intern(std::shared_ptr<ModdableBase<T>>(new ValueModdable<T>(t))) {}
//    Moddable(const std::function<T()>& func) :
//            intern(std::shared_ptr<ModdableBase<T>>(new FunctionModdable<T>(func))) {}
    Moddable(std::shared_ptr<ModdableBase<T>> raw) : intern(raw) {}

    template <typename F, typename R = typename std::result_of<T()>>
    Moddable(const F& f) {
        std::function<T()> asFunc = f;
        intern = std::shared_ptr<ModdableBase<T>>(new FunctionModdable<T>(asFunc));
    }

    T resolve() {
        return intern->resolve();
    }
};

//template<typename T>
//struct Mod {
//    typedef std::shared_ptr<Moddable<T>> Ptr;
//};

//template<typename F, typename std::enable_if<std::is_callable<Event,E>::value>::type* = nullptr>

template <typename T>
Moddable<typename std::result_of<T()>::type> M(const T& t) {
    const std::function<typename std::result_of<T()>::type(void)> asFunc = t;
    return Moddable<typename std::result_of<T()>::type>(std::shared_ptr<ModdableBase<typename std::result_of<T()>::type>>(new FunctionModdable<typename std::result_of<T()>::type>(asFunc)));
}

template<typename T,typename std::enable_if<std::is_fundamental<T>::value>::type* = nullptr>
static Moddable<T> M(const T& val) {
    return Moddable<T>(val);
}



#endif //TEST2_MODDABLE_H
