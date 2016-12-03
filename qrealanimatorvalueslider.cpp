#include "qrealanimatorvalueslider.h"
#include "qrealanimator.h"
#include "mainwindow.h"

QrealAnimatorValueSlider::QrealAnimatorValueSlider(qreal minVal, qreal maxVal,
                                                   QWidget *parent) :
    QDoubleSlider(minVal, maxVal, parent)
{

}

QrealAnimatorValueSlider::QrealAnimatorValueSlider(qreal minVal, qreal maxVal,
                                                   QrealAnimator *animator,
                                                   QWidget *parent) :
    QDoubleSlider(minVal, maxVal, parent)
{
    setAnimator(animator);
}

QrealAnimatorValueSlider::~QrealAnimatorValueSlider()
{
    setAnimator(NULL);
}

void QrealAnimatorValueSlider::emitValueChanged(qreal value)
{
    if(mAnimator == NULL) return;
    mAnimator->setCurrentValue(value);
    MainWindow::getInstance()->callUpdateSchedulers();
}

void QrealAnimatorValueSlider::emitEditingFinished(qreal value)
{
    if(mAnimator == NULL) return;
    mAnimator->setCurrentValue(value, true);
    MainWindow::getInstance()->callUpdateSchedulers();
}

void QrealAnimatorValueSlider::paint(QPainter *p)
{
    if(mAnimator == NULL) {
        QDoubleSlider::paint(p);
    } else {
        p->fillRect(rect(), (mAnimator->isRecording() ? QColor(255, 200, 200) : QColor(200, 200, 255)));
        if(!mTextEdit) {
            if(mShowValueSlider) {
                qreal valWidth = mValue*width()/(mMaxValue - mMinValue);
                p->fillRect(QRectF(0., 0., valWidth, height()),
                           (mAnimator->isRecording() ? QColor(255, 160, 160) : QColor(160, 160, 255)));
            }
            if(mShowName) {
                p->drawText(rect(), Qt::AlignCenter, mName + ": " + getValueString());
            } else {
                p->drawText(rect(), Qt::AlignCenter, getValueString());
            }
        }
        if(mAnimator->isKeyOnCurrentFrame()) {
            p->setPen(QPen(Qt::red));
        }
        p->drawRect(rect().adjusted(0, 0, -1, -1));
    }
}

void QrealAnimatorValueSlider::setAnimator(QrealAnimator *animator)
{
    if(mAnimator != NULL) mAnimator->removeSlider(this);
    mAnimator = animator;
    if(mAnimator != NULL) mAnimator->addSlider(this);
}
