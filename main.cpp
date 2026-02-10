#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QProxyStyle>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "GuiV_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    qApp->setStyleSheet("QSlider::sub-page:horizontal { background: #2196f3; }");
    QApplication::setStyle("Fusion");
    MainWindow w;
    w.show();
    return a.exec();
}
