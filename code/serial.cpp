#include "serial.h"

namespace JUSTUGV
{
Serial::Serial()
{

}

bool Serial::init(QString portName)
{
    //串口信息类，用于读取可用串口所有的信息
    QSerialPortInfo serialPortInfo;
    foreach(serialPortInfo, QSerialPortInfo::availablePorts())
    {
        if(serialPortInfo.portName() == portName)
        {
            setPort(serialPortInfo);
            break;
        }
    }

    //输出所有串口的相关信息
    qDebug() << serialPortInfo.portName() << endl
             << serialPortInfo.description() << endl;

    if(open(QIODevice::ReadWrite))
    {
        qDebug() << "[serial.cpp]: open(QIODevice::ReadWrite)";
        setBaudRate(QSerialPort::Baud115200);
        setParity(QSerialPort::NoParity);
        setDataBits(QSerialPort::Data8);
        setStopBits(QSerialPort::OneStop);
        setFlowControl(QSerialPort::NoFlowControl);

        clearError();
        clear();

        return true;
    }

    return false;
}

void Serial::writeBytes(int type)
{
    //待写入数据缓冲区
    QByteArray buffer;

    //向缓冲区添加表示一个短整型数的五位数据位
    buffer.append(reinterpret_cast<char*>(&HEAD));
    buffer.append(reinterpret_cast<char*>(&type));
    buffer.append(reinterpret_cast<char*>(&TAIL));

    //显示被写入的数据
    for(int i = 0; i < buffer.size(); i++)
    {
        qDebug() << "buffer:" << static_cast<int>(buffer[i]);
    }
    qDebug() << endl;

    //写入数据
    write(buffer);
    waitForBytesWritten(1);
}
}
