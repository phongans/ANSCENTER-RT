#pragma once

#ifndef SHAREDIMAGEBUFFER_H
#define SHAREDIMAGEBUFFER_H

#include <QHash>
#include <QSet>
#include <QWaitCondition>
#include <QMutex>

#include <opencv2/opencv.hpp>

#include "Buffer.h"

class SharedImageBuffer
{
    public:
        SharedImageBuffer();
        void add(QString deviceNumber, Buffer<cv::Mat> *imageBuffer, bool sync = false);
        Buffer<cv::Mat>* getByDeviceNumber(QString deviceNumber);
        void removeByDeviceNumber(QString deviceNumber);
        void sync(QString deviceNumber);
        void wakeAll();
        void setSyncEnabled(bool enable);
        bool isSyncEnabledForDeviceNumber(QString deviceNumber);
        bool getSyncEnabled();
        bool containsImageBufferForDeviceNumber(QString deviceNumber);

    private:
        QHash<QString, Buffer<cv::Mat>* > m_imageBufferMap;
        QSet<QString> m_syncSet;
        QWaitCondition m_wc;
        QMutex m_mutex;
        int m_nArrived;
        bool m_doSync;
};

#endif // SHAREDIMAGEBUFFER_H
