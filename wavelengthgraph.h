/*MIT License

Copyright (c) 2021 Starstone Software Systems, Inc.
Copyright (c) 2021 Richard S. Wright Jr.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE

This class draws the wavelength graph.
*/

#ifndef WAVELENGTHGRAPH_H
#define WAVELENGTHGRAPH_H

#include <QWidget>
#include <QPaintEvent>

class WavelengthGraph : public QWidget
{
    Q_OBJECT
public:
    explicit WavelengthGraph(QWidget *parent);

    inline void SetDesignWavelength(float fCenter)     { fDesignWavelength = fCenter; }
    inline void SetCurrentWavelength(float fCurrent)   { fCurrentWavelength = fCurrent; }
    inline void SetCurrentWingshift(float fShift)      { fWingshift = fShift; }
    inline void SetOnBand(bool bStatus)                { bOnBand = bStatus; }
    inline void SetTargetWavelength(float fTarget)     { fTargetWavelength = fTarget; }

protected:
    float   fDesignWavelength;      // Center wavelength as designed
    float   fCurrentWavelength;     // Current wavelength
    float   fWingshift;             // Current wingshift
    float   fTargetWavelength;      // Current target
    bool    bOnBand;                // Are you on band?

    // Angstrom Symbol
    const unsigned char angstromEncode[4] = { 0xe2, 0x84, 0xab, 0x0 };
    const QString angstromSymbol = QString().fromUtf8((const char *)angstromEncode);

    virtual void	paintEvent(QPaintEvent *event);

signals:

};

#endif // WAVELENGTHGRAPH_H
