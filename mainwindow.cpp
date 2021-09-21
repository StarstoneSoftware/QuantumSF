// mainwindow.cpp
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

#include <QCloseEvent>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->statusBar()->showMessage(tr("Quantum Not Connected"));
    pSerialChooser = new SerialChooser(this);
    this->setCentralWidget(pSerialChooser);

    connect(pSerialChooser, SIGNAL(connectedToQuantum(QuantumDevice*)), this, SLOT(quantumHasConnected(QuantumDevice*)), Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(pQuantumDevice) {
        pQuantumDevice->exit();
        QDeadlineTimer deadline(500);
        if(!pQuantumDevice->wait(deadline))
            pQuantumDevice->terminate();

        delete pQuantumDevice;
        pQuantumDevice = nullptr;
        }

    event->accept();
}

//////////////////////////////////////////////////////////////////////
// We have a connetion! Get rid of the serial chooser and put up the
// main quantum gui
void MainWindow::quantumHasConnected(QuantumDevice *pDevice)
{
    // Hang on to this.
    pQuantumDevice = pDevice;

    // Serial chooser is no longer needed and in the way
    pSerialChooser->close();
    delete pSerialChooser;
    pSerialChooser = nullptr;

    // New GUI is now in charge
    pQuantumGui = new QuantumGui(this, pDevice);
    setCentralWidget(pQuantumGui);

    QApplication::restoreOverrideCursor();

    // Update the status bar
    QString status = " 0";
    status += pQuantumDevice->getBandwidthString();
    status += pQuantumGui->angstromSymbol;
    status += " ";
    status += pQuantumDevice->getModelString();
    status += " ";
    status += pQuantumDevice->getSerialNumber()[1];
    status += "E";
    status += "      Center wavelength: ";
    status += pQuantumDevice->getWavelengthString();
    status += pQuantumGui->angstromSymbol;
    status += "      SN:";
    status += pQuantumDevice->getSerialNumber();
    status += "      Firmware: ";
    status += pQuantumDevice->getFirmwareVersion();

    ui->statusbar->showMessage(status);
    pQuantumGui->updateStatusDisplay();
}


