#include "CompteurDeMine.h"

CompteurDeMine::CompteurDeMine(QWidget* parent)
    : QLCDNumber(parent)
{
    this->setDigitCount(3);
    this->display(0);
    this->setStyleSheet(".QLCDNumber { border: 2px inset gray; background-color: black; color: red; }");
    this->setSegmentStyle(QLCDNumber::Flat);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void CompteurDeMine::SetNombreMines(int numMines)
{
    nombreTotalMines = numMines;
    display((int)nombreTotalMines);
}

void CompteurDeMine::SetNombreDrapeau(unsigned int flagCount)
{
    nombreDrapeau = flagCount;
    display((int)nombreTotalMines - (int)flagCount);
}

QSize CompteurDeMine::sizeHint() const
{
    return QSize(65, 35);
}

