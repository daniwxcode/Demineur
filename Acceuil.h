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

class Acceuil : public QMainWindow
{
    Q_OBJECT
signals:

    void demarrerJeu();
    void victoire();
    void echec();
    void DemarrerNouveuJeu();


public:



    Acceuil(QWidget *parent = nullptr);
private:
 /*!
     *  \brief Ajout du Menu de jeu
     *
     *  Methode qui permet d'ajouter le menu sur l'écran d'MainWindow

     */
    void miseEnPlaceMenus();
    void initialisationJeux();
    void ConfigMachineEtat();
    void AfficheMessage(char* Message);

private:
    QFrame* CadrePrincipal;
    QAction* actionNouveauJeu;
    Horloge* horloge;
    ChampDejeu* champDejeu;
    CompteurDeMine* compteurDeMines;
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



// Pattern State
    QStateMachine* m_machine;
    QState* EtatNonDemarre;
    QState* EtatEnCours;
    QState* EtatVictorieux;
    QState* EtatPartiePerdue;
    // Niveau du Jeu
    void defNiveau(Niveau niveau);

    Niveau niveau;
    quint32 nbLigne;
    quint32 nbCols;
    quint32 nbMines;

};
#endif // MAINWINDOW_H
