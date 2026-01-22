#include "sortfilebydate.h"

#include <QDate>
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>
#include <QVector>
#include <algorithm>

struct Row {
    QDate date;
    QString line;
    bool hasDate;
};

SortFileByDate::SortFileByDate()
{
}

int SortFileByDate::sortResultData(QString& outTempText, const OutputData& logs)
{
    QString outText;
    QStringList resLines = outTempText.split("\n");

    // Grab all lines
    QVector<Row> rows;
    QRegularExpression dateRe(R"((\d{2}\.\d{2}\.\d{4}))"); // dd.MM.yyyy anywhere in the line
    for (auto it = resLines.begin(); it != resLines.end(); ++it) {
        const QString line = *it;
        QRegularExpressionMatch m = dateRe.match(line);
        if (m.hasMatch()) {
            const QString d = m.captured(1);
            QDate qd = QDate::fromString(d, "dd.MM.yyyy");
            rows.push_back({ qd, line, qd.isValid() });
        } else {
            rows.push_back({ QDate(), line, false });
        }
    }

    // Sort: valid dates first (ascending), then lines without a date (preserve original order among them)
    std::stable_sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) {
        if (a.hasDate != b.hasDate)
            return a.hasDate && !b.hasDate;
        if (!a.hasDate && !b.hasDate)
            return false;
        if (a.date != b.date)
            return a.date < b.date;
        return a.line < b.line; // tiebreaker by text
    });

    outText = "\n\t|Searched " + QString::number(logs.searchedNames) + " from " + QString::number(logs.allFiles) + "|\n\n";

    auto printHeader = [&](const QString& label) {
        outText += "___________________________________________________________\n";
        outText += "______________________|" + label + "|____________________________________\n";
        outText += "___________________________________________________________\n";
    };

    int lastYear = -1;
    bool printedOthers = false;

    for (const auto& r : rows) {
        if (r.hasDate) {
            int y = r.date.year();
            if (y != lastYear) {
                printHeader(QString::number(y));
                lastYear = y;
            }
            outText += r.line + "\n";
        } else {
            if (!printedOthers) {
                printHeader("OTHERS");
                printedOthers = true;
            }
            outText += r.line + "\n";
        }
    }

    outTempText = outText;

    return 0;
}
