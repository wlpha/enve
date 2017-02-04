#include "canvaswidget.h"
#include "canvas.h"
#include <QComboBox>
#include "mainwindow.h"
CanvasWidget::CanvasWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(500, 500);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

Canvas *CanvasWidget::getCurrentCanvas() {
    return mCurrentCanvas;
}

void CanvasWidget::setCurrentCanvas(const int &id) {
    if(mCanvasList.isEmpty()) {
        setCurrentCanvas((Canvas*)NULL);
    } else {
        setCurrentCanvas(mCanvasList.at(id));
    }
}

void CanvasWidget::setCurrentCanvas(Canvas *canvas) {
    mCurrentCanvas = canvas;
    if(mCurrentCanvas != NULL) {
        setCanvasMode(mCurrentCanvas->getCurrentCanvasMode());
    }
    updateDisplayedFillStrokeSettings();
    MainWindow::getInstance()->updateSettingsForCurrentCanvas();
    update();
}

void CanvasWidget::addCanvasToList(Canvas *canvas) {
    canvas->incNumberPointers();
    mCanvasList << canvas;
}

void CanvasWidget::removeCanvas(const int &id) {
    Canvas *canvas = mCanvasList.takeAt(id);
    canvas->decNumberPointers();
    if(mCanvasList.isEmpty()) {
        setCurrentCanvas((Canvas*)NULL);
    } else if(id < mCanvasList.count()) {
        setCurrentCanvas(id);
    } else {
        setCurrentCanvas(id - 1);
    }
}

void CanvasWidget::setCanvasMode(const CanvasMode &mode) {
    if(hasNoCanvas()) {
        setCursor(QCursor(Qt::ArrowCursor) );
        return;
    }

    if(mode == MOVE_PATH) {
        setCursor(QCursor(Qt::ArrowCursor) );
    } else if(mode == MOVE_POINT) {
        setCursor(QCursor(QPixmap(":/cursors/cursor-node.xpm"), 0, 0) );
    } else if(mode == PICK_PATH_SETTINGS) {
        setCursor(QCursor(QPixmap(":/cursors/cursor_color_picker.png"), 2, 20) );
    } else if(mode == ADD_CIRCLE) {
        setCursor(QCursor(QPixmap(":/cursors/cursor-ellipse.xpm"), 4, 4) );
    } else if(mode == ADD_RECTANGLE) {
        setCursor(QCursor(QPixmap(":/cursors/cursor-rect.xpm"), 4, 4) );
    } else if(mode == ADD_TEXT) {
        setCursor(QCursor(QPixmap(":/cursors/cursor-text.xpm"), 4, 4) );
    } else {
        setCursor(QCursor(QPixmap(":/cursors/cursor-pen.xpm"), 4, 4) );
    }

    mCurrentCanvas->setCanvasMode(mode);
    MainWindow::getInstance()->updateCanvasModeButtonsChecked();
    callUpdateSchedulers();
}

void CanvasWidget::callUpdateSchedulers() {
    MainWindow::getInstance()->callUpdateSchedulers();
}

void CanvasWidget::setMovePathMode() {
    setCanvasMode(MOVE_PATH);
}

void CanvasWidget::setMovePointMode() {
    setCanvasMode(MOVE_POINT);
}

void CanvasWidget::setAddPointMode() {
    setCanvasMode(ADD_POINT);
}

void CanvasWidget::setRectangleMode() {
    setCanvasMode(ADD_RECTANGLE);
}

void CanvasWidget::setCircleMode() {
    setCanvasMode(ADD_CIRCLE);
}

void CanvasWidget::setTextMode() {
    setCanvasMode(ADD_TEXT);
}

void CanvasWidget::addCanvasToListAndSetAsCurrent(Canvas *canvas) {
    addCanvasToList(canvas);
    setCurrentCanvas(canvas);
}

void CanvasWidget::renameCanvas(Canvas *canvas, const QString &newName) {
    canvas->setName(newName);
}

void CanvasWidget::renameCanvas(const int &id, const QString &newName) {
    renameCanvas(mCanvasList.at(id), newName);
}

bool CanvasWidget::hasNoCanvas() {
    return mCurrentCanvas == NULL;
}

void CanvasWidget::renameCurrentCanvas(const QString &newName) {
    if(mCurrentCanvas == NULL) return;
    renameCanvas(mCurrentCanvas, newName);
}

void CanvasWidget::paintEvent(QPaintEvent *) {
    if(mCurrentCanvas == NULL) return;
    QPainter p(this);
    mCurrentCanvas->paintEvent(&p);
    p.end();
}

void CanvasWidget::mousePressEvent(QMouseEvent *event) {
    if(mCurrentCanvas == NULL) return;
    mCurrentCanvas->mousePressEvent(event);
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event) {
    if(mCurrentCanvas == NULL) return;
    mCurrentCanvas->mouseReleaseEvent(event);
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event) {
    if(mCurrentCanvas == NULL) return;
    mCurrentCanvas->mouseMoveEvent(event);
}

void CanvasWidget::wheelEvent(QWheelEvent *event) {
    if(mCurrentCanvas == NULL) return;
    mCurrentCanvas->wheelEvent(event);
}

void CanvasWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    if(mCurrentCanvas == NULL) return;
    mCurrentCanvas->mouseDoubleClickEvent(event);
}

void CanvasWidget::keyPressEvent(QKeyEvent *event) {
    if(mCurrentCanvas == NULL) return;
    mCurrentCanvas->keyPressEvent(event);
}

bool CanvasWidget::processUnfilteredKeyEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_F1) {
        setCanvasMode(CanvasMode::MOVE_PATH);
    } else if(event->key() == Qt::Key_F2) {
        setCanvasMode(CanvasMode::MOVE_POINT);
    } else if(event->key() == Qt::Key_F3) {
        setCanvasMode(CanvasMode::ADD_POINT);
    } else if(event->key() == Qt::Key_F4) {
        setCanvasMode(CanvasMode::ADD_CIRCLE);
    } else if(event->key() == Qt::Key_F5) {
        setCanvasMode(CanvasMode::ADD_RECTANGLE);
    } else if(event->key() == Qt::Key_F6) {
        setCanvasMode(CanvasMode::ADD_TEXT);
    } else {
        return false;
    }
    return true;
}

bool CanvasWidget::processFilteredKeyEvent(QKeyEvent *event) {
    if(processUnfilteredKeyEvent(event)) return true;
    if(hasNoCanvas()) return false;
    return mCurrentCanvas->processFilteredKeyEvent(event);
}

void CanvasWidget::raiseAction() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->raiseAction();
    callUpdateSchedulers();
}

void CanvasWidget::lowerAction() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->lowerAction();
    callUpdateSchedulers();
}

void CanvasWidget::raiseToTopAction() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->raiseToTopAction();
    callUpdateSchedulers();
}

void CanvasWidget::lowerToBottomAction() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->lowerToBottomAction();
    callUpdateSchedulers();
}

void CanvasWidget::objectsToPathAction() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->objectsToPathAction();
    callUpdateSchedulers();
}

void CanvasWidget::pathsUnionAction() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->pathsUnionAction();
    callUpdateSchedulers();
}

void CanvasWidget::pathsDifferenceAction() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->pathsDifferenceAction();
    callUpdateSchedulers();
}

void CanvasWidget::pathsIntersectionAction() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->pathsIntersectionAction();
    callUpdateSchedulers();
}

void CanvasWidget::pathsDivisionAction() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->pathsDivisionAction();
    callUpdateSchedulers();
}

void CanvasWidget::pathsExclusionAction() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->pathsExclusionAction();
    callUpdateSchedulers();
}

void CanvasWidget::setFontFamilyAndStyle(QString family, QString style) {
    if(hasNoCanvas()) return;
    mCurrentCanvas->setFontFamilyAndStyle(family, style);
    callUpdateSchedulers();
}

void CanvasWidget::setFontSize(qreal size) {
    if(hasNoCanvas()) return;
    mCurrentCanvas->setFontSize(size);
    callUpdateSchedulers();
}

void CanvasWidget::connectPointsSlot() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->connectPointsSlot();
    callUpdateSchedulers();
}

void CanvasWidget::disconnectPointsSlot() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->disconnectPointsSlot();
    callUpdateSchedulers();
}

void CanvasWidget::mergePointsSlot() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->mergePointsSlot();
    callUpdateSchedulers();
}

void CanvasWidget::makePointCtrlsSymmetric() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->makePointCtrlsSymmetric();
    callUpdateSchedulers();
}

void CanvasWidget::makePointCtrlsSmooth() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->makePointCtrlsSmooth();
    callUpdateSchedulers();
}

void CanvasWidget::makePointCtrlsCorner() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->makePointCtrlsCorner();
    callUpdateSchedulers();
}

void CanvasWidget::makeSegmentLine() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->makeSegmentLine();
    callUpdateSchedulers();
}

void CanvasWidget::makeSegmentCurve() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->makeSegmentCurve();
    callUpdateSchedulers();
}

void CanvasWidget::startSelectedStrokeWidthTransform() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->startSelectedStrokeWidthTransform();
    callUpdateSchedulers();
}

void CanvasWidget::startSelectedStrokeColorTransform() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->startSelectedStrokeColorTransform();
    callUpdateSchedulers();
}

void CanvasWidget::startSelectedFillColorTransform() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->startSelectedFillColorTransform();
    callUpdateSchedulers();
}

void CanvasWidget::fillPaintTypeChanged(const PaintType &paintType,
                                        const Color &color,
                                        Gradient *gradient) {
    if(hasNoCanvas()) return;
    mCurrentCanvas->fillPaintTypeChanged(paintType,
                                         color,
                                         gradient);
    callUpdateSchedulers();
}

void CanvasWidget::strokePaintTypeChanged(const PaintType &paintType,
                                          const Color &color,
                                          Gradient *gradient) {
    if(hasNoCanvas()) return;
    mCurrentCanvas->strokePaintTypeChanged(paintType,
                                           color,
                                           gradient);
    callUpdateSchedulers();
}

void CanvasWidget::strokeCapStyleChanged(const Qt::PenCapStyle &capStyle) {
    if(hasNoCanvas()) return;
    mCurrentCanvas->strokeCapStyleChanged(capStyle);
    callUpdateSchedulers();
}

void CanvasWidget::strokeJoinStyleChanged(const Qt::PenJoinStyle &joinStyle) {
    if(hasNoCanvas()) return;
    mCurrentCanvas->strokeJoinStyleChanged(joinStyle);
    callUpdateSchedulers();
}

void CanvasWidget::strokeWidthChanged(const qreal &strokeWidth,
                                      const bool &finish) {
    if(hasNoCanvas()) return;
    mCurrentCanvas->strokeWidthChanged(strokeWidth, finish);
    callUpdateSchedulers();
}

void CanvasWidget::strokeFlatColorChanged(const Color &color,
                                      const bool &finish) {
    if(hasNoCanvas()) return;
    mCurrentCanvas->strokeFlatColorChanged(color, finish);
    callUpdateSchedulers();
}

void CanvasWidget::fillFlatColorChanged(const Color &color,
                                        const bool &finish) {
    if(hasNoCanvas()) return;
    mCurrentCanvas->fillFlatColorChanged(color, finish);
    callUpdateSchedulers();
}

void CanvasWidget::fillGradientChanged(Gradient *gradient,
                                       const bool &finish) {
    if(hasNoCanvas()) return;
    mCurrentCanvas->fillGradientChanged(gradient, finish);
    callUpdateSchedulers();
}

void CanvasWidget::strokeGradientChanged(Gradient *gradient,
                                       const bool &finish) {
    if(hasNoCanvas()) return;
    mCurrentCanvas->strokeGradientChanged(gradient, finish);
    callUpdateSchedulers();
}

void CanvasWidget::pickPathForSettings() {
    if(hasNoCanvas()) return;
    setCanvasMode(PICK_PATH_SETTINGS);
}

void CanvasWidget::updateDisplayedFillStrokeSettings() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->updateDisplayedFillStrokeSettings();
}

void CanvasWidget::setHighQualityView(const bool &bT) {
    if(hasNoCanvas()) return;
    mCurrentCanvas->setHighQualityPaint(bT);
    mCurrentCanvas->updateAllBoxes();
}

void CanvasWidget::setEffectsPaintEnabled(const bool &bT) {
    if(hasNoCanvas()) return;
    mCurrentCanvas->setEffectsPaintEnabled(bT);
    mCurrentCanvas->updateAllBoxes();
}

void CanvasWidget::setResolutionPercent(const qreal &percent) {
    if(hasNoCanvas()) return;
    mCurrentCanvas->setResolutionPercent(percent);
    mCurrentCanvas->updateAllBoxes();
}

void CanvasWidget::updatePivotIfNeeded() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->updatePivotIfNeeded();
}

void CanvasWidget::schedulePivotUpdate() {
    if(hasNoCanvas()) return;
    mCurrentCanvas->schedulePivotUpdate();
}
