#ifndef RASTERWINDOW_H
#define RASTERWINDOW_H


#include <QtGui>
#include <chrono>

class RasterWindow : public QWindow
{
    Q_OBJECT

public:
    explicit RasterWindow(QWindow* parent = 0);
    ~RasterWindow();

    virtual void render(QPainter* painter, float fElapsedTime);

    int screenHeight();
    int screenWidth();

    void drawTriangle(QPainter* painter, int x1, int y1, int x2, int y2, int x3, int y3, QColor col);

    void fillTriangle(QPainter* painter, int x1, int y1, int x2, int y2, int x3, int y3, QColor col);

    float getElapsedTime();

public slots:
    void renderLater();
    void renderNow();

protected:
    bool event(QEvent* event) override;

    void resizeEvent(QResizeEvent *) override;
    void exposeEvent(QExposeEvent *) override;

private:
    QBackingStore *m_backingStore;
    std::chrono::time_point<std::chrono::system_clock> time1;
    std::chrono::time_point<std::chrono::system_clock> time2;
    float lastElapsedTime;

};
#endif // RASTERWINDOW_H
