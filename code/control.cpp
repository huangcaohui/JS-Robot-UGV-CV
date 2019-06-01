#include "control.h"

namespace JUSTUGV
{
Control::Control()
{
    //初始化摄像头
    if(!camera.init(0))
    {
        qDebug() << "[control.cpp] [error]: Camera inits unsuccessfully!" << endl;
    }

    if(serial.init("COM4"))
    {
        qDebug() << "[control.cpp]: Serial inits successfully!" << endl;
    }
}

void Control::run()
{
    Mat frame;

    while(true)
    {        
        //按顺序抓取和检索视频的每一个帧
        camera >> frame;

        //视频播放完毕跳出程序
        if(frame.empty())
        {
            break;
        }

        int type = detector.targetType(frame);

        //向串口写入相对坐标
        serial.writeBytes(type);

        //imshow("camera", frame);

        waitKey(1);
    }

    destroyAllWindows();
}
}
