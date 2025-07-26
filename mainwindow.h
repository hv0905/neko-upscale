#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QFileInfo>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setInputPath(const QString &path);

private slots:
    void on_browseFileButton_clicked();
    void on_browseFolderButton_clicked();
    void on_browseOutputButton_clicked();
    void on_advancedSettingsButton_toggled(bool checked);
    void on_startButton_clicked();
    void onProcessReadyReadStandardError();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;


private:
    void processPath(const QString &path);
    void startUpscaling();
    void updateUiForProcessing(bool isProcessing);
    QString getOutputName(const QFileInfo& info, int scale);

    Ui::MainWindow *ui;
    QProcess *m_process;
    QString m_inputPath;
    QList<QFileInfo> m_filesToProcess;
    int m_totalFiles = 0;
    int m_processedFiles = 0;
    bool m_isSingleFileMode = false;
};
#endif // MAINWINDOW_H