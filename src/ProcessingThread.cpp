#include "ProcessingThread.h"

#include "SharedImageBuffer.h"
#include "Buffer.h"
#include "MatToQImage.h"
#include "Config.h"

#include <QDebug>

using namespace cv;

ProcessingThread::ProcessingThread(SharedImageBuffer *sharedImageBuffer, QString deviceNumber) :
    QThread(),
    m_sharedImageBuffer(sharedImageBuffer)
{
    m_deviceNumber = deviceNumber;
    m_doStop = false;
    m_sampleNumber = 0;
    m_fpsSum = 0;
    m_fps.clear();
    m_statsData.averageFPS = 0;
    m_statsData.nFramesProcessed = 0;
}

void ProcessingThread::run()
{
    while(1)
    {
        ////////////////////////////////
        // Stop thread if doStop=TRUE //
        ////////////////////////////////
        m_doStopMutex.lock();
        if (m_doStop)
        {
            m_doStop = false;
            m_doStopMutex.unlock();
            break;
        }
        m_doStopMutex.unlock();
        /////////////////////////////////
        /////////////////////////////////

        // Save processing time
        m_processingTime = m_t.elapsed();
        // Start timer (used to calculate processing rate)
        m_t.start();

        m_processingMutex.lock();
        // Get frame from queue, store in currentFrame, set ROI
        m_currentFrame = cv::Mat(m_sharedImageBuffer->getByDeviceNumber(m_deviceNumber)->get().clone(), m_currentROI);

        // Example of how to grab a frame from another stream (where Device Number=1)
        // Note: This requires stream synchronization to be ENABLED (in the Options menu of MainWindow) and frame processing for the stream you are grabbing FROM to be DISABLED.
        /*
        if(sharedImageBuffer->containsImageBufferForDeviceNumber(1))
        {
            // Grab frame from another stream (connected to camera with Device Number=1)
            cv::Mat frameFromAnotherStream = cv::Mat(sharedImageBuffer->getByDeviceNumber(1)->getFrame(), currentROI);
            // Linear blend images together using OpenCV and save the result to currentFrame. Note: beta = 1 - alpha
            cv::addWeighted(frameFromAnotherStream, 0.5, currentFrame, 0.5, 0.0, currentFrame);
        }
        */

        ////////////////////////////////////
        // PERFORM IMAGE PROCESSING BELOW //
        ////////////////////////////////////
        // Smooth
        if (m_imgProcFlags.smoothOn)
        {
            switch (m_imgProcSettings.smoothType)
            {
                // Blur
                case 0:
                    blur(m_currentFrame,
                        m_currentFrame,
                        cv::Size(m_imgProcSettings.smoothParam1, m_imgProcSettings.smoothParam2));
                    break;
                // Gaussian
                case 1:
                    GaussianBlur(m_currentFrame,
                        m_currentFrame,
                        cv::Size(m_imgProcSettings.smoothParam1, m_imgProcSettings.smoothParam2),
                        m_imgProcSettings.smoothParam3,
                        m_imgProcSettings.smoothParam4);
                    break;
                // Median
                case 2:
                    medianBlur(m_currentFrame,
                        m_currentFrame,
                        m_imgProcSettings.smoothParam1);
                    break;
            }
        }
        // Dilate
        if (m_imgProcFlags.dilateOn)
        {
            dilate(m_currentFrame,
                m_currentFrame,
                cv::Mat(),
                cv::Point(-1, -1),
                m_imgProcSettings.dilateNumberOfIterations);
        }
        // Erode
        if (m_imgProcFlags.erodeOn)
        {
            erode(m_currentFrame,
                m_currentFrame,
                cv::Mat(),
                cv::Point(-1, -1),
                m_imgProcSettings.erodeNumberOfIterations);
        }

        // Inference here
        auto objects = inference.detectObjects(m_currentFrame);

        // Canny edge detection tracker
        if (m_imgProcFlags.cannyOn)
        {
            std::vector<std::vector<float>> data;
            for (auto const& value : objects) {
                std::vector<float> row;
                for (;;) {
                    row.push_back(value.rect.x);
                    row.push_back(value.rect.y);
                    row.push_back(value.rect.x + value.rect.width);
                    row.push_back(value.rect.y + value.rect.height);
                    row.push_back(value.probability);
                    row.push_back(value.label);
                    break;
                }
                data.push_back(row);

                std::vector<Eigen::RowVectorXf> res = tracker.update(Vector2Matrix(data));

                data.clear();

                for (auto j : res) {
                    int ID = int(j[4]);
                    int Class = int(j[5]);
                    float conf = j[6];
                    cv::putText(m_currentFrame, cv::format("ID:%d", ID), cv::Point(j[0], j[1] - 5), 0, 0.5, cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
                    cv::rectangle(m_currentFrame, cv::Rect(j[0], j[1], j[2] - j[0] + 1, j[3] - j[1] + 1), cv::Scalar(0, 0, 255), 1);
                }
            }
        }

        ////////////////////////////////////
        // PERFORM IMAGE PROCESSING ABOVE //
        ////////////////////////////////////
        
        // Flip
        if (m_imgProcFlags.flipOn)
        {
            inference.drawObjectLabels(m_currentFrame, objects);
            qDebug() << "Detector On";
        }

        inference.drawObjectLabels(m_currentFrame, objects);

        // Convert Mat to QImage
        m_frame = MatToQImage(m_currentFrame);
        m_processingMutex.unlock();

        // Inform GUI thread of new frame (QImage)
        emit newFrame(m_frame);

        // Update statistics
        updateFPS(m_processingTime);
        m_statsData.nFramesProcessed++;
        // Inform GUI of updated statistics
        emit updateStatisticsInGUI(m_statsData);
    }

    qDebug() << "Stopping processing thread...";
}

void ProcessingThread::updateFPS(int timeElapsed)
{
    // Add instantaneous FPS value to queue
    if(timeElapsed > 0)
    {
        m_fps.enqueue((int)1000 / timeElapsed);
        // Increment sample number
        m_sampleNumber++;
    }

    // Maximum size of queue is DEFAULT_PROCESSING_FPS_STAT_QUEUE_LENGTH
    if (m_fps.size() > PROCESSING_FPS_STAT_QUEUE_LENGTH)
    {
        m_fps.dequeue();
    }

    // Update FPS value every DEFAULT_PROCESSING_FPS_STAT_QUEUE_LENGTH samples
    if ((m_fps.size() == PROCESSING_FPS_STAT_QUEUE_LENGTH) && (m_sampleNumber == PROCESSING_FPS_STAT_QUEUE_LENGTH))
    {
        // Empty queue and store sum
        while (!m_fps.empty())
        {
            m_fpsSum += m_fps.dequeue();
        }
        // Calculate average FPS
        m_statsData.averageFPS = m_fpsSum / PROCESSING_FPS_STAT_QUEUE_LENGTH;
        // Reset sum
        m_fpsSum = 0;
        // Reset sample number
        m_sampleNumber = 0;
    }
}

void ProcessingThread::stop()
{
    QMutexLocker locker(&m_doStopMutex);
    m_doStop = true;
}

void ProcessingThread::updateImageProcessingFlags(ImageProcessingFlags imgProcFlags)
{
    QMutexLocker locker(&m_processingMutex);
    m_imgProcFlags.grayscaleOn = imgProcFlags.grayscaleOn;
    m_imgProcFlags.smoothOn = imgProcFlags.smoothOn;
    m_imgProcFlags.dilateOn=imgProcFlags.dilateOn;
    m_imgProcFlags.erodeOn=imgProcFlags.erodeOn;
    m_imgProcFlags.flipOn=imgProcFlags.flipOn;
    m_imgProcFlags.cannyOn=imgProcFlags.cannyOn;
}

void ProcessingThread::updateImageProcessingSettings(ImageProcessingSettings imgProcSettings)
{
    QMutexLocker locker(&m_processingMutex);
    m_imgProcSettings.smoothType=imgProcSettings.smoothType;
    m_imgProcSettings.smoothParam1=imgProcSettings.smoothParam1;
    m_imgProcSettings.smoothParam2=imgProcSettings.smoothParam2;
    m_imgProcSettings.smoothParam3=imgProcSettings.smoothParam3;
    m_imgProcSettings.smoothParam4=imgProcSettings.smoothParam4;
    m_imgProcSettings.dilateNumberOfIterations=imgProcSettings.dilateNumberOfIterations;
    m_imgProcSettings.erodeNumberOfIterations=imgProcSettings.erodeNumberOfIterations;
    m_imgProcSettings.flipCode=imgProcSettings.flipCode;
    m_imgProcSettings.cannyThreshold1=imgProcSettings.cannyThreshold1;
    m_imgProcSettings.cannyThreshold2=imgProcSettings.cannyThreshold2;
    m_imgProcSettings.cannyApertureSize=imgProcSettings.cannyApertureSize;
    m_imgProcSettings.cannyL2gradient=imgProcSettings.cannyL2gradient;
}

void ProcessingThread::setROI(QRect roi)
{
    QMutexLocker locker(&m_processingMutex);
    m_currentROI.x = roi.x();
    m_currentROI.y = roi.y();
    m_currentROI.width = roi.width();
    m_currentROI.height = roi.height();
}

QRect ProcessingThread::getCurrentROI()
{
    return QRect(m_currentROI.x, m_currentROI.y, m_currentROI.width, m_currentROI.height);
}

Eigen::Matrix<float, Eigen::Dynamic, 6> ProcessingThread::Vector2Matrix(std::vector<std::vector<float>> data) {
    Eigen::Matrix<float, Eigen::Dynamic, 6> matrix(data.size(), data[0].size());
    for (int i = 0; i < data.size(); ++i) {
        for (int j = 0; j < data[0].size(); ++j) {
            matrix(i, j) = data[i][j];
        }
    }
    return matrix;
}