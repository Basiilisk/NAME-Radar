#include "highlightlineedit.h"

HighlightLineEdit::HighlightLineEdit(QWidget* parent)
    : QLineEdit(parent)
{
}

void HighlightLineEdit::paintEvent(QPaintEvent* event)
{
    // 1. Base paint (draws the actual text, cursor, and focus frame)
    QLineEdit::paintEvent(event);

    QPainter painter(this);
    QString txt = text();
    if (txt.isEmpty())
        return;

    QFontMetrics fm = fontMetrics();
    QStyleOptionFrame panel;
    initStyleOption(&panel);

    // SE_LineEditContents gives us the internal "typing area" rect
    QRect textRect = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);

    // 2. The Absolute Scroll Fix
    // We calculate the starting X by subtracting the width of text before the cursor
    // from the actual cursor's visual position.
    int textBeforeCursorWidth = fm.horizontalAdvance(txt.left(cursorPosition()));
    int x_start = cursorRect().left() - textBeforeCursorWidth + 5;

    // 3. Regex for Non-Cyrillic (Strict Hex Range)
    static QRegularExpression nonCyrillic(R"([^\x{0400}-\x{04FF}\s\.,!?\-])");

    // Prevent highlights from drawing over the widget borders
    painter.setClipRect(textRect);

    for (int i = 0; i < txt.length(); ++i) {
        // --- The "Anti-Drift" Calculation ---
        // Instead of adding widths, we calculate the absolute start and end of this specific char.
        // This handles cases where logical pixels != physical pixels.
        int charStartRel = fm.horizontalAdvance(txt.left(i));
        int charEndRel = fm.horizontalAdvance(txt.left(i + 1));
        int exactWidth = charEndRel - charStartRel;

        int drawX = x_start + charStartRel;

        // Only draw if the character is inside the visible area
        if (drawX + exactWidth >= textRect.left() && drawX <= textRect.right()) {

            QChar c = txt.at(i);

            // --- Highlight 1: Spaces (Blue) ---
            if (c == ' ') {
                // 'exactWidth - 1' creates a 1px gap so multiple spaces don't look like a solid bar
                painter.fillRect(drawX, textRect.top() + 2, exactWidth, textRect.height(),
                    QColor(255, 77, 77, 160));
            }

            // --- Highlight 2: Non-Cyrillic (Red) ---
            else if (nonCyrillic.match(c).hasMatch()) {
                painter.fillRect(drawX, textRect.top() + 2, exactWidth, textRect.height(),
                    QColor(102, 204, 255, 160));
            }
        }

        // Optimization: if we are already past the right edge, stop looping
        if (drawX > textRect.right())
            break;
    }
}
