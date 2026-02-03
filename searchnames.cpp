#include "searchnames.h"

#include <QDirIterator>

SearchNames::SearchNames() { }

void SearchNames::searchNameInFile(
    const QString& rootDir,
    QString& outText,
    const SearchedName& names,
    OutputData& logs)
{
    if (names.last.isEmpty() && names.first.isEmpty() && names.father.isEmpty())
        return;

    QDirIterator it(
        rootDir,
        { "*.txt" },
        QDir::Files,
        QDirIterator::Subdirectories);

    while (it.hasNext()) {
        const QString path = it.next();
        ++logs.allFiles;

        QFile f(path);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Cannot open:" << path;
            continue;
        }

        QTextStream in(&f);
        in.setEncoding(QStringConverter::Utf8);

        QString fullContent = in.readAll();
        fullContent.replace('\r', ' ');

        auto matches = findBeforeLastFirst(fullContent, names);

        for (const auto& m : matches) {
            outText += QFileInfo(path).fileName()
                + " : \t"
                + m.names.last + " "
                + m.names.first + " "
                + m.names.father
                + "\n";

            ++logs.searchedNames;
        }
    }
}

QVector<MatchBeforeAndName> SearchNames::findBeforeLastFirst(const QString& text, const SearchedName& names)
{

    QVector<MatchBeforeAndName> results;

    // %1 = Surname, %2 = First Name, %3 = Patronymic
    // (?:^|\p{Zs}) ensures we start at the beginning of a word and there is a space or start-of-string before it.
    const QString pattern = QString(R"((?:^|\p{Zs})([\p{L}’ʼ'-]*%1[\p{L}’ʼ'-]*)\p{Zs}+([\p{L}’ʼ'-]*%2[\p{L}’ʼ'-]*)\p{Zs}+([\p{L}’ʼ'-]*%3[\p{L}’ʼ'-]*)(?=$|\P{L}))")
                                .arg(("(?<!\\p{L})" + QRegularExpression::escape(names.last) + "[\\p{L}’ʼ'-]*"),
                                    QRegularExpression::escape(names.first),
                                    QRegularExpression::escape(names.father));

    QRegularExpression re(pattern, QRegularExpression::CaseInsensitiveOption);

    auto it = re.globalMatch(text);

    while (it.hasNext()) {
        auto m = it.next();

        MatchBeforeAndName res;
        res.names.last = m.captured(1);
        res.names.first = m.captured(2);
        res.names.father = m.captured(3);

        int start = m.capturedStart(1);
        QString beforeText = text.left(start);

        res.found = true;
        results.push_back(res);
    }

    return results;
}
