/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "dfmapplication.h"
#include "private/dfmapplication_p.h"

#include "dfmsettings.h"

#include <QCoreApplication>
#include <QMetaEnum>

DFM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC_WITH_ARGS(DFMSettings, gsGlobal, ("deepin/dde-file-manager", DFMSettings::GenericConfig))
Q_GLOBAL_STATIC_WITH_ARGS(DFMSettings, asGlobal, ("dde-file-manager", DFMSettings::AppConfig))

DFMApplication *DFMApplicationPrivate::self = nullptr;

DFMApplicationPrivate::DFMApplicationPrivate(DFMApplication *qq)
{
    Q_ASSERT_X(!self, "DFMApplication", "there should be only one application object");
    self = qq;
}

void DFMApplicationPrivate::_q_onSettingsValueChanged(const QString &group, const QString &key, const QVariant &value, bool edited)
{
    if (group == QT_STRINGIFY(ApplicationAttribute)) {
        const QMetaEnum &me = QMetaEnum::fromType<DFMApplication::ApplicationAttribute>();

        DFMApplication::ApplicationAttribute aa = (DFMApplication::ApplicationAttribute)me.keyToValue(QByteArray("AA_" + key.toLatin1()).constData());

        if (edited)
            Q_EMIT self->appAttributeEdited(aa, value);

        Q_EMIT self->appAttributeChanged(aa, value);

        if (aa == DFMApplication::AA_IconSizeLevel) {
            Q_EMIT self->iconSizeLevelChanged(value.toInt());
        } else if (aa == DFMApplication::AA_ViewMode) {
            Q_EMIT self->viewModeChanged(value.toInt());
        }
    } else if (group == QT_STRINGIFY(GenericAttribute)) {
        const QMetaEnum &me = QMetaEnum::fromType<DFMApplication::GenericAttribute>();

        DFMApplication::GenericAttribute ga = (DFMApplication::GenericAttribute)me.keyToValue(QByteArray("GA_" + key.toLatin1()).constData());

        if (edited)
            Q_EMIT self->genericAttributeEdited(ga, value);

        Q_EMIT self->genericAttributeChanged(ga, value);

        switch (ga) {
        case DFMApplication::GA_PreviewDocumentFile:
        case DFMApplication::GA_PreviewImage:
        case DFMApplication::GA_PreviewTextFile:
        case DFMApplication::GA_PreviewVideo:
            Q_EMIT self->previewAttributeChanged(ga, value.toBool());
            break;
        case DFMApplication::GA_ShowedHiddenFiles:
            Q_EMIT self->showedHiddenFilesChanged(value.toBool());
            break;
        case DFMApplication::GA_PreviewCompressFile:
            Q_EMIT self->previewCompressFileChanged(value.toBool());
        default:
            break;
        }
    }
}

void DFMApplicationPrivate::_q_onSettingsValueEdited(const QString &group, const QString &key, const QVariant &value)
{
    _q_onSettingsValueChanged(group, key, value, true);
}

DFMApplication::DFMApplication(QObject *parent)
    : DFMApplication(new DFMApplicationPrivate(this), parent)
{
    qRegisterMetaType<ApplicationAttribute>();
    qRegisterMetaType<GenericAttribute>();
}

DFMApplication::~DFMApplication()
{

}

QVariant DFMApplication::appAttribute(DFMApplication::ApplicationAttribute aa) const
{
    const QString group(QT_STRINGIFY(ApplicationAttribute));
    const QMetaEnum &me = QMetaEnum::fromType<ApplicationAttribute>();
    const QString key = QString::fromLatin1(me.valueToKey(aa)).split("_").last();

    return appSetting()->value(group, key);
}

void DFMApplication::setAppAttribute(DFMApplication::ApplicationAttribute aa, const QVariant &value)
{
    const QString group(QT_STRINGIFY(ApplicationAttribute));
    const QMetaEnum &me = QMetaEnum::fromType<ApplicationAttribute>();
    const QString key = QString::fromLatin1(me.valueToKey(aa)).split("_").last();

    appSetting()->setValue(group, key, value);
}

bool DFMApplication::syncAppAttribute()
{
    return appSetting()->sync();
}

QVariant DFMApplication::genericAttribute(DFMApplication::GenericAttribute ga) const
{
    const QString group(QT_STRINGIFY(GenericAttribute));
    const QMetaEnum &me = QMetaEnum::fromType<GenericAttribute>();
    const QString key = QString::fromLatin1(me.valueToKey(ga)).split("_").last();

    return genericSetting()->value(group, key);
}

void DFMApplication::setGenericAttribute(DFMApplication::GenericAttribute ga, const QVariant &value)
{
    const QString group(QT_STRINGIFY(GenericAttribute));
    const QMetaEnum &me = QMetaEnum::fromType<GenericAttribute>();
    const QString key = QString::fromLatin1(me.valueToKey(ga)).split("_").last();

    genericSetting()->setValue(group, key, value);
}

bool DFMApplication::syncGenericAttribute()
{
    return genericSetting()->sync();
}

DFMApplication *DFMApplication::instance()
{
    return DFMApplicationPrivate::self;
}

DFMSettings *DFMApplication::genericSetting()
{
    if (!gsGlobal.exists()) {
        if (instance()) {
            gsGlobal->moveToThread(instance()->thread());
            connect(gsGlobal, SIGNAL(valueChanged(QString, QString, QVariant)),
                    instance(), SLOT(_q_onSettingsValueChanged(QString, QString, QVariant)));
            connect(gsGlobal, SIGNAL(valueEdited(QString, QString, QVariant)),
                    instance(), SLOT(_q_onSettingsValueEdited(QString, QString, QVariant)));
        }

        gsGlobal->setAutoSync(true);
        gsGlobal->setWatchChanges(true);

        Q_EMIT instance()->genericSettingCreated(gsGlobal);
    }

    return gsGlobal;
}

DFMSettings *DFMApplication::appSetting()
{
    if (!asGlobal.exists()) {
        if (instance()) {
            asGlobal->moveToThread(instance()->thread());
            connect(asGlobal, SIGNAL(valueChanged(QString, QString, QVariant)),
                    instance(), SLOT(_q_onSettingsValueChanged(QString, QString, QVariant)));
            connect(asGlobal, SIGNAL(valueEdited(QString, QString, QVariant)),
                    instance(), SLOT(_q_onSettingsValueEdited(QString, QString, QVariant)));
        }

        asGlobal->setAutoSync(true);
        asGlobal->setWatchChanges(true);

        Q_EMIT instance()->appSettingCreated(asGlobal);
    }

    return asGlobal;
}

DFMApplication::DFMApplication(DFMApplicationPrivate *dd, QObject *parent)
    : QObject(parent)
    , d_ptr(dd)
{
    if (gsGlobal.exists()) {
        gsGlobal->moveToThread(thread());
        connect(gsGlobal, SIGNAL(valueChanged(QString, QString, QVariant)),
                this, SLOT(_q_onSettingsValueChanged(QString, QString, QVariant)));
    }

    if (asGlobal.exists()) {
        asGlobal->moveToThread(thread());
        connect(asGlobal, SIGNAL(valueChanged(QString, QString, QVariant)),
                this, SLOT(_q_onSettingsValueChanged(QString, QString, QVariant)));
    }
}

DFM_END_NAMESPACE

#include "moc_dfmapplication.cpp"