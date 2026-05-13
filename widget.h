#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QVector>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QTimer>

struct TowerObject {
    int x, y;
    int type;    // 1:楼梯, 2:怪物, 3:药水
    long long val;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void generateMap(long long currentAtk);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int gameState;      // 0:菜单, 1:游戏中, 2:结束
    int curFloor;
    int playerX, playerY;
    long long atk;
    int walkFrame;

    QRect easyBtn, midBtn, hardBtn;
    QVector<TowerObject> objs;
    bool occupied[5][5];

    // 音频组件
    QMediaPlayer *bgmPlayer;
    QMediaPlayer *effectPlayer;
    QAudioOutput *bgmOutput;
    QAudioOutput *effectOutput;

    // 战斗台词
    QString bubbleText;
};

#endif