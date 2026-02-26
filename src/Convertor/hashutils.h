#pragma once

#include <QString>

class HashUtils {
public:
    static QString calculateCompositeHash(const QString& rootDir, const QString& absoluteFilePath);
};
