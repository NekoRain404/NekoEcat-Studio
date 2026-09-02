#include "UITestFixture.h"

#include <QApplication>

UITestFixture::UITestFixture(QObject* parent) : QObject(parent) {}

UITestFixture::~UITestFixture() {
    for (int i = managedWidgets_.size() - 1; i >= 0; --i) {
        QWidget* w = managedWidgets_[i];
        if (w && w->parentWidget() == nullptr)
            delete w;
    }
}

QWidget* UITestFixture::createWidget(QWidget* parent) {
    auto* w = new QWidget(parent);
    managedWidgets_.append(w);
    return w;
}

void UITestFixture::destroyWidget(QWidget* widget) {
    managedWidgets_.removeOne(widget);
    delete widget;
}

bool UITestFixture::widgetExists(QWidget* widget) const {
    return managedWidgets_.contains(widget);
}

void UITestFixture::initOffscreen() {
    qputenv("QT_QPA_PLATFORM", "offscreen");
}
