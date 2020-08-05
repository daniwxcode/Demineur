#pragma once
#ifndef CHAMPDEJEU_H
#define CHAMPDEJEU_H
#include "Case.h"
#include "EtatJeu.h"
#include<vector>

#include <QList>
#include <QFrame>
#include <QSet>

class ChampDejeu : public QFrame
{
    Q_OBJECT

public:
    ChampDejeu(unsigned int nbLignes, unsigned int nbCols, unsigned int numMines, QWidget* parent = nullptr);
    unsigned int nbCols() const { return _nbCols; }
    unsigned int nbLignes() const { return _nbLignes; }
    unsigned int numMines() const { return _numMines; }

public slots:
     void placeMines(Case* firstClicked);

signals:
    void initialized();
    void Victoire();
    void defeat();
    void flagCountChanged(unsigned int flagCount);

private:
    void CreationDesCases();
    void _miseEnPlace();
    void AjoutVoisins();
    void _verifVictoire();
    void onVictory();
    void defeatAnimation();

    unsigned int _nbLignes;
    unsigned int _nbCols;
    unsigned int _numMines;

    QList<QList<Case*>> _Cases;
    QSet<Case*> _mines;
    QSet<Case*> _BonsDrapeaux;
    QSet<Case*> _MauvaisDrapeaux;
    QSet<Case*> _CaseDecouverts;

    QTimer* explosionTimer;
    EtatJeu _etatJeux;

};
#endif // CHAMPDEJEU_H
