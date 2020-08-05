#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "Niveau.h"
#include "Horloge.h"
#include "ChampDeJeu.h"
#include "CompteurDeMine.h"
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QFrame>
#include <QPushButton>
#include <QStateMachine>
#include <QState>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
signals:

    void demarrerJeu();
    void victoire();
    void echec();
    void DemarrerNouveuJeu();

//protected slots:
//
//    void onVictory();

public:



    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();
private:
    void miseEnPlaceMenus();
    void initialisationJeux();
    void setupStateMachine();

private:
    Ui::MainWindow *ui;

    QFrame* CadrePrincipal;
    QAction* actionNouveauJeu;
    Horloge* horloge;
    ChampDejeu* champDejeu;
    CompteurDeMine* compteurDeMines;
    QPushButton* btnNouveauJeu;
    QMenu* menuJeu;

    QMenu* niveauMenu;
    QActionGroup* niveauActionGroup;
    QAction* facileAction;
    QAction* intermediaireAction;
    QAction* expertAction;
    QAction* persoAction;
    QAction* highScoreAction;
    QAction* exitAction;
    QMenu* menuApropos;
    QAction* aProposAction;

    //Pour Afficher le Temps
    QTimer* horlogeJeux;


// Code trouvé sur internet possibilité de mieux faire
// Pattern State
    QStateMachine* m_machine;
    QState* unstartedState;
    QState* inProgressState;
    QState* victoryState;
    QState* defeatState;
    // Niveau du Jeu
    void defNiveau(Niveau niveau);

    Niveau niveau;
    quint32 nbLigne;
    quint32 nbCols;
    quint32 nbMines;

};
#endif // MAINWINDOW_H
