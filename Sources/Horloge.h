#pragma once
#include <QLCDNumber>
#ifndef HORLOGE_H
#define HORLOGE_H
class Horloge: public QLCDNumber
{
public:
    Horloge(QWidget* parent = nullptr);

    void incrementTime();
    int time() const;
    virtual QSize sizeHint() const override;

private:

    int m_seconds;
};
#endif // HORLOGE_H
