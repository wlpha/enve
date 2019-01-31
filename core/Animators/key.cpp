#include "key.h"
#include "qrealpoint.h"

Key::Key(Animator * const parentAnimator) {
    mParentAnimator = parentAnimator;
    mRelFrame = 0;
}

bool Key::isSelected() const { return mIsSelected; }

void Key::removeFromAnimator() {
    if(!mParentAnimator) return;
    mParentAnimator->anim_removeKey(ref<Key>());
}

bool Key::hasPrevKey() const {
    if(!mParentAnimator) return false;
    return mParentAnimator->anim_hasPrevKey(this);
}

bool Key::hasNextKey() const {
    if(!mParentAnimator) return false;
    return mParentAnimator->anim_hasNextKey(this);
}

void Key::incFrameAndUpdateParentAnimator(const int &inc) {
    setRelFrameAndUpdateParentAnimator(mRelFrame + inc);
}

void Key::setRelFrameAndUpdateParentAnimator(const int &relFrame) {
    if(!mParentAnimator) return;
    mParentAnimator->anim_moveKeyToRelFrame(this, relFrame);
}

void Key::addToSelection(QList<qptr<Animator>> &selectedAnimators) {
    if(isSelected()) return;
    if(!mParentAnimator->hasSelectedKeys()) {
        selectedAnimators.append(mParentAnimator);
    }
    mParentAnimator->addKeyToSelected(this);
}

void Key::removeFromSelection(QList<qptr<Animator>> &selectedAnimators) {
    if(isSelected()) {
        mParentAnimator->removeKeyFromSelected(this);
        if(!mParentAnimator->hasSelectedKeys()) {
            selectedAnimators.removeOne(mParentAnimator);
        }
    }
}

void Key::startFrameTransform() {
    mSavedRelFrame = mRelFrame;
}

void Key::cancelFrameTransform() {
    mParentAnimator->anim_moveKeyToRelFrame(this, mSavedRelFrame);
}

void Key::scaleFrameAndUpdateParentAnimator(
        const int &relativeToFrame,
        const qreal &scaleFactor,
        const bool &useSavedFrame) {
    int thisRelFrame = useSavedFrame ? mSavedRelFrame : mRelFrame;
    int relativeToRelFrame =
            mParentAnimator->prp_absFrameToRelFrame(relativeToFrame);
    int newFrame = qRound(relativeToRelFrame +
                          (thisRelFrame - relativeToRelFrame)*scaleFactor);
    if(newFrame == mRelFrame) return;
    setRelFrameAndUpdateParentAnimator(newFrame);
}

void Key::setSelected(const bool &bT) {
    mIsSelected = bT;
}

void Key::finishFrameTransform() {
    if(!mParentAnimator) return;
//    mParentAnimator->addUndoRedo(
//                new ChangeKeyFrameUndoRedo(mSavedRelFrame,
//                                           mRelFrame, this));
}

int Key::getAbsFrame() const {
    return mParentAnimator->prp_relFrameToAbsFrame(mRelFrame);
}

int Key::getRelFrame() const {
    return mRelFrame;
}

void Key::setRelFrame(const int &frame) {
    if(frame == mRelFrame) return;
    mRelFrame = frame;
    if(!mParentAnimator) return;
    mParentAnimator->anim_updateKeyOnCurrrentFrame();
}

void Key::setAbsFrame(const int &frame) {
    setRelFrame(mParentAnimator->prp_absFrameToRelFrame(frame));
}

void Key::afterKeyChanged() {
    mParentAnimator->anim_updateAfterChangedKey(this);
}

