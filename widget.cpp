#include "widget.h"
#include <QRandomGenerator>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    setFixedSize(600, 750);
    setWindowTitle("芮娜，你抓住贤者了吗");

    gameState = 0;
    curFloor = 0;
    atk = 0;

    // 菜单按钮位置
    easyBtn = QRect(200, 280, 200, 50);
    midBtn  = QRect(200, 350, 200, 50);
    hardBtn = QRect(200, 420, 200, 50);

    // --- 背景音乐 (BGM) 初始化 ---
    bgmPlayer = new QMediaPlayer(this);
    bgmOutput = new QAudioOutput(this);
    bgmPlayer->setAudioOutput(bgmOutput);
    bgmPlayer->setSource(QUrl::fromLocalFile("bgm.mp3"));

    // 强制无限循环：-1 代表无限次
    bgmPlayer->setLoops(QMediaPlayer::Infinite);
    bgmOutput->setVolume(0.4);

    // --- 音效播放器初始化 ---
    effectPlayer = new QMediaPlayer(this);
    effectOutput = new QAudioOutput(this);
    effectPlayer->setAudioOutput(effectOutput);
    effectOutput->setVolume(0.8);
}

Widget::~Widget() {}

void Widget::generateMap(long long currentAtk) {
    objs.clear();
    for(int i=0; i<5; i++) for(int j=0; j<5; j++) occupied[i][j] = false;

    // 坐标锁定：玩家左下，楼梯右上
    playerX = 0; playerY = 4;
    occupied[0][4] = true;
    occupied[4][0] = true;
    objs.append({4, 0, 1, 0});

    int count = 0;
    while(count < 8) {
        int rx = QRandomGenerator::global()->bounded(0, 5);
        int ry = QRandomGenerator::global()->bounded(0, 5);

        if(!occupied[rx][ry]) {
            occupied[rx][ry] = true;
            // 降低药水概率：约 15%
            int type = (QRandomGenerator::global()->bounded(0, 100) > 85) ? 3 : 2;
            long long val;

            if(type == 3) {
                val = 2;
            } else {
                // 梯度难度算法：计算离起点(0,4)的曼哈顿距离
                int dist = rx + (4 - ry);
                double ratio;
                // 距离起点 <= 3 格为“新手区”，否则为“挑战区”
                if (dist <= 3) ratio = 0.6 + (QRandomGenerator::global()->bounded(0, 21) / 100.0);
                else ratio = 1.1 + (QRandomGenerator::global()->bounded(0, 25) / 100.0);

                val = static_cast<long long>(currentAtk * ratio);
                if(val < 10) val = 10 + count; // 初始保底数值
            }
            objs.append({rx, ry, type, val});
            count++;
        }
    }
}

void Widget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (gameState == 0) {
        // --- 主界面绘制 ---
        // 你可以将这里换成一张专门的背景图 menu_bg.png
        painter.drawPixmap(rect(), QPixmap("menu_bg.png"));

        painter.setPen(Qt::white);
        painter.setFont(QFont("微软雅黑", 26, QFont::Bold));
        painter.drawText(0, 120, 600, 100, Qt::AlignCenter, "芮娜，你抓住贤者了吗");

        auto drawBtn = [&](QRect r, QString text, QColor col) {
            painter.setBrush(col); painter.drawRoundedRect(r, 10, 10);
            painter.setPen(Qt::white); painter.setFont(QFont("微软雅黑", 12));
            painter.drawText(r, Qt::AlignCenter, text);
        };
        drawBtn(easyBtn, "初入契约 (简单)", QColor(46, 204, 113));
        drawBtn(midBtn,  "颗秒高手 (普通)", QColor(52, 152, 219));
        drawBtn(hardBtn, "炸鱼大哥 (困难)", QColor(231, 76, 60));
        return;
    }

    // --- 游戏界面绘制 ---
    painter.drawPixmap(rect(), QPixmap("game_bg.png")); // 游戏关卡背景图
    int gridS = 100, offX = 50, offY = 180;

    // 画格子里的物件
    for(auto &obj : objs) {
        int ox = offX + obj.x * gridS + 10, oy = offY + obj.y * gridS + 10;
        if(obj.type == 2) {
            painter.drawPixmap(ox, oy, 80, 80, QPixmap("monster.png"));
            painter.setPen(obj.val > atk ? Qt::red : Qt::green);
            painter.setFont(QFont("Arial", 11, QFont::Bold));
            painter.drawText(ox + 5, oy + 20, QString::number(obj.val));
        } else if(obj.type == 3) {
            painter.drawPixmap(ox, oy, 80, 80, QPixmap("xueping.png"));
            painter.setPen(Qt::yellow); painter.drawText(ox + 5, oy + 20, "x2 倍");
        } else if(obj.type == 1) {
            painter.drawPixmap(ox, oy, 80, 80, QPixmap("stairs.png"));
        }
    }

    // 画玩家
    int px = offX + playerX * gridS + 10, py = offY + playerY * gridS + 10;
    painter.drawPixmap(px, py, 80, 80, QPixmap(walkFrame % 2 == 0 ? "hero_1.png" : "hero_2.png"));

    // 英雄数值
    painter.setPen(Qt::cyan);
    painter.setFont(QFont("Arial", 13, QFont::Bold));
    painter.drawText(px + 10, py - 8, QString::number(atk));

    // 战斗对话
    if(!bubbleText.isEmpty()){
        painter.setPen(Qt::white);
        painter.setFont(QFont("微软雅黑", 10));
        painter.drawText(px, py - 35, bubbleText);
    }

    // UI 栏
    painter.setPen(Qt::white);
    painter.setFont(QFont("微软雅黑", 14));
    painter.drawText(50, 80, QString("当前楼层: 第 %1 层").arg(curFloor + 1));

    if (gameState == 2) {
        painter.fillRect(rect(), QColor(0,0,0,200));
        painter.setPen(Qt::red);
        painter.setFont(QFont("微软雅黑", 22, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "贤者离开你了！\n\n请按 [F5] 重生");
    }
}

void Widget::mousePressEvent(QMouseEvent *event) {
    if(gameState == 0) {
        if(easyBtn.contains(event->pos())) { atk = 50; gameState = 1; }
        else if(midBtn.contains(event->pos()))  { atk = 20; gameState = 1; }
        else if(hardBtn.contains(event->pos())) { atk = 5;  gameState = 1; }

        if(gameState == 1) {
            if(bgmPlayer->playbackState() != QMediaPlayer::PlayingState) {
                bgmPlayer->play(); // 开始循环 BGM
            }
            curFloor = 0;
            generateMap(atk);
            update();
        }
    }
}

void Widget::keyPressEvent(QKeyEvent *event) {
    if(gameState == 2 && event->key() == Qt::Key_F5) {
        gameState = 0;
        update();
        return;
    }
    if(gameState != 1) return;

    int nx = playerX, ny = playerY;
    if(event->key() == Qt::Key_W && playerY > 0) ny--;
    else if(event->key() == Qt::Key_S && playerY < 4) ny++;
    else if(event->key() == Qt::Key_A && playerX > 0) nx--;
    else if(event->key() == Qt::Key_D && playerX < 4) nx++;
    else return;

    for(int i = 0; i < objs.size(); i++) {
        if(objs[i].x == nx && objs[i].y == ny) {
            if(objs[i].type == 2) { // 战斗
                if(atk >= objs[i].val) {
                    atk += objs[i].val;
                    objs.removeAt(i);
                    effectPlayer->setSource(QUrl::fromLocalFile("battle.mp3"));
                    effectPlayer->play(); // 播放击杀音效
                    bubbleText = "不堪一击！";
                    QTimer::singleShot(800, this, [=](){ bubbleText = ""; update(); });
                } else {
                    gameState = 2; // 失败
                    effectPlayer->setSource(QUrl::fromLocalFile("lose.mp3"));
                    effectPlayer->play();
                    update(); return;
                }
            } else if(objs[i].type == 3) { // 药水
                if (atk < 2000000000LL) atk *= 2;
                objs.removeAt(i);
            } else if(objs[i].type == 1) { // 楼梯
                curFloor++;
                generateMap(atk);
                update();
                return;
            }
            break;
        }
    }
    playerX = nx; playerY = ny; walkFrame++;
    update();
}