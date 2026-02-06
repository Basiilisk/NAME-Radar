#include "highlightlineedit.h"

HighlightLineEdit::HighlightLineEdit(QWidget* parent)
    : QLineEdit(parent)
{
}

void HighlightLineEdit::paintEvent(QPaintEvent* event)
{
    // 1. Draw the base widget first (background, cursor)
    QLineEdit::paintEvent(event);

    QPainter painter(this);
    QString txt = text();
    if (txt.isEmpty())
        return;

    // QFontMetrics fm = fontMetrics();

    // // Find the text starting position
    // QStyleOptionFrame panel;
    // initStyleOption(&panel);
    // QRect textRect = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
    // painter.fillRect(textRect.adjusted(-2, 2, 2, -2), QColor(255, 255, 153, 100));
    // int x_offset = textRect.left() + 2;
    // int baseline = (height() + fm.ascent() - fm.descent()) / 2;

    // // Regex for "Not Cyrillic, Not Space, Not Punctuation"
    // // Adjust the "allowed" list inside the [^...] brackets
    // QRegularExpression nonCyrillic("[^\\u0400-\\u04FF\\s\\.,!?\\-]");

    // for (int i = 0; i < txt.length(); ++i) {
    //     QString charStr = txt.at(i);
    //     int charWidth = fm.horizontalAdvance(charStr);

    //     // --- Highlight 1: The Space Symbol ---
    //     if (charStr == " ") {
    //         painter.fillRect(x_offset, textRect.top(), charWidth, textRect.height() - 4,
    //             QColor(0, 89, 179, 100)); // Transparent red
    //         // painter.setPen(QColor(180, 180, 180));
    //         painter.drawText(x_offset, 0, charWidth, height(), Qt::AlignCenter, "");
    //     }

    //     // // --- Highlight 2: Non-Cyrillic Detection ---
    //     // else if (nonCyrillic.match(charStr).hasMatch()) {
    //     //     // Draw a subtle red background for the "bad" character
    //     //     painter.fillRect(x_offset, textRect.top() + 2, charWidth, textRect.height() - 4,
    //     //         QColor(255, 0, 0, 50)); // Transparent red

    //     //     // Optional: Draw the character itself in red to make it pop
    //     //     painter.setPen(Qt::red);
    //     //     painter.drawText(x_offset, baseline, charStr);
    //     // }

    //     x_offset += charWidth;
    // }
}
