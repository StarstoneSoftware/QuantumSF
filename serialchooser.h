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

/* This dialog handles selection of a serial device.
 * It enumerates all detected serial devices, and one can be selected,
 * or all serial ports can be probed to see if a Quantum is present.
*/


#ifndef SERIALCHOOSER_H
#define SERIALCHOOSER_H

#include <QDialog>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QTreeWidgetItem>

#include "quantumdevice.h"

namespace Ui {
class SerialChooser;
}

class SerialChooser : public QDialog
{
    Q_OBJECT

public:
    explicit SerialChooser(QWidget *parent);
    ~SerialChooser();

private:
    Ui::SerialChooser   *ui;

    QList<QSerialPortInfo> listOfPorts;

public Q_SLOTS:
    void pressedAbout(void);
    void itemSelected(void);
    void refreshPortList(void);
    void attemptOneConnection(void);
    void itemDoubleClicked(QTreeWidgetItem *item, int column);

    void gotConnected(QuantumDevice* pDevice);
    void failedConnection(QuantumDevice* pDevice);

signals:
    void connectedToQuantum(QuantumDevice* pDevice);
};

#endif // SERIALCHOOSER_H
