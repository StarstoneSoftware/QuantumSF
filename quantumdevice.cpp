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

#include <QTimer>

#include "quantumdevice.h"

const char* qCmdGetInfo = "GI\n";               // Gather common info
const char* qCmdGetSerialNumber = "GS\n";       // Get serial number
const char* qCmdGetRuntime = "GY\n";            // Boot count and runtime
const char* qCmdGetBodyStyle = "GA\n";          // Body Style
const char* qCmdGetBandwidth = "GB\n";          // Get Bandwidth
const char* qCmdGetModelName = "GN\n";          // Get Modelname
const char* qCmdGetDesignWavelength = "GX\n";   // Get Design wavelength


/////////////////////////////////////////////////////////////////////////////////////////
// This is all called within the calling thread. Just setup variables here, but do
// not do any real work.
QuantumDevice::QuantumDevice(QObject *parent, QSerialPortInfo serialPortInformation) : QThread(parent)
{
    serialPortInfo = serialPortInformation;
}

QuantumDevice::~QuantumDevice(void)
{

}


////////////////////////////////////////////////////////////////////////////////////////////
// The lowest level read command. Just waiting for ready read is too agressive, so
// a small delay is added to give the device a chance to respond. All returned strings
bool QuantumDevice::sendCommand(const char* szCommand)
    {
    // Clear buffer - just so nothing from past responses can come through
    memset(szReturnBuffer, 0, MAX_COMM_BUFFER_SIZE);

    // This actually never should have been called
    Q_ASSERT(pSerialPort != nullptr);
    if(!pSerialPort)
        return false;

    ///////////////////////////////////////////////////////////
    /// There is a slight chance some commands can be dropped
    /// Allow up to two retries
    int nTries = 0;
    while(nTries++ < 3) {
        pSerialPort->write(szCommand);
        if(!pSerialPort->waitForBytesWritten())
            return false; // This is an actual error... no retries

        if(pSerialPort->waitForReadyRead(QUANTUM_TIMEOUT))
            break;
        else
            continue;
    }

    if(nTries > 3) return false; // Gave up..

    int iIndex = 0;
    while(!pSerialPort->atEnd() && iIndex < MAX_COMM_BUFFER_SIZE) {
        pSerialPort->getChar(szReturnBuffer+iIndex);
        iIndex++;
        pSerialPort->waitForReadyRead(100);
        }
    szReturnBuffer[iIndex] = 0x0;

    return true;
    }


/////////////////////////////////////////////////////////////////////////////////////////
// Convert a string to an integer. For firmware versions prior to 1.25, this is decimal
// After that it is hexidecmial.
int QuantumDevice::toInteger(const char* szStringField)
{
    // It's just ascii decimal
    if(bOldFirmware)
        return atoi(szStringField);

    // It's in hex
    int returnValue;
    sscanf(szStringField, "%x", &returnValue);
    return returnValue;
}

////////////////////////////////////////////////////////////////////////////////////////
// Convert a string to a decimal. Hex value is 2's complement
int QuantumDevice::toSignedInteger(const char* szStringField)
{
    // It's just ascii decimal
    if(bOldFirmware)
        return atoi(szStringField);

    // It's in hex
    unsigned int returnValue;
    sscanf(szStringField, "%x", &returnValue);
    signed char twos = returnValue;
    return ~(-twos) + 1;
}


/////////////////////////////////////////////////////////////////////////////////////////
/// We are in the thread, but the event loop is not running yet. This is where we
/// attempt to connect to the device. If we can't connect it is game over. Once we are
/// connected, everything is event driven
/// There is a big assumption here... I'm firing off signals before the event loop has
/// started. First run/testing this works, but different OS's and future versions of
/// Qt may not? Evaluate, but if in the futre this stops working, a past version is
/// is saying "I told you so"...
void QuantumDevice::run()
{
    setTerminationEnabled(true);

    //Settings are 9600 baud, 8 bits, no parity, 1 stop bit, with no handshaking.
    pSerialPort = new QSerialPort(serialPortInfo, nullptr);
    pSerialPort->setBaudRate(9600);
    pSerialPort->setDataBits(QSerialPort::Data8);
    pSerialPort->setParity(QSerialPort::NoParity);
    pSerialPort->setStopBits(QSerialPort::OneStop);

    // Basic serial port opening, doesn't prove anything yet..
    bool bReady = false;
    if(pSerialPort->open(QIODevice::ReadWrite))
        if(getStaticInfoFromDevice())
            bReady = true;

    if(bReady) {
        emit connectedToQuantum(this);
        updateStatus();
        }
    else
        emit couldNotOpen(this);

    // This begins the default behavior, which starts an event loop for this thread.
    QThread::run();

    // The event loop has terminated. Do any remaining cleanup.
    pSerialPort->close();
    delete pSerialPort;

}

///////////////////////////////////////////////////////////////////////////////////////////
// Get static information from device that does not have to be thread safe. This data is
// read during initialization before the device interface should be used.
bool QuantumDevice::getStaticInfoFromDevice(void)
    {
    if(!sendCommand(qCmdGetInfo))
        return false;

    if(strlen(szReturnBuffer) < 74)
        return false;

    // Get the initial status info
    parseStatusInfo();

    // The firmware version and serial number are retrieved here
    const char *szFW = strtok(szReturnBuffer, " ");
    qsFirmwareVersion = QString::fromUtf8(szFW);
    bOldFirmware = atof(szFW+1) < 1.26f;

    // Serial number
    if(!sendCommand(qCmdGetSerialNumber))
        return false;

    qsSerialNumber = QString::fromUtf8(szReturnBuffer);

    // Body Style
    if(!sendCommand(qCmdGetBodyStyle))
        return false;

    nBodyStyle = toInteger(szReturnBuffer);

    // Bandwidth String
    if(!sendCommand(qCmdGetDesignWavelength))
        return false;

    qsDesignWavelength = QString::asprintf("%0.1f", float(toInteger(szReturnBuffer))*0.1f);

    // Model String
    if(!sendCommand(qCmdGetModelName))
        return false;

    qsModelString = QString::fromUtf8(szReturnBuffer);

    // Design wavelength
    if(!sendCommand(qCmdGetBandwidth))
        return false;

     qsBandwidthString = QString::fromUtf8(szReturnBuffer);

     // Number of boots and run time. Does not seem to work. Always times out.
     //sendCommand("GY\n");

    return true;
    }


///////////////////////////////////////////////////////////////////////////////////////////
/// Parse the status string
/// example: v2.00 00 01 0001005C 00 0026 039D 0000289F 00000488 00000000 00002EE0 00ED
/// Firmware Version
/// On Error code
/// On band indicator
/// Wavelength
/// Wingshift
/// Current first heater raw PMW
/// First heater PMW Limit
/// Temperature of first heater
/// Temperature of the second heater (optional)
/// Current Voltage
/// Calibration pot position
/// 2nd heater temperature
/// 2nd heater pmw
///
void QuantumDevice::parseStatusInfo()
{
    mutexBlocker.lock();

    // NO... use the GA command to get the body style
    // Count the number of spaces so we now if we have two heaters or one
    int nSpaces = 0;
    int length = strlen(szReturnBuffer);
    for(int i = 0; i < length; i++)
        if(szReturnBuffer[i] == 32)
            nSpaces++;

    deviceStatus.bDualHeaters = (nSpaces > 11);

    // First field is firmware, we already have that
    char *nextField = strtok(szReturnBuffer, " ");
    (void)nextField;

    // Next field is the error code
    nextField = strtok(NULL, " ");
    deviceStatus.nErrorCode = toInteger(nextField);

    // Next field is on band indication
    nextField = strtok(NULL, " ");
    deviceStatus.bOnBand = (toInteger(nextField) == 1);

    // Current wavelength
    nextField = strtok(NULL, " ");
    int currWavelength = toInteger(nextField);
    deviceStatus.centerWavelength = float(currWavelength) * 0.1f;

    // Wingshift
    nextField = strtok(NULL, " ");
    int currWingShift = toSignedInteger(nextField);
    deviceStatus.wingShift = float(currWingShift) * 0.1f;

    // First heaters raw PMW
    nextField = strtok(NULL, " ");
    int pmw = toInteger(nextField);

    // PMW Limit
    nextField = strtok(NULL, " ");
    deviceStatus.heater1PMWLimit = toInteger(nextField);
    deviceStatus.heater1PMW = (float(pmw) * 100.0f) / float(deviceStatus.heater1PMWLimit);

    // Temperature of first heater
    nextField = strtok(NULL, " ");
    int temp = toInteger(nextField);
    deviceStatus.heater1Temprature = float(temp) * 0.01;

    // Temperature of the second heater
    if(deviceStatus.bDualHeaters) {
        nextField = strtok(NULL, " ");
        temp = toInteger(nextField);
        deviceStatus.heater2Temperature = float(temp) * 0.01;
       }

    // Input voltage
    nextField = strtok(NULL, " ");
    deviceStatus.inputVoltage = float(toInteger(nextField)) * 0.01;

    // Calibration Pot Position
    nextField = strtok(NULL, " ");
    deviceStatus.calibrationPotPos = toInteger(nextField);

    // Second heater duty cycle (if present)
    if(deviceStatus.bDualHeaters) {
        // First heaters raw PMW
        nextField = strtok(NULL, " ");
        int pmw = toInteger(nextField);

        // PMW Limit
        nextField = strtok(NULL, " ");
        deviceStatus.heater2PMWLimit = toInteger(nextField);
        deviceStatus.heater2PMW = (float(pmw) * 100.0f) / float(deviceStatus.heater2PMWLimit);
        }

    mutexBlocker.unlock();
}

void QuantumDevice::parseBootCount()
{
    mutexBlocker.lock();

    // Boot count is first
    char *nextField = strtok(szReturnBuffer, " ");
    deviceStatus.nBootCount = toInteger(nextField);

    // Next runtime in minutes
    nextField = strtok(NULL, " ");
    deviceStatus.nRunMinutes = toInteger(nextField);

    mutexBlocker.unlock();
}


////////////////////////////////////////////////////////////////////////////////////////////
/// This is the polling function. It calls a one shot timer each cycle to prevent flooding
/// the message queue if it get's backed up.
void QuantumDevice::updateStatus(void)
{
    // Are there any commands in the queue to be run?
    if(!commandQueue.isEmpty()) {
        // Process the next command
        mutexBlocker.lock();
        QString cmd = commandQueue.dequeue();

        // Parse the command results
        mutexBlocker.unlock();

        // Send the command
        sendCommand(cmd.toUtf8());

        // Check return based on command
        // E OK
        printf("%s\n", szReturnBuffer);

        }

    // Every cycle, we want the GI (Get Info) to run which contains a lot of useful data
    if(sendCommand(qCmdGetInfo))
        parseStatusInfo();
    else
        emit fatalError(-1);

    emit statusUpdated();

    // Do this again in a second...
    QTimer::singleShot(1000, this, &QuantumDevice::updateStatus);
}
