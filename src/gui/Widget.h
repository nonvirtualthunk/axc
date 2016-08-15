//
// Created by nvt on 8/14/16.
//

#ifndef TEST2_WIDGET_H
#define TEST2_WIDGET_H


#include <glm/vec2.hpp>
#include <math/Shapes.h>
#include <vector>

class Widget;

struct WidgetPosition {
    Widget * relativeTo = nullptr;
    bool alignOpposite = false;
    bool percentageBased = false;
    union {
        int value = 0;
        float percentage = 0.0f;
    };

    float drawnValue = 0.0f;
};

struct WidgetDimension {
    Widget * relativeTo = nullptr;
    bool percentageBased = false;
    bool encapsulateChildren = false;

    int minValue = 0;
    int padding = 0;
    union {
        int value = 0;
        float percentage = 0.0f;
    };
};

/*
 * Use cases:
 * Match parent dimension
 * Match combined child extents (with possible padding)
 * Match sibling dimensions
 * Occupy n% of parent dimension
 *
 * Align bottom/right/left/top
 * Align below/above/left/right of sibling
 */

struct WidgetDrawContext {
    int revision = 1;
    glm::vec2 translation;
    std::vector<Rect<int>> boundsStack;

    WidgetDrawContext(int revision) : revision(revision) {
        boundsStack.push_back(Rect<int>(0,0,10000,10000));
    }
    void pushBoundsIntersection(const Rect& other) {
        boundsStack.push_back(boundsStack.back().intersect(other));
    }
    void popBounds() {
        boundsStack.pop_back();
    }
    const Rect<int>& bounds() {
        return boundsStack.back();
    }
};

class Widget {
protected:
    WidgetPosition _x;
    WidgetPosition _y;
    WidgetDimension _width;
    WidgetDimension _height;

    int drawnRevision = 0;


public:
    void drawCore(WidgetDrawContext& context);

protected:
    void ensureRelativeWidgetsDrawn(WidgetDrawContext& context);

    virtual void draw(WidgetDrawContext& context);
};


#endif //TEST2_WIDGET_H
