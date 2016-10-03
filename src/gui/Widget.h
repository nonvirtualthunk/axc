//
// Created by nvt on 8/14/16.
//

#ifndef TEST2_WIDGET_H
#define TEST2_WIDGET_H


#include <glm/vec2.hpp>
#include <math/Shapes2d.h>
#include <vector>
#include <containers/Seq.h>
#include <graphics/Image.h>
#include <graphics/TextureBlock.h>
#include <graphics/VBO.h>
#include <glm/vec3.hpp>
#include <graphics/Color.h>

class Widget;

enum PositionType {
    pValue,
    pPercentage,
    pFunction
};

enum DimensionType {
    dValue,
    dPercentage,
    dEncapsulate,
    dFunction
};

struct WidgetPosition {
    Widget * relativeTo = nullptr;
    Axis axis;
    bool alignOpposite = false;
    PositionType positionType = pValue;

    union {
        int value = 0;
        float percentage;
    };
    std::function<int()> function = [](){ return 0; };

    int drawnValue = 0;

    WidgetPosition(Axis axis) : axis(axis) {}

};

struct WidgetDimension {
    Widget * relativeTo = nullptr;
    Axis axis;
    DimensionType dimensionType = dValue;

    int minValue = 0;
    int padding = 0;
    union {
        int value = 0;
        float percentage;
    };
    std::function<int()> function = [](){ return 0; };
    int drawnValue = 0;

    WidgetDimension(Axis axis) : axis(axis) {}
};

struct WidgetAttributeProfile {
    glm::vec3 position;
    glm::vec2 texCoord;
    Color color;
    glm::tvec4<uint16_t> bounds;
    float fontPcnt;

    static const Arx::Sequence<Attribute> attributes;
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
    glm::ivec2 translation;
    std::vector<Rect<int>> boundsStack;
    TextureBlock* textureBlock = new TextureBlock(1024,1024);
	TextureBlock* fontTextureBlock = new TextureBlock(2048,2048);
    VBO<WidgetAttributeProfile>* vbo = new VBO<WidgetAttributeProfile>();

    WidgetDrawContext(int revision) : revision(revision) {
        boundsStack.push_back(Rect<int>(0,0,10000,10000));
    }
    void pushBoundsIntersection(const Rect<int>& other) {
        boundsStack.push_back(boundsStack.back().intersect(other));
    }
    void popBounds() {
        boundsStack.pop_back();
    }
    const Rect<int>& bounds() {
        return boundsStack.back();
    }

    struct TranslationBlock {
        glm::ivec2 prev;
        WidgetDrawContext& ctxt;
        TranslationBlock(WidgetDrawContext& ctxt, const glm::ivec2& trans) : ctxt(ctxt){
            prev = ctxt.translation;

            ctxt.translation += trans;
        }

        virtual ~TranslationBlock() {
            ctxt.translation = prev;
        }
    };
};

//
//struct WidgetLoc {
//    Widget * relativeTo = nullptr;
//    bool inverted = false;
//    bool percentageBased = false;
//    union {
//        int value = 0;
//        float percentage;
//    };
//};

class Widget {
public:
    WidgetPosition _x = WidgetPosition(Axis::X);
    WidgetPosition _y = WidgetPosition(Axis::Y);
    WidgetDimension _width = WidgetDimension(Axis::X);
    WidgetDimension _height = WidgetDimension(Axis::Y);

    static /*thread_local*/ Arx::Sequence<Widget*> ResolutionStack;

    int drawnRevision = 0;

    int uid = getTaggedIdCounter<Widget>().fetch_add(1);

    Widget* parent = nullptr;
    Arx::Sequence<Widget*> children;

    ImagePtr backgroundImage = Image::sentinel();
    Color backgroundColor = Color::fromPercent(1.0f,1.0f,1.0f,1.0f);
    int backgroundPixelScale = 2;
    bool segmented = true;
public:
    Widget(Widget *parent);

	virtual void draw(WidgetDrawContext& context);

    virtual void updateInternal();
protected:
    void ensureRelativeWidgetsDrawn(WidgetDrawContext& context);

    virtual void update(){}

    virtual void drawPre(WidgetDrawContext& context);
    virtual void drawPost(WidgetDrawContext& context);


    void drawQuad(WidgetDrawContext &context, float x, float y, float w, float h, const Color &color,
                      const Rect<float> &texCoordRect, int rot, bool flipVertical, bool isGlyph);

public:
    const WidgetDimension& dimension(Axis axis) const {
        return axis == Axis::X ? _width : _height;
    }

    const WidgetPosition& position(Axis axis) const {
        return axis == Axis::X ? _x : _y;
    }

    void setBackground(const std::string& imgName);

    int resolve(const WidgetDimension& dim);
    int resolve(const WidgetPosition& pos);
    int pad(Axis axis);

    int x() {
        return resolve(_x);
    }
    int y() {
        return resolve(_y);
    }
    int width() {
        return resolve(_width);
    }
    int height() {
        return resolve(_height);
    }
    glm::ivec2 xy() {
        return glm::ivec2(x(),y());
    }
    glm::ivec2 dimensions() {
        return glm::ivec2(width(),height());
    }
};


#endif //TEST2_WIDGET_H
