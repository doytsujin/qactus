/*
 *  Qactus - A Qt based OBS notifier
 *
 *  Copyright (C) 2018-2019 Javier Llorente <javier@opensuse.org>
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

#include "metaconfigeditor.h"
#include "ui_metaconfigeditor.h"
#include <QTreeWidget>
#include <QHeaderView>

MetaConfigEditor::MetaConfigEditor(QWidget *parent, OBS *obs, const QString &project, const QString &package, MCEMode mode) :
    QDialog(parent),
    ui(new Ui::MetaConfigEditor),
    m_obs(obs),
    m_project(project),
    m_package(package),
    m_mode(mode)
{
    ui->setupUi(this);
    QString windowTitle;

    switch (m_mode) {
    case MCEMode::CreateProject:
        windowTitle = tr("Create project");
        ui->projectLineEdit->setText(project + ":");
        ui->packageLabel->hide();
        ui->packageLineEdit->hide();
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        break;
    case MCEMode::EditProject:
        windowTitle = tr("Edit project");
        ui->projectLineEdit->setText(m_project);
        ui->projectLineEdit->setDisabled(true);
        ui->packageLabel->hide();
        ui->packageLineEdit->hide();
        ui->titleLineEdit->setFocus();
        m_obs->getProjectMetaConfig(m_project);
        break;
    case MCEMode::CreatePackage:
        windowTitle = tr("Create package");
        ui->projectLineEdit->setText(m_project);
        ui->packageLineEdit->setFocus();
        ui->projectLineEdit->setDisabled(true);
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        break;
    case MCEMode::EditPackage:
        windowTitle = tr("Edit package");
        ui->projectLineEdit->setText(m_project);
        ui->projectLineEdit->setDisabled(true);
        ui->packageLineEdit->setText("");
        ui->packageLineEdit->setDisabled(true);
        ui->titleLineEdit->setFocus();
        m_obs->getPackageMetaConfig(m_project, m_package);
        break;
    }

    setWindowTitle(windowTitle);

    connect(this, &MetaConfigEditor::createProject, m_obs, &OBS::createProject);
    connect(this, &MetaConfigEditor::createPackage, m_obs, &OBS::createPackage);
    connect(m_obs, &OBS::finishedParsingCreatePrjStatus, this, &MetaConfigEditor::slotCreateResult);
    connect(m_obs, &OBS::finishedParsingCreatePkgStatus, this, &MetaConfigEditor::slotCreateResult);
    connect(m_obs, &OBS::cannotCreateProject, this, &MetaConfigEditor::slotCreateResult);
    connect(m_obs, &OBS::cannotCreatePackage, this, &MetaConfigEditor::slotCreateResult);

    connect(m_obs, &OBS::finishedParsingProjectMetaConfig, this, &MetaConfigEditor::slotFetchedProjectMetaConfig);
    connect(m_obs, &OBS::finishedParsingPackageMetaConfig, this, &MetaConfigEditor::slotFetchedPackageMetaConfig);
}

MetaConfigEditor::~MetaConfigEditor()
{
    delete ui;
}

void MetaConfigEditor::on_buttonBox_accepted()
{
    QProgressDialog progress(tr("Creating..."), nullptr, 0, 0, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    OBSXmlWriter *xmlWriter = new OBSXmlWriter(this);
    QByteArray data;

    switch (m_mode) {
    case MCEMode::CreateProject:
    case MCEMode::EditProject: {
        OBSPrjMetaConfig *prjMetaConfig = new OBSPrjMetaConfig();
        prjMetaConfig->setName(ui->projectLineEdit->text());
        prjMetaConfig->setTitle(ui->titleLineEdit->text());
        prjMetaConfig->setDescription(ui->descriptionTextEdit->toPlainText());
        prjMetaConfig->insertPerson(m_obs->getUsername(), "maintainer");

        // openSUSE Tumbleweed
        OBSRepository *twRepository = new OBSRepository("openSUSE_Tumbleweed", "openSUSE:Factory", "snapshot", "x86_64");
        prjMetaConfig->appendRepository(twRepository);

        // openSUSE Leap
        OBSRepository *leapRepository = new OBSRepository("openSUSE_Current", "openSUSE:Current", "standard", "x86_64");
        prjMetaConfig->appendRepository(leapRepository);

        data = xmlWriter->createProjectMeta(prjMetaConfig);
        emit createProject(ui->projectLineEdit->text(), data);
        break;
    }
    case MCEMode::CreatePackage:
    case MCEMode::EditPackage: {
        OBSPkgMetaConfig *pkgMetaConfig = new OBSPkgMetaConfig();
        pkgMetaConfig->setName(ui->packageLineEdit->text());
        pkgMetaConfig->setProject(ui->projectLineEdit->text());
        pkgMetaConfig->setTitle(ui->titleLineEdit->text());
        pkgMetaConfig->setDescription(ui->descriptionTextEdit->toPlainText());
        pkgMetaConfig->insertPerson(m_obs->getUsername(), "maintainer");
        data = xmlWriter->createPackageMeta(pkgMetaConfig);
        emit createPackage(ui->projectLineEdit->text(), ui->packageLineEdit->text(), data);
        break;
    }
    }

    delete xmlWriter;
}

void MetaConfigEditor::on_buttonBox_rejected()
{
    close();
}

void MetaConfigEditor::slotCreateResult(OBSStatus *obsStatus)
{
   qDebug() << __PRETTY_FUNCTION__ << obsStatus->getCode();
   const QString title = tr("Warning");
   const QString text = QString("<b>%1</b><br>%2").arg(obsStatus->getSummary(), obsStatus->getDetails());

   if (obsStatus->getCode() == "ok") {
       close();
   } else {
       QMessageBox::warning(this, title, text);
   }
   delete obsStatus;
   obsStatus = nullptr;
}

void MetaConfigEditor::slotFetchedProjectMetaConfig(OBSPrjMetaConfig *prjMetaConfig)
{

    QTreeWidgetItem *item = nullptr;
    QTreeWidget *tableRepositories = new QTreeWidget(ui->tabWidget);
    tableRepositories->setColumnWidth(0, 180);
    tableRepositories->setRootIsDecorated(false);
    tableRepositories->setAlternatingRowColors(true);
    QStringList headers = QStringList() << "Repository" << "Arch" << "Path";
    tableRepositories->setHeaderLabels(headers);
    ui->tabWidget->insertTab(1, tableRepositories, "Repositories");

    for (auto repository : prjMetaConfig->getRepositories()) {
        for (auto arch : repository->getArchs()) {
            item = new QTreeWidgetItem();
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            item->setText(0, repository->getName());
            item->setText(1, arch);
            item->setText(2, repository->getProject() + "/" + repository->getRepository());
            tableRepositories->addTopLevelItem(item);
        }
    }

    fillTabs(prjMetaConfig);
    delete prjMetaConfig;
}

void MetaConfigEditor::slotFetchedPackageMetaConfig(OBSPkgMetaConfig *pkgMetaConfig)
{
    ui->packageLineEdit->setText(pkgMetaConfig->getName());
    fillTabs(pkgMetaConfig);
    delete pkgMetaConfig;
}

void MetaConfigEditor::on_projectLineEdit_textChanged(const QString &project)
{
    bool enable = false;

    switch (m_mode) {
    case MCEMode::CreateProject:
    case MCEMode::EditProject:
        enable = !project.isEmpty();
        break;
    case MCEMode::CreatePackage:
    case MCEMode::EditPackage:
        enable = !project.isEmpty() && !ui->packageLineEdit->text().isEmpty();
        break;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enable);
}

void MetaConfigEditor::on_packageLineEdit_textChanged(const QString &package)
{
    bool enable = (!ui->projectLineEdit->text().isEmpty() && !package.isEmpty());
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enable);
}

void MetaConfigEditor::fillTabs(OBSMetaConfig *metaConfig)
{
    ui->titleLineEdit->setText(metaConfig->getTitle());
    ui->descriptionTextEdit->setText(metaConfig->getDescription());

    QWidget *repositoryFlags = new QWidget();
    QGridLayout *layoutRepositoryFlags = new QGridLayout();
    repositoryFlags->setLayout(layoutRepositoryFlags);
    ui->tabWidget->insertTab(2, repositoryFlags, "Repository flags");
    layoutRepositoryFlags->addWidget(createRepositoryTable("Build repository", metaConfig->getBuildFlag()));
    layoutRepositoryFlags->addWidget(createRepositoryTable("DebugInfo repository", metaConfig->getDebugInfoFlag()));
    layoutRepositoryFlags->addWidget(createRepositoryTable("Publish repository", metaConfig->getPublishFlag()));
    layoutRepositoryFlags->addWidget(createRepositoryTable("UseForBuild repository", metaConfig->getUseForBuildFlag()));

    ui->tabWidget->insertTab(3, createRoleTable("User", metaConfig->getPersons()), "Users");
    ui->tabWidget->insertTab(4, createRoleTable("Group", metaConfig->getGroups()), "Groups");
}

QTreeWidget *MetaConfigEditor::createRepositoryTable(const QString &header, const QHash<QString, bool> &flag)
{
    QTreeWidgetItem *item = nullptr;
    QTreeWidget *treeWidget = new QTreeWidget(ui->tabWidget);
    treeWidget->setColumnWidth(0, 325);
    treeWidget->setRootIsDecorated(false);
    QStringList headersRepositoryFlags = QStringList() << header << "Enabled";
    treeWidget->setHeaderLabels(headersRepositoryFlags);

    QStringList repositories = flag.keys();

    for (auto repository : repositories) {
        item = new QTreeWidgetItem();
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setText(0, repository);
        bool enabled = flag.value(repository);
        item->setText(1, QVariant(enabled).toString());
        treeWidget->addTopLevelItem(item);
    }

    return treeWidget;
}

QTreeWidget *MetaConfigEditor::createRoleTable(const QString &header, const QMultiHash<QString, QString> &userRoles)
{
    QTreeWidgetItem *item = nullptr;
    QTreeWidget *treeWidget = new QTreeWidget(ui->tabWidget);
    treeWidget->setColumnWidth(0, 150);
    treeWidget->setRootIsDecorated(false);
    treeWidget->setAlternatingRowColors(true);
    QStringList headersPersons = QStringList() << header << "Role";
    treeWidget->setHeaderLabels(headersPersons);

    QStringList users = userRoles.keys();
    QString userAdded;

    for (auto user : users) {
        QStringList roles = userRoles.values(user);

        if (user!=userAdded) {
            for (auto role : roles) {
                item = new QTreeWidgetItem();
                item->setFlags(item->flags() | Qt::ItemIsEditable);
                item->setText(0, user);
                item->setText(1, role);
                treeWidget->addTopLevelItem(item);
            }
            userAdded=user;
        }
    }
    treeWidget->sortItems(1, Qt::AscendingOrder);
    return treeWidget;
}
