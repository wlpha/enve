#ifndef EFFECTANIMATORS_H
#define EFFECTANIMATORS_H
#include "complexanimator.h"

namespace fmt_filters { struct image; }

class PixmapEffect;
class BoundingBox;
class SkImage;
class SkCanvas;
class SkBitmap;
struct PixmapEffectRenderData;

class EffectAnimators : public ComplexAnimator {
public:
    EffectAnimators();

    void addEffect(PixmapEffect *effect);

    void applyEffectsSk(const SkBitmap &imgPtr,
                        const fmt_filters::image &img,
                        const qreal &scale);

    qreal getEffectsMargin() const;

    void setParentBox(BoundingBox *box);
    BoundingBox *getParentBox() {
        return mParentBox;
    }

    bool hasEffects();

    bool SWT_isPixmapEffectAnimators() { return true; }
    qreal getEffectsMarginAtRelFrame(const int &relFrame) const;

    void addEffectRenderDataToList(const int &relFrame,
            QList<PixmapEffectRenderData*> *pixmapEffects);

    void ca_removeAllChildAnimators();

    void writeProperty(QIODevice *target);
    void readProperty(QIODevice *target);
    void readPixmapEffect(QIODevice *target);
private:
    BoundingBox *mParentBox;
};

#endif // EFFECTANIMATORS_H
