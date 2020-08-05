#pragma once
#ifndef CASE_H
#define CASE_H
#include"Position.h"

#include <vector>

#include <QPushButton>
#include <QList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QSignalMapper>

class Case : public QPushButton
{
    Q_OBJECT
public:

    Case(Position position, QWidget* parent = nullptr);
    virtual ~Case() override;
    void AfficheResultat();
    void AjoutVoisin(Case* Case);
    Position position() const;
    void placerMine(bool val);

    bool EstMine() const;
    bool AuneMarque() const;
    bool EstOuvert() const;
    bool isUnrevealed() const;
    bool AdesMinesAutour() const;

    unsigned int NbMinesAutour() const;
    unsigned int NbDrapeauAutour() const;

    QList<Case*>& neighbors();

    virtual void mousePressEvent(QMouseEvent* e) override;
    virtual void mouseReleaseEvent(QMouseEvent* e) override;
    virtual void mouseMoveEvent(QMouseEvent* e) override;
    virtual QSize sizeHint() const override;
    //static QIcon getIcon(IconeCase icone);


public slots:

    void incrementNbDrapeauAutour();
    void decrementNbDrapeauAutour();
    void incrementNbMinesAutour();

signals:

    void firstClick(Case*);
    void leftClicked();
    void rightClicked();
    void bothClicked();
    void unClicked();
    void detonated();
    void reveal();
    void revealed();
    void revealNeighbors();
    void preview();
    void unPreview();
    void unPreviewNeighbors();
    void flagged(bool);
    void unFlagged(bool);
    void disable();

private:

    void createStateMachine();


private:
    void setText();
    bool _isMine;
    unsigned int _NbMinesAutour;
    unsigned int _NbDrapeauAutour;
    Position _position;
    QList<Case*> _voisins;
    bool _bothClicked;

    Qt::MouseButtons _buttons;
    static QSize Dimension;
    static bool _firstClick;

    QStateMachine _machine;
    QState* unrevealedState;
    QState* previewState;
    QState* previewNeighborsState;
    QState* flaggedState;
    QState* revealedState;
    QState* revealNeighborsState;
    QFinalState* disabledState;
};
#endif
// CASE_H
