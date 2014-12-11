#include "runner.h"
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfoList>
#include <QFile>
#include <algorithm>
#include <QCollator>

Runner::Runner(QObject *parent) :
    QObject(parent)
{
}

Runner::Runner(const QString& dirPath, bool removeDuplicates) : qCout{stdout}
{
    this->dirPath = dirPath;
    this->removeDuplicates = removeDuplicates;
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
    qDebug() << "obi";
    std::sort(ret.begin(), ret.end(), comperator);
    return ret;
}

QString Runner::extractRpmName(const QString& rpmFile)
{
    int cnt = 2;
    int i;
    int rightIndex = rpmFile.indexOf(".fc20.");
    if (rightIndex == -1)
    {
        QString message = QString("cannot find fc20 in %1").arg(rpmFile);
        qFatal(message.toUtf8().constData());
    }
    for (i = rightIndex - 1; i >= 0 && cnt > 0; --i)
        if (rpmFile[i] == '-')
            --cnt;
    if (cnt != 0)
    {
        QString message = QString("%1 deosn't have two dash characters").arg(rpmFile);
        qFatal(message.toUtf8().constData());
    }
    return rpmFile.mid(0, i + 1);
}

QFileInfoList Runner::findDuplicates(const QFileInfoList& fileList)
{
    QFileInfoList duplicates;
    int i, j;
    for (i = 0; i < fileList.size(); i = j)
    {
        QString name1 = extractRpmName(fileList[i].fileName());
        for (j = i + 1; j < fileList.size(); ++j)
        {
            QString name2 = extractRpmName(fileList[j].fileName());
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
