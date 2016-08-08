//
// Created by nvt on 8/5/16.
//

#ifndef TEST2_GUIEVENTS_H
#define TEST2_GUIEVENTS_H

#include "Event.h"
#include <glm/vec2.hpp>

class KeyModifiers {
public:
    int modifierSet;

    bool ctrl();
    bool shift();
    bool alt();

    KeyModifiers(int modifierSet) : modifierSet(modifierSet) {}
};

NEW_EVENT_TYPE(MouseButtonEvent) {
public:
    const glm::vec2 mousePosition;
    const bool isPress;
    const KeyModifiers modifiers;

    MouseButtonEvent(const glm::vec2 &mousePosition, const bool isPress,
                     const KeyModifiers &modifiers);
};

NEW_EVENT_TYPE(KeyEvent) {
public:
    const int key;
    const bool isPress;
    const KeyModifiers modifiers;

    KeyEvent(const int key, const bool isPress, const KeyModifiers &modifiers);
};

NEW_EVENT_TYPE(MouseMoveEvent) {
public:
    const bool isMove;
    const bool isScroll;
    const bool isDrag;
    const glm::vec2 mousePosition;
    const glm::vec2 delta;
    const KeyModifiers modifiers;

    MouseMoveEvent(const bool isMove, const bool isScroll, const bool isDrag,
                       const glm::vec2 &mousePosition, const glm::vec2 &delta, const KeyModifiers &modifiers);
};


class KeyboardMirror {
protected:
    static bool* const keyStates;
    static KeyModifiers activeModifiers;
public:

    static void setKeyDown(int key, bool isDown) {
        keyStates[key] = isDown;
    }
    static void setActiveModifiers(int mods) {
        activeModifiers.modifierSet = mods;
    }
};

#endif //TEST2_GUIEVENTS_H
