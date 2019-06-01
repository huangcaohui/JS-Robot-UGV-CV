#include "camera.h"

namespace JUSTUGV
{
Camera::Camera()
{

}

bool Camera::init(int cameraId)
{
    FileStorage fs("F:/QT Projects/JS_Robot_UGV/params.xml", FileStorage::READ);
    if(!fs.isOpened())
    {
        cout << "[camera.cpp] [error]: Open file failed" << endl;
    }

    FileNode node = fs["camera_parameter"];

    node["brightness"] >> params.brightness;
    node["contrast"] >> params.contrast;
    node["hue"] >> params.hue;
    node["saturation"] >> params.saturation;
    node["pan"] >> params.pan;
    node["gamma"] >> params.gamma;
    node["white_balance_red_v "] >> params.white_balance_red_v;
    node["exposure"] >> params.exposure;

    //初始化并分配内存以从摄像机加载视频流
    srcFile.open(cameraId);
    srcFile.set(CAP_PROP_FRAME_WIDTH, 640);
    srcFile.set(CAP_PROP_FRAME_HEIGHT, 480);
    srcFile.set(CAP_PROP_SETTINGS, -1);
//    srcFile.set(CAP_PROP_BRIGHTNESS, params.brightness);
//    srcFile.set(CAP_PROP_CONTRAST, params.contrast);
//    srcFile.set(CAP_PROP_HUE, params.hue);
//    srcFile.set(CAP_PROP_SATURATION, params.saturation);
//    srcFile.set(CAP_PROP_PAN, params.pan);
//    srcFile.set(CAP_PROP_GAMMA, params.gamma);
//    srcFile.set(CAP_PROP_WHITE_BALANCE_RED_V, params.white_balance_red_v);
//    srcFile.set(CAP_PROP_EXPOSURE, params.exposure);

    return srcFile.isOpened();
}

VideoCapture &Camera::operator >>(Mat &frame)
{
    srcFile >> frame;
    return srcFile;
}
}
