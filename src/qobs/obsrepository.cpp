/*
 *  Qactus - A Qt-based OBS client
 *
 *  Copyright (C) 2019 Javier Llorente <javier@opensuse.org>
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

#include "obsrepository.h"

OBSRepository::OBSRepository()
{

}

QString OBSRepository::getName() const
{
    return name;
}

void OBSRepository::setName(const QString &value)
{
    name = value;
}

QString OBSRepository::getProject() const
{
    return project;
}

void OBSRepository::setProject(const QString &value)
{
    project = value;
}

QString OBSRepository::getRepository() const
{
    return repository;
}

void OBSRepository::setRepository(const QString &value)
{
    repository = value;
}

QString OBSRepository::getArch() const
{
    return arch;
}

void OBSRepository::setArch(const QString &value)
{
    arch = value;
}
