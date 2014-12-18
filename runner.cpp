#include "runner.h"
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfoList>
#include <QFile>
#include <algorithm>
#include <QCollator>
#include <QStringList>

Runner::Runner(QObject *parent) :
    QObject(parent)
{
}

Runner::Runner(const QString& dirPath, bool removeDuplicates) : qCout{stdout}
{
    this->dirPath = dirPath;
    this->removeDuplicates = removeDuplicates;
    connect(&process, SIGNAL(error(QProcess::ProcessError)), SLOT(errorInProcess(QProcess::ProcessError)));
    connect(&process, SIGNAL(readyReadStandardOutput()), SLOT(readInput()));
}

void Runner::readInput()
{
    rpmName = process.readLine();
}

void Runner::errorInProcess(QProcess::ProcessError error)
{
    qDebug() << error;
}

void Runner::run()
{
    myMain();
    qApp->exit();
}

struct Comperator
{
    QCollator collator;
    Comperator()
    {
        collator.setNumericMode(true);
    }

    bool operator()(QFileInfo left, QFileInfo right)
    {
        int ret = collator.compare(left.absoluteFilePath(), right.absoluteFilePath());
        return ret == -1;
    }
} comperator;

QFileInfoList Runner::obtainFileList()
{
    //return QDir(dirPath).entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    QFileInfoList ret = QDir(dirPath).entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    std::sort(ret.begin(), ret.end(), comperator);
    return ret;
}

QString Runner::extractRpmName(const QString& rpmFile)
{
    QStringList args;
    args << "-qp" << "--qf" << "\"%{NAME}\"" << rpmFile;
    process.start("rpm", args);
    if (!process.waitForFinished())
    {
        QString message = QString("Error in obtaining package name of %1").arg(rpmFile);
        qFatal(message.toUtf8().constData());
    }
    return rpmName;
}

QFileInfoList Runner::findDuplicates(const QFileInfoList& fileList)
{
    QFileInfoList duplicates;
    int i, j;
    for (i = 0; i < fileList.size(); i = j)
    {
        QString name1 = extractRpmName(fileList[i].absoluteFilePath());
        for (j = i + 1; j < fileList.size(); ++j)
        {
            QString name2 = extractRpmName(fileList[j].absoluteFilePath());
            if (name1 != name2)
                break;
        }
        for (; i < (j - 1); ++i)
            duplicates << fileList[i];
    }
    return duplicates;
}

void Runner::myMain()
{
    QDir dir(dirPath);
    QFileInfoList fileList = obtainFileList();
    QFileInfoList duplicates = findDuplicates(fileList);
    qCout << "There are " << duplicates.size() << " duplicates" << endl;
    if (removeDuplicates)
    {
        for (int i = 0; i < duplicates.size(); ++i)
        {
            qCout << "Removing " << duplicates[i].absoluteFilePath() << endl;
            if (!QFile::remove(duplicates[i].absoluteFilePath()))
            {
                QString message = QString("Cannot remove %1").arg(duplicates[i].absoluteFilePath());
                qFatal(message.toUtf8().constData());
            }
        }
    }
    else
    {
        for (int i = 0; i < duplicates.size(); ++i)
            qCout << duplicates[i].absoluteFilePath() << endl;
    }
}
