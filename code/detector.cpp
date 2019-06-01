#include "detector.h"

namespace JUSTUGV
{
Detector::Detector()
{

}

int Detector::targetType(Mat srcImage)
{
    //读取中文路径图片
//    QString strPath = "/home/a/JS_Robot_UGV/Img/sidewalk.png";
//    string path = strPath.toLocal8Bit().toStdString();
//    srcImage = imread(path);

//    resize(srcImage, srcImage, Size(0, 0), 0.3, 0.3);

    //对摄像头获取图片进行预处理，获取H通道红色分量
    Mat threshImg = preprocess(srcImage);
    Mat redImage = gainRedImg(srcImage);

    int type = distinguishType(threshImg, redImage, srcImage);

    return type;
}

Mat Detector::preprocess(Mat srcImage)
{
    Mat grayImage, threshImg;

    //灰度图
    cvtColor(srcImage, grayImage, CV_BGR2GRAY);

    //均值滤波
    blur(grayImage, grayImage, Size(7, 7));

    //二值化
    threshold(grayImage, threshImg, 200, 255, THRESH_BINARY);

    //中值滤波，去除噪声
    medianBlur(threshImg, threshImg, 5);

    //闭操作，填充前景中的小孔
    Mat kernel = getStructuringElement(MORPH_RECT, Size(10, 10));
    morphologyEx(threshImg, threshImg, MORPH_CLOSE, kernel);

    imshow("threshImg", threshImg);

    return threshImg;
}

Mat Detector::gainRedImg(Mat srcImage)
{
    Mat dstImage, hsvImages[3], redImage_a, redImage_b, redImage;

    //获取H通道红色分量
    cvtColor(srcImage, dstImage, CV_BGR2HSV);
    split(dstImage, hsvImages);
    inRange(hsvImages[0], 0, 10, redImage_a);
    inRange(hsvImages[0], 170, 180, redImage_b);
    bitwise_or(redImage_a, redImage_b, redImage);
    medianBlur(redImage, redImage, 9);

    imshow("red", redImage);

    return redImage;
}

int Detector::ostu(Mat grayImage)
{
    //最佳阈值
    int threshold = 0;
    //最大类方差
    double maxVariance = 0;
    //前景与背景像素点所占比例
    double w0=0, w1=0;
    //前景与背景像素值平均灰度
    double u0=0, u1=0;
    //灰度直方图数组
    int histogram[256];
    memset(histogram, 0, sizeof(histogram));
    //图像总大小
    int Num = grayImage.cols*grayImage.rows;

    //统计256个bin，每个bin像素的个数
    for(int i=0; i < grayImage.rows; i++)
    {
        const uchar *p = grayImage.ptr<uchar>(i);

        for(int j=0; j < grayImage.cols; j++)
        {
            histogram[int(*p++)]++;
        }
    }

    //前景像素统计
    for(int i = 50; i <= 200; i++)
    {
        w0=0; w1=0;
        u0=0; u1=0;

        int *p = &histogram[0];

        //前景像素统计
        for(int j = 0; j <= i; j++)
        {
            //以i为阈值，统计前景像素个数
            w0 = w0 + *p;
            //以i为阈值，统计前景像素灰度总和
            u0 = u0 + j*(*p++);
        }
        w0 = w0/Num; u0 = u0/w0;

        p = &histogram[i+1];

        //背景像素统计
        for(int j = i + 1; j <= 255; j++)
        {
            //以i为阈值，统计背景像素个数
            w1 = w1 + *p;
            //以i为阈值，统计背景像素灰度总和
            u1 = u1 + j*(*p++);
        }
        w1 = w1/Num; u1 = u1/w1;

        //当前类间方差计算
        double variance = w0*w1*(u1 - u0)*(u1 - u0);

        if(variance > maxVariance)
        {
            maxVariance = variance;
            threshold = i;
        }
    }

    cout << "threshold:" << threshold << endl;

    return threshold;
}

int Detector::distinguishType(Mat threshImg, Mat redImage, Mat srcImage)
{
    Mat cloneImage = srcImage.clone();

    //找到所有轮廓并建立继承关系
    vector<vector<Point> > contours;
    vector<vector<Point> > red_contours;
    vector<Vec4i> hierarchy;

    //是否检测到减速带，人行道，锥形标
    bool speedBump = false;
    bool sidewalk = false;
    bool towerBeacon = false;
    bool crossJudge = false;

    //人行横道,锥形标底边中心，外接矩形，边长，面积
    vector<Point> centres_side, centres_tower;
    RotatedRect rotatedRect;
    Vec4f line;
    double longEdge = 0, shortEdge = 0;

    //轮廓图
//    Mat contoursImg(threshImg.rows, threshImg.cols, CV_8UC1, Scalar(0));

    //寻找轮廓
    findContours(threshImg, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
    findContours(redImage, red_contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    for(int i = 0; i < contours.size(); i++)
    {
//        if(hierarchy[i][2] > -1)
//            drawContours(contoursImg, contours, i, Scalar(255), 2);

//        cout << "hierarchy:" << i << '\t'
//                             << hierarchy[i][0] << '\t'
//                             << hierarchy[i][1] << '\t'
//                             << hierarchy[i][2] << '\t'
//                             << hierarchy[i][3] << '\t' << endl;

        //减速带判别
        if(hierarchy[i][2] != -1 && hierarchy[i][3] == -1
                && hierarchy[hierarchy[i][2]][2] == -1 && hierarchy[hierarchy[i][2]][3] == i)
        {
            //计算减速带带宽面积与非带宽面积
            double black = contourArea(contours[hierarchy[i][2]]);
            double white = contourArea(contours[i]) - black;

            if(6*black >= white && 6*white >= black)
                speedBump = true;
        }

        //人行道，锥形标判别
        if(hierarchy[i][3] == -1)
        {           
            rotatedRect = minAreaRect(contours[i]);
            longEdge = max(rotatedRect.size.height, rotatedRect.size.width);
            shortEdge = min(rotatedRect.size.height, rotatedRect.size.width);

            //int type = distinguishTower(contours[i], threshImg.size(), cloneImage);

            //判断停车线
            if(judgeCross(rotatedRect, threshImg.size()) == true)
                crossJudge = true;

            if(longEdge > 2*shortEdge)
            {
                centres_side.push_back(gainMidpoint(rotatedRect));
            }
        }       
    }

    for(int i = 0; i < red_contours.size(); i++)
    {
        int area = contourArea(red_contours[i]);
        if(area > 120)
            centres_tower.push_back(gainMidpoint(rotatedRect));
    }

    //锥形标判别
    if(centres_tower.size() > 1)
    {
        fitLine(centres_tower, line, CV_DIST_HUBER, 0, 0.01, 0.01);
        if(line[1] < 0.6*line[0])
            towerBeacon = true;
    }

    //人行道判别
    if(centres_side.size() > 2 && crossJudge == true)
    {
        fitLine(centres_side, line, CV_DIST_HUBER, 0, 0.01, 0.01);
        if(line[1] < 0.36*line[0])
            sidewalk = true;
    }

    if(speedBump == true)
        drawContours(cloneImage, contours, -1, Scalar(0, 255, 0), 2);
    else if(sidewalk == true)
        drawContours(cloneImage, contours, -1, Scalar(0, 0, 255), 2);
    else if(towerBeacon == true)
        drawContours(cloneImage, red_contours, -1, Scalar(255, 0, 0), 2);

    imshow("cloneImage", cloneImage);

    if(speedBump == true)
        return 2;
    else if(sidewalk == true)
        return 3;
    else if(towerBeacon == true)
        return 1;  
    else
        return 4;
}

bool Detector::judgeSide(vector<Point> sidePoints)
{
    int x[sidePoints.size()], differ[sidePoints.size() - 1];

    for(int i = 0; i < sidePoints.size(); i++)
        x[i] = sidePoints[i].x;

    sort(x, x+sidePoints.size());

    //获取两人行道之间的间隔
    for(int i = 0; i < sidePoints.size() - 1; i++)
        differ[i] = x[i+1] - x[i];

    //比较人行道间隔的长短
    if((differ[1] < 2*differ[0]) && (2*differ[1] > differ[0]))
        return true;

    return false;
}

bool Detector::judgeCross(RotatedRect rotatedRect, Size imageSize)
{
    Point2f corners[4], bottom[2];
    Point2f left[2], right[2];
    int a = 0, b = 0;

    //将外接矩形的四个点存入数组中
    rotatedRect.points(corners);

    //获取矩形的中心点的x坐标
    float centerx = 0;
    for(int i = 0; i < 4; i++)
    {
        centerx += corners[i].x;
    }
    centerx /= 4;

    //获取外接矩形底边的两端点
    for(int i = 0; i < 4; i++)
    {
        if(corners[i].x > centerx)
            right[a++] = corners[i];
        else
            left[b++] = corners[i];
    }

    //判断停车线
    if(right[0].x - left[0].x > 2*imageSize.width/3)
        return true;

    return false;
}

int Detector::distinguishTower(vector<Point> contour, Size imageSize, Mat &srcImage)
{
    vector<int> point[imageSize.height];
    int top = imageSize.height, bottom = 0;

    //存储每个轮廓的行对应的点
    for(int i = 0; i < contour.size(); i++)
    {
        if(contour[i].y >= 0 && contour[i].y < imageSize.height)
            point[contour[i].y].push_back(contour[i].x);
        if(contour[i].y < top)
            top = contour[i].y;
        if(contour[i].y > bottom)
            bottom = contour[i].y;
    }

    //用于存储轮廓每行在y轴上投影长度及左右两边的点
    int distance[bottom - top + 1];
    Point left[bottom - top + 1], right[bottom - top + 1];
    vector<int> *p = &point[top];

    //数组初始化为1
    for(int i = 0; i < bottom - top + 1; i++)
        distance[i] = 1;

    //寻找轮廓在y轴上的投影长度
    for(int i = 0; i < bottom - top + 1; i++)
    {
        if(p->size() > 1)
        {
            int leftPoint = *p->begin(), rightPoint = *p->begin();
            for(vector<int>::iterator j = p->begin(); j != p->end(); j++)
            {
                for(vector<int>::iterator k = p->begin(); k != p->end(); k++)
                {
                    if(abs(*j - *k) > distance[i])
                    {
                        distance[i] = abs(*j - *k);
                    }
                }

                //获取同一列最边上两点
                if(*j < leftPoint)
                    leftPoint = *j;
                if(*j > rightPoint)
                    rightPoint = *j;
            }

            left[i] = Point(leftPoint, i + top);
            right[i] = Point(rightPoint, i + top);
        }
        p++;
    }

    //获取投影最长边
    int *q = &distance[0];
    int longEdge = 0, rows = 0;

    for(int i = 0; i < bottom - top + 1; i++)
    {
        if(*q > longEdge)
        {
            longEdge = *q;
            rows = i;
        }
        q++;
    }

    //获取轮廓最长投影上方的边缘
    vector<Point> leftEdge, rightEdge;
    Point *m = &left[0], *n = &right[0];

    if(rows > 0)
    {
        for(int i = 0; i < rows; i++)
        {
            leftEdge.push_back(*m++);
            rightEdge.push_back(*n++);
        }
    }

    //获取两边夹角
    Vec4f line_l, line_r;
    if(!leftEdge.empty() && !rightEdge.empty())
    {
        fitLine(leftEdge, line_l, CV_DIST_HUBER, 0, 0.01, 0.01);
        fitLine(rightEdge, line_r, CV_DIST_HUBER, 0, 0.01, 0.01);
    }

    Point l(40*line_l[0]+line_l[2], 40*line_l[1]+line_l[3]);
    Point r(40*line_r[0]+line_r[2], 40*line_r[1]+line_r[3]);
    line(srcImage, l, Point(line_l[2], line_l[3]), Scalar(255, 255, 0), 2);
    line(srcImage, r, Point(line_r[2], line_r[3]), Scalar(0, 255, 255), 2);

    double thet, module_l, module_r;
    module_l = sqrt(pow(line_l[0], 2) + pow(line_l[1], 2));
    module_r = sqrt(pow(line_r[0], 2) + pow(line_r[1], 2));
    thet = acos((line_l[0]*line_r[0] + line_l[1]*line_r[1])/(module_l*module_r));
    thet = min(thet, CV_PI - thet)*180/CV_PI;

    //cout << "thet:" << thet << endl;

    if(thet > 10 && thet < 40)
        return 1;
    if(thet < 10)
        return 2;
    return 0;
}

Point Detector::gainMidpoint(RotatedRect rotatedRect)
{
    Point2f corners[4], bottom[2];
    Point midPoint;
    int numBottom = 0;

    //将外接矩形的四个点存入数组中
    rotatedRect.points(corners);

    //获取矩形的中心点的y坐标
    float centery = 0;
    for(int i = 0; i < 4; i++)
    {
        centery += corners[i].y;
    }
    centery /= 4;

    //获取外接矩形底边的两端点
    for(int i = 0; i < 4; i++)
    {
        if(corners[i].y > centery)
        {
            bottom[numBottom++] = corners[i];
        }
    }

    midPoint = Point((bottom[0].x+bottom[1].x)/2, (bottom[0].y+bottom[1].y)/2);

    return midPoint;
}
}
