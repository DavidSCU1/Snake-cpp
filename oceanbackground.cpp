#include "oceanbackground.h"
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QRandomGenerator>
#include <QDebug>
#include <QPainterPath>

OceanBackground::OceanBackground(QWidget *parent)
    : QWidget(parent)
    , bubbleTimer(new QTimer(this))
    , newBubbleTimer(new QTimer(this))
{
    // 设置背景透明，让我们可以绘制自定义背景
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_NoSystemBackground, true);
    
    // 加载海洋背景图片
    if (!oceanBackground.load(":/images/ocean-background.png")) {
        qDebug() << "Failed to load ocean background image";
    } else {
        qDebug() << "Ocean background image loaded successfully";
    }
    
    // 初始化定时器
    connect(bubbleTimer, &QTimer::timeout, this, &OceanBackground::updateBubbles);
    connect(newBubbleTimer, &QTimer::timeout, this, &OceanBackground::createNewBubble);
    
    bubbleTimer->start(BUBBLE_UPDATE_INTERVAL);
    newBubbleTimer->start(BUBBLE_CREATE_INTERVAL);
    
    initializeBubbles();
}

OceanBackground::~OceanBackground()
{
    // 清理气泡动画
    for (auto bubble : bubbles) {
        if (bubble->animation) {
            bubble->animation->stop();
            delete bubble->animation;
        }
        delete bubble;
    }
    bubbles.clear();
}

void OceanBackground::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制海洋背景
    drawOceanBackground(painter);
    
    // 绘制气泡（在背景层，不会遮挡UI元素）
    drawBubbles(painter);
}

void OceanBackground::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    // 重新生成背景图片以适应新尺寸
    oceanBackground = QPixmap();
}

void OceanBackground::drawOceanBackground(QPainter &painter)
{
    // 绘制背景图片
    if (!oceanBackground.isNull()) {
        qDebug() << "Drawing ocean background image, widget size:" << size() << "image size:" << oceanBackground.size();
        // 使用IgnoreAspectRatio来填满整个窗口
        QPixmap scaledBackground = oceanBackground.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, scaledBackground);
        qDebug() << "Background image drawn successfully";
    } else {
        qDebug() << "Ocean background image is null, using fallback gradient";
        // 如果图片加载失败，使用备用渐变背景
        QLinearGradient oceanGradient(0, 0, 0, height());
        oceanGradient.setColorAt(0.0, QColor(135, 206, 250, 180));  // 浅蓝色
        oceanGradient.setColorAt(0.3, QColor(70, 130, 180, 200));   // 钢蓝色
        oceanGradient.setColorAt(0.7, QColor(25, 25, 112, 220));    // 深蓝色
        oceanGradient.setColorAt(1.0, QColor(0, 0, 139, 240));      // 深蓝色底部
        painter.fillRect(rect(), oceanGradient);
    }
    
    // 添加一些海洋波纹效果
    painter.setOpacity(0.3);
    QPen wavePen(QColor(255, 255, 255, 100), 2);
    painter.setPen(wavePen);
    
    int waveCount = 5;
    for (int i = 0; i < waveCount; ++i) {
        int y = height() * 0.2 + i * (height() * 0.15);
        QPainterPath wavePath;
        wavePath.moveTo(0, y);
        
        for (int x = 0; x <= width(); x += 20) {
            int waveHeight = 10 * qSin((x + i * 50) * 0.02);
            wavePath.lineTo(x, y + waveHeight);
        }
        
        painter.drawPath(wavePath);
    }
    
    painter.setOpacity(1.0);
}

void OceanBackground::drawBubbles(QPainter &painter)
{
    painter.setOpacity(0.6); // 气泡半透明，不会太显眼
    
    for (const auto& bubble : bubbles) {
        if (bubble->position.y() > -bubble->radius * 2) {
            // 创建气泡的渐变效果
            QRadialGradient bubbleGradient(bubble->position, bubble->radius);
            bubbleGradient.setColorAt(0.0, QColor(255, 255, 255, bubble->opacity * 180));
            bubbleGradient.setColorAt(0.7, QColor(173, 216, 230, bubble->opacity * 120));
            bubbleGradient.setColorAt(1.0, QColor(135, 206, 250, bubble->opacity * 60));
            
            painter.setBrush(bubbleGradient);
            painter.setPen(QPen(QColor(255, 255, 255, bubble->opacity * 100), 1));
            painter.drawEllipse(bubble->position, bubble->radius, bubble->radius);
        }
    }
    
    painter.setOpacity(1.0);
}

void OceanBackground::initializeBubbles()
{
    // 创建初始气泡
    for (int i = 0; i < MAX_BUBBLES / 3; ++i) {
        createNewBubble();
    }
}

void OceanBackground::createNewBubble()
{
    if (bubbles.size() >= MAX_BUBBLES) {
        return;
    }
    
    Bubble* bubble = new Bubble;
    
    // 随机位置和大小
    bubble->position = QPointF(
        QRandomGenerator::global()->bounded(0, width()),
        height() + 50
    );
    bubble->radius = QRandomGenerator::global()->bounded(10, 30);
    bubble->speed = QRandomGenerator::global()->bounded(100, 300) / 100.0; // 1.0 to 3.0
    bubble->opacity = QRandomGenerator::global()->bounded(30, 80) / 100.0; // 0.3 to 0.8
    bubble->animation = nullptr;
    
    bubbles.append(bubble);
}

void OceanBackground::updateBubbles()
{
    // 更新气泡位置
    for (auto it = bubbles.begin(); it != bubbles.end();) {
        Bubble* bubble = *it;
        
        // 向上移动气泡
        bubble->position.setY(bubble->position.y() - bubble->speed);
        
        // 添加轻微的水平摆动
        float sway = 2.0 * qSin(bubble->position.y() * 0.01);
        bubble->position.setX(bubble->position.x() + sway);
        
        // 如果气泡移出屏幕，删除它
        if (bubble->position.y() < -bubble->radius * 2) {
            if (bubble->animation) {
                bubble->animation->stop();
                delete bubble->animation;
            }
            delete bubble;
            it = bubbles.erase(it);
        } else {
            ++it;
        }
    }
    
    // 触发重绘
    update();
}