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


#include "wavelengthgraph.h"
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QFont>
#include <QFontMetrics>


WavelengthGraph::WavelengthGraph(QWidget *parent) : QWidget(parent)
{

}


void WavelengthGraph::paintEvent(QPaintEvent *event)
{
    event->accept();

    QBrush redBrush(QColor(198, 32,32, 255));
    QBrush greenBrush(QColor(32, 198, 32, 255));
    QBrush blueBrush(QColor(32, 32, 198, 255));

    QPainter painter(this);

    painter.setPen(QPen(QColor(198,198,198,255)));
    
    QFont fontGraph("Helvetica", 16);
    QFont fontGraphBold("Helvetica", 18, QFont::Black);
    
    painter.setFont(fontGraph);
    
    if(bOnBand)
        painter.setBrush(greenBrush);
    else
        {
        if(fTargetWavelength > fCurrentWavelength) // Backwards physics, but matches buttons
                                                   // on the Quantum
            painter.setBrush(redBrush);
        else
            painter.setBrush(blueBrush);
        }

    QRect rect = geometry();
    painter.drawRect(rect);

    int nWidth = width();
    int nHeight = height();
    int nMargins = 10;
    int nDivisions = (nWidth - (nMargins * 2)) / 20;

    // Draw tick marks
//    float fTarget = (onBand) ? fTargetWavelength : fCurrentWavelength;
    int nTickSpace = int((fCurrentWavelength - fDesignWavelength) * -10.0) * nDivisions;
    float fStart = fDesignWavelength - 1.0f;
    for(int i= nMargins+5; i < nWidth; i+= nDivisions) {
        painter.drawLine(i + nTickSpace, 10, i + nTickSpace, 30);

        QTransform t;
        t.translate(i + nTickSpace, 25);
        t.rotate(90.0f);
        painter.save();
        painter.setTransform(t);
        QString out = QString::asprintf("%.1f", fStart);
        out += angstromSymbol;
        
        if(fabs(fStart - fTargetWavelength) < 0.01) {
            painter.setPen(QPen(QColor(255,255,255,255)));
            painter.setFont(fontGraphBold);            
            painter.drawText(-10, -2, out);
            painter.setFont(fontGraph);
            painter.setPen(QPen(QColor(198,198,198,255)));

            }
        else 
            painter.drawText(-5, -2, out);
        
        fStart += 0.1f;
        painter.restore();
    }

    painter.setPen(QPen(QColor(255,255,255,255)));
    painter.drawLine(nWidth / 2, 0, nWidth/2, nHeight-25);
    painter.drawLine((nWidth / 2)-1, 0, (nWidth/2)-1, nHeight-25);
    painter.drawLine((nWidth / 2)-2, 0, (nWidth/2)-2, nHeight-25);

}
