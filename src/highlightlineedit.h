#pragma once

#include <QLineEdit>
#include <QPainter>
#include <QRegularExpression>
#include <QStyleOptionFrame>

class HighlightLineEdit : public QLineEdit {
public:
    HighlightLineEdit(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};
