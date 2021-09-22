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
/* The main window. The user is only ever presented with one window/view of the application.
 * They must select a serial device first and connect to the Quantum. There is no useful
 * functionality without an attached Quantum.
 *
 * The serial chooser is presented first, and once a connection is made, it is replaced
 * with the main interface for working with the Quantum filter.
 *
 *
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define QUANTUM_VERSION "0.01"

#include <QMainWindow>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QList>

#include "serialchooser.h"
#include "quantumgui.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow  *ui;
    SerialChooser   *pSerialChooser = nullptr;
    QuantumDevice   *pQuantumDevice = nullptr;
    QuantumGui      *pQuantumGui = nullptr;

    virtual void	closeEvent(QCloseEvent *event) override;

public Q_SLOTS:
    void quantumHasConnected(QuantumDevice *pDevice);
    void quantumHasDropped(int nErrorCode);

};
#endif // MAINWINDOW_H
