#include "qrealanimator.h"
#include <QMenu>
#include "qrealpoint.h"
#include "qrealkey.h"
#include "randomqrealgenerator.h"

QrealAnimator::QrealAnimator(const qreal iniVal,
                             const qreal minVal,
                             const qreal maxVal,
                             const qreal prefferdStep,
                             const QString &name) :
    GraphAnimator(name) {
    mCurrentBaseValue = iniVal;
    mMinPossibleVal = minVal;
    mMaxPossibleVal = maxVal;
    mPrefferedValueStep = prefferdStep;
}

QrealAnimator::QrealAnimator(const QString &name) : GraphAnimator(name) {}

void QrealAnimator::writeProperty(eWriteStream& dst) const {
    writeKeys(dst);
    dst << mCurrentBaseValue;
}

stdsptr<Key> QrealAnimator::createKey() {
    return enve::make_shared<QrealKey>(this);
}

void QrealAnimator::readProperty(eReadStream& src) {
    readKeys(src);

    qreal val; src >> val;
    setCurrentBaseValue(val);
}

void QrealAnimator::graph_getValueConstraints(
        GraphKey *key, const QrealPointType &type,
        qreal &minMoveValue, qreal &maxMoveValue) const {
    Q_UNUSED(key);
    if(type == QrealPointType::END_POINT) {
        minMoveValue = -DBL_MAX;
        maxMoveValue = DBL_MAX;
//        auto nextKey = key->getNextKey<GraphKey>();
//        if(!nextKey) {
//            minMoveValue = mMinPossibleVal;
//            maxMoveValue = mMaxPossibleVal;
//            return;
//        }
//        qreal p0 = key->getValueForGraph();
//        qreal p2 = nextKey->getStartValue();
//        qreal p3 = nextKey->getValueForGraph();
//        int iMax = 2*(nextKey->getRelFrame() - key->getRelFrame());
//        minMoveValue = DBL_MIN;
//        maxMoveValue = DBL_MAX;
//        for(int i = 1; i < iMax; i++) {
//            qreal t = static_cast<qreal>(i)/iMax;
//            qreal maxVal = gSolveForP1(p0, p2, p3, t, mMaxPossibleVal);
//            maxMoveValue = qMin(maxMoveValue, maxVal);
//            qreal minVal = gSolveForP1(p0, p2, p3, t, mMinPossibleVal);
//            minMoveValue = qMax(minMoveValue, minVal);
//        }
    } else if(type == QrealPointType::START_POINT) {
        minMoveValue = -DBL_MAX;
        maxMoveValue = DBL_MAX;
//        auto prevKey = key->getPrevKey<GraphKey>();
//        if(!prevKey) {
//            minMoveValue = mMinPossibleVal;
//            maxMoveValue = mMaxPossibleVal;
//            return;
//        }
//        qreal p0 = prevKey->getValueForGraph();
//        qreal p1 = prevKey->getEndValue();
//        qreal p3 = key->getValueForGraph();
//        int iMax = 2*(key->getRelFrame() - prevKey->getRelFrame());
//        minMoveValue = DBL_MIN;
//        maxMoveValue = DBL_MAX;
//        for(int i = 1; i < iMax; i++) {
//            qreal t = static_cast<qreal>(i)/iMax;
//            qreal maxVal = gSolveForP2(p0, p1, p3, t, mMaxPossibleVal);
//            maxMoveValue = qMin(maxMoveValue, maxVal);
//            qreal minVal = gSolveForP2(p0, p1, p3, t, mMinPossibleVal);
//            minMoveValue = qMax(minMoveValue, minVal);
//        }
    } else { // KEY_POINT
        minMoveValue = mMinPossibleVal;
        maxMoveValue = mMaxPossibleVal;
    }
}

QrealSnapshot QrealAnimator::makeSnapshot(
        const qreal frameMultiplier,
        const qreal valueMultiplier) const {
    QrealSnapshot snapshot(mCurrentBaseValue, frameMultiplier, valueMultiplier);
    for(const auto& key : anim_mKeys) {
        const auto qaKey = static_cast<QrealKey*>(key);
        snapshot.appendKey(qaKey);
    }
    return snapshot;
}

QrealSnapshot QrealAnimator::makeSnapshot(
        const int minFrame, const int maxFrame,
        const qreal frameMultiplier,
        const qreal valueMultiplier) const {
    const int prevKeyId = anim_getPrevKeyId(minFrame);
    const int nextKeyId = anim_getNextKeyId(maxFrame);
    const int minI = clamp(prevKeyId, 0, anim_mKeys.count() - 1);
    const int maxI = clamp(nextKeyId, 0, anim_mKeys.count() - 1);
    QrealSnapshot snapshot(mCurrentBaseValue, frameMultiplier, valueMultiplier);
    for(int i = minI; i <= maxI; i++) {
        const auto iKey = anim_mKeys.atId<QrealKey>(i);
        snapshot.appendKey(iKey);
    }
    return snapshot;
}

void QrealAnimator::setValueRange(const qreal minVal, const qreal maxVal) {
    mMinPossibleVal = minVal;
    mMaxPossibleVal = maxVal;
    setCurrentBaseValue(mCurrentBaseValue);
}

void QrealAnimator::incAllValues(const qreal valInc) {
    for(const auto &key : anim_mKeys) {
        static_cast<QrealKey*>(key)->incValue(valInc);
    }
    incCurrentBaseValue(valInc);
}

QString QrealAnimator::prp_getValueText() {
    return QString::number(mCurrentBaseValue, 'f', 2);
}

qreal QrealAnimator::getMinPossibleValue() {
    return mMinPossibleVal;
}

qreal QrealAnimator::getMaxPossibleValue() {
    return mMaxPossibleVal;
}

qreal QrealAnimator::getPrefferedValueStep() {
    return mPrefferedValueStep;
}

void QrealAnimator::setPrefferedValueStep(const qreal valueStep) {
    mPrefferedValueStep = valueStep;
}

void QrealAnimator::graphFixMinMaxValues() {
    mGraphMinMaxValuesFixed = true;
}

qreal QrealAnimator::getEffectiveValueAtAbsFrame(const qreal frame) const {
    return getEffectiveValue(prp_absFrameToRelFrameF(frame));
}

qreal QrealAnimator::getBaseValueAtAbsFrame(const qreal frame) const {
    return getBaseValue(prp_absFrameToRelFrameF(frame));
}

void QrealAnimator::setGenerator(const qsptr<RandomQrealGenerator>& generator) {
    if(generator == mRandomGenerator.data()) return;

    if(!generator) {
        mRandomGenerator.reset();
    } else {
        mRandomGenerator = generator->ref<RandomQrealGenerator>();
    }

    prp_afterWholeInfluenceRangeChanged();
}

bool QrealAnimator::hasNoise() {
    return !mRandomGenerator.isNull();
}

qreal QrealAnimator::calculateBaseValueAtRelFrame(const qreal frame) const {
    if(anim_mKeys.isEmpty()) return mCurrentBaseValue;
    const auto pn = anim_getPrevAndNextKeyIdF(frame);
    const int prevId = pn.first;
    const int nextId = pn.second;

    const bool adjKeys = nextId - prevId == 1;
    const auto keyAtRelFrame = adjKeys ? nullptr :
                               anim_getKeyAtIndex<QrealKey>(prevId + 1);
    if(keyAtRelFrame) return keyAtRelFrame->getValue();
    const auto prevKey = anim_getKeyAtIndex<QrealKey>(prevId);
    const auto nextKey = anim_getKeyAtIndex<QrealKey>(nextId);

    if(prevKey && nextKey) {
        const qCubicSegment1D seg{qreal(prevKey->getRelFrame()),
                                  prevKey->getEndFrame(),
                                  nextKey->getStartFrame(),
                                  qreal(nextKey->getRelFrame())};
        const qreal t = gTFromX(seg, frame);
        const qreal p0y = prevKey->getValue();
        const qreal p1y = prevKey->getEndValue();
        const qreal p2y = nextKey->getStartValue();
        const qreal p3y = nextKey->getValue();
        return clamp(gCubicValueAtT({p0y, p1y, p2y, p3y}, t),
                     mMinPossibleVal, mMaxPossibleVal);
    } else if(prevKey) {
        return prevKey->getValue();
    } else if(nextKey) {
        return nextKey->getValue();
    }
    return mCurrentBaseValue;
}

qreal QrealAnimator::getBaseValue(const qreal relFrame) const {
    if(isZero4Dec(relFrame - anim_getCurrentRelFrame()))
        return mCurrentBaseValue;
    return calculateBaseValueAtRelFrame(relFrame);
}

qreal QrealAnimator::getEffectiveValue(const qreal relFrame) const {
    if(mRandomGenerator.isNull()) return getBaseValue(relFrame);
    const qreal val = getBaseValue(relFrame) +
            mRandomGenerator->getDevAtRelFrame(relFrame);
    return qMin(mMaxPossibleVal, qMax(mMinPossibleVal, val));
}

qreal QrealAnimator::getCurrentBaseValue() const {
    return mCurrentBaseValue;
}

qreal QrealAnimator::getEffectiveValue() const {
    if(mRandomGenerator.isNull()) return mCurrentBaseValue;
    const qreal val = mCurrentBaseValue +
            mRandomGenerator->getDevAtRelFrame(anim_getCurrentRelFrame());
    return qMin(mMaxPossibleVal, qMax(mMinPossibleVal, val));
}

void QrealAnimator::setCurrentBaseValue(qreal newValue) {
    newValue = clamp(newValue, mMinPossibleVal, mMaxPossibleVal);

    if(isZero4Dec(newValue - mCurrentBaseValue)) return;

    mCurrentBaseValue = newValue;
    const auto currKey = anim_getKeyOnCurrentFrame<QrealKey>();
    if(currKey) currKey->setValue(mCurrentBaseValue);
    else prp_afterWholeInfluenceRangeChanged();

    emit valueChangedSignal(mCurrentBaseValue);

    //anim_updateKeysPath();
}

bool QrealAnimator::updateBaseValueFromCurrentFrame() {
    const qreal newValue = calculateBaseValueAtRelFrame(anim_getCurrentRelFrame());
    if(isZero4Dec(newValue - mCurrentBaseValue)) return false;
    mCurrentBaseValue = newValue;
    emit valueChangedSignal(mCurrentBaseValue);
    return true;
}

void QrealAnimator::saveValueToKey(const int frame, const qreal value) {
    const auto keyAtFrame = anim_getKeyAtAbsFrame<QrealKey>(frame);
    if(keyAtFrame) {
        keyAtFrame->setValue(value);
    } else {
        const auto newKey = enve::make_shared<QrealKey>(value, frame, this);
        anim_appendKey(newKey);
    }
}

void QrealAnimator::anim_setAbsFrame(const int frame) {
    Animator::anim_setAbsFrame(frame);
    const bool changed = updateBaseValueFromCurrentFrame();
    if(changed) prp_afterChangedCurrent(UpdateReason::frameChange);
}

void QrealAnimator::anim_saveCurrentValueAsKey() {
    if(anim_getKeyOnCurrentFrame()) return;
    const auto newKey = enve::make_shared<QrealKey>(mCurrentBaseValue,
                                             anim_getCurrentRelFrame(),
                                             this);
    anim_appendKey(newKey);
}

void QrealAnimator::anim_addKeyAtRelFrame(const int relFrame) {
    if(anim_getKeyAtRelFrame(relFrame)) return;
    const qreal value = getBaseValue(relFrame);
    const auto newKey = enve::make_shared<QrealKey>(value, relFrame, this);
    anim_appendKey(newKey);
}

void QrealAnimator::anim_removeAllKeys() {
    if(anim_mKeys.isEmpty()) return;
    const qreal currentValue = mCurrentBaseValue;

    const auto keys = anim_mKeys;
    for(const auto& key : keys) {
        anim_removeKey(key->ref<Key>());
    }
    setCurrentBaseValue(currentValue);
}

QPainterPath QrealAnimator::graph_getPathForSegment(
        const GraphKey * const prevKey,
        const GraphKey * const nextKey) const {
    QPainterPath path;
    if(prevKey) {
        path.moveTo(prevKey->getRelFrame(), prevKey->getValueForGraph());
        if(nextKey) {
            path.cubicTo(QPointF(prevKey->getEndFrame(),
                                 prevKey->getEndValue()),
                         QPointF(nextKey->getStartFrame(),
                                 nextKey->getStartValue()),
                         QPointF(nextKey->getRelFrame(),
                                 nextKey->getValueForGraph()));
        } else {
            path.lineTo(50000, prevKey->getValueForGraph());
        }
    } else {
        if(nextKey) {
            const qreal nextVal = nextKey->getValueForGraph();
            path.moveTo(-50000, nextVal);
            path.lineTo(nextKey->getRelFrame(), nextVal);
        } else {
            path.moveTo(-50000, mCurrentBaseValue);
            path.lineTo(50000, mCurrentBaseValue);
        }
    }
    return path;
}

qValueRange QrealAnimator::graph_getMinAndMaxValues() const {
    if(mGraphMinMaxValuesFixed) {
        return {mMinPossibleVal, mMaxPossibleVal};
    }
    if(anim_mKeys.isEmpty()) {
        return {mCurrentBaseValue - mPrefferedValueStep,
                mCurrentBaseValue + mPrefferedValueStep};
    }
    qreal minVal = TEN_MIL;
    qreal maxVal = -TEN_MIL;
    for(const auto &key : anim_mKeys) {
        const auto qaKey = static_cast<QrealKey*>(key);
        const qreal keyVal = qaKey->getValue();
        const qreal startVal = qaKey->getStartValue();
        const qreal endVal = qaKey->getEndValue();
        const qreal maxKeyVal = qMax(qMax(startVal, endVal), keyVal);
        const qreal minKeyVal = qMin(qMin(startVal, endVal), keyVal);
        if(maxKeyVal > maxVal) maxVal = maxKeyVal;
        if(minKeyVal < minVal) minVal = minKeyVal;
    }

    return {minVal - mPrefferedValueStep, maxVal + mPrefferedValueStep};
}

qValueRange QrealAnimator::graph_getMinAndMaxValuesBetweenFrames(
        const int startFrame, const int endFrame) const {
    if(anim_mKeys.isEmpty()) return {mCurrentBaseValue, mCurrentBaseValue};
    bool first = true;
    qreal minVal = TEN_MIL;
    qreal maxVal = -TEN_MIL;
    for(const auto &key : anim_mKeys) {
        const auto qaKey = static_cast<QrealKey*>(key);
        const int keyFrame = key->getAbsFrame();
        if(keyFrame > endFrame || keyFrame < startFrame) continue;
        if(first) first = false;
        const qreal keyVal = qaKey->getValue();
        const qreal startVal = qaKey->getStartValue();
        const qreal endVal = qaKey->getEndValue();
        const qreal maxKeyVal = qMax(qMax(startVal, endVal), keyVal);
        const qreal minKeyVal = qMin(qMin(startVal, endVal), keyVal);
        if(maxKeyVal > maxVal) maxVal = maxKeyVal;
        if(minKeyVal < minVal) minVal = minKeyVal;
    }
    if(first) {
        const int midFrame = (startFrame + endFrame)/2;
        const qreal value = getBaseValueAtAbsFrame(midFrame);
        return {value, value};
    }
    return {minVal, maxVal};
}

qreal QrealAnimator::graph_clampGraphValue(const qreal value) {
    return value;
}

void QrealAnimator::incSavedValueToCurrentValue(const qreal incBy) {
    setCurrentBaseValue(mSavedCurrentValue + incBy);
}

void QrealAnimator::multSavedValueToCurrentValue(const qreal multBy) {
    setCurrentBaseValue(mSavedCurrentValue * multBy);
}

void QrealAnimator::setCurrentBaseValueNoUpdate(
        const qreal newValue) {
    mCurrentBaseValue = clamp(newValue, mMinPossibleVal, mMaxPossibleVal);
    emit valueChangedSignal(mCurrentBaseValue);
}

void QrealAnimator::incCurrentValueNoUpdate(
        const qreal incBy) {
    setCurrentBaseValueNoUpdate(mCurrentBaseValue + incBy);
}

void QrealAnimator::incCurrentBaseValue(const qreal incBy) {
    setCurrentBaseValue(mCurrentBaseValue + incBy);
}

void QrealAnimator::prp_startTransform() {
    if(mTransformed) return;
    if(anim_isRecording() && !anim_getKeyOnCurrentFrame()) {
        anim_saveCurrentValueAsKey();
    }
    mSavedCurrentValue = mCurrentBaseValue;
    mTransformed = true;
}

void QrealAnimator::prp_finishTransform() {
    if(mTransformed) {
//        addUndoRedo(new ChangeQrealAnimatorValue(mSavedCurrentValue,
//                                                 mCurrentValue,
//                                                 this) );

        mTransformed = false;

        emit prp_finishTransform();
    }
}

void QrealAnimator::prp_cancelTransform() {
    if(mTransformed) {
        mTransformed = false;
        setCurrentBaseValue(mSavedCurrentValue);
    }
}

void QrealAnimator::multCurrentBaseValue(const qreal mult) {
    setCurrentBaseValue(mCurrentBaseValue*mult);
}

qreal QrealAnimator::getSavedBaseValue() {
    return mSavedCurrentValue;
}
