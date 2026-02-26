#include "hashutils.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>

QString HashUtils::calculateCompositeHash(const QString& rootDir, const QString& absoluteFilePath)
{
    QFile file(absoluteFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return "";
    }

    QFileInfo info(absoluteFilePath);
    QString relativePath = QDir(rootDir).relativeFilePath(absoluteFilePath);
    qint64 lastModified = info.lastModified().toMSecsSinceEpoch();

    QCryptographicHash hash(QCryptographicHash::Sha256);

    hash.addData(relativePath.toUtf8());
    hash.addData(QString::number(lastModified).toUtf8());

    if (hash.addData(&file)) {
        return hash.result().toHex();
    }

    return "";
}
