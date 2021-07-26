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
#include "quantumgui.h"
#include "ui_quantumgui.h"


QuantumGui::QuantumGui(QWidget *parent, QuantumDevice *pDevice) :
    QDialog(parent),
    ui(new Ui::QuantumGui)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Widget);

    // This displays the wavelength adjustments
    pWavelengthGraph = new WavelengthGraph(ui->graphFrame);
    QSize size = ui->graphFrame->frameSize();
    pWavelengthGraph->setGeometry(0, 0, size.width(), size.height());

    // This is how we talk to the quantum.
    pQuantumDevice = pDevice;

    connect(pQuantumDevice, SIGNAL(statusUpdated()), this, SLOT(updateStatusDisplay()), Qt::QueuedConnection);
    connect(ui->toolButtonUp, SIGNAL(pressed()), this, SLOT(pressedUp()));
    connect(ui->toolButtonDown, SIGNAL(pressed()), this, SLOT(pressedDown()));
    connect(ui->toolButtonCenter, SIGNAL(pressed()), this, SLOT(pressedCenter()));
}

QuantumGui::~QuantumGui()
{
    delete ui;
}

void QuantumGui::resizeEvent(QResizeEvent *event)
    {
    //QSize size = ui->graphFrame->frameSize();
    //pWavelengthGraph->setGeometry(0, 0, size.width(), size.height());

    event->accept();
    }

////////////////////////////////////////////////////////////////////
/// Increase wingshift by .1 angstroms
void QuantumGui::pressedUp(void)
{
    QuantumStatus deviceStatus;
    pQuantumDevice->getDeviceStatus(&deviceStatus);

    // If wingshift is already maxed out, ignore
    if(deviceStatus.wingShift >= 1.0)
        return;

    // Okay, increment it
    int wingshift = int(deviceStatus.wingShift * 10.0f);
    wingshift += 1;

    char cCmdString[16];
    sprintf(cCmdString, "SE%d\n", wingshift);

    pQuantumDevice->addCommand(cCmdString);
}

////////////////////////////////////////////////////////////////////
/// Decrease wingshift by .1 angstroms
void QuantumGui::pressedDown(void)
{
    QuantumStatus deviceStatus;
    pQuantumDevice->getDeviceStatus(&deviceStatus);

    // If wingshift is already maxed out, ignore
    if(deviceStatus.wingShift <= -1.0)
        return;

    // Okay, increment it
    int wingshift = int(deviceStatus.wingShift * 10.0f);
    wingshift -= 1;

    char cCmdString[16];
    sprintf(cCmdString, "SE%d\n", wingshift);

    pQuantumDevice->addCommand(cCmdString);
}

////////////////////////////////////////////////////////////////////
/// Set Wingshift to zero
void QuantumGui::pressedCenter(void)
{
    pQuantumDevice->addCommand(QString("SE0\n"));
}


////////////////////////////////////////////////////////////////////
void QuantumGui::updateStatusDisplay(void)
{
    QuantumStatus deviceStatus;
    pQuantumDevice->getDeviceStatus(&deviceStatus);
    QString output;

    // Update error string
    output = "Error Status: ";
    switch (deviceStatus.nErrorCode) {
        case 0:
            output += "No Errors";
            break;
        case 1:
            output += "Supply voltage too low (less than 8V";
            break;
        case 2:
            output += "Ambient temperature too low. Cannot reach setpoint.";
            break;
        case 3:
            output += "Low power (supply voltage below 10v).";
            break;
        case 4:
            output += "High voltage (supply voltage above 30v)";
            break;
        case 5:
            output += "Ambient temperature too hot. Cannot reach setpoint.";
            break;
        case 0x0a:
            output += "Thermistor connection open (broken wire). Return to Daystar for service.";
            break;
        case 0x0b:
            output += "Thermistor connection shorted. Return to Daystar for service.";
            break;
        default:
            output += QString::asprintf("Unknown error, code %x", deviceStatus.nErrorCode);
        }

    ui->labelErrorCode->setText(output);

    // On or off band
    if(deviceStatus.bOnBand)
        output = "On Band";
    else
        output = "Not on Band";
    ui->labelBandStatus->setText(output);

    // Current wavelength
    output = QString::asprintf("Current Center Wavelength: %.1f", deviceStatus.centerWavelength);
    output += angstromSymbol;
    ui->labelWavelength->setText(output);

    // Current wingshift
    output = QString::asprintf("Current Wingshift: %0.1f", deviceStatus.wingShift);
    output += angstromSymbol;
    ui->labelWingshift->setText(output);

    // Voltage
    output = QString::asprintf("Current Voltage: %.02fV", deviceStatus.inputVoltage);
    ui->labelVoltage->setText(output);

    // Temp
    output = QString::asprintf("Heater temperature: %.1f", deviceStatus.heater1Temprature);
    output += degreeSymbol;
    output += " F ";
    ui->labelTemp->setText(output);

    // Boots
    //output = QString::asprintf("Boot Cycles: %d", deviceStatus.nBootCount);
    //ui->labelBootCount->setText(output);

    // Runtime
    //output = QString::asprintf("Run Time: %d minutes", deviceStatus.nRunMinutes);
    //ui->labelRunTime->setText(output);
    pWavelengthGraph->SetCurrentWavelength(deviceStatus.centerWavelength);
    pWavelengthGraph->SetDesignWavelength(pQuantumDevice->getWavelengthString().toFloat());
    pWavelengthGraph->SetCurrentWingshift(deviceStatus.wingShift);
    pWavelengthGraph->update();

    }



