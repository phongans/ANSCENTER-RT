#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "SharedImageBuffer.h"
#include "CameraView.h"
#include "CameraConnectDialog.h"
#include "Config.h"

#include <QLabel>
#include <QtCore>
#include <QMessageBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Setup UI
    ui->setupUi(this);
    // Set start tab as blank
    QLabel *newTab = new QLabel(ui->tabWidget);
    newTab->setText(tr("No camera connected."));
    newTab->setAlignment(Qt::AlignCenter);
    ui->tabWidget->addTab(newTab, "");
    ui->tabWidget->setTabsClosable(false);
    // Add "Connect to Camera" button to tab
    m_connectToCameraButton = new QPushButton();
    m_connectToCameraButton->setText(tr("Connect to Camera..."));
    ui->tabWidget->setCornerWidget(m_connectToCameraButton, Qt::TopLeftCorner);
    connect(m_connectToCameraButton, &QPushButton::released, this, &MainWindow::connectToCamera);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::disconnectCamera);
    // Set focus on button
    m_connectToCameraButton->setFocus();
    // Connect other signals/slots
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionFullScreen, &QAction::toggled, this, &MainWindow::setFullScreen);
    // Create SharedImageBuffer object
    m_sharedImageBuffer = new SharedImageBuffer();
    m_cameraNum = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectToCamera()
{
    // We cannot connect to a camera if devices are already connected and stream synchronization is in progress
    if (ui->actionSynchronizeStreams->isChecked() && (m_deviceNumberMap.size() > 0) && m_sharedImageBuffer->getSyncEnabled())
    {
        // Prompt user
        QMessageBox::warning(this,
            APP_NAME,
            QString("%1\n\n%2").arg(tr("Stream synchronization is in progress.")).arg(tr("Please close all currently open streams before attempting to open a new stream.")),
            QMessageBox::Ok);
    }
    // Attempt to connect to camera
    else
    {
        // Get next tab index
        int nextTabIndex = (m_deviceNumberMap.size() == 0) ? 0 : ui->tabWidget->count();
        // Show dialog
        CameraConnectDialog *cameraConnectDialog = new CameraConnectDialog(this, ui->actionSynchronizeStreams->isChecked());
        if(cameraConnectDialog->exec() == QDialog::Accepted)
        {
            // Save user-defined device number
            std::string deviceNumber = cameraConnectDialog->getDeviceUrl();// cameraConnectDialog->getDeviceNumber();
            // Check if this camera is already connected
            if (true)
            {
                // Create ImageBuffer with user-defined size
                Buffer<cv::Mat> *imageBuffer = new Buffer<cv::Mat>(cameraConnectDialog->getImageBufferSize());
                // Add created ImageBuffer to SharedImageBuffer object
                m_sharedImageBuffer->add(QString::fromStdString(deviceNumber), imageBuffer, ui->actionSynchronizeStreams->isChecked());
                // Create CameraView
                CameraView* cameraview = new CameraView(QString::fromStdString(deviceNumber), m_sharedImageBuffer, ui->tabWidget);

                // Check if stream synchronization is enabled
                if(ui->actionSynchronizeStreams->isChecked())
                {
                    // Prompt user
                    int ret = QMessageBox::question(this,
                        APP_NAME,
                        QString("%1\n\n%2\n\n%3").arg(tr("Stream synchronization is enabled.")).arg(tr("Do you want to start processing?")).arg(tr("Choose 'No' if you would like to open additional streams.")),
                        QMessageBox::Yes | QMessageBox::No,
                        QMessageBox::Yes);
                    // Start processing
                    if(ret == QMessageBox::Yes)
                    {
                        std::cout << "Start processing" << std::endl;
                        m_sharedImageBuffer->setSyncEnabled(true);
                    }
                    // Defer processing
                    else
                    {
                        m_sharedImageBuffer->setSyncEnabled(false);
                    }
                }

                // Attempt to connect to camera
                if (cameraview->connectToCamera(cameraConnectDialog->getDropFrameCheckBoxState(),
                                               cameraConnectDialog->getCaptureThreadPrio(),
                                               cameraConnectDialog->getProcessingThreadPrio(),
                                               cameraConnectDialog->getEnableFrameProcessingCheckBoxState(),
                                               cameraConnectDialog->getResolutionWidth(),
                                               cameraConnectDialog->getResolutionHeight()))
                {
                    
                    // Add to map
                    // cameraview = nextTabIndex;
                    m_deviceNumberMap[deviceNumber] = nextTabIndex;
                    m_cameraViewMap[deviceNumber] = cameraview;

                    // Save tab label
                    QString tabLabel = cameraConnectDialog->getTabLabel();
                    // Allow tabs to be closed
                    ui->tabWidget->setTabsClosable(true);

                    // If start tab, remove
                    if(nextTabIndex == 0)
                    {
                        ui->tabWidget->removeTab(0);
                    }

                    // Add tab
                    ui->tabWidget->addTab(cameraview, tabLabel + "[" + QString::fromStdString(deviceNumber) + "]");
                    ui->tabWidget->setCurrentWidget(cameraview);
                    // Debug
                    qDebug() << "Add Tab: " + QString::fromStdString(deviceNumber);

                    // Set tooltips
                    setTabCloseToolTips(ui->tabWidget, tr("Disconnect Camera"));
                    // Prevent user from enabling/disabling stream synchronization after a camera has been connected
                    ui->actionSynchronizeStreams->setEnabled(false);
                }
            }
            // Display error message
            else
            {
                QMessageBox::warning(this, tr("Connect to Camera"), tr("Could not connect to camera. Already connected."));
            }
        }
        // Delete dialog
        delete cameraConnectDialog;
    }
}

void MainWindow::disconnectCamera(int index)
{
    // Local variable(s)
    bool doDisconnect = true;

    // Check if stream synchronization is enabled, more than 1 camera connected, and frame processing is not in progress
    if (ui->actionSynchronizeStreams->isChecked() && (m_cameraViewMap.size() > 1) && !m_sharedImageBuffer->getSyncEnabled())
    {
        // Prompt user
        int ret = QMessageBox::question(this,
            APP_NAME,
            QString("%1\n\n%2\n\n%3").arg(tr("Stream synchronization is enabled.")).arg(tr("Disconnecting this camera will cause frame processing to begin on other streams.")).arg(tr("Do you wish to proceed?")),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::Yes);
        // Disconnect
        if(ret == QMessageBox::Yes)
        {
            doDisconnect = true;
        }
        // Do not disconnect
        else
        {
            doDisconnect = false;
        }
    }

    // Disconnect camera
    if(doDisconnect)
    {
        // Save number of tabs
        int nTabs = ui->tabWidget->count();
        // Close tab
        ui->tabWidget->removeTab(index);

        //// Delete widget (CameraView) contained in tab
        //delete m_cameraViewMap[m_deviceNumberMap.contains(index)];
        //m_cameraViewMap.remove(m_deviceNumberMap.contains(index));

        // Remove from map
        std::string deviceNumber =  removeFromMapByTabIndex(m_deviceNumberMap, index);
        // Delete widget (CameraView) contained in tab
        delete m_cameraViewMap[deviceNumber];

        // Remove from dict
        m_cameraViewMap.erase(deviceNumber);
        m_deviceNumberMap.erase(deviceNumber);

        // Update map (if tab closed is not last)
        if(index != (nTabs - 1))
        {
            updateMapValues(m_deviceNumberMap, index);
        }

        // If start tab, set tab as blank
        if(nTabs == 1)
        {
            QLabel *newTab = new QLabel(ui->tabWidget);
            newTab->setText(tr("No camera connected."));
            newTab->setAlignment(Qt::AlignCenter);
            ui->tabWidget->addTab(newTab, "");
            ui->tabWidget->setTabsClosable(false);
            ui->actionSynchronizeStreams->setEnabled(true);
        }
    }
}

void MainWindow::showAboutDialog()
{
    QMessageBox::information(this, tr("About"), QString("%1 v%2\n\nCreated by %3\nEmail: %4\nWebsite: %5").arg(APP_NAME).arg(APP_VERSION).arg(APP_AUTHOR_NAME).arg(APP_AUTHOR_EMAIL).arg(APP_AUTHOR_WEBSITE));
}

std::string MainWindow::removeFromMapByTabIndex(std::map<std::string, int>& map,int tabIndex)
{
    for (auto& [key, value] : map) {
        if (value == tabIndex) {
            return key;
        }
    }
}

void print_map(std::string_view comment, const std::map<std::string, int>& m)
{
    std::cout << comment;
    // iterate using C++17 facilities
    for (const auto& [key, value] : m)
        std::cout << '[' << key << "] = " << value << "; ";

    // C++11 alternative:
    //  for (const auto& n : m)
    //      std::cout << n.first << " = " << n.second << "; ";
    //
    // C++98 alternative
    //  for (std::map<std::string, int>::const_iterator it = m.begin(); it != m.end(); it++)
    //      std::cout << it->first << " = " << it->second << "; ";

    std::cout << '\n';
}

void MainWindow::updateMapValues(std::map<std::string, int> &map, int tabIndex)
{
    for (auto& [key, value] : map) {
        if (value > tabIndex)
            map[key] = value - 1;
    }
}

void MainWindow::setTabCloseToolTips(QTabWidget *tabs, QString tooltip)
{
    QList<QAbstractButton*> allPButtons = tabs->findChildren<QAbstractButton*>();
    for (int ind = 0; ind < allPButtons.size(); ind++)
    {
        QAbstractButton* item = allPButtons.at(ind);
        if (item->inherits("CloseButton"))
        {
            item->setToolTip(tooltip);
        }
    }
}

void MainWindow::setFullScreen(bool input)
{
    if(input)
    {
        showFullScreen();
    }
    else
    {
        showNormal();
    }
}
