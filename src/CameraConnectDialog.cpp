#include "CameraConnectDialog.h"
#include "ui_CameraConnectDialog.h"

#include "Config.h"

#include <QThread>
#include <QMessageBox>

CameraConnectDialog::CameraConnectDialog(QWidget *parent, bool isStreamSyncEnabled) :
    QDialog(parent),
    ui(new Ui::CameraConnectDialog)
{  
    // Setup dialog
    ui->setupUi(this);
    // deviceNumberEdit (device number) input validation
    QRegExp rx1("^[0-9]{1,3}$"); // Integers 0 to 999
    QRegExpValidator *validator1 = new QRegExpValidator(rx1, 0);
    ui->deviceNumberEdit->setValidator(validator1);
    // imageBufferSizeEdit (image buffer size) input validation
    QRegExp rx2("^[0-9]{1,3}$"); // Integers 0 to 999
    QRegExpValidator *validator2 = new QRegExpValidator(rx2, 0);
    ui->imageBufferSizeEdit->setValidator(validator2);
    // resWEdit (resolution: width) input validation
    QRegExp rx3("^[0-9]{1,4}$"); // Integers 0 to 9999
    QRegExpValidator *validator3 = new QRegExpValidator(rx3, 0);
    ui->resWEditCamera->setValidator(validator3);
    // resHEdit (resolution: height) input validation
    QRegExp rx4("^[0-9]{1,4}$"); // Integers 0 to 9999
    QRegExpValidator *validator4 = new QRegExpValidator(rx4, 0);
    ui->resHEditCamera->setValidator(validator4);
    // Setup combo boxes
    QStringList threadPriorities;
    threadPriorities << tr("Idle") << tr("Lowest") << tr("Low") << tr("Normal") << tr("High") << tr("Highest") << tr("Time Critical") << tr("Inherit");
    ui->capturePrioComboBox->addItems(threadPriorities);
    ui->processingPrioComboBox->addItems(threadPriorities);
    // Set dialog to defaults
    resetToDefaults();
    // Enable/disable checkbox
    ui->enableFrameProcessingCheckBox->setEnabled(isStreamSyncEnabled);
    // Connect button to slot
    connect(ui->resetToDefaultsPushButton, &QPushButton::released, this, &CameraConnectDialog::resetToDefaults);
}

CameraConnectDialog::~CameraConnectDialog()
{
    delete ui;
}

int CameraConnectDialog::getDeviceNumber()
{
    // Set device number to default (any available camera) if field is blank
    if(ui->deviceNumberEdit->text().isEmpty())
    {
        QMessageBox::warning(parentWidget(), tr("Device Number"), QString("%1\n\n%2").arg(tr("Device Number not specified.")).arg("Automatically set to 0."));
        return 0;
    }
    else
    {
        return ui->deviceNumberEdit->text().toInt();
    }
}

std::string CameraConnectDialog::getDeviceUrl()
{
    if (ui->radioDevice->isChecked()) {
        return "0";
    }
    else if (ui->radioIPCamera->isChecked()) {
        if (!ui->rtspEdit->text().isEmpty()) {
            return ui->rtspEdit->text().toStdString();
        }
        return "0";
    }
    else if (ui->radioGStreamer->isChecked()) {
        return "0";
    }
    else {
        QMessageBox::warning(parentWidget(), tr("Device Number"), QString("%1\n\n%2").arg(tr("Device Number not specified.")).arg("Automatically set to 0."));
        return "0";
    }
}

int CameraConnectDialog::getResolutionWidth()
{
    // Return -1 if field is blank
    if(ui->resWEditCamera->text().isEmpty())
    {
        return -1;
    }
    else
    {
        return ui->resWEditCamera->text().toInt();
    }
}

int CameraConnectDialog::getResolutionHeight()
{
    // Return -1 if field is blank
    if(ui->resHEditCamera->text().isEmpty())
    {
        return -1;
    }
    else
    {
        return ui->resHEditCamera->text().toInt();
    }
}

int CameraConnectDialog::getImageBufferSize()
{
    // Set image buffer size to default if field is blank
    if(ui->imageBufferSizeEdit->text().isEmpty())
    {
        QMessageBox::warning(parentWidget(), tr("Image Buffer Size"), QString("%1\n\n%2").arg(tr("Image Buffer Size not specified.")).arg(tr("Automatically set to default value.")));
        return DEFAULT_IMAGE_BUFFER_SIZE;
    }
    // Set image buffer size to default if field is zero
    else if(ui->imageBufferSizeEdit->text().toInt() == 0)
    {
        QMessageBox::warning(parentWidget(), tr("Image Buffer Size"), QString("%1\n\n%2").arg(tr("Image Buffer Size cannot be zero.")).arg(tr("Automatically set to default value.")));
        return DEFAULT_IMAGE_BUFFER_SIZE;;
    }
    // Use image buffer size specified by user
    else
    {
        return ui->imageBufferSizeEdit->text().toInt();
    }
}

bool CameraConnectDialog::getDropFrameCheckBoxState()
{
    return ui->dropFrameCheckBox->isChecked();
}

int CameraConnectDialog::getCaptureThreadPrio()
{
    return ui->capturePrioComboBox->currentIndex();
}

int CameraConnectDialog::getProcessingThreadPrio()
{
    return ui->processingPrioComboBox->currentIndex();
}

QString CameraConnectDialog::getTabLabel()
{
    return ui->tabLabelEdit->text();
}

bool CameraConnectDialog::getEnableFrameProcessingCheckBoxState()
{
    return ui->enableFrameProcessingCheckBox->isChecked();
}

void CameraConnectDialog::resetToDefaults()
{
    // Default camera
    ui->deviceNumberEdit->clear();
    // Resolution
    ui->resWEditCamera->clear();
    ui->resHEditCamera->clear();
    // Image buffer size
    ui->imageBufferSizeEdit->setText(QString::number(DEFAULT_IMAGE_BUFFER_SIZE));
    // Drop frames
    ui->dropFrameCheckBox->setChecked(DEFAULT_DROP_FRAMES);
    // Capture thread
    if(DEFAULT_CAP_THREAD_PRIO == QThread::IdlePriority)
    {
        ui->capturePrioComboBox->setCurrentIndex(0);
    }
    else if(DEFAULT_CAP_THREAD_PRIO == QThread::LowestPriority)
    {
        ui->capturePrioComboBox->setCurrentIndex(1);
    }
    else if(DEFAULT_CAP_THREAD_PRIO == QThread::LowPriority)
    {
        ui->capturePrioComboBox->setCurrentIndex(2);
    }
    else if(DEFAULT_CAP_THREAD_PRIO == QThread::NormalPriority)
    {
        ui->capturePrioComboBox->setCurrentIndex(3);
    }
    else if(DEFAULT_CAP_THREAD_PRIO == QThread::HighPriority)
    {
        ui->capturePrioComboBox->setCurrentIndex(4);
    }
    else if(DEFAULT_CAP_THREAD_PRIO == QThread::HighestPriority)
    {
        ui->capturePrioComboBox->setCurrentIndex(5);
    }
    else if(DEFAULT_CAP_THREAD_PRIO == QThread::TimeCriticalPriority)
    {
        ui->capturePrioComboBox->setCurrentIndex(6);
    }
    else if(DEFAULT_CAP_THREAD_PRIO == QThread::InheritPriority)
    {
        ui->capturePrioComboBox->setCurrentIndex(7);
    }
    // Processing thread
    if(DEFAULT_PROC_THREAD_PRIO == QThread::IdlePriority)
    {
        ui->processingPrioComboBox->setCurrentIndex(0);
    }
    else if(DEFAULT_PROC_THREAD_PRIO == QThread::LowestPriority)
    {
        ui->processingPrioComboBox->setCurrentIndex(1);
    }
    else if(DEFAULT_PROC_THREAD_PRIO == QThread::LowPriority)
    {
        ui->processingPrioComboBox->setCurrentIndex(2);
    }
    else if(DEFAULT_PROC_THREAD_PRIO == QThread::NormalPriority)
    {
        ui->processingPrioComboBox->setCurrentIndex(3);
    }
    else if(DEFAULT_PROC_THREAD_PRIO == QThread::HighPriority)
    {
        ui->processingPrioComboBox->setCurrentIndex(4);
    }
    else if(DEFAULT_PROC_THREAD_PRIO == QThread::HighestPriority)
    {
        ui->processingPrioComboBox->setCurrentIndex(5);
    }
    else if(DEFAULT_PROC_THREAD_PRIO == QThread::TimeCriticalPriority)
    {
        ui->processingPrioComboBox->setCurrentIndex(6);
    }
    else if(DEFAULT_PROC_THREAD_PRIO == QThread::InheritPriority)
    {
        ui->processingPrioComboBox->setCurrentIndex(7);
    }
    // Tab label
    ui->tabLabelEdit->setText("");
    // Enable Frame Processing checkbox
    ui->enableFrameProcessingCheckBox->setChecked(true);
}
