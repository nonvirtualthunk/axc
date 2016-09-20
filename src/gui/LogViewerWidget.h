//
// Created by nvt on 9/17/16.
//

#ifndef TEST2_LOGVIEWERWIDGET_H
#define TEST2_LOGVIEWERWIDGET_H


#include "TextWidget.h"

class LogViewerWidget : public Widget {
protected:
    TextWidget * logDisplay;
public:
    LogViewerWidget(Widget *parent);

};


#endif //TEST2_LOGVIEWERWIDGET_H
