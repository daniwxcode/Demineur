#include "Case.h"
#include <thread>
#include <chrono>
#include "Styles.h"
#include <QDebug>
#include <QState>
#include <QFinalState>
#include <QMouseEvent>
#include <QSizePolicy>

bool Case::_firstClick = false;
QSize Case::Dimension= QSize(50,50);



Case::Case(Position position, QWidget* parent /*= nullptr*/)
    : _isMine(false)
    , _NbMinesAutour(0)
    , _NbDrapeauAutour(0)
    , _position(position)
    , QPushButton(parent)
{
    ConfigGestionEtatCase();
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setCheckable(true);
    setMouseTracking(true);
}

Case::~Case()
{
    _firstClick = false;
    delete ModeNormal;
    delete ModeAppercu;
    delete ModeDrapeauPointe;
    delete ModeDecouvert;
    delete ModeInactive;
}

void Case::AjoutVoisin(Case* Case)
{
    _voisins += Case;
    connect(this, &Case::revealNeighbors, Case, &Case::reveal, Qt::QueuedConnection);
    connect(this, &Case::unPreviewNeighbors, Case, &Case::unPreview, Qt::QueuedConnection);
}

Position Case::position() const
{
    return _position;
}

void Case::placerMine(bool val)
{
    _isMine = val;
    for (auto voisin : _voisins)
        voisin->incrementNbMinesAutour();
}
/*!
    *  \brief Informe sur l'existence ou non d'une Mine
    *
    *  Si la case est minée elle retourne Vrai
    */
bool Case::EstMine() const
{
    return _isMine;
}
/*!
    *  \brief Informe sur l'existence de mines dans au moins l'une des cases voisines
    *
    *  Si une case voisine est minée elle retourne Vrai
    */
bool Case::AdesMinesAutour() const
{
    return _NbMinesAutour;
}
/*!
    *  \brief Informe sur le nombre de mines dans les cases voisines
    *
    *  Elle retour un Entier Positif
    */
unsigned int Case::NbMinesAutour() const
{
    return _NbMinesAutour;
}
/*!
    *  \brief Incrémente le nomre de mine autour d'elle
    */
void Case::incrementNbMinesAutour()
{
    ++_NbMinesAutour;
}
/*!
    *  \brief Informe sur l'existence ou non d'un marquage de drapeau
    */
bool Case::AuneMarque() const
{
    return _machine.configuration().contains(ModeDrapeauPointe);
}

bool Case::EstOuvert() const
{
    return _machine.configuration().contains(ModeDecouvert);
}

bool Case::isUnrevealed() const
{
    return _machine.configuration().contains(ModeNormal);
}
/*!
    *  \brief Retourne le nombre de drapeaux autour d'elle
    * A la fin du jeux si chaque case a le même nombre de drapeaux autour égal au  nombre de mine autour c'est
    * que les mines ont bien été ifentifiées donc le jeu peut s'arrêter
    */
unsigned int Case::NbDrapeauAutour() const
{
    return _NbDrapeauAutour;
}
/*!
    *  \brief Incréménte le nombre de drapeaux autour d'elle
    */
void Case::incrementNbDrapeauAutour()
{
    ++_NbDrapeauAutour;
}
/*!
    *  \brief Décrémente le nombre de drapeaux autour d'elle
    */
void Case::decrementNbDrapeauAutour()
{
    --_NbDrapeauAutour;
}
/*!
    *  \brief Liste des cases qui contituent le voisinage
    */
QList<Case*>& Case::Voisinage()
{
    return _voisins;
}

void Case::mousePressEvent(QMouseEvent* e)
{
    if (!_firstClick)
    {
        _firstClick = true;
        emit PremierClick(this);
    }

    _bothClicked = false;

    if (e->buttons() == (Qt::LeftButton | Qt::RightButton))
    {
        emit ClickGaucheEtDroit();
        _buttons = Qt::LeftButton | Qt::RightButton;
    }
    else if (e->buttons() == Qt::LeftButton)
        _buttons = Qt::LeftButton;
    else if (e->buttons() == Qt::RightButton)
        _buttons = Qt::RightButton;
}

void Case::mouseReleaseEvent(QMouseEvent* e)
{
    if (_buttons == (Qt::LeftButton | Qt::RightButton))
        emit PasDeClick();
    else if (_buttons == Qt::LeftButton)
        emit ClickNormal();
    else if (_buttons == Qt::RightButton)
        emit ClickDroit();
}

void Case::mouseMoveEvent(QMouseEvent* e)
{
    if (e->buttons() == (Qt::LeftButton | Qt::RightButton))
    {
        if (!this->rect().contains(this->mapFromGlobal(QCursor::pos())))
            emit unPreview();
    }
}

QSize Case::sizeHint() const
{
    return Dimension;
}
/*!
    *  \brief Configuration de la Machine des Etats d'une case
    *
    *  Configuration de la machine qui suit les Etats et les transitions possible sur
    * une Case

    */

void Case::ConfigGestionEtatCase()
{
    ModeNormal = new QState;
    ModeAppercu = new QState;
    AppercuDesVoisins = new QState;
    ModeDrapeauPointe = new QState;
    ModeDecouvert = new QState;
    ModeVoisinsDecouverts = new QState;
    ModeInactive = new QFinalState;
// Gestion des transtions
    // quitter Etat normal de la case aux evenements
    ModeNormal->addTransition(this, &Case::ClickNormal, ModeDecouvert);
    ModeNormal->addTransition(this, &Case::ClickDroit, ModeDrapeauPointe);
    ModeNormal->addTransition(this, &Case::reveal, ModeDecouvert);
    ModeNormal->addTransition(this, &Case::preview, ModeAppercu);
    ModeNormal->addTransition(this, &Case::Inactivation, ModeInactive);

    ModeAppercu->addTransition(this, &Case::reveal, ModeDecouvert);
    ModeAppercu->addTransition(this, &Case::unPreview, ModeNormal);
    ModeAppercu->addTransition(this, &Case::Inactivation, ModeInactive);

    ModeDrapeauPointe->addTransition(this, &Case::ClickDroit, ModeNormal);

    ModeDecouvert->addTransition(this, &Case::ClickGaucheEtDroit, AppercuDesVoisins);

    AppercuDesVoisins->addTransition(this, &Case::PasDeClick, ModeVoisinsDecouverts);
    AppercuDesVoisins->addTransition(this, &Case::unPreview, ModeDecouvert);

    ModeVoisinsDecouverts->addTransition(this, &Case::reveal, ModeDecouvert);

    connect(ModeNormal, &QState::entered, [this]()
    {
        //this->setIcon(getIcon(IconeCase::Vide));
        this->setStyleSheet(NormalStyle);


    });

    connect(ModeAppercu, &QState::entered, [this]()
    {
        this->setStyleSheet(CaseCliqueStyle);
    });

    connect(AppercuDesVoisins, &QState::entered, [this]()
    {
        for (auto neighbor : _voisins)
            neighbor->preview();
    });

    connect(ModeVoisinsDecouverts, &QState::entered, [this]()
    {
        if (_NbDrapeauAutour == _NbMinesAutour && _NbMinesAutour)
            revealNeighbors();
        else
            unPreviewNeighbors();
        emit reveal();
    });

    connect(ModeDecouvert, &QState::entered, [this]()
    {
        unPreviewNeighbors();
        this->setChecked(true);
        if (!EstMine())
        {
            Afficher();

            if (!AdesMinesAutour())
                revealNeighbors();

            emit revealed();
        }
        else
        {
            emit Explosion();
            this->setStyleSheet(CaseCliqueStyle);
            QPushButton::setText("X");
           // setIcon(getIcon(IconeCase::Mine));
        }
    });

    connect(ModeDrapeauPointe, &QState::entered, [this]()
    {
        QPushButton::setStyleSheet(NormalStyle.arg("blue"));
         QPushButton::setText("?");
        for (auto neighbor : _voisins)
            neighbor->incrementNbDrapeauAutour();
        emit PointDrapeau(_isMine);

    });

    connect(ModeDrapeauPointe, &QState::exited, [this]()
    {
         QPushButton::setText("");
        for (auto neighbor : _voisins)
            neighbor->decrementNbDrapeauAutour();
        emit SuppressionDrappeau(_isMine);
    });

    connect(ModeInactive, &QState::entered, [this]{});

    _machine.addState(ModeNormal);
    _machine.addState(ModeAppercu);
    _machine.addState(AppercuDesVoisins);
    _machine.addState(ModeDrapeauPointe);
    _machine.addState(ModeDecouvert);
    _machine.addState(ModeVoisinsDecouverts);
    _machine.addState(ModeInactive);

    _machine.setInitialState(ModeNormal);
    _machine.start();
}

/*!
    *  \brief Gesstion de l'affichage des Indices
    *
    *  Elle permet d'afficher le nombre de Mines autour avec des couleurs différentes
    */
void Case::Afficher()
{
    QString color;
    switch (_NbMinesAutour)
    {
    case 1:
        color = "blue";
        break;
    case 2:
        color = "green";
        break;
    case 3:
        color = "red";
        break;
    case 4:
        color = "midnightblue";
        break;
    case 5:
        color = "maroon";
        break;
    case 6:
        color = "darkcyan";
        break;
    case 7:
        color = "black";
        break;
    case 8:
        color = "grey";
        break;
    default:
           break;
    }
    QPushButton::setStyleSheet(NumeroStyle.arg(color));
    if(_NbMinesAutour)
        QPushButton::setText(QString::number(_NbMinesAutour));
}

/*!
    *  \brief Corrige les Affichages à la fin de la partie
    */

void Case:: AfficheResultat(){
    if(EstMine()){
        QPushButton::setStyleSheet(NumeroStyle.arg("red"));
        QPushButton::setText("X");
    }
    if(EstMine() && AuneMarque()){
        QPushButton::setStyleSheet(NumeroStyle.arg("green"));
        QPushButton::setText("X");
    }
}
