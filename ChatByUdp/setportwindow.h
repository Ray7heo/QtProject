#ifndef SETPORTWINDOW_H
#define SETPORTWINDOW_H

#include <QWidget>
#include <QUdpSocket>
#include <QTcpServer>


namespace Ui {
class SetPortWindow;
}

class SetPortWindow : public QWidget
{
    Q_OBJECT

public:
    //explicit SetPortWindow(QWidget *parent = nullptr);
    explicit SetPortWindow(QUdpSocket* socket, QTcpServer* server, int* port, QWidget *parent = nullptr);

    void init();

    void setStyle();

    void reBind(QUdpSocket* socket, QTcpServer* server, int* port);

    ~SetPortWindow();

private:
    int* port;
    Ui::SetPortWindow *ui;
};

#endif // SETPORTWINDOW_H
