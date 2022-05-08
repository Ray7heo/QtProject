#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init() //初始化
{
    this->setStyle();
    //初始化套接字
    m_socket = new QUdpSocket(this);
    m_port = new int(8888);
    m_socket->bind(QHostAddress::AnyIPv4,*m_port);
    this->recvText();
    //初始化设置窗口
    this->on_btnSetPort_clicked();
    //判断对方IP端口
    connect(ui->lineEdit_Port,&QLineEdit::editingFinished,this,&MainWindow::on_textEditSendText_textChanged);
    //是否发送文件
    isSendFile = false;
}

void MainWindow::setStyle() //样式
{
    //样式
    btnSetPort = new QPushButton(ui->toolBar);
    btnSetPort->setIcon(QIcon(":/src/adjust.png"));
    ui->toolBar->addWidget(btnSetPort);
    ui->textEditAllText->setReadOnly(1);
    ui->lineEdit_IP->setPlaceholderText("IP地址");
    ui->lineEdit_Port->setPlaceholderText("端口");
    ui->btnSend->setEnabled(0);
    ui->btnClear->setEnabled(0);
    //this->setWindowFlags(Qt::FramelessWindowHint);
    //qss
    this->setStyleSheet("QPushButton{border-radius:8px;height:25px;background:white;font-size:15px;}"
                        "QPushButton:hover{background:#519ce6;}"
                        "QPushButton:pressed{background:#2773be;}"
                        "QTextEdit:hover{border:2px solid black;}"
                        "QTextEdit{border:0px;}"
                        "QLineEdit{border-radius:2px;border-bottom:1px solid black}"
                        "QGroupBox{border-radius:10px;background:white;}"
                        "QToolBar{background:white;}");

    this->btnSetPort->setStyleSheet("QPushButton{border-radius:5px;height:25px;width:30px;background:white;border:0px solid black;font-size:10px;}"
                                    "QPushButton:hover{background:#519ce6;}"
                                    "QPushButton:pressed{background:#2773be;}");

}

void MainWindow::sendText() //发送信息
{
    m_socket->writeDatagram(ui->textEditSendText->toPlainText().toUtf8(),
                            QHostAddress(ui->lineEdit_IP->text()),
                            ui->lineEdit_Port->text().toInt());
    QString text = ui->textEditSendText->toPlainText();
    ui->textEditAllText->append(text);
    ui->textEditSendText->clear();
    ui->btnSend->setEnabled(0);
}

void MainWindow::sendFile() //发送文件
{
    QTcpSocket* socket_file = new QTcpSocket(this);
    //建立连接
    socket_file->connectToHost(QHostAddress(ui->lineEdit_IP->text()),ui->lineEdit_Port->text().toInt());
    //打包文件名字和大小
    QString fileInfo = m_filePath + " " + QString::number(m_fileSize);
    //发送文件名和大小
    socket_file->write(fileInfo.toLatin1());
    //发送文件
    socket_file->write(m_file);
    this->sendText();
    isSendFile = false;
}

void MainWindow::recvText() //接收消息
{
    connect(m_socket,&QUdpSocket::readyRead,this,[=]()
    {
        QByteArray buf;
        QHostAddress addr(ui->lineEdit_IP->text());
        quint16 port = ui->lineEdit_Port->text().toInt();
        buf.resize(m_socket->pendingDatagramSize());
        //读取文本信息
        m_socket->readDatagram(buf.data(), buf.size(), &addr, &port);
        QString ip = ui->lineEdit_IP->text() + " : ";
        ip += buf;
        ui->textEditAllText->append(ip);
    });
    //监听
    m_server = new QTcpServer(this);
    m_server->listen(QHostAddress::AnyIPv4,*m_port);
    //客户端发起连接请求
    connect(m_server,&QTcpServer::newConnection,this,[=]()
    {
        m_socket_file = m_server->nextPendingConnection();
        m_fileInfo = nullptr;
        //接收客户端数据
        connect(m_socket_file,&QTcpSocket::readyRead, this,[&]()
        {
            //接收文件信息
            if (m_fileInfo == nullptr)
            {
                m_fileInfo = m_socket_file->readAll();
                //开始解包
                m_fileName  = m_fileInfo.left(m_fileInfo.indexOf(" "));
                m_fileSize_recv = m_fileInfo.right(m_fileInfo.size() - m_fileName.size() - 1).toLong();
            }
            //开始收取文件;
            QFile f(m_fileName);
            //准备文件
            f.open(QIODevice::WriteOnly | QIODevice::Append);
            f.write(m_socket_file->readAll());
            f.close();
        });
    });

}

void MainWindow::on_btnSetPort_clicked() //设置按钮点击事件
{
    connect(this->btnSetPort,&QPushButton::clicked,this,[&]()
    {
        win = new SetPortWindow(this->m_socket, this->m_server, this->m_port);
        win->show();
    });
}

void MainWindow::on_btnClear_clicked() //清空按钮点击事件
{
    ui->textEditAllText->clear();
    ui->btnClear->setEnabled(0);
}

void MainWindow::on_btnSend_clicked() //发送按钮点击事件
{
    if (isSendFile)
    {
        this->sendFile();
    }
    else
    {  
        this->sendText();
    }
}

void MainWindow::on_btnFile_clicked() //添加文件按钮点击事件
{
    QFileDialog* fd = new QFileDialog(this);
    m_filePath = fd->getOpenFileName();
    ui->textEditSendText->append(m_filePath);
    //读取文件
    QFile f(m_filePath);
    f.open(QIODevice::ReadOnly);
    if(f.isOpen())
    {
        isSendFile = true;
    }
    m_file = f.readAll();
    m_fileSize = f.size();
    f.close();
    //裁剪到文件名
    m_filePath.erase(m_filePath.begin(), m_filePath.begin() + m_filePath.lastIndexOf("/") + 1);
}

void MainWindow::on_textEditSendText_textChanged() //发送内容改变事件
{

    if (ui->textEditSendText->toPlainText() != nullptr &&
        (QHostAddress::AnyIPv4 != ui->lineEdit_IP->text().toInt() &&
        ui->lineEdit_Port->text().toInt() != *m_port))
    {
        ui->btnSend->setEnabled(1);
    }
    else
    {
        ui->btnSend->setEnabled(0);
    }
}

void MainWindow::on_textEditAllText_textChanged() //聊天内容改变事件
{
    if (ui->textEditAllText->toPlainText() != nullptr)
    {
        ui->btnClear->setEnabled(1);
    }
    else
    {
        ui->btnClear->setEnabled(0);
    }
}

void MainWindow::closeEvent(QCloseEvent *event) //退出事件
{
    QMessageBox* box = new QMessageBox(this);
    QMessageBox::StandardButton res = box->question(this," ","确认退出？",QMessageBox::Yes | QMessageBox::No);
    if (res == QMessageBox::Yes)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}
