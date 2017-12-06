#ifndef QPOINTFANIMATOR_H
#define QPOINTFANIMATOR_H
#include "Animators/complexanimator.h"
#include "skiaincludes.h"
class QrealAnimator;
typedef QSharedPointer<QrealAnimator> QrealAnimatorQSPtr;

class QPointFAnimator : public ComplexAnimator
{
public:
    QPointFAnimator();
    qreal getXValue();
    qreal getYValue();

    qreal getXValueAtRelFrame(const int &relFrame);
    qreal getYValueAtRelFrame(const int &relFrame);

    void setCurrentPointValue(const QPointF &val,
                              const bool &saveUndoRedo = false,
                              const bool &finish = false,
                              const bool &callUpdater = true);
    void incCurrentValues(const qreal &x,
                          const qreal &y);
    void multCurrentValues(qreal sx, qreal sy);

    QPointF getSavedPointValue();
    qreal getSavedXValue();
    qreal getSavedYValue();

    void incAllValues(const qreal &x,
                      const qreal &y,
                      const bool &saveUndoRedo = false,
                      const bool &finish = false,
                      const bool &callUpdater = true);

    QrealAnimator *getXAnimator();
    QrealAnimator *getYAnimator();

    void multSavedValueToCurrentValue(const qreal &sx,
                                      const qreal &sy);
    void incSavedValueToCurrentValue(const qreal &incXBy,
                                     const qreal &incYBy);

    void setValuesRange(const qreal &minVal,
                        const qreal &maxVal);
    QPointF getCurrentPointValue() const;
    QPointF getCurrentPointValueAtAbsFrame(const int &frame) const;
    QPointF getCurrentPointValueAtRelFrame(const int &frame) const;

    void setPrefferedValueStep(const qreal &valueStep);

    bool SWT_isQPointFAnimator() { return true; }
    void writeProperty(QIODevice *target);
    void readProperty(QIODevice *target);

    bool getBeingTransformed();
    QPointF getCurrentEffectivePointValueAtAbsFrame(const int &frame) const;
    QPointF getCurrentEffectivePointValueAtRelFrame(const int &frame) const;
    QPointF getCurrentEffectivePointValue() const;
    qreal getEffectiveXValueAtRelFrame(const int &relFrame);
    qreal getEffectiveYValueAtRelFrame(const int &relFrame);
    qreal getEffectiveXValue();
    qreal getEffectiveYValue();
protected:
    QrealAnimatorQSPtr mXAnimator;
    QrealAnimatorQSPtr mYAnimator;
};

#endif // QPOINTFANIMATOR_H
