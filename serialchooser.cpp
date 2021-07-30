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

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QMessageBox>

#include "dlgabout.h"
#include "serialchooser.h"
#include "ui_serialchooser.h"


SerialChooser::SerialChooser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialChooser)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Widget);
    ui->pushButtonFind->hide();

    connect(ui->pushButtonAbout, SIGNAL(pressed()), this, SLOT(pressedAbout()));
    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelected()));
    connect(ui->pushButtonRefresh, SIGNAL(pressed()), this, SLOT(refreshPortList()));
    connect(ui->pushButtonUseSelected, SIGNAL(pressed()), this, SLOT(attemptOneConnection()));

    refreshPortList();

    this->show();
}

SerialChooser::~SerialChooser()
{
    delete ui;
}

void SerialChooser::pressedAbout()
{
    DlgAbout aboutBox(this);
    aboutBox.exec();
}

//////////////////////////////////////////////////////////////////////////////////////
void SerialChooser::itemSelected()
    {
    ui->pushButtonUseSelected->setEnabled(true);
    }

/////////////////////////////////////////////////////////////////////////////////////
void SerialChooser::refreshPortList(void)
{
    listOfPorts.clear();
    listOfPorts = QSerialPortInfo::availablePorts();

    ui->treeWidget->clear();
    ui->treeWidget->setColumnCount(3);
    QStringList headers = { "Port Name", "Description", "Manufacturer" };
    ui->treeWidget->setHeaderLabels(headers);

    for(int i = 0; i < listOfPorts.size(); i++) {
        // On macOS, filter out tty.Bluetooth and other tty. listings.
        // CU is for callinging out, tty is for calling in. We are going
        // out...
        if(listOfPorts[i].portName().contains("tty.")) {
            listOfPorts.removeAt(i);
            i--;
            continue;
            }

        // Build the string to identify the serial port
        QStringList line;
        line << listOfPorts[i].portName();
        line << listOfPorts[i].description();
        line << listOfPorts[i].manufacturer();

        // Add to the tree widget
        QTreeWidgetItem *pItem = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), line);
        ui->treeWidget->addTopLevelItem(pItem);

        // If this port is busy, it is in use and should be disqualified. Display it anyway
        // so the end user can see it is at least detected, and it could be a device that is
        // holding onto the port that they would need to know about.
        if(listOfPorts[i].isBusy())
            pItem->setDisabled(true);
        }

    ui->treeWidget->resizeColumnToContents(0);
    ui->treeWidget->resizeColumnToContents(1);
    ui->treeWidget->resizeColumnToContents(2);
    ui->pushButtonUseSelected->setEnabled(false);
    }

///////////////////////////////////////////////////////////////////////
// Go try a connection with the selected device
void SerialChooser::attemptOneConnection(void)
{
    QTreeWidgetItem *pSelectedItem = ui->treeWidget->currentItem();
    Q_ASSERT(pSelectedItem != nullptr);
    int iItem = ui->treeWidget->indexOfTopLevelItem(pSelectedItem);
    Q_ASSERT(iItem >= 0);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QuantumDevice *pConnect = new QuantumDevice(nullptr, listOfPorts[iItem]);

    connect(pConnect, SIGNAL(connectedToQuantum(QuantumDevice*)), this, SLOT(gotConnected(QuantumDevice*)), Qt::QueuedConnection);
    connect(pConnect, SIGNAL(couldNotOpen(QuantumDevice*)), this, SLOT(failedConnection(QuantumDevice*)), Qt::QueuedConnection);
    pConnect->start();

}

///////////////////////////////////////////////////////////////////////
// We got a connection! Pass the info on to the parent window,
// and close the serial chooser.
void SerialChooser::gotConnected(QuantumDevice* pDevice)
{
    // This slot catches the signal from the device and is
    // forwarding it up to the main window
    emit connectedToQuantum(pDevice);
}

///////////////////////////////////////////////////////////////////////
// Connection failed
void SerialChooser::failedConnection(QuantumDevice* pDevice)
{
    QApplication::restoreOverrideCursor();

    // Tell the threads event loop to quit
    pDevice->quit();

    QMessageBox msgBox;
    msgBox.setText(tr("Could not connect to a Quantum on the selected serial port"));
    msgBox.exec();

    // Delete it, but make sure event loop has stopped
    pDevice->wait();
    delete pDevice;
}
