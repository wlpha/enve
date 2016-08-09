#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include "vectorpath.h"

class MainWindow;

class UndoRedo;

class UndoRedoStack;

enum CanvasMode : short {
    MOVE_PATH,
    MOVE_POINT,
    ADD_POINT
};

class Canvas : public QWidget, public ConnectedToMainWindow
{
    Q_OBJECT
public:
    explicit Canvas(MainWindow *parent = 0);
    void callKeyPress(QKeyEvent *event);
    void addPathToSelection(VectorPath *path);
    void clearPathsSelection();
    void selectOnlyLastPressedPath();
    void removePointFromSelection(PathPoint *point);
    void removePathFromSelection(VectorPath *path);
    void selectOnlyLastPressedPoint();
    bool isShiftPressed();
    void connectPointsFromDifferentPaths(PathPoint *pointSrc, PathPoint *pointDest);

    void addPath(VectorPath *path, bool saveUndoRedo = true);
    void removePath(VectorPath *path, bool saveUndoRedo = true);

    void scheduleRepaint();

    void repaintIfNeeded();
    void setCanvasMode(CanvasMode mode);
    void startSelectionAtPoint(QPointF pos);
    void moveSecondSelectionPoint(QPointF pos);
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void clearPointsSelection();
    void addPointToSelection(PathPoint *point);

    void setCurrentPoint(PathPoint *point);

    PathPoint *getCurrentPoint();

    void handleMovePathMouseRelease(QMouseEvent *event);
    void handleMovePointMouseRelease(QMouseEvent *event);
signals:

public slots:

private:
    bool mRepaintNeeded = false;

    bool mFirstMouseMove = false;
    bool mSelecting = false;
//    bool mMoving = false;
    QPoint mPressPos;
    QRectF mSelectionRect;
    CanvasMode mCurrentMode = ADD_POINT;
    QList<VectorPath*> mPaths;
    PathPoint *mLastPressedPoint = NULL;
    PathPoint *mCurrentPoint = NULL;
    QList<PathPoint*> mSelectedPoints;
    VectorPath *mLastPressedPath = NULL;
    QList<VectorPath*> mSelectedPaths;
};

#endif // CANVAS_H
