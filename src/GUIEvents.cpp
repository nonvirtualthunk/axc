//
// Created by nvt on 8/5/16.
//

#include "GUIEvents.h"

#include <glfw/glfw3.h>


bool* const KeyboardMirror::keyStates = new bool[GLFW_KEY_LAST];
KeyModifiers KeyboardMirror::activeModifiers(0);

bool KeyModifiers::ctrl() {
    return isBitSet(modifierSet, GLFW_KEY_LEFT_CONTROL) ||
           isBitSet(modifierSet, GLFW_KEY_RIGHT_CONTROL) ||
           isBitSet(modifierSet, GLFW_KEY_LEFT_SUPER) ||
           isBitSet(modifierSet, GLFW_KEY_RIGHT_SUPER);
}

bool KeyModifiers::shift() {
    return isBitSet(modifierSet, GLFW_KEY_LEFT_SHIFT) ||
           isBitSet(modifierSet, GLFW_KEY_RIGHT_SHIFT);
}

bool KeyModifiers::alt() {
    return isBitSet(modifierSet, GLFW_KEY_LEFT_ALT) ||
           isBitSet(modifierSet, GLFW_KEY_RIGHT_ALT);
}

MouseMoveEvent::MouseMoveEvent(const bool isMove, const bool isScroll,
                                       const bool isDrag, const glm::vec2 &mousePosition, const glm::vec2 &delta,
                                       const KeyModifiers &modifiers) : Event(MouseMoveEventType), isMove(isMove),
                                                                        isScroll(isScroll), isDrag(isDrag),
                                                                        mousePosition(mousePosition), delta(delta),
                                                                        modifiers(modifiers) {}

KeyEvent::KeyEvent(const int key, const bool isPress, const KeyModifiers &modifiers)
        : Event(KeyEventType), key(key), isPress(isPress), modifiers(modifiers) {}

MouseButtonEvent::MouseButtonEvent(const glm::vec2 &mousePosition, const bool isPress,
                                   const KeyModifiers &modifiers) : Event(MouseButtonEventType), mousePosition(mousePosition),
                                                                    isPress(isPress), modifiers(modifiers) {}
