#include "Horloge.h"

Horloge::Horloge(QWidget* parent /*= nullptr*/)
    : QLCDNumber(parent)
    , m_seconds(0)
{
    this->setDigitCount(4);
    this->display(0);
    this->setStyleSheet(".QLCDNumber { border: 2px inset gray; }");
    this->setSegmentStyle(QLCDNumber::Flat);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void Horloge::incrementTime()
{
    display(++m_seconds);
}

int Horloge::time() const
{
    return m_seconds;
}

QSize Horloge::sizeHint() const
{
    return QSize(65, 35);
}

