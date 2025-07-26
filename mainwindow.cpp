#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QRegularExpression>
#include <QMessageBox>

#include <QDragEnterEvent>
#include <QMimeData>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_process(new QProcess(this))
{
    ui->setupUi(this);
    setAcceptDrops(true); // Enable Drag and Drop

    // --- Initialize UI ---
    ui->scaleComboBox->addItems({"2", "3", "4"});
    ui->scaleComboBox->setCurrentText("4");
    ui->formatComboBox->addItems({"png", "jpg", "webp"});
    ui->formatComboBox->setCurrentText("png");
    ui->advancedGroup->setVisible(false);
    ui->progressBar->setVisible(false);

    // --- Connections ---
    connect(ui->advancedSettingsButton, &QToolButton::toggled, this, &MainWindow::on_advancedSettingsButton_toggled);
    connect(m_process, &QProcess::readyReadStandardError, this, &MainWindow::onProcessReadyReadStandardError);
    connect(m_process, &QProcess::finished, this, &MainWindow::onProcessFinished);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setInputPath(const QString &path)
{
    if (!path.isEmpty()) {
        processPath(path);
    }
}

void MainWindow::on_browseFileButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Select Image File"), "", tr("Images (*.png *.jpg *.jpeg *.webp)"));
    if (!filePath.isEmpty()) {
        processPath(filePath);
    }
}

void MainWindow::on_browseFolderButton_clicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select Folder"));
    if (!dirPath.isEmpty()) {
        processPath(dirPath);
    }
}

void MainWindow::on_browseOutputButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Output Directory"));
    if (!dir.isEmpty()) {
        ui->outputPathLineEdit->setText(dir);
    }
}

void MainWindow::on_advancedSettingsButton_toggled(bool checked)
{
    ui->advancedGroup->setVisible(checked);
    ui->advancedSettingsButton->setArrowType(checked ? Qt::DownArrow : Qt::RightArrow);
}

void MainWindow::on_startButton_clicked()
{
    if (m_inputPath.isEmpty()) {
        QMessageBox::warning(this, tr("Input Missing"), tr("Please select an input file or directory first."));
        return;
    }
    startUpscaling();
}

void MainWindow::onProcessReadyReadStandardError()
{
    // Read all available data from stderr. This is more robust against buffering issues.
    QString data = QString::fromLocal8Bit(m_process->readAllStandardError());
    qDebug() << "stderr chunk:" << data; // For debugging

    if (m_isSingleFileMode) {
        // In single file mode, find the last percentage value in the output chunk.
        QRegularExpression re("(\\d+\\.\\d+)%");
        QRegularExpressionMatchIterator i = re.globalMatch(data);
        QRegularExpressionMatch lastMatch;
        while (i.hasNext()) {
            lastMatch = i.next();
        }

        if (lastMatch.hasMatch()) {
            float progress = lastMatch.captured(1).toFloat();
            ui->progressBar->setValue(static_cast<int>(progress));
        }
    } else {
        // In directory mode, count the number of completed files in the chunk.
        // The "done" message is a reliable indicator of a finished file.
        int completedInChunk = data.count("done");
        if (completedInChunk > 0) {
            m_processedFiles += completedInChunk;
            int progress = (m_totalFiles > 0) ? (m_processedFiles * 100) / m_totalFiles : 0;
            // Clamp progress to 100, just in case.
            if (progress > 100) progress = 100;
            ui->progressBar->setValue(progress);
            ui->progressBar->setFormat(tr("Processing %1/%2...").arg(m_processedFiles).arg(m_totalFiles));
        }
    }
}

void MainWindow::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Process finished with exit code:" << exitCode;
    updateUiForProcessing(false);

    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        if (!m_isSingleFileMode) {
            // Ensure the progress bar shows 100% on completion
            m_processedFiles = m_totalFiles;
            ui->progressBar->setValue(100);
            ui->progressBar->setFormat(tr("Completed %1/%2").arg(m_processedFiles).arg(m_totalFiles));
        } else {
            ui->progressBar->setValue(100);
        }

        QMessageBox::information(this, tr("Success"), tr("Upscaling finished successfully!"));
        QApplication::quit(); // Exit the application

    } else {
        QString errorMsg = m_process->readAllStandardError();
        QMessageBox::critical(this, tr("Error"), tr("An error occurred during upscaling:\n%1").arg(errorMsg));
        ui->statusbar->showMessage(tr("Upscaling failed."), 5000);
    }
}



void MainWindow::processPath(const QString &path)
{
    m_inputPath = path;
    QFileInfo fileInfo(path);

    if (fileInfo.isFile()) {
        m_isSingleFileMode = true;
        m_filesToProcess.clear();
        m_filesToProcess.append(fileInfo);
        m_totalFiles = 1;
        ui->inputPathLabel->setText(tr("File: %1").arg(fileInfo.fileName()));
    } else if (fileInfo.isDir()) {
        m_isSingleFileMode = false;
        QDir dir(path);
        QStringList filters = {"*.png", "*.jpg", "*.jpeg", "*.webp"};
        m_filesToProcess = dir.entryInfoList(filters, QDir::Files);
        m_totalFiles = m_filesToProcess.count();
        ui->inputPathLabel->setText(tr("Directory: %1 (%2 files)").arg(fileInfo.fileName()).arg(m_totalFiles));
    }

     if (m_totalFiles == 0 && !m_inputPath.isEmpty()) {
        QMessageBox::warning(this, tr("No Files Found"), tr("The selected directory contains no supported image files."));
        m_inputPath.clear();
        ui->inputPathLabel->setText("Please select a file or directory...");
    }
}

void MainWindow::startUpscaling()
{
    updateUiForProcessing(true);
    m_processedFiles = 0;

    QStringList args;
    args << "-i" << m_inputPath;

    // --- Output Path ---
    QString outputPath;
    if (!ui->outputPathLineEdit->text().isEmpty()) {
        outputPath = ui->outputPathLineEdit->text();
    } else {
        QFileInfo inputInfo(m_inputPath);
        if (inputInfo.isDir()) {
            outputPath = getOutputName(inputInfo, ui->scaleComboBox->currentText().toInt());
            QDir().mkdir(outputPath); // Create the directory
        } else {
            outputPath = getOutputName(inputInfo, ui->scaleComboBox->currentText().toInt());
        }
    }
    args << "-o" << outputPath;


    // --- Basic Settings ---
    args << "-s" << ui->scaleComboBox->currentText();
    args << "-f" << ui->formatComboBox->currentText();

    // --- Advanced Settings ---
    if (!ui->threadsLineEdit->text().isEmpty()) {
        args << "-j" << ui->threadsLineEdit->text();
    }

    // Always use verbose for progress parsing
    args << "-v";

    qDebug() << "Starting process: realesrgan-ncnn-vulkan" << args;

    if (m_isSingleFileMode) {
        ui->progressBar->setFormat("%p%");
    } else {
        ui->progressBar->setFormat(tr("Processing %1/%2...").arg(m_processedFiles).arg(m_totalFiles));
        ui->progressBar->setValue(0);
    }

    m_process->start("realesrgan-ncnn-vulkan", args);
}

void MainWindow::updateUiForProcessing(bool isProcessing)
{
    ui->settingsGroup->setEnabled(!isProcessing);
    ui->advancedSettingsButton->setEnabled(!isProcessing);
    ui->advancedGroup->setEnabled(!isProcessing);
    ui->browseFileButton->setEnabled(!isProcessing);
    ui->browseFolderButton->setEnabled(!isProcessing);
    ui->startButton->setEnabled(!isProcessing);
    ui->progressBar->setVisible(isProcessing);

    if (!isProcessing) {
       ui->progressBar->setValue(0);
       // Reset label to default after processing
       ui->inputPathLabel->setText(tr("Drag & Drop a file/folder here or select one below."));
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    // We only accept files by URL
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        if (!urlList.isEmpty()) {
            // We only process the first dropped item
            QString path = urlList.first().toLocalFile();
            if (!path.isEmpty()) {
                processPath(path);
            }
        }
    }
}


QString MainWindow::getOutputName(const QFileInfo& info, int scale)
{
    if (info.isDir()) {
        return QString("%1/%2_%3x").arg(info.absolutePath()).arg(info.fileName()).arg(scale);
    } else {
        return QString("%1/%2_%3x.%4")
            .arg(info.absolutePath())
            .arg(info.baseName())
            .arg(scale)
            .arg(info.suffix());
    }
}
