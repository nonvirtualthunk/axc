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

NEW_EVENT_TYPE(CharEvent) {
public:
    unsigned int codePoint;
    const KeyModifiers modifiers;

    CharEvent(unsigned int codePoint, const KeyModifiers &modifiers);
};

NEW_EVENT_TYPE(MouseMoveEvent) {
public:
    const bool isMove;
    const bool isDrag;
    const glm::vec2 mousePosition;
    const glm::vec2 delta;
    const KeyModifiers modifiers;

    MouseMoveEvent(const bool isMove, const bool isDrag,
                       const glm::vec2 &mousePosition, const glm::vec2 &delta, const KeyModifiers &modifiers);
};

NEW_EVENT_TYPE(ScrollEvent) {
public:
    const glm::vec2 delta;
    const KeyModifiers modifiers;

    ScrollEvent(const glm::vec2 &delta, const KeyModifiers &modifiers);
};

NEW_EVENT_TYPE(ResizeEvent) {
public:
    const glm::vec2 windowDimensions;
    const glm::vec2 framebufferDimensions;

    ResizeEvent(const glm::vec2 &windowDimensions, const glm::vec2 &framebufferDimensions);
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
    static bool isKeyDown(int key) {
        return keyStates[key];
    }
    static KeyModifiers getActiveModifiers() {
        return activeModifiers;
    }
};

class MouseMirror {
protected:
    static glm::vec2 mousePosition;
    static bool leftDown;
    static bool rightDown;

public:
    static void setMousePosition(const glm::vec2& pos) {
        mousePosition = pos;
    }
    static const glm::vec2& getCurrentMousePosition() {
        return mousePosition;
    }

    static void setLeftDown(bool down) {
        leftDown = down;
    }
    static void setRightDown(bool down) {
        rightDown = down;
    }
    static bool isLeftDown() {
        return leftDown;
    }
    static bool isRightDown() {
        return rightDown;
    }

};

#endif //TEST2_GUIEVENTS_H
