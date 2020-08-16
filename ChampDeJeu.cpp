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
   connect(this, &ChampDejeu::Perdu, [this]()
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
        mine->AfficheResultat();

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
            connect(_Cases[r][c], &Case::PremierClick, this, &ChampDejeu::EpandageDesMines);
            connect(_Cases[r][c], &Case::PointDrapeau, [this, Case = _Cases[r][c]](bool isMine)
            {
                if (isMine)
                    _BonsDrapeaux.insert(Case);
                else
                    _MauvaisDrapeaux.insert(Case);
                _verifVictoire();
            });
            connect(_Cases[r][c], &Case::SuppressionDrappeau, [this, Case = _Cases[r][c]](bool isMine)
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
            connect(_Cases[r][c], &Case::Explosion, this, &ChampDejeu::Findejeu);
            connect(this, &ChampDejeu::Perdu, _Cases[r][c], &Case::Inactivation);
            connect(this, &ChampDejeu::Victoire, _Cases[r][c], &Case::Inactivation);
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
    //envoie le signal pour notifier que le nombre de Drapeaux a changé
    emit ChangementNbDrapeau(_BonsDrapeaux.size() + _MauvaisDrapeaux.size());
    if (_etatJeux!=EtatJeu::Succes){
    if (((_CaseDecouverts.size() ==(int) (_nbCols * _nbLignes) -(int) _numMines) && _MauvaisDrapeaux.isEmpty())
                || (_numMines == _BonsDrapeaux.size()))
        {
            emit Victoire();
            _etatJeux = EtatJeu::Succes;
            QTimer::singleShot(0, explosionTimer, [this]()
            {
                explosionTimer->start(25);
            });
    }
}}

void ChampDejeu::Findejeu()
{
    QTimer::singleShot(500, this, [this]()
    {
        for (auto wrong : _MauvaisDrapeaux)
        {
           wrong->AfficheResultat();
        }
        for (auto mine : _mines)
        {
            disconnect(mine, &Case::Explosion, this, &ChampDejeu::Findejeu);
            if (!mine->AuneMarque()){
                mine->reveal();
            }
            mine->AfficheResultat();
        }
        emit Perdu();
    });

    QTimer::singleShot(1000, explosionTimer, [this]()
    {
        explosionTimer->start(25);
    });
}

void ChampDejeu::EpandageDesMines(Case* AuPremierClick)
{

    QList<Case*> Cases;
    QSet<Case*> _caseAnePasMiner;
    _caseAnePasMiner += AuPremierClick;
    _caseAnePasMiner += QSet<Case*>(AuPremierClick->Voisinage().begin(),AuPremierClick->Voisinage().end());
    for (unsigned int r = 0; r < _nbLignes; ++r)
    {
        _Cases += QList<Case*>{};
        for (unsigned int c = 0; c < _nbCols; ++c)
        {
            // Ajout au rang à moins que ce soit la première mine sur la quelle on a cliqué
            if (auto Case = _Cases[r][c]; !_caseAnePasMiner.contains(Case))
                Cases += Case;
        }
    }

    // Minage du terrain
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(Cases.begin(), Cases.end(), g);

    for (unsigned int i = 0; i < _numMines; ++i)
    {
        Cases[i]->placerMine(true);
        _mines.insert(Cases[i]);
    }

    emit PretADemarrer();
}
