#include "searchnames.h"

#include <QDirIterator>

SearchNames::SearchNames() { }

void SearchNames::searchNameInFile(const QString& rootDir, QString& outText, const SearchedName& names, OutputData& logs)
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
    // %1 = Surname, %2 = First Name, %3 = Patronymic
    // (?:^|\p{Zs}) ensures we start at the beginning of a word and there is a space or start-of-string before it.
    const QString pattern = QString(R"((?:^|\p{Zs})([\p{L}’ʼ'-]*%1[\p{L}’ʼ'-]*)\p{Zs}+([\p{L}’ʼ'-]*%2[\p{L}’ʼ'-]*)\p{Zs}+([\p{L}’ʼ'-]*%3[\p{L}’ʼ'-]*)(?=$|\P{L}))")
                                .arg(("(?<!\\p{L})" + QRegularExpression::escape(names.last) + "[\\p{L}’ʼ'-]*"),
                                    QRegularExpression::escape(names.first),
                                    QRegularExpression::escape(names.father));

    QRegularExpression re(pattern, QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch m = re.match(text);
    MatchBeforeAndName res;

    if (m.hasMatch()) {
        // m.captured(1) is Surname
        // m.captured(2) is First Name
        // m.captured(3) is Patronymic
        res.names.last = m.captured(1).trimmed();
        res.names.first = m.captured(2).trimmed();
        res.names.father = m.captured(3).trimmed();

        // To get the "before" part (the word immediately preceding the name)
        // We look at the text before the start of the surname
        int matchStart = m.capturedStart(1);
        QString textBefore = text.left(matchStart).trimmed();
        res.before = textBefore.section(QRegularExpression(R"(\p{Zs}+)"), -1); // Get the very last word before the name

        res.found = true;
    }
    return res;
}
