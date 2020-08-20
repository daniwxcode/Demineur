#include "CompteurDeMine.h"
#include "Styles.h"

CompteurDeMine::CompteurDeMine(QWidget* parent)
    : QLCDNumber(parent)
{

    this->setSegmentStyle(QLCDNumber::Flat);
    this->display(0);
    this->setStyleSheet(CompteurStyle);
    this->setDigitCount(3);
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
    return QSize(69, 38);
}

