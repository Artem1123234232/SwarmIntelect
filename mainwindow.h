#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define PunctRadius 15
#define BotRadius 5
#define VolumeRadius 50
#define BotCount 500
#define ScoutBotCount 200
#define EliteBotCount 300
#define ScoutBotID 2
#define EliteBotID 1
#define NormalBotID 0

#include <QMainWindow>
#include <QTimer>
#include <QPainter>
#include <random>
#include <cmath>
const float PI = std::acos(-1.0f);

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Vec2 {
    float x, y;
    Vec2(int _x = 0, int _y = 0) : x(_x), y(_y) {}
    float& operator[](int index) {
        if (index == 0) return x;
        return y;
    }
    float operator[](int index) const {
        if (index == 0) return x;
        return y;
    }
    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }
    bool operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }
    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }
    Vec2 operator/(const Vec2& other) const {
        return Vec2(x / other.x, y / other.y);
    }
    Vec2 operator*(const Vec2& other) const {
        return Vec2(x * other.x, y * other.y);
    }
    Vec2 operator%(const Vec2& other) const {
        float rx = std::fmod(x, other.x);
        if (rx < 0) rx += other.x;

        float ry = std::fmod(y, other.y);
        if (ry < 0) ry += other.y;

        return Vec2(rx, ry);
    }
    int LSQ() const { //length()**2
        return x*x+y*y;
    }
    void reset() {
        x = 0; y = 0;
    }
};
struct Bot {
    Vec2 pos=Vec2(0,0);
    Vec2 dis=Vec2(0,0);
    float vel=5;
    float angle=PI/16;
    int go=0;
    int type=NormalBotID;
    bool colision=false;
    bool colision2=false;

    void step(std::mt19937 &gen, Vec2 Size) {
        if(type!=EliteBotID){
            std::uniform_real_distribution<float> disA(-0.05f, 0.05f);
            angle += disA(gen);
        }
        std::uniform_real_distribution<float> disV(-4.0f, 4.0f);
        float v=vel+disV(gen);
        pos.x+=cos(angle)*v;
        pos.y+=sin(angle)*v;
        dis.x+=vel;dis.y+=vel;
        //pos=pos%Size;
        if(pos.x<0 or pos.y<0 or pos.x>Size.x or pos.y>Size.y){
            if(!colision2){
                angle+=PI;
            }
            else{pos=Vec2(std::max(std::min(pos.x,Size.x),0.0f),std::max(std::min(pos.y,Size.y),0.0f));}
            colision2=true;
        }
        else{
            colision2=false;
        }
    }
    void reset(){
        pos=Vec2(0,0);
        dis=Vec2(0,0);
        angle=PI/16;
        go=0;
        type=NormalBotID;
        colision=false;
        colision2=false;
    }
    void draw(QPainter &painter){
        if (type==NormalBotID) painter.setBrush(Qt::cyan);
        else if (type==EliteBotID) painter.setBrush(Qt::red);
        else if (type==ScoutBotID) painter.setBrush(Qt::green);
        painter.drawEllipse(pos.x-BotRadius, pos.y-BotRadius, BotRadius*2, BotRadius*2);
    }
    void collision(Vec2 A, Vec2 B){
        float r = (PunctRadius+BotRadius);
        if((pos-A).LSQ()<r*r){
            if(!colision){
                angle+=PI;
                dis.x=0;
                go=1;
                colision=true;
            }
        }
        else if((pos-B).LSQ()<r*r){
            if(!colision){
                angle+=PI;
                dis.y=0;
                go=0;
                colision=true;
            }
        }
        else{
            colision=false;
        }
    }
};
struct World {
    Vec2 Size=Vec2(300,300);
    Vec2 A=Vec2(50,100);
    Vec2 B=Vec2(250,50);
    Bot BotList[BotCount];
    bool start=true;
    int time = 0;
    const int clock=1;
    bool tick(std::mt19937 &gen){
        if(start){
            int v=BotCount-EliteBotCount-ScoutBotCount;
            int countSide = std::sqrt(ScoutBotCount);
            if (countSide == 0) countSide = 1;

            // 2. Считаем шаг между ботами
            float stepX = Size.x / countSide;
            float stepY = Size.y / countSide;

            // Расставляем разведчиков
            for (int i = 0; i < ScoutBotCount; i++) {
                int idx = i + v;

                BotList[idx].type = ScoutBotID;
                BotList[idx].pos = Vec2(int(i%countSide)*stepX, int(i/countSide)*stepY);
                BotList[idx].dis = Vec2(Size.x,Size.y);

                unsigned int seed = static_cast<unsigned int>(BotList[idx].pos.x) * 73856093 ^ static_cast<unsigned int>(BotList[idx].pos.y) * 19349663;
                // 2. Создаем временный генератор специально для этого бота
                std::mt19937 localGen(seed);
                std::uniform_real_distribution<float> angleDist(0, 2 * PI);
                // 3. Угол теперь зависит только от позиции
                BotList[idx].angle = angleDist(localGen);
            }
            for (int i=0;i<EliteBotCount;i++){BotList[i+v+ScoutBotCount].type=EliteBotID;BotList[i+v+ScoutBotCount].pos=A;BotList[i+v+ScoutBotCount].colision=true;BotList[i+v+ScoutBotCount].angle=atan2(A.y-B.y,A.x-B.x)+PI;}
            start=false;
        }
        //A.y=(sin(float(time)/1000.0f)+1)/2*Size.y;
        for (int i=0;i<BotCount;i++){
            BotList[i].step(gen,Size);
            BotList[i].collision(A,B);
            for (int j=0;j<BotCount;j++){
                if (BotList[j].type!=ScoutBotID){
                    for (int g=0;g<2;g++){
                        float dx=BotList[i].pos.x-BotList[j].pos.x;
                        float dy=BotList[i].pos.y-BotList[j].pos.y;
                        if(dx*dx+dy*dy<VolumeRadius*VolumeRadius){
                            float v=BotList[i].dis[g]+VolumeRadius;
                            if(BotList[j].dis[g]>v){
                                BotList[j].dis[g]=v;
                                if(BotList[j].go==g){BotList[j].angle=atan2(dy,dx);}
                            }
                        }
                    }
                }
            }
        }
        time+=clock;
        return(time%16==0);
    }
    void reset(){
        for (int i=0;i<BotCount;i++){
            BotList[i].reset();
        }
        time=0;
        start=true;
    }
    void draw(QPainter &painter){
        painter.setBrush(Qt::white);
        painter.drawRect(0,0,Size.x,Size.y);
        painter.setBrush(Qt::green);
        painter.drawEllipse(A.x-PunctRadius, A.y-PunctRadius, PunctRadius*2, PunctRadius*2);
        painter.setBrush(Qt::blue);
        painter.drawEllipse(B.x-PunctRadius, B.y-PunctRadius, PunctRadius*2, PunctRadius*2);
        for (int i=0;i<BotCount;i++){
            BotList[i].draw(painter);
        }
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QTimer *timer;
    std::mt19937 gen;
    World world;
    Ui::MainWindow *ui;
private slots:
    void nextStep();   // Метод для расчетов симуляции
protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
};
#endif // MAINWINDOW_H
