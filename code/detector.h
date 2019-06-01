#ifndef DETECTOR_H
#define DETECTOR_H

/*公共头文件*/
#include "common.h"

namespace JUSTUGV
{
//! @addtogroup recognition
//! @{

/**
  * @brief 识别模块
  * @details 识别减速带，人行道，锥形标
  */

class Detector
{
public:
    /**
     * @brief 初始化
     *
     */
    Detector();

    /**
     * @brief 返回检测到的目标类型
     * @param[in] srcImage 输入的摄像头捕获图像
     * @return 检测到的目标类型
     *         1为锥形标
     *         2为减速带
     *         3为人行道
     *         4为未检测到目标；
     */
    int targetType(Mat srcImage);

    /**
     * @brief 对摄像头获取的图片进行预处理
     * @param[in] srcImage 输入的摄像头捕获图像
     * @return 预处理之后的图像
     */
    Mat preprocess(Mat grayImage);

    /**
     * @brief 对摄像头获取的图片进行预处理
     * @param[in] srcImage 输入的摄像头捕获图像
     * @return 预处理之后的图像
     */
    Mat gainRedImg(Mat srcImage);

    /**
     * @brief 大津算法，实现前景与背景的分割
     * @param[in] grayImage
     * @return 最适应分割阈值
     */
    int ostu(Mat grayImage);

    /**
     * @brief 通过二值化图判别减速带与人行道
     * @param[in] threshImg 经过预处理后的二值化图
     * @param[in] srcImage 输入的原图像
     * @return 检测到的目标类型
     *         1为锥形标
     *         2为减速带
     *         3为人行道
     *         4为未检测到目标；
     */
    int distinguishType(Mat threshImg, Mat redImage, Mat srcImage);

    /**
     * @brief judgeSide
     * @param[in] sidePoints 轮廓的底边中点
     * @return
     */
    bool judgeSide(vector<Point> sidePoints);

    /**
     * @brief 判断人行道
     * @param[in] rotatedRect 轮廓的外接矩形
     * @param[in] imageSize 图片的大小
     * @return
     */
    bool judgeCross(RotatedRect rotatedRect, Size imageSize);

    /**
     * @brief 锥形标判断
     * @param[in] contour 输入的待判断的轮廓
     * @param[in] imageSize 输入图形的大小
     * @param[in] srcImage 待观察的图像
     * @return 检测到的目标类型
     *         0表示未检测到东西
     *         1表示检测到锥形标
     *         2表示检测到人行道
     */
    int distinguishTower(vector<Point> contour, Size imageSize, Mat &srcImage);

    /**
     * @brief 获取人行道条带的外接矩形底边的中点
     * @param[in] rotatedRect 人行道条带的外接矩形
     * @return 人行道条带的外接矩形底边的中点
     */
    Point gainMidpoint(RotatedRect rotatedRect);
};

//! @}
}

#endif // DETECTOR_H
