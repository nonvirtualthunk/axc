//
// Created by nvt on 8/14/16.
//

#include <Noto.h>
#include <application/ResourceManager.h>
#include <core/ArxString.h>
#include "Widget.h"

#include "core/Predef.h"
#include "BackgroundImageMetrics.h"

Arx::Sequence<Widget*> Widget::ResolutionStack;

const Arx::Sequence<Attribute> WidgetAttributeProfile::attributes =
        Seq<Attribute>({ Attribute(3,GL_FLOAT), Attribute(2,GL_FLOAT), Attribute(4,GL_UNSIGNED_BYTE),
              Attribute(4,GL_UNSIGNED_SHORT,false, true) });

void Widget::draw(WidgetDrawContext& context) {
    static Arx::Sequence<Widget*> drawStack;

    if (context.revision > this->drawnRevision) {
        if (drawStack.contains(this)) {
            Noto::error("Recursive draw loop discovered for widget {}", uid);
            return;
        }
        drawStack.push(this);
        ensureRelativeWidgetsDrawn(context);

        glm::ivec2 min = xy();
        WidgetDrawContext::TranslationBlock xyBlock(context, min);

        drawPre(context);
        drawnRevision = context.revision;

        int xpad = pad(Axis::X);
        int ypad = pad(Axis::Y);
        Rect<int> selfBounds(x() + xpad, y() + ypad, width() - xpad * 2, height() - ypad * 2);
        context.pushBoundsIntersection(selfBounds);

        {
            auto trans = selfBounds.xy();
            WidgetDrawContext::TranslationBlock clientXYBlock(context, trans);

            for (Widget * child : children) {
                child->draw(context);
            }
        }

        context.popBounds();

        drawPost(context);

        drawStack.pop();
    }
}

int Widget::pad(Axis axis) {
    int ret = dimension(axis).padding;
    if (!backgroundImage->isSentinel()) {
        auto metrics = BackgroundImageMetrics::imageMetricsFor(backgroundImage);
        ret += metrics.borderPixelWidth * backgroundPixelScale;
    }
    return ret;
}


void Widget::ensureRelativeWidgetsDrawn(WidgetDrawContext &context) {
    auto dependentWidgets = Seq({_x.relativeTo, _y.relativeTo, _width.relativeTo, _height.relativeTo});

    for (int i = 0; i < dependentWidgets.size(); ++i) {
        Widget * dw = dependentWidgets[i];
        // If the dependent widget hasn't been drawn yet this pass, ensure it is drawn before continuing
        if (dw != nullptr && dw->drawnRevision < context.revision) {
            dw->draw(context);
        }
    }

}

void Widget::drawPre(WidgetDrawContext& context) {
    glm::ivec2 dim = dimensions();


    const auto& img = backgroundImage;
    if (!img->isSentinel()) {
        auto metrics = BackgroundImageMetrics::imageMetricsFor(img);

        int sideW = metrics.borderPixelWidth * backgroundPixelScale;
        const TextureBlock::Cell& textureCell = context.textureBlock->getOrElseUpdateCell(img);
        const Rect<float> tcr = textureCell.texCoordRect;

        glm::vec2 cornerTexPercent(1.0f,1.0f);
        glm::ivec2 cornerDim(img->width/2 * backgroundPixelScale,img->height/2 * backgroundPixelScale);
        for (int axis = 0; axis < 2; ++axis) {
            cornerTexPercent[axis] = std::min(1.0f,float(dim[axis]/2) / float(cornerDim[axis]));
            cornerDim[axis] = std::min(dim[axis]/2,cornerDim[axis]);
        }

        glm::ivec2 sideDim = dim - cornerDim * 2;
        Rect<float> cornerTC(tcr.x,tcr.y + tcr.height * (0.5f + (1.0f - cornerTexPercent.y) * 0.5f),
                             tcr.width * cornerTexPercent.x * 0.5f, tcr.height * cornerTexPercent.y * 0.5f);
        Rect<float> horizSideTC(tcr.x + tcr.width * 0.5f,
                                tcr.y + tcr.height * 0.5f + (tcr.width * 0.5f * (1.0f - cornerTexPercent.y)),
                                0, cornerTC.height);
        Rect<float> vertSideTC(tcr.x,tcr.y,cornerTC.width,0);
        Rect<float> middleTC = context.textureBlock->getOrElseUpdateCell(ResourceManager::image("blank.png")).texCoordRect;

        int bpw = metrics.borderPixelWidth;
        drawQuad(context, bpw,bpw,dim.x - bpw, dim.y - bpw,backgroundColor * metrics.centerColor,middleTC,0);

        drawQuad(context, 0,0,cornerDim.x,cornerDim.y,backgroundColor,cornerTC,90);
        drawQuad(context, dim.x - cornerDim.x,0,cornerDim.x,cornerDim.y,backgroundColor,cornerTC,180);
        drawQuad(context, dim.x - cornerDim.x,dim.y - cornerDim.y,cornerDim.x,cornerDim.y,backgroundColor,cornerTC,270);
        drawQuad(context, 0,dim.y - cornerDim.y,cornerDim.x,cornerDim.y,backgroundColor,cornerTC,0);

        drawQuad(context, 0,cornerDim.y,cornerDim.x,sideDim.y,backgroundColor,vertSideTC,0);
        drawQuad(context, dim.x - cornerDim.x,cornerDim.y,cornerDim.x,sideDim.y,backgroundColor,vertSideTC,180);

        drawQuad(context, cornerDim.x,0,sideDim.x,cornerDim.y,backgroundColor,horizSideTC,180);
        drawQuad(context, cornerDim.x,dim.y - cornerDim.y,sideDim.x,cornerDim.y,backgroundColor,horizSideTC,0);
    } else {
        auto blankImage = ResourceManager::image("blank.png");
        const TextureBlock::Cell& textureCell = context.textureBlock->getOrElseUpdateCell(blankImage);
        drawQuad(context, 0,0, dim.x, dim.y, backgroundColor, textureCell.texCoordRect, 0);
    }
}

void Widget::drawPost(WidgetDrawContext& context) {

}

void Widget::drawQuad(WidgetDrawContext &ctxt, float x, float y, float w, float h, const Color &color,
                      const Rect<float> &texCoordRect, int rot) {
    WidgetAttributeProfile* vp = ctxt.vbo->addQuad();
    vp[0].position = glm::vec3(ctxt.translation.x + x,ctxt.translation.y + y,0.0f);
    vp[1].position = glm::vec3(ctxt.translation.x + x + w,ctxt.translation.y + y,0.0f);
    vp[2].position = glm::vec3(ctxt.translation.x + x + w,ctxt.translation.y + y + h,0.0f);
    vp[3].position = glm::vec3(ctxt.translation.x + x,ctxt.translation.y + y + h,0.0f);

    int tcShifts = rot/90;

    vp[(0 + tcShifts)%4].texCoord = glm::vec2(texCoordRect.x,texCoordRect.y);
    vp[(1 + tcShifts)%4].texCoord = glm::vec2(texCoordRect.x + texCoordRect.width,texCoordRect.y);
    vp[(2 + tcShifts)%4].texCoord = glm::vec2(texCoordRect.x + texCoordRect.width,texCoordRect.y + texCoordRect.height);
    vp[(3 + tcShifts)%4].texCoord = glm::vec2(texCoordRect.x,texCoordRect.y + texCoordRect.height);

    const auto& b = ctxt.bounds();
    glm::tvec4<uint16_t> bv(b.x,b.y,b.x+b.width,b.y+b.height);
    for (int i = 0; i < 4; ++i) {
        vp[i].color = color;
        vp[i].bounds = bv;
    }
}

Widget::Widget(Widget *parent)
        : parent(parent) {
    if (parent != nullptr) {
        parent->children.add(this);
    }
}

int Widget::resolve(const WidgetPosition& pos) {
    if (ResolutionStack.contains(this)) {
        Noto::warn("Infinite loop detected in position resolution");
        return pos.drawnValue;
    }

    ResolutionStack.push(this);

    auto axis = pos.axis;
    Widget * relativeTo = pos.relativeTo == nullptr ? parent : pos.relativeTo;

    int baseValue = pos.value;
    if (pos.positionType == pPercentage) {
        int referencePoint = relativeTo->resolve(relativeTo->dimension(axis));
        if (relativeTo == parent) {
            referencePoint -= relativeTo->pad(axis) * 2;
        }
        baseValue = int(referencePoint * pos.percentage);
        if (relativeTo == parent) {

        }
    } else if (pos.positionType == pFunction) {
        baseValue = pos.function();
    }

    int effValue = baseValue;
    if (pos.alignOpposite) {
        effValue = relativeTo->resolve(relativeTo->dimension(axis)) - baseValue - this->resolve(this->dimension(axis));
    }

    if( !ResolutionStack.pop(this) ) { Noto::error("Unbalanced push/pop in position resolution"); }

    return effValue;
}

int Widget::resolve(const WidgetDimension& dim) {
    if (ResolutionStack.contains(this)) {
        Noto::warn("Infinite loop detected in dimension resolution");
        return dim.drawnValue;
    }

    ResolutionStack.push(this);
    int ret;

    auto axis = dim.axis;
    Widget * relativeTo = dim.relativeTo == nullptr ? parent : dim.relativeTo;

    if (dim.dimensionType == dEncapsulate) {
        int minV = 100000;
        int maxV = -100000;

        for (Widget * child : relativeTo->children) {
            minV = std::min(child->resolve(child->position(axis)),minV);
            maxV = std::max(child->resolve(child->position(axis)) + child->resolve(child->dimension(axis)),maxV);
        }

        ret = std::max(maxV - minV + dim.padding * 2, dim.minValue);
    } else {
        int baseValue = dim.value;
        if (dim.dimensionType == dPercentage) {
            int referencePoint = relativeTo->resolve(relativeTo->dimension(axis));
            if (relativeTo == parent) {
                referencePoint -= relativeTo->pad(axis) * 2;
            }
            baseValue = int(referencePoint * dim.percentage);
        } else if (dim.dimensionType == dFunction) {
            baseValue = dim.function();
        }

        ret = baseValue;
    }

    if( !ResolutionStack.pop(this) ) { Noto::error("Unbalanced push/pop in dimension resolution"); }
    return ret;
}

void Widget::setBackground(const std::string &imgName) {
    Arx::String name(imgName);
    if (!name.endsWith(".png")) {
        name.append(".png");
    }
    backgroundImage = ResourceManager::image(Arx::String("ui/backgrounds/") + name);
}
