#ifndef STYLES_H
#define STYLES_H
#include <QString>
 static const  QString NormalStyle =
            "Case"
            "{"
            "	border: 1px solid darkgray;"
            "font-weight: bold;"
            "	background: qradialgradient(cx : 0.4, cy : -0.1, fx : 0.4, fy : -0.1, radius : 1.35, stop : 0 #fff, stop: 1 #bbb);"
            "	border - radius: 1px;"
            "}";
 static const QString CaseCliqueStyle =
            "Case"
            "{"
            "	border: 1px solid lightgray;"
            "}";
static  const QString NumeroStyle =
            "Case"
            "{"
            "	color: %3;"
            "	font-weight: bold;"
            "	border: 1px solid lightgray;"
            "}";
static const QString CompteurStyle= ".QLCDNumber { border: 2px inset gray; }";

#endif // STYLES_H
