#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define PunctRadius 15
#define BotRadius 5
#define VolumeRadius 50
#define MaxBotCount 333 // Максимальное количество ботов одного типа
#define ScoutBotID 2
#define EliteBotID 1
#define NormalBotID 0

#include <QMainWindow>
#include <QTimer>
#include <QPainter>
#include <random>
#include <cmath>
#include <numbers>
#include <ranges>
#include <span>
constexpr float PI = std::numbers::pi_v<float>;

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
    Bot BotListNormal[MaxBotCount];
    Bot BotListElite[MaxBotCount];
    Bot BotListScout[MaxBotCount];
    int NormalBotCount=0;
    int EliteBotCount=300;
    int ScoutBotCount=200;
    bool start=true;
    int time = 0;
    int odraw = 0;
    int clock=16;
    bool tick(std::mt19937 &gen){
        auto groups = {
            std::span(BotListNormal).subspan(0, std::min((size_t)NormalBotCount, (size_t)std::size(BotListNormal))),
            std::span(BotListScout).subspan(0, std::min((size_t)ScoutBotCount, (size_t)std::size(BotListScout))),
            std::span(BotListElite).subspan(0, std::min((size_t)EliteBotCount, (size_t)std::size(BotListElite)))
        };
        auto groupsNS = {
            std::span(BotListNormal).subspan(0, std::min((size_t)NormalBotCount, (size_t)std::size(BotListNormal))),
            std::span(BotListElite).subspan(0, std::min((size_t)EliteBotCount, (size_t)std::size(BotListElite)))
        };
        if(start){
            std::mt19937 localGen(338877028838);
            std::uniform_real_distribution<float> angleDist(0, 2 * PI);
            std::uniform_real_distribution<float> posDistX(0, Size.x-1);
            std::uniform_real_distribution<float> posDistY(0, Size.y-1);
            for (auto& BotI : BotListNormal) {
                BotI.reset();
                BotI.type=NormalBotID;
                BotI.dis = Vec2(Size.x,Size.y);
                BotI.angle = angleDist(localGen);
                BotI.pos = Vec2(posDistX(localGen),posDistY(localGen));
            }
            for (auto& BotI : BotListScout) {
                BotI.reset();
                BotI.type=ScoutBotID;
                BotI.dis = Vec2(Size.x,Size.y);
                BotI.angle = angleDist(localGen);
                BotI.pos = Vec2(posDistX(localGen),posDistY(localGen));
            }
            for (auto& BotI : BotListElite) {
                BotI.reset();
                BotI.type=EliteBotID;
                BotI.dis = Vec2(Size.x,Size.y);
                BotI.angle = angleDist(localGen);
                BotI.pos = Vec2(posDistX(localGen),posDistY(localGen));
            }
            /*int countSide = std::sqrt(ScoutBotCount);
            if (countSide == 0) countSide = 1;

            // 2. Считаем шаг между ботами
            float stepX = Size.x / countSide;
            float stepY = Size.y / countSide;

            // Расставляем разведчиков
            for (int i = 0; i < ScoutBotCount; i++) {
                BotList[i].type = ScoutBotID;
                BotList[i].pos = Vec2(int(i%countSide)*stepX, int(i/countSide)*stepY);
                BotList[i].dis = Vec2(Size.x,Size.y);

                unsigned int seed = static_cast<unsigned int>(BotList[i].pos.x) * 73856093 ^ static_cast<unsigned int>(BotList[i].pos.y) * 19349663;
                // 2. Создаем временный генератор специально для этого бота
                std::mt19937 localGen(seed);
                std::uniform_real_distribution<float> angleDist(0, 2 * PI);
                // 3. Угол теперь зависит только от позиции
                BotList[i].angle = angleDist(localGen);
            }
            for (int i=0;i<EliteBotCount;i++){
                BotList[i+ScoutBotCount].type=EliteBotID;
                BotList[i+ScoutBotCount].pos=A;
                BotList[i+ScoutBotCount].colision=true;
                BotList[i+ScoutBotCount].angle=atan2(A.y-B.y,A.x-B.x)+PI;
            }*/
            start=false;
        }
        A.y=(sin(float(time)/1000.0f)+1)/2*Size.y;
        for (auto groupI : groups) {
            for (auto& BotI : groupI) {

                BotI.step(gen,Size);
                BotI.collision(A,B);
                for (auto groupJ : groupsNS) {
                    for (auto& BotJ : groupJ) {
                        for (int g=0;g<2;g++){
                            float dx=BotI.pos.x-BotJ.pos.x;
                            float dy=BotI.pos.y-BotJ.pos.y;
                            if(dx*dx+dy*dy<VolumeRadius*VolumeRadius){
                                float v=BotI.dis[g]+VolumeRadius;
                                if(BotJ.dis[g]>v){
                                    BotJ.dis[g]=v;
                                    if(BotJ.go==g){BotJ.angle=atan2(dy,dx);}
                                }
                            }
                        }
                    }
                }
            }
        }
        time+=clock;
        if(time-odraw>=16){odraw=time;return(true);}
        return(false);
    }
    void reset(){
        time=0;
        odraw=0;
        start=true;
    }
    void draw(QPainter &painter){
        auto groups = {
            std::span(BotListNormal).subspan(0, std::min((size_t)NormalBotCount, (size_t)std::size(BotListNormal))),
            std::span(BotListScout).subspan(0, std::min((size_t)ScoutBotCount, (size_t)std::size(BotListScout))),
            std::span(BotListElite).subspan(0, std::min((size_t)EliteBotCount, (size_t)std::size(BotListElite)))
        };
        painter.setBrush(Qt::white);
        painter.drawRect(0,0,Size.x,Size.y);
        painter.setBrush(Qt::green);
        painter.drawEllipse(A.x-PunctRadius, A.y-PunctRadius, PunctRadius*2, PunctRadius*2);
        painter.setBrush(Qt::blue);
        painter.drawEllipse(B.x-PunctRadius, B.y-PunctRadius, PunctRadius*2, PunctRadius*2);
        for (auto groupI : groups) {
            for (auto& BotI : groupI) {
                BotI.draw(painter);
            }
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
    void on_ClockSlider_valueChanged(int value);

    void on_BotCountSlider_valueChanged(int value);

    void on_ScoutCountSlider_valueChanged(int value);

    void on_EliteCountSlider_valueChanged(int value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
};
#endif // MAINWINDOW_H
