#include "searchnames.h"

#include <QDirIterator>

SearchNames::SearchNames() { }

void SearchNames::searchNameInFile(QString& rootDir, QString& outText, SearchedName& names, OutputData& logs)
{
    QDirIterator it(rootDir, { "*.txt" }, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        const QString path = it.next();
        ++logs.allFiles;

        QFile f(path);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Cannot open:" << path;
            continue;
        }
        QTextStream in(&f);

        const QString& fullContent = in.readAll();

        MatchBeforeAndName fullNames = findBeforeLastFirst(fullContent, names);

        if (fullNames.found) {
            outText += QFileInfo(path).fileName() + " : \t" + fullNames.names.last + " " + fullNames.names.first + " " + fullNames.names.father + "\n";
            // qDebug() << "\n IN searchNameInFile" << QFileInfo(path).fileName();
            // qDebug() << "\n fullNames.names.last" << fullNames.names.last;
            // qDebug() << "\n fullNames.names.first" << fullNames.names.first;
            // qDebug() << "\n fullNames.names.father" << fullNames.names.father;

            ++logs.searchedNames;
        }
    }
}

MatchBeforeAndName SearchNames::findBeforeLastFirst(const QString& text, const SearchedName& names)
{
    // Build pattern with your inputs, escaped, and case-insensitive matching.
    const QString pattern = QString::fromUtf8(
        // R"((?<=^|\P{L})([\p{L}’ʼ'-]+)\p{Zs}+([\p{L}’ʼ'-]*%1[\p{L}’ʼ'-]*)\p{Zs}+([\p{L}’ʼ'-]*%2[\p{L}’ʼ'-]*)\p{Zs}+([\p{L}’ʼ'-]*%3[\p{L}’ʼ'-]*)(?=$|\P{L}))")
        R"(([\p{L}’ʼ'-]+)\p{Zs}+([\p{L}’ʼ'-]*%1[\p{L}’ʼ'-]*)\p{Zs}+([\p{L}’ʼ'-]*%2[\p{L}’ʼ'-]*)\p{Zs}+([\p{L}’ʼ'-]*%3[\p{L}’ʼ'-]*)(?=$|\P{L}))")
                                .arg(QRegularExpression::escape(names.last),
                                    QRegularExpression::escape(names.first),
                                    QRegularExpression::escape(names.father));

    QRegularExpression re(
        pattern,
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::UseUnicodePropertiesOption);

    QRegularExpressionMatch m = re.match(text);
    MatchBeforeAndName res;
    if (m.hasMatch()) {
        // captured(0) = whole match; captured(1)=before; the name parts we need to re-extract:
        // Re-run a small regex on the matched span to split name parts exactly.
        const QString span = m.captured(0);
        // Split into tokens; last two tokens are the name we matched
        const QStringList tokens = span.split(QRegularExpression(R"(\p{Zs}+)",
                                                  QRegularExpression::UseUnicodePropertiesOption),
            Qt::SkipEmptyParts);
        if (tokens.size() >= 4) {
            res.before = tokens[tokens.size() - 4];
            res.names.last = tokens[tokens.size() - 3];
            res.names.first = tokens[tokens.size() - 2];
            res.names.father = tokens[tokens.size() - 1];
            res.found = true;
        }
    }
    return res;
}
