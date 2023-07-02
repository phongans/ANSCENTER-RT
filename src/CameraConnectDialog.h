#pragma once

#ifndef CAMERACONNECTDIALOG_H
#define CAMERACONNECTDIALOG_H

#include <QDialog>

namespace Ui {
class CameraConnectDialog;
}

class CameraConnectDialog : public QDialog
{
    Q_OBJECT
    
    public:
        explicit CameraConnectDialog(QWidget *parent = 0, bool isStreamSyncEnabled = false);
        ~CameraConnectDialog();
        void setDeviceNumber();
        void setImageBufferSize();
        int getDeviceNumber();
        std::string getDeviceUrl();
        int getResolutionWidth();
        int getResolutionHeight();
        int getImageBufferSize();
        bool getDropFrameCheckBoxState();
        int getCaptureThreadPrio();
        int getProcessingThreadPrio();
        QString getTabLabel();
        bool getEnableFrameProcessingCheckBoxState();

    private:
        Ui::CameraConnectDialog *ui;

    public slots:
        void resetToDefaults();
};

#endif // CAMERACONNECTDIALOG_H
