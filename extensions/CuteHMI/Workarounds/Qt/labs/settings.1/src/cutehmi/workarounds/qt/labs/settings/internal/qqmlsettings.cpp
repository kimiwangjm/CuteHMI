/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qqmlsettings_p.h"
#include <qcoreevent.h>
#include <qcoreapplication.h>
#include <qloggingcategory.h>
#include <qsettings.h>
#include <qpointer.h>
#include <qjsvalue.h>
#include <qqmlinfo.h>
#include <qdebug.h>
#include <qhash.h>

#include <cutehmi/workarounds/qt/labs/settings/logging.hpp>

namespace cutehmi {
namespace workarounds {
namespace qt {
namespace labs {
namespace settings {
namespace internal {

/*!
	\qmlmodule Qt.labs.settings 1.0
	\title Qt Labs Settings QML Types
	\ingroup qmlmodules
	\brief Provides persistent platform-independent application settings.

	To use this module, import the module with the following line:

	\code
	import Qt.labs.settings 1.0
	\endcode
*/

/*!
	\qmltype Settings
	\instantiates QQmlSettings
	\inqmlmodule Qt.labs.settings
	\ingroup settings
	\brief Provides persistent platform-independent application settings.

	The Settings type provides persistent platform-independent application settings.

	\note This type is made available by importing the \b Qt.labs.settings module.
	\e {Types in the Qt.labs module are not guaranteed to remain compatible
	in future versions.}

	Users normally expect an application to remember its settings (window sizes
	and positions, options, etc.) across sessions. The Settings type enables you
	to save and restore such application settings with the minimum of effort.

	Individual setting values are specified by declaring properties within a
	Settings element. All \l {QML Basic Types}{basic type} properties are
	supported. The recommended approach is to use property aliases in order
	to get automatic property updates both ways. The following example shows
	how to use Settings to store and restore the geometry of a window.

	\qml
	import QtQuick.Window 2.1
	import Qt.labs.settings 1.0

	Window {
		id: window

		width: 800
		height: 600

		Settings {
			property alias x: window.x
			property alias y: window.y
			property alias width: window.width
			property alias height: window.height
		}
	}
	\endqml

	At first application startup, the window gets default dimensions specified
	as 800x600. Notice that no default position is specified - we let the window
	manager handle that. Later when the window geometry changes, new values will
	be automatically stored to the persistent settings. The second application
	run will get initial values from the persistent settings, bringing the window
	back to the previous position and size.

	A fully declarative syntax, achieved by using property aliases, comes at the
	cost of storing persistent settings whenever the values of aliased properties
	change. Normal properties can be used to gain more fine-grained control over
	storing the persistent settings. The following example illustrates how to save
	a setting on component destruction.

	\qml
	import QtQuick 2.1
	import Qt.labs.settings 1.0

	Item {
		id: page

		state: settings.state

		states: [
			State {
				name: "active"
				// ...
			},
			State {
				name: "inactive"
				// ...
			}
		]

		Settings {
			id: settings
			property string state: "active"
		}

		Component.onDestruction: {
			settings.state = page.state
		}
	}
	\endqml

	Notice how the default value is now specified in the persistent setting property,
	and the actual property is bound to the setting in order to get the initial value
	from the persistent settings.

	\section1 Application Identifiers

	Application specific settings are identified by providing application
	\l {QCoreApplication::applicationName}{name},
	\l {QCoreApplication::organizationName}{organization} and
	\l {QCoreApplication::organizationDomain}{domain}, or by specifying
	\l fileName.

	\code
	#include <QGuiApplication>
	#include <QQmlApplicationEngine>

	int main(int argc, char *argv[])
	{
		QGuiApplication app(argc, argv);
		app.setOrganizationName("Some Company");
		app.setOrganizationDomain("somecompany.com");
		app.setApplicationName("Amazing Application");

		QQmlApplicationEngine engine("main.qml");
		return app.exec();
	}
	\endcode

	These are typically specified in C++ in the beginning of \c main(),
	but can also be controlled in QML via the following properties:
	\list
		\li \l {Qt::application}{Qt.application.name},
		\li \l {Qt::application}{Qt.application.organization} and
		\li \l {Qt::application}{Qt.application.domain}.
	\endlist

	\section1 Categories

	Application settings may be divided into logical categories by specifying
	a category name via the \l category property. Using logical categories not
	only provides a cleaner settings structure, but also prevents possible
	conflicts between setting keys.

	If several categories are required, use several Settings objects, each with
	their own category:

	\qml
	Item {
		id: panel

		visible: true

		Settings {
			category: "OutputPanel"
			property alias visible: panel.visible
			// ...
		}

		Settings {
			category: "General"
			property alias fontSize: fontSizeSpinBox.value
			// ...
		}
	}
	\endqml

	Instead of ensuring that all settings in the application have unique names,
	the settings can be divided into unique categories that may then contain
	settings using the same names that are used in other categories - without
	a conflict.

	\section1 Notes

	The current implementation is based on \l QSettings. This imposes certain
	limitations, such as missing change notifications. Writing a setting value
	using one instance of Settings does not update the value in another Settings
	instance, even if they are referring to the same setting in the same category.

	The information is stored in the system registry on Windows, and in XML
	preferences files on \macos. On other Unix systems, in the absence of a
	standard, INI text files are used. See \l QSettings documentation for
	more details.

	\sa QSettings
*/

static const int settingsWriteDelay = 500;

class QQmlSettingsPrivate
{
		Q_DECLARE_PUBLIC(QQmlSettings)

	public:
		QQmlSettingsPrivate();

		QSettings * instance() const;

		void init();
		void reset();

		void load();
		void store();

		void _q_propertyChanged();
		QVariant readProperty(const QMetaProperty & property) const;

		QQmlSettings * q_ptr = nullptr;
		int timerId = 0;
		bool initialized = false;
		QString category;
		QString fileName;
		mutable QPointer<QSettings> settings;
		QHash<const char *, QVariant> changedProperties;
};

QQmlSettingsPrivate::QQmlSettingsPrivate() {}

QSettings * QQmlSettingsPrivate::instance() const
{
	if (!settings) {
		QQmlSettings * q = const_cast<QQmlSettings *>(q_func());
		settings = fileName.isEmpty() ? new QSettings(q) : new QSettings(fileName, QSettings::IniFormat, q);
		if (settings->status() != QSettings::NoError) {
			// TODO: can't print out the enum due to the following error:
			// error: C2666: 'QQmlInfo::operator <<': 15 overloads have similar conversions
			qmlWarning(q) << "Failed to initialize QSettings instance. Status code is: " << int(settings->status());

			if (settings->status() == QSettings::AccessError) {
				QVector<QString> missingIdentifiers;
				if (QCoreApplication::organizationName().isEmpty())
					missingIdentifiers.append(QLatin1String("organizationName"));
				if (QCoreApplication::organizationDomain().isEmpty())
					missingIdentifiers.append(QLatin1String("organizationDomain"));
				if (QCoreApplication::applicationName().isEmpty())
					missingIdentifiers.append(QLatin1String("applicationName"));

				if (!missingIdentifiers.isEmpty())
					qmlWarning(q) << "The following application identifiers have not been set: " << missingIdentifiers;
			}
			return settings;
		}

		if (!category.isEmpty())
			settings->beginGroup(category);
		if (initialized)
			q->d_func()->load();
	}
	return settings;
}

void QQmlSettingsPrivate::init()
{
	if (!initialized) {
		CUTEHMI_DEBUG("QQmlSettings: stored at " << instance()->fileName());
		load();
		initialized = true;
	}
}

void QQmlSettingsPrivate::reset()
{
	if (initialized && settings && !changedProperties.isEmpty())
		store();
	delete settings;
}

void QQmlSettingsPrivate::load()
{
	Q_Q(QQmlSettings);
	const QMetaObject * mo = q->metaObject();
	const int offset = mo->propertyOffset();
	const int count = mo->propertyCount();

	// don't save built-in properties if there aren't any qml properties
	if (offset == 1)
		return;

	for (int i = offset; i < count; ++i) {
		QMetaProperty property = mo->property(i);

		const QVariant previousValue = readProperty(property);
		const QVariant currentValue = instance()->value(property.name(), previousValue);

		if (!currentValue.isNull() && (!previousValue.isValid()
						|| (currentValue.canConvert(previousValue.type()) && previousValue != currentValue))) {
			property.write(q, currentValue);
			CUTEHMI_DEBUG("QQmlSettings: load " << property.name() << " setting: " << currentValue << " default: " << previousValue);
		}

		// ensure that a non-existent setting gets written
		// even if the property wouldn't change later
		if (!instance()->contains(property.name()))
			_q_propertyChanged();

		// setup change notifications on first load
		if (!initialized && property.hasNotifySignal()) {
			static const int propertyChangedIndex = mo->indexOfSlot("_q_propertyChanged()");
			QMetaObject::connect(q, property.notifySignalIndex(), q, propertyChangedIndex);
		}
	}
}

void QQmlSettingsPrivate::store()
{
	QHash<const char *, QVariant>::const_iterator it = changedProperties.constBegin();
	while (it != changedProperties.constEnd()) {
		instance()->setValue(it.key(), it.value());
		CUTEHMI_DEBUG("QQmlSettings: store " << it.key() << ": " << it.value());
		++it;
	}
	changedProperties.clear();
}

void QQmlSettingsPrivate::_q_propertyChanged()
{
	Q_Q(QQmlSettings);
	const QMetaObject * mo = q->metaObject();
	const int offset = mo->propertyOffset();
	const int count = mo->propertyCount();
	for (int i = offset; i < count; ++i) {
		const QMetaProperty & property = mo->property(i);
		const QVariant value = readProperty(property);
		changedProperties.insert(property.name(), value);
		CUTEHMI_DEBUG("QQmlSettings: cache " << property.name() << ": " << value);
	}
	if (timerId != 0)
		q->killTimer(timerId);
	timerId = q->startTimer(settingsWriteDelay);
}

QVariant QQmlSettingsPrivate::readProperty(const QMetaProperty & property) const
{
	Q_Q(const QQmlSettings);
	QVariant var = property.read(q);
	if (var.userType() == qMetaTypeId<QJSValue>())
		var = var.value<QJSValue>().toVariant();
	return var;
}

QQmlSettings::QQmlSettings(QObject * parent)
	: QObject(parent), d_ptr(new QQmlSettingsPrivate)
{
	Q_D(QQmlSettings);
	d->q_ptr = this;
}

QQmlSettings::~QQmlSettings()
{
	Q_D(QQmlSettings);
	d->reset(); // flush pending changes
}

/*!
	\qmlproperty string Settings::category

	This property holds the name of the settings category.

	Categories can be used to group related settings together.
*/
QString QQmlSettings::category() const
{
	Q_D(const QQmlSettings);
	return d->category;
}

void QQmlSettings::setCategory(const QString & category)
{
	Q_D(QQmlSettings);
	if (d->category != category) {
		d->reset();
		d->category = category;
		if (d->initialized)
			d->load();
		emit categoryChanged();
	}
}

/*!
	\qmlproperty string Settings::fileName

	This property holds the path to the settings file. If the file doesn't
	already exist, it is created.

	\since Qt 5.12

	\sa QSettings::fileName, QSettings::IniFormat
*/
QString QQmlSettings::fileName() const
{
	Q_D(const QQmlSettings);
	return d->fileName;
}

void QQmlSettings::setFileName(const QString & fileName)
{
	Q_D(QQmlSettings);
	if (d->fileName != fileName) {
		d->reset();
		d->fileName = fileName;
		if (d->initialized)
			d->load();
		emit fileNameChanged();
	}
}

/*!
   \qmlmethod var Settings::value(string key, var defaultValue)

   Returns the value for setting \a key. If the setting doesn't exist,
   returns \a defaultValue.

   \since Qt 5.12

   \sa QSettings::value
*/
QVariant QQmlSettings::value(const QString & key, const QVariant & defaultValue) const
{
	Q_D(const QQmlSettings);
	return d->instance()->value(key, defaultValue);
}

/*!
   \qmlmethod Settings::setValue(string key, var value)

   Sets the value of setting \a key to \a value. If the key already exists,
   the previous value is overwritten.

   \since Qt 5.12

   \sa QSettings::setValue
*/
void QQmlSettings::setValue(const QString & key, const QVariant & value)
{
	Q_D(const QQmlSettings);
	d->instance()->setValue(key, value);
	CUTEHMI_DEBUG("QQmlSettings: setValue " << key << ": " << value);
}

/*!
   \qmlmethod Settings::sync()

	Writes any unsaved changes to permanent storage, and reloads any
	settings that have been changed in the meantime by another
	application.

	This function is called automatically from QSettings's destructor and
	by the event loop at regular intervals, so you normally don't need to
	call it yourself.

   \sa QSettings::sync
*/
void QQmlSettings::sync()
{
	Q_D(QQmlSettings);
	d->instance()->sync();
}

void QQmlSettings::remove(const QString & key)
{
	Q_D(QQmlSettings);
	d->instance()->remove(key);
}

void QQmlSettings::clear()
{
	Q_D(QQmlSettings);
	d->instance()->clear();
}

void QQmlSettings::classBegin()
{
}

void QQmlSettings::componentComplete()
{
	Q_D(QQmlSettings);
	d->init();
}

void QQmlSettings::timerEvent(QTimerEvent * event)
{
	Q_D(QQmlSettings);
	if (event->timerId() == d->timerId) {
		killTimer(d->timerId);
		d->timerId = 0;

		d->store();
	}
	QObject::timerEvent(event);
}

}
}
}
}
}
}

#include "moc_qqmlsettings_p.cpp"

//(c)C: Copyright © 2021-2022, Michał Policht <michal@policht.pl>, The Qt Company Ltd <sales@qt.io>. All rights reserved.
//(c)C: SPDX-License-Identifier: LGPL-3.0-or-later OR GPL-2.0-or-later
//(c)C: CuteHMI.Workarounds.Qt.labs.settings.1 extension is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version approved by the KDE Free Qt Foundation.
//(c)C: This file is a part of CuteHMI.Workarounds.Qt.labs.settings.1 extension.
//(c)C: Additionally, this file is licensed under terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version approved by the KDE Free Qt Foundation.
//(c)C: CuteHMI.Workarounds.Qt.labs.settings.1 extension is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or GNU General Public License for more details.
//(c)C: You should have received a copy of the GNU Lesser General Public License along with CuteHMI.Workarounds.Qt.labs.settings.1 extension. If not, see <https://www.gnu.org/licenses/>.
//(c)C: You should have received a copy of the GNU General Public License along with CuteHMI.Workarounds.Qt.labs.settings.1 extension. If not, see <https://www.gnu.org/licenses/>.
