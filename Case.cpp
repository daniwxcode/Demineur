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
    createStateMachine();
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setCheckable(true);
    setMouseTracking(true);
}

Case::~Case()
{
    _firstClick = false;
    delete unrevealedState;
    delete previewState;
    delete flaggedState;
    delete revealedState;
    delete disabledState;
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

bool Case::EstMine() const
{
    return _isMine;
}

bool Case::AdesMinesAutour() const
{
    return _NbMinesAutour;
}

unsigned int Case::NbMinesAutour() const
{
    return _NbMinesAutour;
}

void Case::incrementNbMinesAutour()
{
    ++_NbMinesAutour;
}

bool Case::AuneMarque() const
{
    return _machine.configuration().contains(flaggedState);
}

bool Case::EstOuvert() const
{
    return _machine.configuration().contains(revealedState);
}

bool Case::isUnrevealed() const
{
    return _machine.configuration().contains(unrevealedState);
}

unsigned int Case::NbDrapeauAutour() const
{
    return _NbDrapeauAutour;
}

void Case::incrementNbDrapeauAutour()
{
    ++_NbDrapeauAutour;
}

void Case::decrementNbDrapeauAutour()
{
    --_NbDrapeauAutour;
}

QList<Case*>& Case::neighbors()
{
    return _voisins;
}

void Case::mousePressEvent(QMouseEvent* e)
{
    if (!_firstClick)
    {
        _firstClick = true;
        emit firstClick(this);
    }

    _bothClicked = false;

    if (e->buttons() == (Qt::LeftButton | Qt::RightButton))
    {
        emit bothClicked();
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
        emit unClicked();
    else if (_buttons == Qt::LeftButton)
        emit leftClicked();
    else if (_buttons == Qt::RightButton)
        emit rightClicked();
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

void Case::createStateMachine()
{
    unrevealedState = new QState;
    previewState = new QState;
    previewNeighborsState = new QState;
    flaggedState = new QState;
    revealedState = new QState;
    revealNeighborsState = new QState;
    disabledState = new QFinalState;

    unrevealedState->addTransition(this, &Case::leftClicked, revealedState);
    unrevealedState->addTransition(this, &Case::rightClicked, flaggedState);
    unrevealedState->addTransition(this, &Case::reveal, revealedState);
    unrevealedState->addTransition(this, &Case::preview, previewState);
    unrevealedState->addTransition(this, &Case::disable, disabledState);

    previewState->addTransition(this, &Case::reveal, revealedState);
    previewState->addTransition(this, &Case::unPreview, unrevealedState);
    previewState->addTransition(this, &Case::disable, disabledState);

    flaggedState->addTransition(this, &Case::rightClicked, unrevealedState);

    revealedState->addTransition(this, &Case::bothClicked, previewNeighborsState);

    previewNeighborsState->addTransition(this, &Case::unClicked, revealNeighborsState);
    previewNeighborsState->addTransition(this, &Case::unPreview, revealedState);

    revealNeighborsState->addTransition(this, &Case::reveal, revealedState);

    connect(unrevealedState, &QState::entered, [this]()
    {
        //this->setIcon(getIcon(IconeCase::Vide));
        this->setStyleSheet(NormalStyle);


    });

    connect(previewState, &QState::entered, [this]()
    {
        this->setStyleSheet(CaseCliqueStyle);
    });

    connect(previewNeighborsState, &QState::entered, [this]()
    {
        for (auto neighbor : _voisins)
            neighbor->preview();
    });

    connect(revealNeighborsState, &QState::entered, [this]()
    {
        if (_NbDrapeauAutour == _NbMinesAutour && _NbMinesAutour)
            revealNeighbors();
        else
            unPreviewNeighbors();
        emit reveal();
    });

    connect(revealedState, &QState::entered, [this]()
    {
        unPreviewNeighbors();
       // this->setIcon(getIcon(IconeCase::Vide));
        this->setChecked(true);
        if (!EstMine())
        {
           // setText();
            if (!AdesMinesAutour())
                revealNeighbors();
            emit revealed();
        }
        else
        {
            emit detonated();
            this->setStyleSheet(CaseCliqueStyle);
            QPushButton::setText("X");
           // setIcon(getIcon(IconeCase::Mine));
        }
    });

    connect(flaggedState, &QState::entered, [this]()
    {
        //this->setIcon(getIcon(IconeCase::Vide));
        this->setText();
        for (auto neighbor : _voisins)
            neighbor->incrementNbDrapeauAutour();
        emit flagged(_isMine);

    });

    connect(flaggedState, &QState::exited, [this]()
    {
        for (auto neighbor : _voisins){
            this->setText();
            neighbor->decrementNbDrapeauAutour();}
        emit unFlagged(_isMine);
    });

    connect(disabledState, &QState::entered, [this]{});

    _machine.addState(unrevealedState);
    _machine.addState(previewState);
    _machine.addState(previewNeighborsState);
    _machine.addState(flaggedState);
    _machine.addState(revealedState);
    _machine.addState(revealNeighborsState);
    _machine.addState(disabledState);

    _machine.setInitialState(unrevealedState);
    _machine.start();
}

void Case::setText()
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

    if(AuneMarque()){
        QPushButton::setStyleSheet(CaseCliqueStyle.arg("blue"));
        QPushButton::setText("?");
    }else{
        QPushButton::setText("");
    }
    if(!EstMine() && !AuneMarque()){
    QPushButton::setStyleSheet(NormalStyle.arg(color));
    if(_NbMinesAutour)
        QPushButton::setText(QString::number(_NbMinesAutour));
}


}

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
