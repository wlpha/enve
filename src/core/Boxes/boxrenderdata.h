#ifndef BOXRENDERDATA_H
#define BOXRENDERDATA_H
#include "skia/skiaincludes.h"

#include <QWeakPointer>
#include "Tasks/updatable.h"
#include "Animators/animator.h"
#include <QMatrix>
class BoundingBox;
class ShaderProgramCallerBase;
#include "smartPointers/sharedpointerdefs.h"
#include "effectsrenderer.h"

class RenderDataCustomizerFunctor;
struct BoxRenderData : public eTask {
    friend class StdSelfRef;
protected:
    enum class Step { BOX_IMAGE, EFFECTS };

    BoxRenderData(BoundingBox *parentBoxT);

    virtual void drawSk(SkCanvas * const canvas) = 0;
    virtual void setupRenderData() {}
    virtual void transformRenderCanvas(SkCanvas& canvas) const;
    virtual void copyFrom(BoxRenderData *src);
    virtual void updateRelBoundingRect() = 0;
    virtual void updateGlobalRect();

    HardwareSupport hardwareSupport() const;

    void scheduleTaskNow() final;
    void afterCanceled() {}
    void beforeProcessing(const Hardware hw) final;
    void afterProcessing() final;
    void afterQued() final;

    virtual SkColor eraseColor() const { return SK_ColorTRANSPARENT; }
public:
    virtual QPointF getCenterPosition() {
        return fRelBoundingRect.center();
    }

    bool nextStep() {
        if(mState == eTaskState::waiting) mState = eTaskState::processing;
        const bool result = !mEffectsRenderer.isEmpty();
        if(result) mStep = Step::EFFECTS;
        return result;
    }

    void processGpu(QGL33 * const gl, SwitchableContext &context);
    void process();

    stdsptr<BoxRenderData> makeCopy();

    Animator::UpdateReason fReason;

    uint fBoxStateId = 0;

    QMatrix fResolutionScale;
    QMatrix fScaledTransform;
    QMatrix fRelTransform;
    QMatrix fTransform;
    QMatrix fRenderTransform;

    bool fRelBoundingRectSet = false;
    QRectF fRelBoundingRect;
    QRect fGlobalRect;
    QList<QRectF> fOtherGlobalRects;
    QRect fMaxBoundsRect;

    QMargins fBaseMargin;

    qreal fOpacity = 1;
    qreal fResolution;
    qreal fRelFrame;

    // for motion blur
    stdptr<BoxRenderData> fMotionBlurTarget;
    // for motion blur

    SkBlendMode fBlendMode = SkBlendMode::kSrcOver;

    bool fParentIsTarget = true;
    qptr<BoundingBox> fParentBox;
    sk_sp<SkImage> fRenderedImage;

    void drawRenderedImageForParent(SkCanvas * const canvas);

    void dataSet();

    void addEffect(const stdsptr<RasterEffectCaller>& effect) {
        mEffectsRenderer.add(effect);
    }
protected:
    bool hasEffects() const { return !mEffectsRenderer.isEmpty(); }

    void setBaseGlobalRect(const QRectF &baseRectF);

    bool mDelayDataSet = false;
    bool mDataSet = false;
private:
    Step mStep = Step::BOX_IMAGE;
    EffectsRenderer mEffectsRenderer;
};

#endif // BOXRENDERDATA_H
