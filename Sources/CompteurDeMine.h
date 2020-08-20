#ifndef COMPTEURDEMINE_H
#define COMPTEURDEMINE_H
#pragma once
#include <QLCDNumber>

class CompteurDeMine : public QLCDNumber
{
public:
    CompteurDeMine(QWidget* parent = nullptr);

    void SetNombreMines(int numMines);
    void SetNombreDrapeau(unsigned int flagCount);
    virtual QSize sizeHint() const override;

private:

    unsigned int  nombreTotalMines;
    unsigned int nombreDrapeau;
};

#endif // COMPTEURDEMINE_H
