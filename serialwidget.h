#ifndef SERIALWIDGET_H
#define SERIALWIDGET_H
#include <QMainWindow>
#include <QSerialPort>

class QWidget;
class QComboBox ;
class QGroupBox;
class QPushButton;
class QTextEdit;
class QCheckBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSpinBox;
class QStatusBar;
class QActionGroup;
class QPlainTextEdit;
class QTimer;

class SerialWidget : public QMainWindow
{
    Q_OBJECT
public:
    explicit SerialWidget(QWidget *parent = nullptr);
    ~SerialWidget();
    struct setting
    {
        QString portName;
        QSerialPort::DataBits dataBits;
        qint32  baudRate;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;

    };
    QSerialPort *serial;
    QWidget *centerWidget;

private:
    setting portSet;
    QComboBox *comboPortNo;//串口号
    QComboBox *comboBaudRate;//波特率
    QComboBox *comboParity;//校验位
    QComboBox *comboDataBits;//数据位
    QComboBox *comboStopBits;//停止位
    QComboBox *comboFlowControl;//流控
    QComboBox *comboTxt;//文本记录
    QGroupBox *groupComSet;
    QGroupBox *groupSendSet;
    QGroupBox *groupStatus;
    QGroupBox *groupRecvSet;

    QPushButton *btnOpen;//打开串口
    QPushButton *btnClearRecv;//清空
    QPushButton *btnClose;//关闭
    QPushButton *btnStopShow;//停止显示
    QPushButton *btnSend;//发送
    QPushButton *btnClearSend;//清空发送

    QPlainTextEdit *txtRecv;//接收区
    QTextEdit *txtSend;//发送区
    QCheckBox *ckbAutoClear;//自动清空
    QCheckBox *ckbAutoNewLine;//自动换行
    QCheckBox *ckbShowSend;//显示发送

    QRadioButton *asciiRcv;//Ascii格式接收
    QRadioButton *hexRcv;//十六进制接收

    QRadioButton *asciiSend;//ascii发送
    QRadioButton *hexSend;//十六进制发送
    QLabel *labelPortStatus;//串口状态
    QSpinBox *spinSendPeriod;//发送周期
    QCheckBox *ckbAutoSend;//自动发送

    QMenu *fileMenu;//文件
    QMenu *editMenu;//编辑
    QMenu *viewMenu;//视图
    QMenu *toolMenu;//工具
    QMenu *helpMenu;//帮助
    QMenuBar *menuBar;
    QToolBar *toolBar;

    QAction *exitAction;//退出
    QAction *logAction;//日志
    QAction *openDirAction;//打开文件夹

    QAction *addAction;
    QAction *reduceAction;
    QAction *startAction;
    QAction *pauseAction;
    QAction *stopAction;
    QAction *clearAction;
    QAction *setAction;

    QActionGroup *operateActions;
    QTimer *timer;
    bool curPortStatus;

protected:
    void initWidgets();
    void initPortSet();
    void initConnections();
    void initMenu();

    QSerialPort::Parity getSerialParity(int index);
    QSerialPort::StopBits getSerialStopBits(int index);
    QSerialPort::DataBits getSerialDataBits(int index);
    QSerialPort::FlowControl getSerialFlowControl(int index);

    QByteArray hexStringToByteArray(QString hexString);
    int ConvertHexChar(char ch);
    QStatusBar *statusBar;

signals:

public slots:
    void openPort();
    void closePort();
    void readData();
    void sendData();
    void clearReadTxt();
    void clearSendTxt();
    void pauseStatus();
    void updateOperateStatus(QAction* action);
    void changePortNo(int index);
};

#endif // SERIALWIDGET_H
