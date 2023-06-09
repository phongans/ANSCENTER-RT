#pragma once

#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H

#include <QThread>
#include <QTime>
#include <QQueue>
#include <QImage>
#include <QMutex>

#include <opencv2/opencv.hpp>

#include "Structures.h"
#include <inference_trt.h>
#include <OCSort.hpp>

class SharedImageBuffer;

class ProcessingThread : public QThread
{
    Q_OBJECT

    public:
        ProcessingThread(SharedImageBuffer *sharedImageBuffer, QString deviceNumber);
        QRect getCurrentROI();
        void stop();

    private:
        void updateFPS(int);
        void resetROI();
        Eigen::Matrix<float, Eigen::Dynamic, 6> Vector2Matrix(std::vector<std::vector<float>> data);
        SharedImageBuffer *m_sharedImageBuffer;
        cv::Mat m_currentFrame;
        cv::Mat m_currentFrameGrayscale;
        cv::Rect m_currentROI;
        QImage m_frame;
        QTime m_t;
        QQueue<int> m_fps;
        QMutex m_doStopMutex;
        QMutex m_processingMutex;
        cv::Size m_frameSize;
        cv::Point m_framePoint;
        ImageProcessingFlags m_imgProcFlags;
        ImageProcessingSettings m_imgProcSettings;
        ThreadStatisticsData m_statsData;
        volatile bool m_doStop;
        int m_processingTime;
        int m_fpsSum;
        int m_sampleNumber;
        QString m_deviceNumber;
        bool m_enableFrameProcessing;
        InferenceTRT inference = InferenceTRT("C:/Projects/Research/Models/yolov8s.onnx");
        ocsort::OCSort tracker = ocsort::OCSort(0, 50, 1, 0.22136877277096445, 1, "giou", 0.3941737016672115, true);
    protected:
        void run();

    public slots:
        void updateImageProcessingFlags(ImageProcessingFlags flags);
        void updateImageProcessingSettings(ImageProcessingSettings settings);
        void setROI(QRect roi);

    signals:
        void newFrame(const QImage& frame);
        void updateStatisticsInGUI(ThreadStatisticsData statData);
};

#endif // PROCESSINGTHREAD_H
