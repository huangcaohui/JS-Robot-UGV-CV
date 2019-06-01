/**
* @defgroup device 设备驱动模块组
*
* @defgroup recognition 图像识别模块
*
* @defgroup control 总控制模块组
*/

#ifndef CONTROL_H
#define CONTROL_H

/*自定义库*/
#include "camera.h"
#include "detector.h"
#include "serial.h"

/**
 * @brief JUST CV code for JiangSu robot competition UGV.
 */
namespace JUSTUGV
{
//! @addtogroup control
//! @{

/**
  * @brief 系统总体逻辑控制
  * @details 包括双目摄取图像分割，xml参数导入，立体矫正，深度计算
  */

class Control
{
public:
    /**
     * @brief 初始化
     *
     */
    Control();

    /**
     * @brief 运行整体系统并显示运行结果
     * @return null
     */
    void run();

protected:
    //! 图像检测器，处理并找出图像中的三个目标
    Detector detector;

private:
    //! 摄像头
    Camera camera;

    //! 串口通信
    Serial serial;

};

//! @}
}

#endif // CONTROL_H
