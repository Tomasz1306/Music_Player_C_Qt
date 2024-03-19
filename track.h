#ifndef TRACK_H
#define TRACK_H

#include <iostream>
#include <QString>
#include <QUrl>
#include <QString>
class Track
{
public:
    Track(QString name, QString path, QString format);

    QString getName(){return this->t_name;}
    QString getPath(){return this->t_path;}
    QString getFormat(){return this->t_format;}
    QUrl getUrl(){return QUrl{this->t_path + this->t_name + this->t_format};}

private:

    QString t_name;
    QString t_path;
    QString t_format;
    float t_lenght;
};

#endif // TRACK_H
