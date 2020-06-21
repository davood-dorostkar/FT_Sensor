#include "mainwindow.h"
#include "ui_mainwindow.h"
static QList<QSerialPortInfo> portList;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    portList = QSerialPortInfo::availablePorts();
    for (int i=0;i<portList.length();i++)
        ui->portBox->addItem(portList[i].portName());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readyRead()
{
    QByteArray data = comPort.readAll();
    //qDebug()<<data.toHex();
    qDebug()<<data;
    //buffer.append("\n");
    buffer.append(data);
    ui->monitorBox->setText(buffer.append("\r\n"));
}

void MainWindow::on_connectButton_clicked()
{
    comPort.setPortName(ui->portBox->currentText());// ttyUSB0
    comPort.setBaudRate(ui->baudBox->currentText().toInt());
    //comPort.setDataBits(QSerialPort::Data8);
    //comPort.setParity(QSerialPort::NoParity);
    //comPort.setStopBits(QSerialPort::OneStop);
    //comPort.setFlowControl(QSerialPort::);
    connect(&comPort,SIGNAL(readyRead()),this,SLOT(readyRead()));
    bool result= comPort.open(QSerialPort::ReadWrite);
    if (!result)
        ui->statusBar->showMessage("Open Port Error!");
    else
    {
        ui->statusBar->showMessage("connected successfully");
        buffer.clear();
        comPort.write("CV 3F\r\n");
    }
}

void MainWindow::on_disconnectButton_clicked()
{
    if (comPort.isOpen())
        comPort.close();
    buffer.clear();
    ui->statusBar->showMessage("disconnected successfully");
}

void MainWindow::on_refreshButton_clicked()
{
    portList = QSerialPortInfo::availablePorts();
    for (int i=0;i<portList.length();i++)
        ui->portBox->addItem(portList[i].portName());
}


void MainWindow::on_getSampleButton_clicked()
{
    buffer.clear();
    timer.disconnect();
    samples = ui->samplesBox->value();
    interval=ui->intervalBox->value();
    timer.start(interval);
    connect(&timer,SIGNAL(timeout()),this,SLOT(SendMessageSamples()));
    //isSampleButtonPressed = true;
}

void MainWindow::on_getSecondsButtons_clicked()
{
    buffer.clear();
    timer.disconnect();
    fullTimeMilliseconds=ui->secondsBox->value();
    interval=ui->intervalBox->value();
    timer.start(interval);
    connect(&timer,SIGNAL(timeout()),this,SLOT(SendMessageSeconds()));
    //isSampleButtonPressed = false;
}

void MainWindow::SendMessageSamples()
{
    comPort.write("QR\r\n");
    samples--;
    if (samples<=0)
        timer.stop();
}

void MainWindow::SendMessageSeconds()
{
    comPort.write("QR\r\n");
    fullTimeMilliseconds -= interval;
    if (fullTimeMilliseconds<=0)
        timer.stop();
}


void MainWindow::on_saveButton_clicked()
{
    QFileDialog *dlg = new QFileDialog();
    // dlg->exec();
    QString saveURL=dlg->getSaveFileName();
    qDebug() << "save dialog"<<saveURL;
    //    if (dlg->getSaveFileName()!=nullptr)
    //        return;
    QFile file(saveURL);
    //file.write(buffer);
    if (!file.open(QFile::ReadWrite))
        return;
    file.write(buffer);
    file.close();
}
