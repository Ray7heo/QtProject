#include "setportwindow.h"
#include "ui_setportwindow.h"


SetPortWindow::SetPortWindow(QUdpSocket* socket, QTcpServer* server, int* port, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetPortWindow)
{
    ui->setupUi(this);
    init();
    ui->lineEditPort->setText(QString::number(*port));
    reBind(socket, server, port);

}

void SetPortWindow::init()
{
    this->setFixedSize(400,300);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    this->setStyle();
}

void SetPortWindow::setStyle()
{
    this->setStyleSheet("QLineEdit{border-radius:5px;border:1px solid black;}"
                        "QGroupBox{border-radius:10px;background:white;}"
                        "QMainWindow{background:#eef0f1;}");
}

void SetPortWindow::reBind(QUdpSocket* socket, QTcpServer* server, int *port)
{
    this->port = port;
    connect(ui->lineEditPort,&QLineEdit::editingFinished,this,[=]()
    {
        *this->port = ui->lineEditPort->text().toInt();
        socket->close();
        server->close();
        socket->bind(QHostAddress::AnyIPv4,*this->port);
        server->listen(QHostAddress::AnyIPv4, *this->port);
    });
}

SetPortWindow::~SetPortWindow()
{
    delete ui;
}
