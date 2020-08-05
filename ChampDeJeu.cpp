#include "ChampDejeu.h"
#include <algorithm>
#include <random>
#include <QGridLayout>
#include <QSpacerItem>
#include <QSet>
#include <QTimer>

ChampDejeu::ChampDejeu(unsigned int nbLignes, unsigned int nbCols, unsigned int numMines, QWidget* parent /*= nullptr*/)
    : _nbLignes(nbLignes)
    , _nbCols(nbCols)
    , _numMines(numMines)
    , QFrame(parent)
    , explosionTimer(new QTimer(this))
{
    _miseEnPlace();
    CreationDesCases();
    AjoutVoisins();

   connect(this, &ChampDejeu::Victoire, [this]()
   {
       explosionTimer->setProperty("Victoire", true);
   });
   connect(this, &ChampDejeu::defeat, [this]()
   {
       explosionTimer->setProperty("Victoire", false);
   }
    );

    connect(explosionTimer, &QTimer::timeout, [this]()
    {
        if (_mines.isEmpty())
        {
            explosionTimer->stop();
            return;
        }

        Case* mine = _mines.values().front();
        _mines.remove(mine);

        if (explosionTimer->property("Victoire").toBool()){
            //mine->setIcon(mine->getIcon(IconeCase::Bon));
            mine->AfficheResultat();
        }

     //  else
     //  {
     //      if (!_BonsDrapeaux.contains(mine))
     //          //mine->setIcon(mine->getIcon(IconeCase::Explosion));
     //  }
    });
}

void ChampDejeu::_miseEnPlace()
{
    this->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    auto layout = new QGridLayout;

    layout->setSpacing(0);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    this->setLayout(layout);
}

void ChampDejeu::CreationDesCases()
{
    for (unsigned int r = 0; r < _nbLignes; ++r)
    {
        _Cases += QList<Case*>{};
        for (unsigned int c = 0; c < _nbCols; ++c)
        {
            // Ajout d'une nouvelle case
            _Cases[r] += new Case({ r, c }, this);
            static_cast<QGridLayout*>(this->layout())->addWidget(_Cases[r][c], r, c);
            connect(_Cases[r][c], &Case::firstClick, this, &ChampDejeu::placeMines);
            connect(_Cases[r][c], &Case::flagged, [this, Case = _Cases[r][c]](bool isMine)
            {
                if (isMine)
                    _BonsDrapeaux.insert(Case);
                else
                    _MauvaisDrapeaux.insert(Case);
                _verifVictoire();
            });
            connect(_Cases[r][c], &Case::unFlagged, [this, Case = _Cases[r][c]](bool isMine)
            {
                if (isMine)
                    _BonsDrapeaux.remove(Case);
                else
                    _MauvaisDrapeaux.remove(Case);
                _verifVictoire();
            });
            connect(_Cases[r][c], &Case::revealed, [this, Case = _Cases[r][c]]()
            {
                _CaseDecouverts.insert(Case);
                _verifVictoire();
            });
            connect(_Cases[r][c], &Case::detonated, this, &ChampDejeu::defeatAnimation);
            connect(this, &ChampDejeu::defeat, _Cases[r][c], &Case::disable);
            connect(this, &ChampDejeu::Victoire, _Cases[r][c], &Case::disable);
        }
    }
    _Cases[0][0]->setDown(true);
}

void ChampDejeu::AjoutVoisins()
{
    for (unsigned int r = 0; r < _nbLignes; ++r)
    {
        for (unsigned int c = 0; c < _nbCols; ++c)
        {
            // add a new Case to the row
            auto* Case = _Cases[r][c];
            auto lastRow = _nbLignes - 1;
            auto lastCol = _nbCols - 1;

            if (r && c)						Case->AjoutVoisin(_Cases[r - 1][c - 1]);				// haut gauche
            if (r)							Case->AjoutVoisin(_Cases[r - 1][c]);					// haut
            if (r && c < lastCol)			Case->AjoutVoisin(_Cases[r - 1][c + 1]);				// haut droit
            if (c < lastCol)				Case->AjoutVoisin(_Cases[r][c + 1]);					// droit
            if (r < lastRow && c < lastCol)	Case->AjoutVoisin(_Cases[r + 1][c + 1]);				// bas droite
            if (r < lastRow)				Case->AjoutVoisin(_Cases[r + 1][c]);					// bas
            if (r < lastRow && c)			Case->AjoutVoisin(_Cases[r + 1][c - 1]);				// bas gauche
            if (c)							Case->AjoutVoisin(_Cases[r][c - 1]);					// gauche
        }
    }
}

void ChampDejeu::_verifVictoire()
{

    if (!_etatJeux==EtatJeu::Succes)
    {
        emit flagCountChanged(_BonsDrapeaux.size() + _MauvaisDrapeaux.size());
        if ((_CaseDecouverts.size() ==(int) (_nbCols * _nbLignes) -(int) _numMines) && _MauvaisDrapeaux.isEmpty())
        {
            emit Victoire();
            _etatJeux = EtatJeu::Succes;
            QTimer::singleShot(0, explosionTimer, [this]()
            {
                explosionTimer->start(25);
            });
        }
    }
}

void ChampDejeu::defeatAnimation()
{
    Case* sender = dynamic_cast<Case*>(this->sender());
    QTimer::singleShot(350, this, [sender]()
    {
        //sender->setIcon(sender->getIcon(IconeCase::Explosion));
    });
    QTimer::singleShot(500, this, [this]()
    {
        for (auto wrong : _MauvaisDrapeaux)
        {
            //wrong->setIcon(wrong->getIcon(IconeCase::Erreur));
           wrong->AfficheResultat();
        }
        for (auto mine : _mines)
        {
            disconnect(mine, &Case::detonated, this, &ChampDejeu::defeatAnimation);
            if (!mine->AuneMarque()){
                mine->reveal();

            }
            mine->AfficheResultat();
        }
        emit defeat();
    });

    QTimer::singleShot(1000, explosionTimer, [this]()
    {
        explosionTimer->start(25);
    });
}

void ChampDejeu::placeMines(Case* firstClicked)
{

    QList<Case*> Cases;
    QSet<Case*> doneUse;
    doneUse += firstClicked;
    doneUse += QSet<Case*>(firstClicked->neighbors().begin(),firstClicked->neighbors().end());
    for (unsigned int r = 0; r < _nbLignes; ++r)
    {
        _Cases += QList<Case*>{};
        for (unsigned int c = 0; c < _nbCols; ++c)
        {
            // Ajout au rang à moins que ce soit la première mine sur la quelle on a cliqué
            if (auto Case = _Cases[r][c]; !doneUse.contains(Case))
                Cases += Case;
        }
    }

    // Melange
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(Cases.begin(), Cases.end(), g);

    for (unsigned int i = 0; i < _numMines; ++i)
    {
        Cases[i]->placerMine(true);
        _mines.insert(Cases[i]);
    }

    emit initialized();
}
