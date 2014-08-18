/*
    Copyright 2014 Christian Weber

    This file is part of V800 Downloader.

    V800 Downloader is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    V800 Downloader is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with V800 Downloader.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef V800EXPORT_H
#define V800EXPORT_H

#include <QObject>

class V800export : public QObject
{
    Q_OBJECT
public:
    explicit V800export(QObject *parent = 0);

    enum {
        TCX_EXPORT = 0x01,
        HRM_EXPORT = 0x02,
        GPX_EXPORT = 0x04
    };

    enum {
        RENAME_ERROR = 1,
        PARSE_ERROR,
        TCX_ERROR,
        HRM_ERROR,
        GPX_ERROR
    };

signals:
    void export_session_done(int session_iter, int session_cnt);
    void export_sessions_done();
    void export_session_error(QString session, int error);

public slots:
    void start();
    void export_sessions(QList<QString> sessions, unsigned char mode);

private:
    bool make_bipolar_names(QString session);

};

#endif // V800EXPORT_H
