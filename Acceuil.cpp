#include "Acceuil.h"
#include "ui_mainwindow.h"
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


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupStateMachine();
    miseEnPlaceMenus();

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setupStateMachine()
{
    //Gestion des Etats de l'application
    m_machine = new QStateMachine;

    unstartedState = new QState;
    inProgressState = new QState;
    victoryState = new QState;
    defeatState = new QState;

    unstartedState->addTransition(this, &MainWindow::demarrerJeu, inProgressState);

    inProgressState->addTransition(this, &MainWindow::victoire, victoryState);
    inProgressState->addTransition(this, &MainWindow::echec, defeatState);
    inProgressState->addTransition(this, &MainWindow::DemarrerNouveuJeu, unstartedState);

    victoryState->addTransition(this, &MainWindow::DemarrerNouveuJeu, unstartedState);

    defeatState->addTransition(this, &MainWindow::DemarrerNouveuJeu, unstartedState);

    connect(unstartedState, &QState::entered, [this]()
    {
        initialisationJeux();
    });

    connect(inProgressState, &QState::entered, [this]()
    {
        horlogeJeux->start();
    });

    connect(victoryState, &QState::entered, [this]()
    {
        horlogeJeux->stop();
        QMessageBox msgBox;
    //horloge->time()
        msgBox.setText( tr("Félicitations! <br> Vous avez Réussi!<br>Temps:"));
        msgBox.exec();
    });

    connect(defeatState, &QState::entered, [this]()
    {

        QMessageBox msgBox;
        msgBox.setText( tr("Vous Avez Perdu"));
        msgBox.exec();
        horlogeJeux->stop();
    });

    m_machine->addState(unstartedState);
    m_machine->addState(inProgressState);
    m_machine->addState(victoryState);
    m_machine->addState(defeatState);

    m_machine->setInitialState(unstartedState);
    m_machine->start();
}



void MainWindow::miseEnPlaceMenus()
{
    //Ajout du sousmenu Nouveau Jeu
    menuJeu = new QMenu(tr("Jeux"));
     //option Nouveau
    actionNouveauJeu = new QAction(tr("Nouveau"));
    actionNouveauJeu->setShortcut(QKeySequence(Qt::Key_F2));
    connect(actionNouveauJeu, &QAction::triggered, this, &MainWindow::demarrerJeu);
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



    //Ajout du sous-menu d'Apropos
    menuApropos = new QMenu(tr("?"));
    aProposAction = new QAction(tr("?"));
    menuApropos->addAction(aProposAction);


// Ajout des sous-menus au Menu
    this->menuBar()->addMenu(menuJeu);
    this->menuBar()->addMenu(niveauMenu);
    this->menuBar()->addMenu(menuApropos);
    defNiveau(Niveau::Facile);
}

void MainWindow::initialisationJeux(){
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
  connect(champDejeu, &ChampDejeu::initialized, this, &MainWindow::demarrerJeu, Qt::UniqueConnection);
  connect(champDejeu, &ChampDejeu::flagCountChanged, compteurDeMines, &CompteurDeMine::SetNombreDrapeau, Qt::UniqueConnection);
  connect(champDejeu, &ChampDejeu::Victoire, this, &MainWindow::victoire, Qt::UniqueConnection);
  connect(champDejeu, &ChampDejeu::defeat, this, &MainWindow::echec, Qt::UniqueConnection);


   btnNouveauJeu = new QPushButton(_cadrePrincipal);
   btnNouveauJeu->setMinimumSize(35, 35);
   btnNouveauJeu->setIconSize(QSize(30, 30));
   btnNouveauJeu->setText(":)");
 //  connect(btnNouveauJeu, &QPushButton::clicked, this, &MainWindow::btnNouveauJeu, Qt::UniqueConnection);
   _cadreInfo->addWidget(btnNouveauJeu);
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
void MainWindow::defNiveau(Niveau niveau){
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

    initialisationJeux();
    adjustSize();
}
