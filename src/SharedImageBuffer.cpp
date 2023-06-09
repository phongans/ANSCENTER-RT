
#include "SharedImageBuffer.h"

SharedImageBuffer::SharedImageBuffer()
{
    m_nArrived = 0;
    m_doSync = false;
}

void SharedImageBuffer::add(QString deviceNumber, Buffer<cv::Mat>* imageBuffer, bool sync)
{
    // Device stream is to be synchronized
    if(sync)
    {
        m_mutex.lock();
        m_syncSet.insert(deviceNumber);
        m_mutex.unlock();
    }
    // Add image buffer to map
    m_imageBufferMap[deviceNumber] = imageBuffer;
}

Buffer<cv::Mat>* SharedImageBuffer::getByDeviceNumber(QString deviceNumber)
{
    return m_imageBufferMap[deviceNumber];
}

void SharedImageBuffer::removeByDeviceNumber(QString deviceNumber)
{
    // Remove buffer for device from imageBufferMap
    m_imageBufferMap.remove(deviceNumber);

    // Also remove from syncSet (if present)
    m_mutex.lock();
    if (m_syncSet.contains(deviceNumber))
    {
        m_syncSet.remove(deviceNumber);
        m_wc.wakeAll();
    }
    m_mutex.unlock();
}

void SharedImageBuffer::sync(QString deviceNumber)
{
    // Only perform sync if enabled for specified device/stream
    m_mutex.lock();
    if (m_syncSet.contains(deviceNumber))
    {
        // Increment arrived count
        m_nArrived++;
        // We are the last to arrive: wake all waiting threads
        if (m_doSync && (m_nArrived == m_syncSet.size()))
        {
            m_wc.wakeAll();
        }
        // Still waiting for other streams to arrive: wait
        else
        {
            m_wc.wait(&m_mutex);
        }
        // Decrement arrived count
        m_nArrived--;
    }
    m_mutex.unlock();
}

void SharedImageBuffer::wakeAll()
{
    QMutexLocker locker(&m_mutex);
    m_wc.wakeAll();
}

void SharedImageBuffer::setSyncEnabled(bool enable)
{
    m_doSync = enable;
}

bool SharedImageBuffer::isSyncEnabledForDeviceNumber(QString deviceNumber)
{
    return m_syncSet.contains(deviceNumber);
}

bool SharedImageBuffer::getSyncEnabled()
{
    return m_doSync;
}

bool SharedImageBuffer::containsImageBufferForDeviceNumber(QString deviceNumber)
{
    return m_imageBufferMap.contains(deviceNumber);
}
