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


SerialWidget::SerialWidget(QWidget *parent) : QMainWindow(parent)
{
    serial = new QSerialPort;
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
    ckbHexRecv = new QCheckBox(tr("十六进制显示"));
    ckbHexSend = new QCheckBox(tr("十六进制发送"));
    ckbAutoClear = new QCheckBox(tr("自动清空"));
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
    layout2->addWidget(ckbAutoClear,1,0,1,2);
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

    txtRecv = new QTextEdit;
    txtSend = new QTextEdit;

    centerLayout->addWidget(txtRecv);
    QHBoxLayout *layout1 = new QHBoxLayout;
    layout1->addWidget(txtSend);
    layout1->addWidget(btnOpen);
    //centerLayout->addWidget(txtSend);
    centerLayout->addLayout(layout1);
    centerLayout->setStretch(0,3);
    centerLayout->setStretch(1,1);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(centerLayout);
    centerWidget->setLayout(mainLayout);
    setCentralWidget(centerWidget);
    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    statusBar->showMessage(tr("COM2 CLOSED"));



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
    QStringList listBaudRate ;
    listBaudRate <<"1200"<<"2400"<<"4800"<<"9600"<<"19200"<<"38400"<<"115200"<<"Custom";
    comboBaudRate->addItems(listBaudRate);
    comboBaudRate->setCurrentIndex(3);

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
}

void SerialWidget::initMenu()
{
    menu = new QMenu(tr("文件"),this);
    startAction = menu->addAction(QIcon(":/images/start.png"),tr("开始"));
    addAction = menu->addAction(QIcon(":/images/add.png"),tr("增加"));
    reduceAction = menu->addAction(QIcon(":/images/reduce.png"),tr("减少"));
    stopAction = menu->addAction(QIcon(":/images/stop.png"),tr("停止"));
    pauseAction = menu->addAction(QIcon(":/images/pause.png"),tr("暂停"));
    exitAction = menu->addAction(QIcon(":/images/exit.png"),tr("退出"));
    menuBar = new QMenuBar(this);
    menuBar->addMenu(menu);
    toolBar = new QToolBar(this);
    toolBar->addAction(startAction);
    toolBar->addAction(reduceAction);
    toolBar->addSeparator();
    toolBar->addAction(stopAction);
    toolBar->addAction(pauseAction);
    toolBar->addAction(addAction);
    toolBar->addAction(exitAction);
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

    if(btnOpen->text() == tr("打开串口"))
    {
        if(serial->open(QIODevice::ReadWrite)){
            btnOpen->setText(tr("关闭串口"));
        }
        else{
            QMessageBox::warning(this,tr("提示"),tr("串口打开失败!"),QMessageBox::Yes);
        }
    }
    else{
        serial->close();
        btnOpen->setText(tr("打开串口"));
    }


}

void SerialWidget::readData()
{
    const QByteArray data = serial->readAll();
    txtRecv->append(data);
}

//发送数据
void SerialWidget::sendData()
{
    if(!serial->isOpen()){
        QMessageBox::warning(this,tr("提示"),tr("串口未打开，请先打开串口"),QMessageBox::Yes);
        return;
    }
    QString data = this->txtSend->toPlainText();

    QByteArray arrData = data.toLatin1();
    if(asciiSend->isChecked()){
        serial->write(arrData);
    }
    else{
        QByteArray temp = hexStringToByteArray(arrData);
        serial->write(temp);
    }
}

void SerialWidget::clearReadTxt()
{
    this->txtRecv->clear();
}

void SerialWidget::clearSendTxt()
{
    this->txtSend->clear();
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
        hstr=hexString[i].toLatin1();   //字符型
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
