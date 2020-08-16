#include "Acceuil.h"
#include "Horloge.h"
#include "ChampDeJeu.h"
#include "Case.h"
#include <QDebug>
#include <QMenuBar>
#include <qmessagebox.h>
#include <QVBoxLayout>
#include <QFrame>
#include <QInputDialog>
#include <QTimer>
#include <QStatusBar>
#include <QSettings>
#include <QScopedArrayPointer>

Acceuil::Acceuil(QWidget* parent)
    : QMainWindow(parent),
      CadrePrincipal(nullptr)
{
    ConfigMachineEtat();
    miseEnPlaceMenus();

}


void Acceuil::ConfigMachineEtat()
{
    //Gestion des Etats de l'application
    m_machine = new QStateMachine;
    EtatNonDemarre = new QState;
    EtatEnCours = new QState;
    EtatVictorieux = new QState;
    EtatPartiePerdue = new QState;

    // logique des état dans lesquels l'application peut se retrouver

    //1- Demarrer pour devenir une partie en Cours
    EtatNonDemarre->addTransition(this, &Acceuil::demarrerJeu, EtatEnCours);
    //2- Une partie en Cours peut être gagnée
    EtatEnCours->addTransition(this, &Acceuil::victoire, EtatVictorieux);
    //3- Une partie en Cours peut être perdue
    EtatEnCours->addTransition(this, &Acceuil::echec, EtatPartiePerdue);
    //4- Une partie en Cours peut être abandonnée
    EtatEnCours->addTransition(this, &Acceuil::DemarrerNouveuJeu, EtatNonDemarre);

    //5- Après une victoire ou une défaite on peut toujours reprendre une nouvelle partie
    EtatVictorieux->addTransition(this, &Acceuil::DemarrerNouveuJeu, EtatNonDemarre);
    EtatPartiePerdue->addTransition(this, &Acceuil::DemarrerNouveuJeu, EtatNonDemarre);


    connect(EtatNonDemarre, &QState::entered, [this]()
    {
        initialisationJeux();
    });

    connect(EtatEnCours, &QState::entered, [this]()
    {
        horlogeJeux->start();
    });

    connect(EtatVictorieux, &QState::entered, [this]()
    {
        horlogeJeux->stop();
        QMessageBox::information(this, tr("Démineur"),
         tr("Félicitations! <br> Vous avez Réussi!<br>"));
    });

    connect(EtatPartiePerdue, &QState::entered, [this]()
    {
        horlogeJeux->stop();
        QMessageBox::critical(this, tr("Démineur"),
         tr("Vous Avez Perdu :("));
    });
    //Enregistrement des Etats
    m_machine->addState(EtatNonDemarre);
    m_machine->addState(EtatEnCours);
    m_machine->addState(EtatVictorieux);
    m_machine->addState(EtatPartiePerdue);
    m_machine->setInitialState(EtatNonDemarre);
    m_machine->start();
}


void Acceuil::miseEnPlaceMenus()
{
    //Ajout du sousmenu Nouveau Jeu
    menuJeu = new QMenu(tr("Jeux"));
     //option Nouveau
    actionNouveauJeu = new QAction(tr("Nouveau"));
    actionNouveauJeu->setShortcut(QKeySequence(Qt::Key_F2));
    connect(actionNouveauJeu, &QAction::triggered, this, &Acceuil::DemarrerNouveuJeu);
    menuJeu->addAction(actionNouveauJeu);

    //Ajout du sousmenu de choix du niveau

    niveauMenu = new QMenu(tr("Niveau"));
    niveauActionGroup = new QActionGroup(niveauMenu);

    facileAction = new QAction(tr("Facile"), niveauActionGroup);
    facileAction->setCheckable(true);
   QObject::connect(facileAction, &QAction::triggered, [this]()
   {
       defNiveau(Niveau::Facile);
   });

    intermediaireAction = new QAction(tr("Intermediate"), niveauActionGroup);
    intermediaireAction->setCheckable(true);

    connect(intermediaireAction, &QAction::triggered, [this]()
    {
        defNiveau(Niveau::Intermediaire);
    });

    expertAction = new QAction(tr("Expert"), niveauActionGroup);
    expertAction->setCheckable(true);
    connect(expertAction, &QAction::triggered, [this]()
    {
        defNiveau(Niveau::Expert);
    });

    niveauMenu->addAction(facileAction);
    niveauMenu->addAction(intermediaireAction);
    niveauMenu->addAction(expertAction);


// Ajout des sous-menus au Menu
    this->menuBar()->addMenu(menuJeu);
    this->menuBar()->addMenu(niveauMenu);
    defNiveau(Niveau::Facile);
}

void Acceuil::initialisationJeux(){
     // creation du chmps de jeu
    QFrame* _cadrePrincipal = new QFrame(this);
    auto _planPrincipal = new QVBoxLayout;
    auto _cadreInfo = new QHBoxLayout;

    // intialisateion du compteur de Mines
   compteurDeMines = new CompteurDeMine(_cadrePrincipal);
   compteurDeMines->SetNombreMines(nbMines);
   _cadreInfo->addWidget(this->compteurDeMines);
   _cadreInfo->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding));

    // Ajout du jeu
  champDejeu = new ChampDejeu(nbLigne, nbCols, nbMines, _cadrePrincipal);
  connect(champDejeu, &ChampDejeu::PretADemarrer, this, &Acceuil::demarrerJeu, Qt::UniqueConnection);
  connect(champDejeu, &ChampDejeu::ChangementNbDrapeau, compteurDeMines, &CompteurDeMine::SetNombreDrapeau, Qt::UniqueConnection);
  connect(champDejeu, &ChampDejeu::Victoire, this, &Acceuil::victoire, Qt::UniqueConnection);
  connect(champDejeu, &ChampDejeu::Perdu, this, &Acceuil::echec, Qt::UniqueConnection);

  _cadreInfo->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding));


   // Ajout de l'horloge
    horloge = new Horloge(_cadrePrincipal);
    horlogeJeux = new QTimer(this);
    horlogeJeux->setInterval(1000);
    connect(horlogeJeux, &QTimer::timeout, horloge, &Horloge::incrementTime, Qt::UniqueConnection);
    _cadreInfo->addWidget(horloge);

    // Ajout du Cadre d'information à l'interface
    _planPrincipal->addLayout(_cadreInfo);
    // Ajout du champ de Mine
    _planPrincipal->addWidget(champDejeu);


    // ajout du PlanPrincipal au cadre
    _cadrePrincipal->setLayout(_planPrincipal);

    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->setCentralWidget(_cadrePrincipal);


}
void Acceuil::defNiveau(Niveau niveau){
    this->niveau = niveau;

    switch (niveau)
    {
    case Niveau::Facile:
        nbLigne = 10;
        nbCols = 10;
        nbMines = 10;
        facileAction->setChecked(true);
        break;
    case Niveau::Intermediaire:
        nbLigne = 16;
        nbCols = 16;
        nbMines = 40;
        intermediaireAction->setChecked(true);
        break;
    case Niveau::Expert:
        nbLigne = 16;
        nbCols = 30;
        nbMines = 99;
        expertAction->setChecked(true);
        break;
    default:
        break;
    }
    DemarrerNouveuJeu();
}

void Acceuil:: DemarrerNouveuJeu(){
    ConfigMachineEtat();
    initialisationJeux();
    adjustSize();
}
