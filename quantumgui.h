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
*/
/* This is the main gui for displaying the Quantum status and sending it
 * commands.
 *
 *
*/
#ifndef QUANTUMGUI_H
#define QUANTUMGUI_H

#include <QDialog>
#include "quantumdevice.h"


namespace Ui {
class QuantumGui;
}

class QuantumGui : public QDialog
{
    Q_OBJECT

public:
    explicit QuantumGui(QWidget *parent, QuantumDevice *pDevice);
    ~QuantumGui();

    // Angstrom Symbol
    const unsigned char angstromEncode[4] = { 0xe2, 0x84, 0xab, 0x0 };
    const QString angstromSymbol = QString().fromUtf8((const char *)angstromEncode);

    // Degree Symbol
    const unsigned char degreeEncode[2] = { 0xc2, 0xb0 };
    const QString degreeSymbol = QString().fromUtf8((const char *)degreeEncode);


private:
    Ui::QuantumGui *ui;
    QuantumDevice  *pQuantumDevice = nullptr;

public Q_SLOTS:
    void updateStatusDisplay(void);
    void pressedUp(void);
    void pressedDown(void);
    void pressedCenter(void);
};

#endif // QUANTUMGUI_H
