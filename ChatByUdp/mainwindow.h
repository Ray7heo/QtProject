#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <thread>


#include "setportwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    friend class SetPortWindow;

    void init();

    void setStyle();

    void sendText();

    void sendFile();

    void recvText();

    void on_btnSetPort_clicked();




private slots:
    void on_btnClear_clicked();

    void on_btnSend_clicked();

    void on_btnFile_clicked();

    void on_textEditSendText_textChanged();

    void on_textEditAllText_textChanged();

private:
    QUdpSocket* m_socket;

    QTcpSocket* m_socket_file;

    QTcpServer* m_server;

    QPushButton* btnSetPort;

    int* m_port;

    QString m_filePath;

    QString m_fileInfo;
    QString m_fileName;
    long m_fileSize_recv;

    QByteArray m_file;

    long m_fileSize;

    bool isSendFile;

    SetPortWindow* win;

    Ui::MainWindow *ui;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
