#ifndef OCEANBACKGROUND_H
#define OCEANBACKGROUND_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QRandomGenerator>
#include <QPixmap>
#include <QResizeEvent>

struct Bubble {
    QPointF position;
    float radius;
    float speed;
    float opacity;
    QPropertyAnimation* animation;
};

class OceanBackground : public QWidget
{
    Q_OBJECT

public:
    explicit OceanBackground(QWidget *parent = nullptr);
    ~OceanBackground();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateBubbles();
    void createNewBubble();

private:
    void initializeBubbles();
    void setupAnimations();
    void drawOceanBackground(QPainter &painter);
    void drawBubbles(QPainter &painter);
    
    QTimer *bubbleTimer;
    QTimer *newBubbleTimer;
    QList<Bubble*> bubbles;
    
    static const int MAX_BUBBLES = 15;
    static const int BUBBLE_CREATE_INTERVAL = 2000; // 2秒创建一个新气泡
    static const int BUBBLE_UPDATE_INTERVAL = 50;   // 50ms更新一次
};

#endif // OCEANBACKGROUND_H