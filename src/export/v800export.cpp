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

#include <QSettings>
#include <QDir>

#include "v800export.h"
#include "trainingsession.h"

V800export::V800export(QObject *parent) :
    QObject(parent)
{
}

void V800export::start()
{
}

void V800export::export_sessions(QList<QString> sessions, unsigned char mode)
{
    QSettings settings;
    QString default_dir = settings.value(tr("default_dir")).toString();

    for(int sessions_iter = 0; sessions_iter < sessions.length(); sessions_iter++)
    {
        QStringList filters;
        filters << QString(tr("%1_*")).arg(sessions[sessions_iter]);

        QDir filter_dir(default_dir);
        filter_dir.setNameFilters(filters);
        filter_dir.setFilter(QDir::Dirs);

        int multi_sessions_iter;
        QStringList multi_sessions = filter_dir.entryList();
        for(multi_sessions_iter = 0; multi_sessions_iter < multi_sessions.length(); multi_sessions_iter++)
        {
            if(!make_bipolar_names(multi_sessions[multi_sessions_iter]))
            {
                emit export_session_error(sessions[sessions_iter], RENAME_ERROR);
                continue;
            }

            QString session_info(QString(tr("%1/%2/v2-users-0000000-training-sessions-%3")).arg(default_dir).arg(multi_sessions[multi_sessions_iter]).arg(multi_sessions[multi_sessions_iter]));
            polar::v2::TrainingSession parser(session_info);

            qDebug("Parser: %s", session_info.toUtf8().constData());

            parser.setTcxOption(polar::v2::TrainingSession::ForceTcxUTC, true);

            if(!parser.parse())
                emit export_session_error(sessions[sessions_iter], PARSE_ERROR);

            if(mode & TCX_EXPORT)
            {
                QString tcx(QString(tr("%1/%2.tcx")).arg(default_dir).arg(multi_sessions[multi_sessions_iter]));
                if(!parser.writeTCX(tcx))
                    emit export_session_error(sessions[sessions_iter], TCX_ERROR);
            }

            if(mode & HRM_EXPORT)
            {
                QString hrm(QString(tr("%1/%2")).arg(default_dir).arg(multi_sessions[multi_sessions_iter]));
                QStringList hrm_out = parser.writeHRM(hrm);
                if(hrm_out.length() < 1)
                    emit export_session_error(sessions[sessions_iter], HRM_ERROR);
            }

            if(mode & GPX_EXPORT)
            {
                QString gpx(QString(tr("%1/%2.gpx")).arg(default_dir).arg(multi_sessions[multi_sessions_iter]));
                if(!parser.writeGPX(gpx))
                    emit export_session_error(sessions[sessions_iter], GPX_ERROR);
            }

            QDir(QString(tr("%1/%2")).arg(default_dir).arg(multi_sessions[multi_sessions_iter])).removeRecursively();
        }

        emit export_session_done(sessions_iter, sessions.length());
    }

    emit export_sessions_done();
}

bool V800export::make_bipolar_names(QString session)
{
    QSettings settings;
    QString default_dir = settings.value(tr("default_dir")).toString();
    QString session_path(QString(tr("%1/%2")).arg(default_dir).arg(session));

    QString file, new_file;

    file = QString(tr("%1/TSESS.BPB")).arg(session_path);
    new_file = QString(tr("%1/v2-users-0000000-training-sessions-%2-create")).arg(session_path).arg(session);
    if(QFile::exists(file))
        QFile::rename(file, new_file);
    else
        return false;

    file = QString(tr("%1/PHYSDATA.BPB")).arg(session_path);
    new_file = QString(tr("%1/v2-users-0000000-training-sessions-%2-physical-information")).arg(session_path).arg(session);
    if(QFile::exists(file))
        QFile::rename(file, new_file);
    else
        return false;

    file = QString(tr("%1/BASE.BPB")).arg(session_path);
    new_file = QString(tr("%1/v2-users-0000000-training-sessions-%2-exercises-%3-create")).arg(session_path).arg(session).arg(session);
    if(QFile::exists(file))
        QFile::rename(file, new_file);
    else
        return false;

    file = QString(tr("%1/ALAPS.BPB")).arg(session_path);
    new_file = QString(tr("%1/v2-users-0000000-training-sessions-%2-exercises-%3-autolaps")).arg(session_path).arg(session).arg(session);
    if(QFile::exists(file))
        QFile::rename(file, new_file);

    file = QString(tr("%1/LAPS.BPB")).arg(session_path);
    new_file = QString(tr("%1/v2-users-0000000-training-sessions-%2-exercises-%3-laps")).arg(session_path).arg(session).arg(session);
    if(QFile::exists(file))
        QFile::rename(file, new_file);

    file = QString(tr("%1/ROUTE.GZB")).arg(session_path);
    new_file = QString(tr("%1/v2-users-0000000-training-sessions-%2-exercises-%3-route")).arg(session_path).arg(session).arg(session);
    if(QFile::exists(file))
        QFile::rename(file, new_file);
    else
        return false;

    file = QString(tr("%1/SAMPLES.GZB")).arg(session_path);
    new_file = QString(tr("%1/v2-users-0000000-training-sessions-%2-exercises-%3-samples")).arg(session_path).arg(session).arg(session);
    if(QFile::exists(file))
        QFile::rename(file, new_file);

    file = QString(tr("%1/STATS.BPB")).arg(session_path);
    new_file = QString(tr("%1/v2-users-0000000-training-sessions-%2-exercises-%3-statistics")).arg(session_path).arg(session).arg(session);
    if(QFile::exists(file))
        QFile::rename(file, new_file);

    file = QString(tr("%1/ZONES.BPB")).arg(session_path);
    new_file = QString(tr("%1/v2-users-0000000-training-sessions-%2-exercises-%3-zones")).arg(session_path).arg(session).arg(session);
    if(QFile::exists(file))
        QFile::rename(file, new_file);

    file = QString(tr("%1/RR.GZB")).arg(session_path);
    new_file = QString(tr("%1/v2-users-0000000-training-sessions-%2-exercises-%3-rrsamples")).arg(session_path).arg(session).arg(session);
    if(QFile::exists(file))
        QFile::rename(file, new_file);

    return true;
}
