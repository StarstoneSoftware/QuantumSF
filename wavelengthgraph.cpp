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
#include <QFontMetrics>


WavelengthGraph::WavelengthGraph(QWidget *parent) : QWidget(parent)
{

}


void WavelengthGraph::paintEvent(QPaintEvent *event)
{
    event->accept();

    QPainter painter(this);


    painter.setPen(QPen(QColor(255,255,255,255)));
    painter.setBrush(QBrush(QColor(255, 0, 0, 255)));

    QRect rect = geometry();
    painter.drawRect(rect);

    int nWidth = width();
    int nHeight = height();
    int nMargins = 10;
    int nDivisions = (nWidth - (nMargins * 2)) / 20;

    // Draw tick marks
    float fStart = fDesignWavelength - 1.0f;
    for(int i= nMargins+5; i < nWidth; i+= nDivisions) {
        painter.drawLine(i, 10, i, 30);

        QTransform t;
        t.translate(i, 25);
        t.rotate(90.0f);
        painter.save();
        painter.setTransform(t);
        QString out = QString::asprintf("%.1f", fStart);
        out += angstromSymbol;
        painter.drawText(0, -2, out);
        fStart += 0.1f;
        painter.restore();
    }

    painter.drawLine(nWidth / 2, 0, nWidth/2, nHeight-25);

    QString out = QString::asprintf("%.1f", fDesignWavelength);
    out += angstromSymbol;

    // How wide is the above string
    QFontMetrics fm(font());

    int pixelsWide = fm.horizontalAdvance(out);
    //int pixelsHigh = fm.height();
    int nBottomMargin = 2;

    painter.drawText(nWidth / 2 - pixelsWide / 2, nHeight - nBottomMargin, out);




}
