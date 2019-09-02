// enve - 2D animations software
// Copyright (C) 2016-2019 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef WRAPPERNODE_H
#define WRAPPERNODE_H

#include <QBoxLayout>
#include <QtCore>
#include "smartPointers/ememory.h"
#include "widgetstack.h"
#include "basicreadwrite.h"

enum class WrapperNodeType {
    base,
    widget,
    splitH,
    splitV
};

class SplitWrapperNode;
class ParentWrapperNode;
class WidgetWrapperNode;
class Canvas;

class WrapperNode {
public:
    typedef std::function<WidgetWrapperNode*(Canvas*)> WrapperNodeCreator;
    WrapperNode(const WrapperNodeType type,
                const WrapperNodeCreator& creator) :
        fType(type), fCreator(creator) {}
    virtual ~WrapperNode() {}

    const WrapperNodeType fType;
    const WrapperNodeCreator fCreator;
    ParentWrapperNode* fParent = nullptr;

    virtual QWidget* widget() = 0;

    void write(eWriteStream& dst) {
        dst.write(&fType, sizeof(WrapperNodeType));
        writeData(dst);
    }

    static WrapperNode *sRead(eReadStream& src,
                              const WrapperNodeCreator& creator);
protected:
    virtual void readData(eReadStream& src) = 0;
    virtual void writeData(eWriteStream& dst) = 0;
};

class ParentWrapperNode : public WrapperNode {
protected:
    using WrapperNode::WrapperNode;
public:
    virtual void closeChild(WrapperNode* const child) = 0;

    virtual void replaceChild(WrapperNode* const child,
                              WrapperNode* const with) = 0;
};

class BaseWrapperNode : public QWidget, public ParentWrapperNode {
public:
    BaseWrapperNode(const WrapperNodeCreator& creator) :
        ParentWrapperNode(WrapperNodeType::base, creator) {
        setLayout(new QVBoxLayout);
        layout()->setContentsMargins(0, 0, 0, 0);
    }

    WrapperNode* fChild = nullptr;

    void closeChild(WrapperNode* const child) {
        Q_UNUSED(child);
        return;
//        if(child == fChild) fChild.reset();
//        else Q_ASSERT(false);
    }

    void replaceChild(WrapperNode* const child,
                      WrapperNode* const with) {
        layout()->replaceWidget(child->widget(), with->widget());

        with->fParent = this;
        if(child == fChild) fChild = with;
        else Q_ASSERT(false);
    }

    QWidget* widget() { return this; }

    void reset(Canvas * const scene = nullptr);

    void readData(eReadStream& src) {
        const auto newChild = sRead(src, fCreator);
        newChild->fParent = this;
        if(fChild) layout()->replaceWidget(fChild->widget(), newChild->widget());
        else layout()->addWidget(newChild->widget());
        fChild = newChild;
    }

    void writeData(eWriteStream& dst) {
        fChild->write(dst);
    }
private:
    using WrapperNode::write;
};

class SplitWrapperNode : public ParentWrapperNode {
protected:
    using ParentWrapperNode::ParentWrapperNode;
public:
    WrapperNode* fChild1 = nullptr;
    WrapperNode* fChild2 = nullptr;

    void closeChild(WrapperNode* const child) {
        Q_ASSERT(child == fChild1 || child == fChild2);
        const auto remChild = child == fChild1 ? fChild2 : fChild1;
        takeWidget(remChild->widget());
        fParent->replaceChild(this, remChild);
        delete this;
    }

    void replaceChild(WrapperNode* const child,
                      WrapperNode* const with) {
        with->fParent = this;
        if(child == fChild1) fChild1 = with;
        else if(child == fChild2) fChild2 = with;
        else Q_ASSERT(false);
    }
protected:
    virtual QWidget* takeWidget(QWidget* const wid) = 0;

    void readData(eReadStream& src) {
        fChild1 = sRead(src, fCreator);
        fChild2 = sRead(src, fCreator);

        fChild1->fParent = this;
        fChild2->fParent = this;
    }

    void writeData(eWriteStream& dst) {
        fChild1->write(dst);
        fChild2->write(dst);
    }
};

class VWidgetStackNode : public VWidgetStack, public SplitWrapperNode {
public:
    VWidgetStackNode(const WrapperNodeCreator& creator) :
        SplitWrapperNode(WrapperNodeType::splitV, creator) {}

    QWidget* widget() { return this; }

    void replaceChild(WrapperNode* const child,
                      WrapperNode* const with) {
        replaceWidget(child->widget(), with->widget());
        SplitWrapperNode::replaceChild(child, with);
    }
protected:
    QWidget* takeWidget(QWidget* const wid) {
        return VWidgetStack::takeWidget(wid);
    }

    void readData(eReadStream& src) {
        SplitWrapperNode::readData(src);
        qreal child2frac; src >> child2frac;
        appendWidget(fChild1->widget());
        appendWidget(fChild2->widget(), child2frac);
    }

    void writeData(eWriteStream& dst) {
        SplitWrapperNode::writeData(dst);
        dst << percentAt(1);
    }
};

class HWidgetStackNode : public HWidgetStack, public SplitWrapperNode {
public:
    HWidgetStackNode(const WrapperNodeCreator& creator) :
        SplitWrapperNode(WrapperNodeType::splitH, creator) {}

    QWidget* widget() { return this; }

    void replaceChild(WrapperNode* const child,
                      WrapperNode* const with) {
        replaceWidget(child->widget(), with->widget());
        SplitWrapperNode::replaceChild(child, with);
    }
protected:
    QWidget* takeWidget(QWidget* const wid) {
        return HWidgetStack::takeWidget(wid);
    }

    void readData(eReadStream& src) {
        SplitWrapperNode::readData(src);
        qreal child2frac; src >> child2frac;
        appendWidget(fChild1->widget());
        appendWidget(fChild2->widget(), child2frac);
    }

    void writeData(eWriteStream& dst) {
        SplitWrapperNode::writeData(dst);
        dst << percentAt(1);
    }
};

#endif // WRAPPERNODE_H