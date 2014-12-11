#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QString>

#include "runner.h"

int main(int argc, char *argv[])
{
    if (argc != 2 && argc != 3)
    {
        qDebug() << "Parameters: dir-path [remove]";
        return 1;
    }
    bool removeDuplicates = (argc == 3 && QString(argv[2]) == "remove");
    QCoreApplication a(argc, argv);
    Runner runner(argv[1], removeDuplicates);
    QTimer::singleShot(0, &runner, SLOT(run()));
    return a.exec();
}
