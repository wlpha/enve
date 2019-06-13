#ifndef CANVAS_H
#define CANVAS_H

#include "Boxes/containerbox.h"
#include "colorhelpers.h"
#include <QThread>
#include "CacheHandlers/hddcachablecachehandler.h"
#include "skia/skiaincludes.h"
#include "GUI/valueinput.h"
#include "GUI/canvaswindow.h"
#include "Animators/coloranimator.h"
#include "MovablePoints/segment.h"
#include "MovablePoints/movablepoint.h"
#include "Boxes/canvasrenderdata.h"
#include "Paint/drawableautotiledsurface.h"
#include "canvasbase.h"
#include "Paint/animatedsurface.h"
#include <QAction>

class AnimatedSurface;
class PaintBox;
class TextBox;
class Circle;
class ParticleBox;
class Rectangle;
class PathPivot;
class SoundComposition;
class SkCanvas;
class ImageSequenceBox;
class Brush;
class NodePoint;
class UndoRedoStack;
class ExternalLinkBox;
struct GPURasterEffectCreator;

#define getAtIndexOrGiveNull(index, list) (( (index) >= (list).count() || (index) < 0 ) ? nullptr : (list).at( (index) ))

#define Q_FOREACHInverted(item, list) item = getAtIndexOrGiveNull((list).count() - 1, (list)); \
    for(int i = (list).count() - 1; i >= 0; i--, item = getAtIndexOrGiveNull(i, (list)) )
#define Q_FOREACHInverted2(item, list) for(int i = (list).count() - 1; i >= 0; i--) { \
            item = getAtIndexOrGiveNull(i, (list));

enum CtrlsMode : short;

extern bool zLessThan(const qptr<BoundingBox> &box1,
                      const qptr<BoundingBox> &box2);

extern bool boxesZSort(const qptr<BoundingBox> &box1,
                       const qptr<BoundingBox> &box2);

class Canvas : public ContainerBox, public CanvasBase {
    Q_OBJECT
    friend class SelfRef;
protected:
    explicit Canvas(CanvasWindow *canvasWidget,
                    const int canvasWidth = 1920,
                    const int canvasHeight = 1080,
                    const int frameCount = 200,
                    const qreal fps = 24);
public:
    QRectF getPixBoundingRect();
    void selectOnlyLastPressedBox();
    void selectOnlyLastPressedPoint();

    void repaintIfNeeded();
    void setCanvasMode(const CanvasMode &mode);
    void startSelectionAtPoint(const QPointF &pos);
    void moveSecondSelectionPoint(const QPointF &pos);
    void setPointCtrlsMode(const CtrlsMode& mode);
    void setCurrentBoxesGroup(ContainerBox * const group);

    void updatePivot();

    void updatePivotIfNeeded();

    void awaitUpdate() {}

    void resetTransormation();
    void fitCanvasToSize();
    void zoomCanvas(const qreal scaleBy, const QPointF &absOrigin);
    void moveByRel(const QPointF &trans);

    //void updateAfterFrameChanged(const int currentFrame);

    QSize getCanvasSize();

    void centerPivotPosition() {}

    //
    void finishSelectedPointsTransform();
    void finishSelectedBoxesTransform();
    void moveSelectedPointsByAbs(const QPointF &by,
                                 const bool startTransform);
    void moveSelectedBoxesByAbs(const QPointF &by,
                                const bool startTransform);
    void groupSelectedBoxes();

    //void selectAllBoxes();
    void deselectAllBoxes();

    void applyShadowToSelected();

    void selectedPathsUnion();
    void selectedPathsDifference();
    void selectedPathsIntersection();
    void selectedPathsDivision();
    void selectedPathsExclusion();
    void makeSelectedPointsSegmentsCurves();
    void makeSelectedPointsSegmentsLines();

    void centerPivotForSelected();
    void resetSelectedScale();
    void resetSelectedTranslation();
    void resetSelectedRotation();
    void convertSelectedBoxesToPath();
    void convertSelectedPathStrokesToPath();

    void applySampledMotionBlurToSelected();
    void applyLinesEffectToSelected();
    void applyCirclesEffectToSelected();
    void applySwirlEffectToSelected();
    void applyOilEffectToSelected();
    void applyImplodeEffectToSelected();
    void applyDesaturateEffectToSelected();
    void applyColorizeEffectToSelected();
    void applyReplaceColorEffectToSelected();
    void applyContrastEffectToSelected();
    void applyBrightnessEffectToSelected();

    void rotateSelectedBy(const qreal rotBy,
                          const QPointF &absOrigin,
                          const bool startTrans);

    QPointF getSelectedBoxesAbsPivotPos();
    bool isBoxSelectionEmpty() const;

    void ungroupSelectedBoxes();
    void scaleSelectedBy(const qreal scaleBy,
                         const QPointF &absOrigin,
                         const bool startTrans);
    void cancelSelectedBoxesTransform();
    void cancelSelectedPointsTransform();

    void setSelectedCapStyle(const Qt::PenCapStyle& capStyle);
    void setSelectedJoinStyle(const Qt::PenJoinStyle &joinStyle);
    void setSelectedStrokeWidth(const qreal strokeWidth);
    void setSelectedStrokeBrush(SimpleBrushWrapper * const brush);
    void setSelectedStrokeBrushWidthCurve(
            const qCubicSegment1D& curve);
    void setSelectedStrokeBrushTimeCurve(
            const qCubicSegment1D& curve);
    void setSelectedStrokeBrushPressureCurve(
            const qCubicSegment1D& curve);
    void setSelectedStrokeBrushSpacingCurve(
            const qCubicSegment1D& curve);

    void startSelectedStrokeWidthTransform();
    void startSelectedStrokeColorTransform();
    void startSelectedFillColorTransform();

    void getDisplayedFillStrokeSettingsFromLastSelected(
            PaintSettingsAnimator*& fillSetings, OutlineSettingsAnimator*& strokeSettings);
    void scaleSelectedBy(const qreal scaleXBy, const qreal scaleYBy,
                         const QPointF &absOrigin, const bool startTrans);

    void grabMouseAndTrack();

    void setPartialRepaintRect(QRectF absRect);
    void makePartialRepaintInclude(QPointF pointToInclude);
    void partialRepaintRectToPoint(QPointF point);

    qreal getResolutionFraction();
    void setResolutionFraction(const qreal percent);

    void applyCurrentTransformationToSelected();
    QPointF getSelectedPointsAbsPivotPos();
    bool isPointSelectionEmpty() const;
    void scaleSelectedPointsBy(const qreal scaleXBy,
                               const qreal scaleYBy,
                               const QPointF &absOrigin,
                               const bool startTrans);
    void rotateSelectedPointsBy(const qreal rotBy,
                                const QPointF &absOrigin,
                                const bool startTrans);
    int getPointsSelectionCount() const ;

    void clearPointsSelectionOrDeselect();
    NormalSegment getSmartEdgeAt(const QPointF& absPos) const;

    void createLinkBoxForSelected();
    void startSelectedPointsTransform();

    void mergePoints();
    void disconnectPoints();
    void connectPoints();

    void setSelectedFontFamilyAndStyle(
            const QString& family, const QString& style);
    void setSelectedFontSize(const qreal size);
    void removeSelectedPointsAndClearList();
    void removeSelectedBoxesAndClearList();

    void addBoxToSelection(BoundingBox *box);
    void removeBoxFromSelection(BoundingBox *box);
    void clearBoxesSelection();
    void clearBoxesSelectionList();

    void addPointToSelection(MovablePoint * const point);
    void removePointFromSelection(MovablePoint * const point);

    void clearPointsSelection();
    void raiseSelectedBoxesToTop();
    void lowerSelectedBoxesToBottom();
    void raiseSelectedBoxes();
    void lowerSelectedBoxes();

    void selectAndAddContainedPointsToSelection(const QRectF &absRect);
//
    void mousePressEvent(const QMouseEvent * const event);
    void mouseReleaseEvent(const QMouseEvent * const event);
    void mouseMoveEvent(const QMouseEvent * const event);
    void wheelEvent(const QWheelEvent * const event);
    void mouseDoubleClickEvent(const QMouseEvent * const e);

    bool keyPressEvent(QKeyEvent *event);

    qsptr<BoundingBox> createLink();
    ImageBox* createImageBox(const QString &path);
    ImageSequenceBox* createAnimationBoxForPaths(const QStringList &paths);
    VideoBox* createVideoForPath(const QString &path);
    ExternalLinkBox *createLinkToFileWithPath(const QString &path);
    SingleSound* createSoundForPath(const QString &path);

    void setPreviewing(const bool bT);
    void setOutputRendering(const bool bT);

    const CanvasMode &getCurrentCanvasMode() const {
        return mCurrentMode;
    }

    Canvas *getParentCanvas() {
        return this;
    }

    bool SWT_shouldBeVisible(const SWT_RulesCollection &rules,
                             const bool parentSatisfies,
                             const bool parentMainTarget) const;

    ContainerBox *getCurrentBoxesGroup() {
        return mCurrentBoxesGroup;
    }

    void updateTotalTransform() {}

    QMatrix getTotalTransform() const {
        return QMatrix();
    }

    QMatrix getRelativeTransformAtCurrentFrame() {
        return QMatrix();
    }

    QPointF mapAbsPosToRel(const QPointF &absPos) {
        return absPos;
    }

    void setIsCurrentCanvas(const bool bT);

    void scheduleEffectsMarginUpdate() {}

    void renderSk(SkCanvas * const canvas,
                  GrContext * const grContext);

    void setCanvasSize(const int width, const int height) {
        if(width == mWidth && height == mHeight) return;
        mWidth = width;
        mHeight = height;
        fitCanvasToSize();
    }

    int getCanvasWidth() const {
        return mWidth;
    }

    QRectF getMaxBoundsRect() const {
        if(mClipToCanvasSize) {
            return QRectF(0, 0, mWidth, mHeight);
        } else {
            return QRectF(-mWidth, - mHeight, 3*mWidth, 3*mHeight);
        }
    }

    int getCanvasHeight() const {
        return mHeight;
    }

    void setMaxFrame(const int frame);

    ColorAnimator *getBgColorAnimator() {
        return mBackgroundColor.get();
    }

    stdsptr<BoundingBoxRenderData> createRenderData();

    void setupRenderData(const qreal relFrame,
                         BoundingBoxRenderData * const data) {
        ContainerBox::setupRenderData(relFrame, data);
        auto canvasData = GetAsPtr(data, CanvasRenderData);
        canvasData->fBgColor = toSkColor(mBackgroundColor->getColor());
        canvasData->fCanvasHeight = mHeight*mResolutionFraction;
        canvasData->fCanvasWidth = mWidth*mResolutionFraction;
    }

    bool clipToCanvas() { return mClipToCanvasSize; }

    const SimpleBrushWrapper * getCurrentBrush() const;
    void setCurrentBrush(const SimpleBrushWrapper * const brush);
    void setBrushColor(const QColor& color);

    void incBrushRadius();
    void decBrushRadius();

    void schedulePivotUpdate();
    void setClipToCanvas(const bool bT) { mClipToCanvasSize = bT; }
    void setRasterEffectsVisible(const bool bT) { mRasterEffectsVisible = bT; }
    void setPathEffectsVisible(const bool bT) { mPathEffectsVisible = bT; }
protected:
//    void updateAfterTotalTransformationChanged() {
////        for(const auto& child : mChildBoxes) {
////            child->updateTotalTransformTmp();
////            child->scheduleSoftUpdate();
////        }
//    }

    void setCurrentSmartEndPoint(SmartNodePoint * const point);
    NodePoint *getCurrentPoint();

    void handleMovePathMouseRelease();
    void handleMovePointMouseRelease();

    void handleRightButtonMousePress(const QMouseEvent * const event);
    void handleLeftButtonMousePress();
signals:
    void canvasNameChanged(Canvas *, QString);
private slots:
    void emitCanvasNameChanged();
public:
    void scheduleDisplayedFillStrokeSettingsUpdate();

    void prp_afterChangedAbsRange(const FrameRange &range);

    void makePointCtrlsSymmetric();
    void makePointCtrlsSmooth();
    void makePointCtrlsCorner();

    void makeSegmentLine();
    void makeSegmentCurve();

    MovablePoint *getPointAtAbsPos(const QPointF &absPos,
                                   const CanvasMode &mode,
                                   const qreal invScale);
    void duplicateSelectedBoxes();
    void clearLastPressedPoint();
    void clearCurrentSmartEndPoint();
    void clearHoveredEdge();
    void applyPaintSettingToSelected(const PaintSettingsApplier &setting);
    void setSelectedFillColorMode(const ColorMode &mode);
    void setSelectedStrokeColorMode(const ColorMode &mode);
    int getCurrentFrame();
    int getFrameCount();

    SoundComposition *getSoundComposition();

    void updateHoveredBox();
    void updateHoveredPoint();
    void updateHoveredEdge();
    void updateHoveredElements();

    void setLocalPivot(const bool localPivot) {
        mLocalPivot = localPivot;
        updatePivot();
    }

    bool getPivotLocal() const {
        return mLocalPivot;
    }

    int getMaxFrame();

    //void updatePixmaps();
    HDDCachableCacheHandler& getCacheHandler() {
        return mCacheHandler;
    }

    HDDCachableCacheHandler& getSoundCacheHandler();

    void setCurrentPreviewContainer(const int relFrame);
    void setCurrentPreviewContainer(const stdsptr<ImageCacheContainer> &cont);
    void setLoadingPreviewContainer(
            const stdsptr<ImageCacheContainer> &cont);

    void setRenderingPreview(const bool bT);

    bool isPreviewingOrRendering() const {
        return mPreviewing || mRenderingPreview || mRenderingOutput;
    }
    QPointF mapCanvasAbsToRel(const QPointF &pos);

    qreal getFps() const { return mFps; }
    void setFps(const qreal fps) { mFps = fps; }

    BoundingBox *getBoxAt(const QPointF &absPos) {
        if(mClipToCanvasSize) {
            if(!getMaxBoundsRect().contains(absPos)) return nullptr;
        }
        return ContainerBox::getBoxAt(absPos);
    }

    void anim_scaleTime(const int pivotAbsFrame, const qreal scale) {
        ContainerBox::anim_scaleTime(pivotAbsFrame, scale);
//        int newAbsPos = qRound(scale*pivotAbsFrame);
//        anim_shiftAllKeys(newAbsPos - pivotAbsFrame);
        setMaxFrame(qRound((mMaxFrame - pivotAbsFrame)*scale));
        mCanvasWindow->setCurrentCanvas(this);
    }

    void changeFpsTo(const qreal fps) {
        anim_scaleTime(0, fps/mFps);
        setFps(fps);
    }
    void drawTransparencyMesh(SkCanvas *canvas, const SkRect &viewRect);

    bool SWT_isCanvas() const { return true; }

    void addSelectedBoxesActions(QMenu * const qMenu);
    void addActionsToMenu(BoxTypeMenu * const menu) { Q_UNUSED(menu); }
    void addActionsToMenu(QMenu* const menu);

    void deleteAction();
    void copyAction();
    void pasteAction();
    void cutAction();
    void duplicateAction();
    void selectAllAction();
    void clearSelectionAction();
    void rotateSelectedBoxesStartAndFinish(const qreal rotBy);
    bool shouldPlanScheduleUpdate() {
        return mCurrentPreviewContainerOutdated;
    }

    void renderDataFinished(BoundingBoxRenderData *renderData);
    FrameRange prp_getIdenticalRelRange(const int relFrame) const;
    void setPickingFromPath(const bool pickFill,
                            const bool pickStroke) {
        mPickFillFromPath = pickFill;
        mPickStrokeFromPath = pickStroke;
    }

    void tabletEvent(const QTabletEvent * const e,
                     const QPointF &absPos);
    QRectF getRelBoundingRect(const qreal );
    void writeBoundingBox(QIODevice * const target);
    void readBoundingBox(QIODevice * const target);
    bool anim_prevRelFrameWithKey(const int relFrame, int &prevRelFrame);
    bool anim_nextRelFrameWithKey(const int relFrame, int &nextRelFrame);

    void shiftAllPointsForAllKeys(const int by);
    void revertAllPointsForAllKeys();
    void shiftAllPoints(const int by);
    void revertAllPoints();
    void flipSelectedBoxesHorizontally();
    void flipSelectedBoxesVertically();
    int getByteCountPerFrame() {
        return qCeil(mWidth*mResolutionFraction)*
                qCeil(mHeight*mResolutionFraction)*4;
        //return mCurrentPreviewContainer->getByteCount();
    }
    int getMaxPreviewFrame(const int minFrame, const int maxFrame);
    void selectedPathsCombine();
    void selectedPathsBreakApart();
    void invertSelectionAction();

    bool getRasterEffectsVisible() const {
        return mRasterEffectsVisible;
    }

    bool getPathEffectsVisible() const {
        return mPathEffectsVisible;
    }

    void anim_setAbsFrame(const int frame);

    void moveDurationRectForAllSelected(const int dFrame);
    void startDurationRectPosTransformForAllSelected();
    void finishDurationRectPosTransformForAllSelected();
    void startMinFramePosTransformForAllSelected();
    void finishMinFramePosTransformForAllSelected();
    void moveMinFrameForAllSelected(const int dFrame);
    void startMaxFramePosTransformForAllSelected();
    void finishMaxFramePosTransformForAllSelected();
    void moveMaxFrameForAllSelected(const int dFrame);

    UndoRedoStack *getUndoRedoStack() {
        return mUndoRedoStack.get();
    }

    void blockUndoRedo();
    void unblockUndoRedo();

    void setParentToLastSelected();
    void clearParentForSelected();
    bool startRotatingAction(const QPointF &cursorPos);
    bool startScalingAction(const QPointF &cursorPos);
    bool startMovingAction(const QPointF &cursorPos);
    void deselectAllBoxesAction();
    void selectAllBoxesAction();
    void selectAllPointsAction();
    bool handlePaintModeKeyPress(QKeyEvent * const event);
    bool handleTransormationInputKeyEvent(QKeyEvent * const event);

    void setCurrentGroupParentAsCurrentGroup();

    void setCurrentRenderRange(const FrameRange& range) {
        mCurrRenderRange = range;
    }
private:
    void openTextEditorForTextBox(TextBox *textBox);
    void callUpdateSchedulers();

    bool isShiftPressed();

    bool isShiftPressed(QKeyEvent *event);
    bool isCtrlPressed();
    bool isCtrlPressed(QKeyEvent *event);
    bool isAltPressed();
    bool isAltPressed(QKeyEvent *event);

    void scaleSelected();
    void rotateSelected();
    qreal mLastDRot = 0;
    int mRotHalfCycles = 0;
    TransformMode mTransMode = MODE_NONE;
protected:
    stdsptr<UndoRedoStack> mUndoRedoStack;

    void paintPress(const ulong ts, const qreal pressure,
                    const qreal xTilt, const qreal yTilt);
    void paintMove(const ulong ts, const qreal pressure,
                   const qreal xTilt, const qreal yTilt);
    void updatePaintBox();
    void setPaintBox(PaintBox * const box);
    void setPaintDrawable(DrawableAutoTiledSurface * const surf);
    void afterPaintAnimSurfaceChanged();

    ulong mLastTs;
    bool mPaintBoxWasVisible;
    qptr<PaintBox> mPaintDrawableBox;
    qptr<AnimatedSurface> mPaintAnimSurface;
    AnimatedSurface::OnionSkin mPaintOnion;
    bool mPaintPressedSinceUpdate = false;
    DrawableAutoTiledSurface * mPaintDrawable = nullptr;

    QColor mCurrentBrushColor;
    const SimpleBrushWrapper * mCurrentBrush = nullptr;
    bool mStylusDrawing = false;
    bool mPickFillFromPath = false;
    bool mPickStrokeFromPath = false;

    uint mLastStateId = 0;
    HDDCachableCacheHandler mCacheHandler;
    bool mUpdateReplaceCache = false;

    sk_sp<SkImage> mRenderImageSk;

    qsptr<ColorAnimator> mBackgroundColor =
            SPtrCreate(ColorAnimator)();

    SmartVectorPath *getPathResultingFromOperation(const SkPathOp &pathOp);

    void sortSelectedBoxesByZAscending();

    QMatrix mCanvasTransform;
    qsptr<SoundComposition> mSoundComposition;

    bool mLocalPivot = false;
    bool mIsCurrentCanvas = true;
    int mMaxFrame = 0;

    qreal mResolutionFraction;

    MainWindow *mMainWindow;
    CanvasWindow *mCanvasWindow;
    QWidget *mCanvasWidget;

    qptr<Circle> mCurrentCircle;
    qptr<Rectangle> mCurrentRectangle;
    qptr<TextBox> mCurrentTextBox;
    qptr<ParticleBox> mCurrentParticleBox;
    qptr<ContainerBox> mCurrentBoxesGroup;

    stdptr<MovablePoint> mHoveredPoint_d;
    qptr<BoundingBox> mHoveredBox;

    qptr<BoundingBox> mLastPressedBox;
    stdsptr<PathPivot> mRotPivot;

    stdptr<SmartNodePoint> mLastEndPoint;

    NormalSegment mHoveredNormalSegment;
    NormalSegment mCurrentNormalSegment;
    qreal mCurrentNormalSegmentT;

    bool mTransformationFinishedBeforeMouseRelease = false;

    ValueInput mValueInput;

    bool mPreviewing = false;
    bool mRenderingPreview = false;
    bool mRenderingOutput = false;
    FrameRange mCurrRenderRange;

    bool mCurrentPreviewContainerOutdated = false;
    stdsptr<ImageCacheContainer> mCurrentPreviewContainer;
    stdsptr<ImageCacheContainer> mLoadingPreviewContainer;

    int mCurrentPreviewFrameId;
    int mMaxPreviewFrameId = 0;

    bool mClipToCanvasSize = false;
    bool mRasterEffectsVisible = true;
    bool mPathEffectsVisible = true;

    bool mIsMouseGrabbing = false;

    bool mDoubleClick = false;
    int mMovesToSkip = 0;

    QColor mFillColor;
    QColor mOutlineColor;

    int mWidth;
    int mHeight;

    qreal mFps = 24;

    qreal mVisibleWidth;
    qreal mVisibleHeight;
    bool mPivotUpdateNeeded = false;

    bool mFirstMouseMove = false;
    bool mSelecting = false;
//    bool mMoving = false;
    QPointF mLastMouseEventPosRel;
    QPointF mLastMouseEventPosAbs;
    QPointF mLastPressPosAbs;
    QPointF mLastPressPosRel;
    QPointF mCurrentMouseEventPosRel;
    QPointF mCurrentMouseEventPosAbs;

    QRectF mSelectionRect;
    CanvasMode mCurrentMode = MOVE_BOX;

    void setCtrlPointsEnabled(bool enabled);
    void handleMovePointMousePressEvent();
    void handleMovePointMouseMove();

    void handleMovePathMousePressEvent();
    void handleMovePathMouseMove();

    void handleAddSmartPointMousePress();
    void handleAddSmartPointMouseMove();
    void handleAddSmartPointMouseRelease();

    void updateTransformation();
    void handleMouseRelease();
    QPointF getMoveByValueForEventPos(const QPointF &eventPos);
    void cancelCurrentTransform();
    void releaseMouseAndDontTrack();
    void setLastMouseEventPosAbs(const QPointF &abs);
    void setLastMousePressPosAbs(const QPointF &abs);
    void setCurrentMouseEventPosAbs(const QPointF &abs);
};

#endif // CANVAS_H
