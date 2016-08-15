//
// Created by nvt on 8/14/16.
//

#include "Widget.h"

#include "core/Predef.h"

void Widget::drawCore(WidgetDrawContext& context) {


}


void Widget::ensureRelativeWidgetsDrawn(WidgetDrawContext &context) {
    Widget* dependentWidgets[] = {_x.relativeTo, _y.relativeTo, _width.relativeTo, _height.relativeTo};
    for (int i = 0; i < 4; ++i) {
        // If the dependent widget hasn't been drawn yet this pass, ensure it is drawn before continuing
        if (dependentWidgets[i] != nullptr && dependentWidgets[i]->drawnRevision < context.revision) {

        }
    }

}



void Widget::draw(WidgetDrawContext& context) {

}