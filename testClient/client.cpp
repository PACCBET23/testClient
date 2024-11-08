#include "client.h"
#include "ui_client.h"
#include "networkthread.h"
#include <QSvgRenderer>
#include <QPainter>
#include <QResizeEvent>
#include <QDataStream>

Client::Client(QWidget *parent)
    : QWidget(parent), ui(new Ui::Client), networkThread(new NetworkThread(this)),
    currentX(0), currentY(0), currentAngle(0), crosshairSelected(false) {
    ui->setupUi(this);

    ui->comboBox_fill->addItem("Чёрный", "black");
    ui->comboBox_fill->addItem("Серый", "grey");
    ui->comboBox_fill->addItem("Белый", "white");

    ui->comboBox_crosshair->addItem("Черный прицел", ":/images/crosshair black.svg");
    ui->comboBox_crosshair->addItem("Красный прицел", ":/images/crosshair red.svg");

    ui->label_main->setStyleSheet("background-color: black;");

    connect(networkThread, &NetworkThread::dataReceived, this, &Client::onDataReceived);
    networkThread->setupSocket(12345);

    renderGreenCrosshair();
}

Client::~Client() {
    networkThread->quit();
    networkThread->wait();
    delete ui;
}

void Client::on_comboBox_fill_textActivated(const QString &arg1) {
    QString color = ui->comboBox_fill->currentData().toString();
    ui->label_main->setStyleSheet("background-color: " + color + ";");
}

void Client::on_comboBox_crosshair_textActivated(const QString &arg1) {
    crosshairSelected = true;

    renderCrosshair();
}

void Client::resizeEvent(QResizeEvent *event) {
    int newWidth = event->size().width();
    int newHeight = newWidth * 3 / 4;

    if (newHeight != event->size().height()) {
        this->resize(newWidth, newHeight);
    }

    QWidget::resizeEvent(event);

    if (!crosshairSelected) {
        renderGreenCrosshair();
    } else {
        renderCrosshair();
    }
}

void Client::onDataReceived(const QByteArray &data) {
    QDataStream stream(data);
    stream >> currentAngle >> currentX >> currentY;

    updateCrosshairPosition(currentX, currentY, currentAngle);

    QString atext = QString("Угол камеры: %1 град.").arg(currentAngle, 0, 'f', 0);
    QString xtext = QString("Отступ по горизонтали: %1").arg(currentX, 0, 'f', 2);
    QString ytext = QString("Отступ по вертикали: %1").arg(currentY, 0, 'f', 2);

    ui->label_a->setText(atext);
    ui->label_x->setText(xtext);
    ui->label_y->setText(ytext);
}

void Client::updateCrosshairPosition(double x, double y, double a) {
    currentX = x;
    currentY = y;
    currentAngle = a;
    renderCrosshair();
}

void Client::drawGreenCrosshair(QPixmap &pixmap) {
    QPainter painter(&pixmap);
    int labelWidth = pixmap.width();
    int labelHeight = pixmap.height();
    int crosshairLength = 100;

    painter.setPen(QPen(Qt::green, 1));
    painter.drawLine(labelWidth / 2 - crosshairLength / 2, labelHeight / 2, labelWidth / 2 + crosshairLength / 2, labelHeight / 2);  // ось x
    painter.drawLine(labelWidth / 2, labelHeight / 2 - crosshairLength / 2, labelWidth / 2, labelHeight / 2 + crosshairLength / 2);  // ось y
}


void Client::renderCrosshair() {
    int labelWidth = ui->label_main->width();
    int labelHeight = ui->label_main->height();

    int adjustedHeight = labelWidth * 3 / 4;
    if (adjustedHeight > labelHeight) {
        adjustedHeight = labelHeight;
        labelWidth = adjustedHeight * 4 / 3;
    }

    QPixmap pixmap(labelWidth, adjustedHeight);
    pixmap.fill(Qt::transparent);

    drawGreenCrosshair(pixmap);

    QString svgPath = ui->comboBox_crosshair->currentData().toString();
    if (!svgPath.isEmpty()) {
        QSvgRenderer renderer(svgPath);
        if (renderer.isValid()) {
            QPainter painter(&pixmap);
            int centerX = labelWidth / 2 + currentX * labelWidth / 2;
            int centerY = adjustedHeight / 2 + currentY * adjustedHeight / 2;

            painter.translate(centerX, centerY);
            painter.rotate(currentAngle);
            renderer.render(&painter, QRectF(-labelWidth / 2, -adjustedHeight / 2, labelWidth, adjustedHeight));
        }
    }

    ui->label_main->setPixmap(pixmap);
}

void Client::renderGreenCrosshair() {
    int labelWidth = ui->label_main->width();
    int labelHeight = ui->label_main->height();

    int adjustedHeight = labelWidth * 3 / 4;
    if (adjustedHeight > labelHeight) {
        adjustedHeight = labelHeight;
        labelWidth = adjustedHeight * 4 / 3;
    }

    QPixmap pixmap(labelWidth, adjustedHeight);
    pixmap.fill(Qt::transparent);

    drawGreenCrosshair(pixmap);

    ui->label_main->setPixmap(pixmap);
}
