//
// Created by nvt on 9/17/16.
//

#include <application/ResourceManager.h>
#include "LogViewerWidget.h"

#include "core/Predef.h"

Arx::Sequence<Arx::String> getLogs() {
    return Noto::history.take(5).map<Arx::String>([](const LogStatement& l){ return l.statement; });
}

LogViewerWidget::LogViewerWidget(Widget *parent) : Widget(parent) {
    setBackground("greenMinorStyledBorder");
    logDisplay = new TextWidget(this,"Hello");

    logDisplay->fontSize = 35;

    logDisplay->_x.value = 0;
    logDisplay->_y.value = 0;

    logDisplay->_width.dimensionType = dPercentage;
    logDisplay->_width.percentage = 1.0f;
    logDisplay->_width.padding = 5;

    logDisplay->_height.dimensionType = dPercentage;
    logDisplay->_height.percentage = 1.0f;
    logDisplay->_height.padding = 5;

    logDisplay->text = [](){ return getLogs(); };
};