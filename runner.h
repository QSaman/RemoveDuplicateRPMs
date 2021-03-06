#ifndef RUNNER_H
#define RUNNER_H

#include <QObject>
#include <QFileInfoList>
#include <QString>
#include <QTextStream>
#include <QProcess>

class Runner : public QObject
{
    Q_OBJECT
public:
    explicit Runner(QObject *parent = 0);
    explicit Runner(const QString& dirPath, bool removeDuplicates);

signals:

public slots:
    void run();
private slots:
    void errorInProcess(QProcess::ProcessError error);
    void readInput();
private:
    void myMain();
    QFileInfoList obtainFileList();
    QFileInfoList findDuplicates(const QFileInfoList& fileList);
    QString extractRpmName(const QString& rpmFile);
    bool removeDuplicates;

    QString dirPath;
    QTextStream qCout;
    QProcess process;
    QString rpmName;

};

#endif // RUNNER_H
