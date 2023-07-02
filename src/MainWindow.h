#pragma once

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

namespace Ui {
    class MainWindow;
}

class SharedImageBuffer;
class CameraView;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private:
        std::string removeFromMapByTabIndex(std::map<std::string, int>& map, int tabIndex);
        void updateMapValues(std::map<std::string, int>& map, int tabIndex);
        void setTabCloseToolTips(QTabWidget *tabs, QString tooltip);
        Ui::MainWindow *ui;
        QPushButton *m_connectToCameraButton;
        std::map<std::string, int> m_deviceNumberMap;
        std::map<std::string, CameraView*> m_cameraViewMap;
        SharedImageBuffer *m_sharedImageBuffer;
        int m_cameraNum;

    public slots:
        void connectToCamera();
        void disconnectCamera(int index);
        void showAboutDialog();
        void setFullScreen(bool enable);
};

#endif // MAINWINDOW_H
