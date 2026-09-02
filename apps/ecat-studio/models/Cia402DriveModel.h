#pragma once

// CiA 402 controlword/statusword recommendation and mode-interlock logic.


#include <QString>

// Recommended controlword action based on current statusword
struct Cia402ControlwordRecommendation {
    QString label;
    QString value;
    QString reason;
};

Cia402ControlwordRecommendation recommendedCia402ControlwordFromStatus(const QString& decodedStatusword);

bool isCia402Object(const QString& index, const QString& mode = QString());
QString decodeCia402Value(const QString& index, const QString& value);
