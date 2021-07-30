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
/* Communications with the Quantum filter is done via this thread. Communications
 * on done on a thread so the GUI is never blocked on I/O operations.
 *
*/
#ifndef QUANTUMDEVICE_H
#define QUANTUMDEVICE_H

#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QTimer>
#include <QSerialPortInfo>
#include <QSerialPort>

// Size of the return buffer
#define MAX_COMM_BUFFER_SIZE    128

// TIMEOUT value in milliseconds (initially 1 second)
#define QUANTUM_TIMEOUT 1000

/////////////////////////////////////////////////////////////
/// Device status, updated by device thread.
///
struct QuantumStatus {
    int     nErrorCode;
    float   centerWavelength;
    float   wingShift;
    float   heater1PMW;
    int     heater1PMWLimit;
    float   heater1Temprature;
    float   heater2Temperature;
    float   inputVoltage;
    int     calibrationPotPos;
    float   heater2PMW;
    int     heater2PMWLimit;

    int32_t nBootCount;
    int32_t nRunMinutes;

    bool    bOnBand;
    bool    bDualHeaters;
};


class QuantumDevice : public QThread
{
    Q_OBJECT
public:
    explicit QuantumDevice(QObject *parent, QSerialPortInfo serialPortInformation);
    ~QuantumDevice(void);

    const QSerialPortInfo& getSerialPortInfo(void) { return serialPortInfo; }

    // Not protected as the thread does not touch these after startup
    const QString& getSerialNumber(void) { return qsSerialNumber; }
    const QString& getFirmwareVersion(void) { return qsFirmwareVersion; }
    const QString& getBandwidthString(void) { return qsBandwidthString; }
    const QString& getModelString(void) { return qsModelString; }
    const QString& getWavelengthString(void) { return qsDesignWavelength; }

    // All protected items
    void getDeviceStatus(QuantumStatus* pStatus) {
        mutexBlocker.lock();
        memcpy(pStatus, &deviceStatus, sizeof(QuantumStatus));
        mutexBlocker.unlock();
    }

    // This just adds the command to be serviced next cycle.
    void addCommand(const QString qsCommand) {
        mutexBlocker.lock();
        commandQueue.enqueue(qsCommand);
        mutexBlocker.unlock();

        // Update the device as soon as possible
        QMetaObject::invokeMethod(this, "updateStatus", Qt::QueuedConnection);
    }



protected:
    QQueue<QString>     commandQueue;           // Commands queued up to send to hardware
    QSerialPort         *pSerialPort = nullptr; // No one outside this thread is to have access to this
    QSerialPortInfo     serialPortInfo;         // Details about the serial connection
    QMutex              mutexBlocker;           // Protects shared dynamic data
    char                szReturnBuffer[MAX_COMM_BUFFER_SIZE];   // Global return buffer for this instance

    // These are statically set once at thread startup, before the thread can be accessed
    // Thus, no protection is required
    QString             qsSerialNumber;
    QString             qsFirmwareVersion;
    QString             qsBandwidthString;
    QString             qsModelString;
    QString             qsDesignWavelength;
    int                 nBodyStyle = 0;
    bool                bOldFirmware = false;

    //////////////////////////////////////////////////////////////////
    // These are all shared and must be synchronized.
    QuantumStatus   deviceStatus;
    QuantumStatus   _deviceStatus;


    //////////////////////////////////////
    /// Internal only utility functions
    bool sendCommand(const char* szCommand);

    bool getStaticInfoFromDevice(void);
    int  toInteger(const char* szStringField);
    int  toSignedInteger(const char* szStringField);
    void parseStatusInfo(void);


    // Thread starts here
    virtual void run();

    // All of these need to be queued connections
public Q_SLOTS:
    void updateStatus(void);


signals:
    void connectedToQuantum(QuantumDevice* pDevice);    // Connection was successful
    void couldNotOpen(QuantumDevice* pDevice);          // No connection could be made

    void statusUpdated(void);                           // Signals new data is available
    void fatalError(int nErrorCode);                    // A communications error has occured

};

#endif // QUANTUMDEVICE_H
