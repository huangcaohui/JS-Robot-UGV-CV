#ifndef SERIAL_H
#define SERIAL_H

/*公共头文件*/
#include "common.h"

/*QT库*/
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>

namespace JUSTUGV
{
//! @addtogroup device
//! @{

/**
  * @brief 串口通信模块
  * @details 提供和电控部分串口通信功能，发送识别到的物体
  */

class Serial: public QSerialPort
{
    Q_OBJECT

public:
    /**
      * @brief 初始化
      *
      */
    Serial();

    /**
     * @brief 初始化串口，设定串口通信参数
     * @param[in] portName 串口名称
     * @return 串口初始化吃否成功
     *         返回true,表示初始化成功；
     *         返回false，表示初始化出现错误
     */
    bool init(QString portName);

    /**
     * @brief 向串口写入目标识别结果
     * @param[in] 输入的检测到的目标类型
     * @return null
     */
    void writeBytes(int type);

private:
    short HEAD = 15; // equal to 01111
    short TAIL = 7; // equal to 00111
};
//! @}

}

#endif // SERIAL_H
