/*
 *  Qactus - A Qt-based OBS client
 *
 *  Copyright (C) 2015-2019 Javier Llorente <javier@opensuse.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef OBS_H
#define OBS_H

#include <QObject>
#include "obscore.h"
#include "obsxmlreader.h"
#include "obsstatus.h"

#if defined(QOBS_LIBRARY)
#  define QOBS_EXPORT Q_DECL_EXPORT
#else
#  define QOBS_EXPORT Q_DECL_IMPORT
#endif

class QOBS_EXPORT OBS : public QObject
{
    Q_OBJECT

public:
    explicit OBS(QObject *parent = 0);
    void setCredentials(const QString &username, const QString &password);
    QString getUsername();
    void setApiUrl(const QString &apiUrl);
    QString getApiUrl() const;
    void request(const QString& resource);
    void request(QNetworkReply *reply);
    bool isAuthenticated();
    void login();
    void getBuildStatus(const QStringList &stringList, int row);
    void getRevisions(const QString &project, const QString &package);
    void getIncomingRequests();
    void getOutgoingRequests();
    void getDeclinedRequests();
    int getRequestCount();
    void getRequestDiff(const QString &source);
    bool isIncludeHomeProjects() const;
    void setIncludeHomeProjects(bool value);
    void getProjects();
    void getPackages(const QString &project);
    void getProjectMetadata(const QString &project);
    void getFiles(const QString &project, const QString &package);
    void getLink(const QString &project, const QString &package);
    void changeSubmitRequest(const QString &resource, const QByteArray &data);
    OBSXmlReader *getXmlReader();
    void branchPackage(const QString &project, const QString &package);
    void deleteProject(const QString &project);
    void deletePackage(const QString &project, const QString &package);
    void deleteFile(const QString &project, const QString &package, const QString &fileName);
    void about();

private:
    OBSCore *obsCore;
    OBSXmlReader *xmlReader;
    bool includeHomeProjects;
    void postRequest(const QString &resource, const QByteArray &data, const QString &contentTypeHeader);
    void deleteRequest(const QString &resource);

signals:
    void apiNotFound(QUrl url);
    void isAuthenticated(bool);
    void selfSignedCertificate(QNetworkReply*);
    void networkError(const QString&);
    void finishedParsingPackage(OBSStatus*, int);
    void finishedParsingBranchPackage(OBSStatus*);
    void finishedParsingCreateRequest(OBSRequest*);
    void finishedParsingCreateRequestStatus(OBSStatus*);
    void finishedParsingCreatePrjStatus(OBSStatus*);
    void finishedParsingCreatePkgStatus(OBSStatus*);
    void cannotCreateProject(OBSStatus *obsStatus);
    void cannotCreatePackage(OBSStatus *obsStatus);
    void finishedParsingUploadFileRevision(OBSRevision*);
    void cannotUploadFile(OBSStatus *obsStatus);
    void fileFetched(const QString &fileName, const QByteArray &data);
    void buildLogFetched(const QString &buildLog);
    void buildLogNotFound();
    void finishedParsingDeletePrjStatus(OBSStatus*);
    void finishedParsingDeletePkgStatus(OBSStatus*);
    void finishedParsingDeleteFileStatus(OBSStatus*);
    void cannotDeleteProject(OBSStatus *obsStatus);
    void cannotDeletePackage(OBSStatus *obsStatus);
    void cannotDeleteFile(OBSStatus *obsStatus);
    void finishedParsingResult(OBSResult*);
    void finishedParsingResultList();
    void finishedParsingRevision(OBSRevision*);
    void finishedParsingIncomingRequest(OBSRequest *request);
    void finishedParsingIncomingRequestList();
    void finishedParsingOutgoingRequest(OBSRequest *request);
    void finishedParsingOutgoingRequestList();
    void finishedParsingDeclinedRequest(OBSRequest *request);
    void finishedParsingDeclinedRequestList();
    void finishedParsingProjectList(QStringList);
    void finishedParsingProjectMetadata(QStringList);
    void finishedParsingPackageList(QStringList);
    void finishedParsingList(QStringList);
    void finishedParsingFile(OBSFile*);
    void finishedParsingFileList();
    void finishedParsingLink(OBSLink *obsLink);
    void srStatus(const QString &);
    void srDiffFetched(const QString &);
    void finishedParsingAbout(OBSAbout *obsAbout);

public slots:
    void getAllBuildStatus(const QString &project, const QString &package);
    void slotChangeSubmitRequest(const QString &id, const QString &comments, bool accepted);
    void srChangeResult(OBSStatus *obsStatus);
    void createRequest(const QByteArray &data);
    void createProject(const QString &project, const QByteArray &data);
    void createPackage(const QString &project, const QString &package, const QByteArray &data);
    void uploadFile(const QString &project, const QString &package, const QString &fileName, const QByteArray &data);
    void downloadFile(const QString &project, const QString &package, const QString &fileName);
    void getBuildLog(const QString &project, const QString &repository, const QString &arch, const QString &package);
};

#endif // OBS_H
