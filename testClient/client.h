#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>

#include "networkthread.h"
namespace Ui {
class Client;
}

class Client : public QWidget
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);
    ~Client();

private slots:
    void on_comboBox_fill_textActivated(const QString &arg1);
    void on_comboBox_crosshair_textActivated(const QString &arg1);
    void onDataReceived(const QByteArray &data);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateCrosshairPosition(double x, double y, double angle);
    void drawGreenCrosshair(QPixmap &pixmap);
    void renderCrosshair();
    void renderGreenCrosshair();

    Ui::Client *ui;
    NetworkThread *networkThread;

    double currentX;
    double currentY;
    double currentAngle;

    bool crosshairSelected = false;

};

#endif // CLIENT_H
