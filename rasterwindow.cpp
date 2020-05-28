#include "rasterwindow.h"
#include <QPolygon>

RasterWindow::RasterWindow(QWindow *parent)
    : QWindow(parent), m_backingStore(new QBackingStore(this))
{
    setGeometry(100,100,1720,900);
}

RasterWindow::~RasterWindow()
{
}

void RasterWindow::exposeEvent(QExposeEvent *)
{
    if (isExposed())
        time1 = std::chrono::system_clock::now();
        time2 = std::chrono::system_clock::now();
        renderNow();
}

void RasterWindow::resizeEvent(QResizeEvent *resizeEvent)
{
    m_backingStore->resize(resizeEvent->size());
}

void RasterWindow::renderNow()
{
    if (!isExposed())
        return;

    QRect rect(0, 0, width(), height());
    m_backingStore->beginPaint(rect);

    QPaintDevice *device = m_backingStore->paintDevice();
    QPainter painter(device);

    time2 = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsedTime = time2 - time1;
    time1 = time2;
    float fElapsedTime = elapsedTime.count();

    lastElapsedTime = fElapsedTime;

    float fps = 1.0f / fElapsedTime;

    setTitle(QString::number(fps));

    painter.fillRect(0, 0, width(), height(), Qt::black);
    render(&painter, fElapsedTime);
    painter.end();

    m_backingStore->endPaint();
    m_backingStore->flush(rect);
}

void RasterWindow::render(QPainter *painter, float fElapsedTime)
{
    QPen pen;
    pen.setColor(Qt::white);
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawLine(50,50,50,100);
    painter->drawLine(50,100,100,75);
    painter->drawLine(100,75,50,50);
}

int RasterWindow::screenHeight()
{
    return height();
}

int RasterWindow::screenWidth()
{
    return width();
}

void RasterWindow::drawTriangle(QPainter *painter, int x1, int y1, int x2, int y2, int x3, int y3, QColor col)
{
    QPen pen;
    pen.setColor(col);
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawLine(x1,y1,x2,y2);
    painter->drawLine(x2,y2,x3,y3);
    painter->drawLine(x3,y3,x1,y1);
}

void RasterWindow::fillTriangle(QPainter *painter, int x1, int y1, int x2, int y2, int x3, int y3, QColor col)
{
    QPolygon poly;
    poly << QPoint(x1,y1) << QPoint(x2,y2) << QPoint(x3,y3);
    QPen pen;
    pen.setColor(col);
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawLine(x1,y1,x2,y2);
    painter->drawLine(x2,y2,x3,y3);
    painter->drawLine(x3,y3,x1,y1);

    //Brush to fill poly
    QBrush brush;
    brush.setColor(col);
    brush.setStyle(Qt::SolidPattern);

    //Fill poly
    QPainterPath path;
    path.addPolygon(poly);

    painter->drawPolygon(poly);
    painter->fillPath(path, brush);
}

float RasterWindow::getElapsedTime()
{
    return lastElapsedTime;
}



void RasterWindow::renderLater()
{
    requestUpdate();
}

bool RasterWindow::event(QEvent *event)
{
    if (event->type() == QEvent::UpdateRequest) {
        renderNow();
        return true;
    }
    return QWindow::event(event);
}

