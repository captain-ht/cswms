/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Solutions component.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#pragma execution_character_set("utf-8")
#include <QMetaObject>
#include <QMetaProperty>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QApplication>
#include <QDebug>
#include "qtobjectcontroller.h"
#include "qtvariantproperty.h"
#include "qtgroupboxpropertybrowser.h"
#include "qttreepropertybrowser.h"
#include "qtpropertybrowser.h"
#include "qtchinesename.h"

class QtObjectControllerPrivate
{
    QtObjectController *q_ptr;
    Q_DECLARE_PUBLIC(QtObjectController)
public:
    void expandAll();
    void collapseAll();
    void addClassProperties(const QMetaObject *metaObject);
    void addClassPropertiesParent(const QMetaObject *metaObject);
    void updateClassProperties(const QMetaObject *metaObject, bool recursive);
    void saveExpandedState();
    void restoreExpandedState();
    void slotValueChanged(QtProperty *property, const QVariant &value);
    int enumToInt(const QMetaEnum &metaEnum, int enumValue) const;
    int intToEnum(const QMetaEnum &metaEnum, int intValue) const;
    int flagToInt(const QMetaEnum &metaEnum, int flagValue) const;
    int intToFlag(const QMetaEnum &metaEnum, int intValue) const;
    bool isSubValue(int value, int subValue) const;
    bool isPowerOf2(int value) const;

    QObject                  *m_object;

    QMap<const QMetaObject *, QtProperty *> m_classToProperty;
    QMap<QtProperty *, const QMetaObject *> m_propertyToClass;
    QMap<QtProperty *, int>     m_propertyToIndex;
    QMap<const QMetaObject *, QMap<int, QtVariantProperty *> > m_classToIndexToProperty;

    QMap<QtProperty *, bool>    m_propertyToExpanded;

    QList<QtProperty *>         m_topLevelProperties;

    QtAbstractPropertyBrowser *m_browser;
    QtVariantPropertyManager *m_manager;
    QtVariantPropertyManager *m_readOnlyManager;
};

int QtObjectControllerPrivate::enumToInt(const QMetaEnum &metaEnum, int enumValue) const
{
    QMap<int, int> valueMap; // dont show multiple enum values which have the same values
    int pos = 0;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value)) {
            if (value == enumValue) {
                return pos;
            }
            valueMap[value] = pos++;
        }
    }
    return -1;
}

int QtObjectControllerPrivate::intToEnum(const QMetaEnum &metaEnum, int intValue) const
{
    QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
    QList<int> values;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value)) {
            valueMap[value] = true;
            values.append(value);
        }
    }
    if (intValue >= values.count()) {
        return -1;
    }
    return values.at(intValue);
}

bool QtObjectControllerPrivate::isSubValue(int value, int subValue) const
{
    if (value == subValue) {
        return true;
    }
    int i = 0;
    while (subValue) {
        if (!(value & (1 << i))) {
            if (subValue & 1) {
                return false;
            }
        }
        i++;
        subValue = subValue >> 1;
    }
    return true;
}

bool QtObjectControllerPrivate::isPowerOf2(int value) const
{
    while (value) {
        if (value & 1) {
            return value == 1;
        }
        value = value >> 1;
    }
    return false;
}

int QtObjectControllerPrivate::flagToInt(const QMetaEnum &metaEnum, int flagValue) const
{
    if (!flagValue) {
        return 0;
    }
    int intValue = 0;
    QMap<int, int> valueMap; // dont show multiple enum values which have the same values
    int pos = 0;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value) && isPowerOf2(value)) {
            if (isSubValue(flagValue, value)) {
                intValue |= (1 << pos);
            }
            valueMap[value] = pos++;
        }
    }
    return intValue;
}

int QtObjectControllerPrivate::intToFlag(const QMetaEnum &metaEnum, int intValue) const
{
    QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
    QList<int> values;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value) && isPowerOf2(value)) {
            valueMap[value] = true;
            values.append(value);
        }
    }
    int flagValue = 0;
    int temp = intValue;
    int i = 0;
    while (temp) {
        if (i >= values.count()) {
            return -1;
        }
        if (temp & 1) {
            flagValue |= values.at(i);
        }
        i++;
        temp = temp >> 1;
    }
    return flagValue;
}

void QtObjectControllerPrivate::updateClassProperties(const QMetaObject *metaObject, bool recursive)
{
    if (!metaObject) {
        return;
    }

    if (recursive) {
        updateClassProperties(metaObject->superClass(), recursive);
    }

    QtProperty *classProperty = m_classToProperty.value(metaObject);
    if (!classProperty) {
        return;
    }

    for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++) {
        QMetaProperty metaProperty = metaObject->property(idx);
        if (metaProperty.isReadable()) {
            if (m_classToIndexToProperty.contains(metaObject) && m_classToIndexToProperty[metaObject].contains(idx)) {
                QtVariantProperty *subProperty = m_classToIndexToProperty[metaObject][idx];
                if (metaProperty.isEnumType()) {
                    if (metaProperty.isFlagType()) {
                        subProperty->setValue(flagToInt(metaProperty.enumerator(), metaProperty.read(m_object).toInt()));
                    } else {
                        subProperty->setValue(enumToInt(metaProperty.enumerator(), metaProperty.read(m_object).toInt()));
                    }
                } else {
                    subProperty->setValue(metaProperty.read(m_object));
                }
            }
        }
    }
}

void QtObjectControllerPrivate::expandAll()
{
    //打开所有节点
    QtTreePropertyBrowser *browser = (QtTreePropertyBrowser *)m_browser;
    browser->expandAll();
}

void QtObjectControllerPrivate::collapseAll()
{
    //折叠所有节点
    QtTreePropertyBrowser *browser = (QtTreePropertyBrowser *)m_browser;
    browser->collapseAll();

    //展开父节点
    QList<QtBrowserItem *> items = browser->topLevelItems();
    foreach (QtBrowserItem *item, items) {
        browser->setExpanded(item, true);
    }
}

void QtObjectControllerPrivate::addClassProperties(const QMetaObject *metaObject)
{
    if (!metaObject) {
        return;
    }

    QtProperty *classProperty = m_classToProperty.value(metaObject);
    if (!classProperty) {
        QString className = QLatin1String(metaObject->className());
        //这里添加的类名
        QString controlName = QtChineseName::getControlName(className) + " - " + className;
        classProperty = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), controlName);
        m_classToProperty[metaObject] = classProperty;
        m_propertyToClass[classProperty] = metaObject;

        for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++) {
            QMetaProperty metaProperty = metaObject->property(idx);
            int type = metaProperty.userType();
            QtVariantProperty *subProperty = 0;

            //将英文属性换成中文属性
            QString propertyName = metaProperty.name();
            propertyName = QtChineseName::getPropertyName(propertyName, className);

            if (!metaProperty.isReadable()) {
                subProperty = m_readOnlyManager->addProperty(QVariant::String, propertyName);
                subProperty->setValue(QLatin1String("< Non Readable >"));
            } else if (metaProperty.isEnumType()) {
                if (metaProperty.isFlagType()) {
                    subProperty = m_manager->addProperty(QtVariantPropertyManager::flagTypeId(), propertyName);
                    QMetaEnum metaEnum = metaProperty.enumerator();
                    QMap<int, bool> valueMap;
                    QStringList flagNames;
                    for (int i = 0; i < metaEnum.keyCount(); i++) {
                        int value = metaEnum.value(i);
                        if (!valueMap.contains(value) && isPowerOf2(value)) {
                            valueMap[value] = true;
                            flagNames.append(QLatin1String(metaEnum.key(i)));
                        }
                        subProperty->setAttribute(QLatin1String("flagNames"), flagNames);
                        subProperty->setValue(flagToInt(metaEnum, metaProperty.read(m_object).toInt()));
                    }
                } else {
                    subProperty = m_manager->addProperty(QtVariantPropertyManager::enumTypeId(), propertyName);
                    QMetaEnum metaEnum = metaProperty.enumerator();
                    QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
                    QStringList enumNames;
                    for (int i = 0; i < metaEnum.keyCount(); i++) {
                        int value = metaEnum.value(i);
                        if (!valueMap.contains(value)) {
                            valueMap[value] = true;

                            //将枚举类型强制转为中文
                            QString enumName = metaEnum.key(i);
                            enumName = QtChineseName::getPropertyName(enumName, className);
                            enumNames.append(enumName);
                        }
                    }
                    subProperty->setAttribute(QLatin1String("enumNames"), enumNames);
                    subProperty->setValue(enumToInt(metaEnum, metaProperty.read(m_object).toInt()));
                }
            } else if (m_manager->isPropertyTypeSupported(type)) {
                if (!metaProperty.isWritable()) {
                    subProperty = m_readOnlyManager->addProperty(type, propertyName + QLatin1String(" (Non Writable)"));
                }
                if (!metaProperty.isDesignable()) {
                    subProperty = m_readOnlyManager->addProperty(type, propertyName + QLatin1String(" (Non Designable)"));
                } else {
                    subProperty = m_manager->addProperty(type, propertyName);
                }
                subProperty->setValue(metaProperty.read(m_object));
            } else {
                subProperty = m_readOnlyManager->addProperty(QVariant::String, propertyName);
                subProperty->setValue(QLatin1String("< Unknown Type >"));
                subProperty->setEnabled(false);
            }
            classProperty->addSubProperty(subProperty);
            m_propertyToIndex[subProperty] = idx;
            m_classToIndexToProperty[metaObject][idx] = subProperty;
        }
    } else {
        updateClassProperties(metaObject, false);
    }

    m_topLevelProperties.append(classProperty);
    m_browser->addProperty(classProperty);
}

void QtObjectControllerPrivate::addClassPropertiesParent(const QMetaObject *metaObject)
{
    if (!metaObject) {
        return;
    }

    //存储需要过滤的属性,有时候大部分属性都用不上
    QStringList keyName;
    keyName << "geometry";

    QtProperty *classProperty = m_classToProperty.value(metaObject);
    if (!classProperty) {
        QString className = QLatin1String(metaObject->className());
        //这里添加父类名称
        QString controlName = "父类控件 - " + className;
        classProperty = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), controlName);
        m_classToProperty[metaObject] = classProperty;
        m_propertyToClass[classProperty] = metaObject;

        for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++) {
            QMetaProperty metaProperty = metaObject->property(idx);
            int type = metaProperty.userType();
            QtVariantProperty *subProperty = 0;

            //如果当前属性不在需要过滤的属性中则继续下一个属性判断
            QString propertyName = metaProperty.name();
            if (!keyName.contains(propertyName)) {
                continue;
            }

            propertyName = QtChineseName::getPropertyName(propertyName, className);

            if (!metaProperty.isReadable()) {
                subProperty = m_readOnlyManager->addProperty(QVariant::String, propertyName);
                subProperty->setValue(QLatin1String("< Non Readable >"));
            } else if (metaProperty.isEnumType()) {
                if (metaProperty.isFlagType()) {
                    subProperty = m_manager->addProperty(QtVariantPropertyManager::flagTypeId(), propertyName);
                    QMetaEnum metaEnum = metaProperty.enumerator();
                    QMap<int, bool> valueMap;
                    QStringList flagNames;
                    for (int i = 0; i < metaEnum.keyCount(); i++) {
                        int value = metaEnum.value(i);
                        if (!valueMap.contains(value) && isPowerOf2(value)) {
                            valueMap[value] = true;
                            flagNames.append(QLatin1String(metaEnum.key(i)));
                        }
                        subProperty->setAttribute(QLatin1String("flagNames"), flagNames);
                        subProperty->setValue(flagToInt(metaEnum, metaProperty.read(m_object).toInt()));
                    }
                } else {
                    subProperty = m_manager->addProperty(QtVariantPropertyManager::enumTypeId(), propertyName);
                    QMetaEnum metaEnum = metaProperty.enumerator();
                    QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
                    QStringList enumNames;
                    for (int i = 0; i < metaEnum.keyCount(); i++) {
                        int value = metaEnum.value(i);
                        if (!valueMap.contains(value)) {
                            valueMap[value] = true;
                            enumNames.append(QLatin1String(metaEnum.key(i)));
                        }
                    }
                    subProperty->setAttribute(QLatin1String("enumNames"), enumNames);
                    subProperty->setValue(enumToInt(metaEnum, metaProperty.read(m_object).toInt()));
                }
            } else if (m_manager->isPropertyTypeSupported(type)) {
                if (!metaProperty.isWritable()) {
                    subProperty = m_readOnlyManager->addProperty(type, propertyName + QLatin1String(" (Non Writable)"));
                }
                if (!metaProperty.isDesignable()) {
                    subProperty = m_readOnlyManager->addProperty(type, propertyName + QLatin1String(" (Non Designable)"));
                } else {
                    subProperty = m_manager->addProperty(type, propertyName);
                }
                subProperty->setValue(metaProperty.read(m_object));
            } else {
                subProperty = m_readOnlyManager->addProperty(QVariant::String, propertyName);
                subProperty->setValue(QLatin1String("< Unknown Type >"));
                subProperty->setEnabled(false);
            }
            classProperty->addSubProperty(subProperty);
            m_propertyToIndex[subProperty] = idx;
            m_classToIndexToProperty[metaObject][idx] = subProperty;
        }
    } else {
        updateClassProperties(metaObject, false);
    }

    m_topLevelProperties.append(classProperty);
    m_browser->addProperty(classProperty);
}

void QtObjectControllerPrivate::saveExpandedState()
{

}

void QtObjectControllerPrivate::restoreExpandedState()
{

}

void QtObjectControllerPrivate::slotValueChanged(QtProperty *property, const QVariant &value)
{
    if (!m_propertyToIndex.contains(property)) {
        return;
    }

    int idx = m_propertyToIndex.value(property);

    const QMetaObject *metaObject = m_object->metaObject();
    QMetaProperty metaProperty = metaObject->property(idx);
    if (metaProperty.isEnumType()) {
        if (metaProperty.isFlagType()) {
            metaProperty.write(m_object, intToFlag(metaProperty.enumerator(), value.toInt()));
        } else {
            metaProperty.write(m_object, intToEnum(metaProperty.enumerator(), value.toInt()));
        }
    } else {
        metaProperty.write(m_object, value);
    }

    updateClassProperties(metaObject, true);
}

///////////////////

QtObjectController::QtObjectController(QWidget *parent)
    : QWidget(parent)
{
    d_ptr = new QtObjectControllerPrivate;
    d_ptr->q_ptr = this;

    d_ptr->m_object = 0;
    /*
        QScrollArea *scroll = new QScrollArea(this);
        scroll->setWidgetResizable(true);

        d_ptr->m_browser = new QtGroupBoxPropertyBrowser(this);
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargin(0, 0, 0, 0);
        layout->addWidget(scroll);
        scroll->setWidget(d_ptr->m_browser);
    */
    QtTreePropertyBrowser *browser = new QtTreePropertyBrowser(this);
    browser->setRootIsDecorated(false);
    d_ptr->m_browser = browser;
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(d_ptr->m_browser);

    d_ptr->m_readOnlyManager = new QtVariantPropertyManager(this);
    d_ptr->m_manager = new QtVariantPropertyManager(this);
    QtVariantEditorFactory *factory = new QtVariantEditorFactory(this);
    d_ptr->m_browser->setFactoryForManager(d_ptr->m_manager, factory);

    connect(d_ptr->m_manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
            this, SLOT(slotValueChanged(QtProperty *, const QVariant &)));
}

QtObjectController::~QtObjectController()
{
    delete d_ptr;
}

void QtObjectController::setObject(QObject *object)
{
    //如果设置的控件已经是当前控件则不处理
    if (d_ptr->m_object == object) {
        return;
    }

    if (d_ptr->m_object) {
        d_ptr->saveExpandedState();
        QListIterator<QtProperty *> it(d_ptr->m_topLevelProperties);
        while (it.hasNext()) {
            d_ptr->m_browser->removeProperty(it.next());
        }
        d_ptr->m_topLevelProperties.clear();
    }

    d_ptr->m_object = object;

    if (!d_ptr->m_object) {
        return;
    }

    //加载父类的属性
    d_ptr->addClassPropertiesParent(d_ptr->m_object->metaObject()->superClass());
    //加载当前控件的属性
    d_ptr->addClassProperties(d_ptr->m_object->metaObject());
    //存储节点状态
    d_ptr->restoreExpandedState();

    //折叠所有节点
    d_ptr->collapseAll();
}

QObject *QtObjectController::object() const
{
    return d_ptr->m_object;
}

#include "moc_qtobjectcontroller.cpp"
