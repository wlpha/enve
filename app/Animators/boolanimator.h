#ifndef BOOLANIMATOR_H
#define BOOLANIMATOR_H
#include "intanimator.h"

class BoolAnimator : public IntAnimator {
    friend class SelfRef;
public:
    bool getCurrentBoolValue();
    void setCurrentBoolValue(const bool &bT,
                             const bool &finish = false);
    bool getCurrentBoolValueAtRelFrame(const int &relFrame) {
        return getCurrentIntValueAtRelFrame(relFrame) == 1;
    }
    bool getCurrentBoolValueAtRelFrameF(const qreal &relFrame) {
        return getCurrentIntValueAtRelFrameF(relFrame) == 1;
    }

    bool SWT_isBoolAnimator() { return true; }
protected:
    BoolAnimator(const QString& name);
};

#endif // BOOLANIMATOR_H