#include "serialwidget.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QSplitter>
#include <QCheckBox>
#include <QLineEdit>
#include <QSerialportInfo>
#include <QMessageBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QActionGroup>
#include <QPlainTextEdit>
#include <QTimer>
#include <QTime>
#include <QDateTime>


SerialWidget::SerialWidget(QWidget *parent) : QMainWindow(parent)
  ,curPortStatus(false)
{
    serial = new QSerialPort;
    timer = new QTimer(this);
    timer->setInterval(1000);
    sendSize = 0;
    recvSize = 0;


    initMenu();
    initWidgets();
    initPortSet();
    initConnections();
    resize(800,400);
}

SerialWidget::~SerialWidget()
{

}
//初始化界面
void SerialWidget::initWidgets()
{
    setWindowTitle(tr("串口调试助手"));
    centerWidget = new QWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *leftLayout = new QVBoxLayout;
    QVBoxLayout *centerLayout = new QVBoxLayout;


    QFormLayout *setLayout = new QFormLayout;
    comboPortNo = new QComboBox;
    comboBaudRate = new QComboBox;
    comboParity = new QComboBox;
    comboDataBits = new QComboBox;
    comboStopBits = new QComboBox;
    comboFlowControl = new QComboBox;

    labelPortStatus = new QLabel(tr("关闭"));
    btnOpen = new QPushButton(tr("打开"));

    setLayout->addRow(tr("串口"),comboPortNo);
    setLayout->addRow(tr("波特率"),comboBaudRate);
    setLayout->addRow(tr("数据位"),comboDataBits);
    setLayout->addRow(tr("校验位"),comboParity);
    setLayout->addRow(tr("停止位"),comboStopBits);
    setLayout->addRow(tr("流控"),comboFlowControl);
 //   setLayout->addRow(btnOpen);

    btnSend = new QPushButton(tr("发送"));
    btnClearRecv = new QPushButton(tr("清空接收"));
    btnClearSend = new QPushButton(tr("清空发送"));
    ckbShowTime = new QCheckBox(tr("显示时间"));
    ckbAutoNewLine = new QCheckBox(tr("自动换行"));
    ckbShowSend = new QCheckBox(tr("显示发送"));


    asciiRcv = new QRadioButton(tr("Ascii"));
    asciiRcv->setChecked(true);
    hexRcv = new QRadioButton(tr("Hex"));

    groupComSet = new QGroupBox(tr("串口设置"),this);
    groupComSet->setLayout(setLayout);

    QGridLayout *layout2 = new QGridLayout;
    layout2->addWidget(asciiRcv,0,0);
    layout2->addWidget(hexRcv,0,1);
    layout2->addWidget(ckbShowTime,1,0,1,2);
    layout2->addWidget(ckbAutoNewLine,2,0,1,2);
    layout2->addWidget(ckbShowSend,3,0,1,2);
  //  layout2->addWidget(btnClearRecv,4,0,1,2);

    groupRecvSet = new QGroupBox(tr("接收设置"),this);
    groupRecvSet->setLayout(layout2);


    asciiSend = new QRadioButton(tr("Ascii"));
    asciiSend->setChecked(true);
    hexSend = new QRadioButton(tr("Hex"));

    spinSendPeriod = new QSpinBox;
    spinSendPeriod->setRange(0,1000000);
    spinSendPeriod->setValue(1000);
    ckbAutoSend = new QCheckBox(tr("自动重发"));
    QGridLayout *layout4 = new QGridLayout;
    QLabel *label = new QLabel("ms");
    layout4->addWidget(asciiSend,0,0);
    layout4->addWidget(hexSend,0,1);
    layout4->addWidget(ckbAutoSend,1,0);
    layout4->addWidget(spinSendPeriod,1,1);
    layout4->addWidget(label,1,2);
   // layout4->addWidget(btnSend,2,0,1,2);



    groupSendSet = new QGroupBox(tr("发送设置"),this);
    groupSendSet->setLayout(layout4);

    leftLayout->addWidget(groupComSet);
    leftLayout->addWidget(groupRecvSet);
    leftLayout->addWidget(groupSendSet);

    txtRecv = new QPlainTextEdit;
    txtSend = new QTextEdit;

    centerLayout->addWidget(txtRecv);

    QVBoxLayout *layout5 = new QVBoxLayout;
    layout5->addWidget(btnOpen);
    layout5->addWidget(btnClearSend);

    QHBoxLayout *layout1 = new QHBoxLayout;
    layout1->addWidget(txtSend);
    layout1->addLayout(layout5);


    //centerLayout->addWidget(txtSend);
    centerLayout->addLayout(layout1);
    centerLayout->setStretch(0,3);
    centerLayout->setStretch(1,1);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(centerLayout);
    mainLayout->setStretch(0,1);
    mainLayout->setStretch(1,3);
    centerWidget->setLayout(mainLayout);
    setCentralWidget(centerWidget);




    comStatus = new QLabel(this);
    comStatus->setMinimumWidth(this->width()/3);
    comStatus->setStyleSheet(QString("QLabel{color:rgb(255, 0, 0);font: 13pt \"ADMUI3Lg\" bold;}"));
    comStatus->setText(tr("CLOSED"));
    recvStatus = new QLabel(this);
    recvStatus->setMinimumWidth(this->width()/4);
    recvStatus->setText(tr("Rx:0 Bytes"));
    sendStatus = new QLabel(this);
    sendStatus->setText(tr("Tx:0 Bytes"));
    sendStatus->setMinimumWidth(this->width()/4);
    placeHolderStatus = new QLabel(this);
    placeHolderStatus->setText(tr(""));
    placeHolderStatus->setMinimumWidth(this->width());


    statusBar()->addWidget(comStatus);
    statusBar()->addWidget(recvStatus);
    statusBar()->addWidget(sendStatus);
    statusBar()->addWidget(placeHolderStatus);


}

//初始化串口设置
void SerialWidget::initPortSet()
{
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    if(!list.isEmpty()){
        foreach(QSerialPortInfo info,list){
            comboPortNo->addItem(info.portName());
        }
    }
    //设置波特率
    foreach(auto const &baudRate,QSerialPortInfo::standardBaudRates()){
        comboBaudRate->addItem(QString::number(baudRate));
    }
    comboBaudRate->setCurrentText(tr("9600"));
//    QStringList listBaudRate ;
//    listBaudRate <<"1200"<<"2400"<<"4800"<<"9600"<<"19200"<<"38400"<<"115200"<<"Custom";
//    comboBaudRate->addItems(listBaudRate);
//    comboBaudRate->setCurrentIndex(3);

    //设置数据位
    QStringList listDataBits;
    listDataBits<<"5"<<"6"<<"7"<<"8";
    comboDataBits->addItems(listDataBits);
    comboDataBits->setCurrentIndex(3);

    //设置校验位
    QStringList listParity;
    listParity <<"None(无)"<<"Odd(偶)"<<"Even(奇)"<<"Mark(=1)"<<"Space(=0)";
    comboParity->addItems(listParity);
    comboParity->setCurrentIndex(0);

    //设置停止位
    QStringList listStopBits;
    listStopBits<<"1"<<"1.5"<<"2";
    comboStopBits->addItems(listStopBits);
    comboStopBits->setCurrentIndex(0);

    //设置流控
    QStringList listFlowControl;
    listFlowControl<<"None"<<"RTS/CTS"<<"XON/XOFF";
    comboFlowControl->addItems(listFlowControl);
    comboFlowControl->setCurrentIndex(0);

}

void SerialWidget::initConnections()
{
    connect(btnOpen,&QPushButton::clicked,this,&SerialWidget::openPort);
    connect(serial,&QSerialPort::readyRead,this,&SerialWidget::readData);
    connect(btnSend,&QPushButton::clicked,this,&SerialWidget::sendData);
    connect(btnClearRecv,&QPushButton::clicked,this,&SerialWidget::clearReadTxt);
    connect(btnClearSend,&QPushButton::clicked,this,&SerialWidget::clearSendTxt);

    connect(exitAction,&QAction::triggered,this,&QWidget::close);
    connect(startAction,&QAction::triggered,this,&SerialWidget::openPort);
    connect(clearAction,&QAction::triggered,this,&SerialWidget::clearReadTxt);
    connect(pauseAction,&QAction::triggered,this,&SerialWidget::pauseStatus);
    connect(operateActions,&QActionGroup::triggered,this,&SerialWidget::updateOperateStatus);
    connect(stopAction,&QAction::triggered,this,&SerialWidget::closePort);

    connect(comboPortNo, QOverload<int>::of(&QComboBox::currentIndexChanged),this,&SerialWidget::changePortNo);
    connect(comboBaudRate,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&SerialWidget::changePortNo);
    connect(comboDataBits,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&SerialWidget::changePortNo);
    connect(comboFlowControl,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&SerialWidget::changePortNo);
    connect(comboStopBits,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&SerialWidget::changePortNo);
    connect(comboFlowControl,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&SerialWidget::changePortNo);
    connect(spinSendPeriod,QOverload<int>::of(&QSpinBox::valueChanged),[this]{
        timer->setInterval(spinSendPeriod->value());
    });
    //定时器
    connect(ckbAutoSend, &QCheckBox::clicked, [this](bool checked) {
        if (checked)
            timer->start();
        else
            timer->stop();
    });

    connect(timer,&QTimer::timeout,this,&SerialWidget::sendData);


}

void SerialWidget::initMenu()
{
    fileMenu = new QMenu(tr("文件(&F)"),this);
    editMenu = new QMenu(tr("编辑(&E)"),this);
    viewMenu = new QMenu(tr("视图(&V)"),this);
    toolMenu = new QMenu(tr("工具(&T)"),this);
    helpMenu = new QMenu(tr("帮助(&H)"),this);



    logAction = fileMenu->addAction(QIcon(":/images/log.png"),tr("记录日志"));
    exitAction = fileMenu->addAction(QIcon(":/images/exit.png"),tr("退出"));

    startAction = editMenu->addAction(QIcon(":/images/start.png"),tr("开始"));
 //   addAction = editMenu->addAction(QIcon(":/images/add.png"),tr("增加"));
 //   reduceAction = editMenu->addAction(QIcon(":/images/reduce.png"),tr("减少"));
    pauseAction = editMenu->addAction(QIcon(":/images/pause.png"),tr("暂停"));
    stopAction = editMenu->addAction(QIcon(":/images/stop.png"),tr("停止"));
    clearAction = editMenu->addAction(QIcon(":/images/clear.png"),tr("清除"));
    setAction = toolMenu->addAction(QIcon(":/images/set.png"),tr("设置"));

    operateActions = new QActionGroup(this);
    operateActions->addAction(startAction);
    operateActions->addAction(pauseAction);
    operateActions->addAction(stopAction);
    stopAction->setCheckable(true);
    stopAction->setChecked(true);

    menuBar = new QMenuBar(this);
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(editMenu);
    menuBar->addMenu(toolMenu);
    toolBar = new QToolBar(this);
    toolBar->addAction(logAction);
    toolBar->addSeparator();
    toolBar->addAction(startAction);
    toolBar->addAction(pauseAction);
    toolBar->addAction(stopAction);
    toolBar->addAction(clearAction);
    toolBar->addSeparator();
    toolBar->addAction(setAction);
    toolBar->addSeparator();
    setMenuBar(menuBar);
    addToolBar(toolBar);
}

QSerialPort::Parity SerialWidget::getSerialParity(int index)
{
    QSerialPort::Parity parity;
    switch (index) {
    case 0:
        parity = QSerialPort::NoParity;
        break;
    case 1:
        parity = QSerialPort::EvenParity;
        break;
    case 2:
        parity = QSerialPort::OddParity;
        break;
    case 3:
        parity = QSerialPort::SpaceParity;
        break;
    case 4:
        parity = QSerialPort::MarkParity;
        break;
    default:
        parity = QSerialPort::UnknownParity;
        break;
    }
    return parity;
}

QSerialPort::StopBits SerialWidget::getSerialStopBits(int index)
{
    QSerialPort::StopBits stopBits;
    switch (index) {
    case 0:
        stopBits = QSerialPort::OneStop;
        break;
    case 1:
        stopBits = QSerialPort::OneAndHalfStop;
        break;
    case 2:
        stopBits = QSerialPort::TwoStop;
        break;
    default:
        stopBits = QSerialPort::UnknownStopBits;
        break;
    }
    return stopBits;
}

QSerialPort::DataBits SerialWidget::getSerialDataBits(int index)
{
    QSerialPort::DataBits dataBits;
    switch (index) {
    case 0:
        dataBits = QSerialPort::Data5;
        break;
    case 1:
        dataBits = QSerialPort::Data6;
        break;
    case 2:
        dataBits = QSerialPort::Data7;
        break;
    case 3:
        dataBits = QSerialPort::Data8;
        break;
    default:
        dataBits = QSerialPort::UnknownDataBits;
        break;
    }
    return dataBits;
}

QSerialPort::FlowControl SerialWidget::getSerialFlowControl(int index)
{
    QSerialPort::FlowControl flowControl;
    switch (index) {
    case 0:
        flowControl = QSerialPort::NoFlowControl;
        break;
    case 1:
        flowControl = QSerialPort::HardwareControl;
        break;
    case 2:
        flowControl = QSerialPort::SoftwareControl;
        break;
    default:
        flowControl = QSerialPort::UnknownFlowControl;
        break;
    }
    return flowControl;
}



void SerialWidget::openPort()
{
    portSet.portName = comboPortNo->currentText();
    portSet.baudRate = comboBaudRate->currentText().toInt();
    portSet.parity = getSerialParity(comboParity->currentIndex());
    portSet.stopBits = getSerialStopBits(comboStopBits->currentIndex());
    portSet.dataBits = getSerialDataBits(comboDataBits->currentIndex());
    portSet.flowControl = getSerialFlowControl(comboFlowControl->currentIndex());

    serial->setPortName(portSet.portName);
    serial->setBaudRate(portSet.baudRate);
    serial->setParity(portSet.parity);
    serial->setStopBits(portSet.stopBits);
    serial->setDataBits(portSet.dataBits);
    serial->setFlowControl(portSet.flowControl);

    if(btnOpen->text() == tr("打开")){
        if(!curPortStatus){
            if(serial->open(QIODevice::ReadWrite)){
                btnOpen->setText(tr("发送"));
                startAction->setCheckable(true);
                startAction->setChecked(true);
                curPortStatus = true;
                comStatus->setText(tr("%1 OPENED,%2,%3,%4,%5,%6").arg(portSet.portName)
                                   .arg(portSet.baudRate)
                                   .arg(portSet.dataBits)
                                   .arg(portSet.parity)
                                   .arg(portSet.stopBits)
                                   .arg(portSet.flowControl));

                comStatus->setStyleSheet(QString("QLabel{color:rgb(0, 128, 0);font: 12pt \"ADMUI3Lg\" bold;}"));
            }
            else{
                //QMessageBox::warning(this,tr("提示"),tr("串口打开失败!"),QMessageBox::Yes);
                  comStatus->setText(tr("%1 打开失败").arg(portSet.portName));
            }
        }
    }
    else if(btnOpen->text() == tr("发送")){
        sendData();//发送数据
    }
}

void SerialWidget::closePort()
{
     serial->close();
     curPortStatus = false;
     btnOpen->setText(tr("打开"));
     comStatus->setText(tr("%1 CLOSED").arg(portSet.portName));
     comStatus->setStyleSheet(QString("QLabel{color:rgb(255, 0, 0);font: 12pt \"ADMUI3Lg\" bold;}"));
}

void SerialWidget::readData()
{
    const QByteArray data = serial->readAll();

    if(txtRecv->blockCount()>100)
    {
        txtRecv->clear();
    }
    //十六进制接收
    if(hexRcv->isChecked()){
        QString strData = data.toHex();
//        if(ckbShowTime->isChecked())
//        {
//            txtRecv->insertPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
//            txtRecv->insertPlainText("  ");
//        }
        for(int i=0;i<strData.length();i=i+2){
            txtRecv->insertPlainText(strData.mid(i,2));
            txtRecv->insertPlainText(tr(" "));
        }

        if(ckbAutoNewLine->isChecked()){        //换行
            txtRecv->insertPlainText(tr("\r\n"));
        }
    }
    else{
//        if(ckbShowTime->isChecked())
//        {
//            txtRecv->insertPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
//            txtRecv->insertPlainText("  ");
//            txtRecv->insertPlainText(data);
//            txtRecv->insertPlainText(tr("\r\n"));
//        }
//        else
            txtRecv->insertPlainText(data);

    }
    recvSize += data.size();
    recvStatus->setText(tr("Rx:%1 Bytes").arg(recvSize));

}

//发送数据
void SerialWidget::sendData()
{
    if(!curPortStatus){
        comStatus->setText(tr("串口未打开"));
        comStatus->setStyleSheet(QString("QLabel{color:rgb(0, 255, 25);font: 12pt \"ADMUI3Lg\" bold;}"));
        return;
    }
    int len = 0;
    QString data = this->txtSend->toPlainText();

    QByteArray arrData = data.toLatin1();
    len = arrData.size();

    if(len > 0)//有数据时
    {
        if(asciiSend->isChecked()){
            sendSize += len;
            serial->write(arrData);
            sendStatus->setText(tr("Tx:%1 Bytes").arg(sendSize));
        }
        else{
            arrData = hexStringToByteArray(arrData);
            serial->write(arrData);
        }

        if(ckbShowSend->isChecked())
        {
            this->txtRecv->appendPlainText(QString(arrData));
        }
    }

}

void SerialWidget::clearReadTxt()
{
    this->txtRecv->clear();
    sendSize = 0;
    sendStatus->setText(tr("Tx:%1 Bytes").arg(sendSize));

}

void SerialWidget::clearSendTxt()
{
    this->txtSend->clear();
}

void SerialWidget::pauseStatus()
{

}


void SerialWidget::updateOperateStatus(QAction *action)
{
    action->setCheckable(true);
    action->setChecked(true);
}

void SerialWidget::changePortNo(int index)
{
    if(curPortStatus){
        closePort();
        openPort();
    }
}



//字符串转16进制发送
QByteArray SerialWidget::hexStringToByteArray(QString hexString)
{
    QByteArray senddata;
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = hexString.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
       hstr=hexString[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = hexString[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}

int SerialWidget::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-'0';
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}
