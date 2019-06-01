#include "taskscheduler.h"
#include "Boxes/boundingboxrenderdata.h"
#include "GPUEffects/gpupostprocessor.h"
#include "canvas.h"
#include "taskexecutor.h"
#include <QThread>

TaskScheduler *TaskScheduler::sInstance;

TaskScheduler::TaskScheduler() {
    sInstance = this;
    const int numberThreads = qMax(1, QThread::idealThreadCount());
    for(int i = 0; i < numberThreads; i++) {
        const auto taskExecutor = new CPUExecController(this);
        connect(taskExecutor, &ExecController::finishedTaskSignal,
                this, &TaskScheduler::afterCPUTaskFinished);

        mCPUExecutors << taskExecutor;
        mFreeCPUExecs << taskExecutor;
    }

    mHDDExecutor = new HDDExecController;
    mHDDExecs << mHDDExecutor;
    connect(mHDDExecutor, &ExecController::finishedTaskSignal,
            this, &TaskScheduler::afterHDDTaskFinished);
    connect(mHDDExecutor, &HDDExecController::HDDPartFinished,
            this, &TaskScheduler::switchToBackupHDDExecutor);

    mFreeBackupHDDExecs << createNewBackupHDDExecutor();
}

TaskScheduler::~TaskScheduler() {
    for(const auto& exec : mCPUExecutors) {
        exec->quit();
        exec->wait();
    }
    for(const auto& exec : mHDDExecs) {
        exec->quit();
        exec->wait();
    }
}

void TaskScheduler::initializeGPU() {
    try {
        mGpuPostProcessor.initialize();
    } catch(...) {
        RuntimeThrow("Failed to initialize gpu for post-processing.");
    }

    connect(&mGpuPostProcessor, &GpuPostProcessor::finished,
            this, &TaskScheduler::processNextTasks);
    connect(&mGpuPostProcessor, &GpuPostProcessor::processedAll,
            this, &TaskScheduler::callAllTasksFinishedFunc);
}

void TaskScheduler::scheduleCPUTask(const stdsptr<Task>& task) {
    mScheduledCPUTasks << task;
}

void TaskScheduler::scheduleHDDTask(const stdsptr<Task>& task) {
    mScheduledHDDTasks << task;
}

void TaskScheduler::queCPUTask(const stdsptr<Task>& task) {
    if(!task->isQued()) task->taskQued();
    mQuedCPUTasks.addTask(task);
    if(task->readyToBeProcessed()) processNextQuedCPUTask();
}

bool TaskScheduler::shouldQueMoreCPUTasks() {
    const int nQues = mQuedCPUTasks.countQues();
    const int maxQues = mCPUExecutors.count();
    const bool overflowed = nQues >= maxQues;
    return !mFreeCPUExecs.isEmpty() && !mCPUQueing && !overflowed;
}

HDDExecController* TaskScheduler::createNewBackupHDDExecutor() {
    const auto newExec = new HDDExecController;
    connect(newExec, &ExecController::finishedTaskSignal,
            this, &TaskScheduler::afterHDDTaskFinished);
    mHDDExecs << newExec;
    return newExec;
}

void TaskScheduler::queTasks() {
    queScheduledCPUTasks();
    queScheduledHDDTasks();
}

void TaskScheduler::queScheduledCPUTasks() {
    if(!shouldQueMoreCPUTasks()) return;
    mCPUQueing = true;
    mQuedCPUTasks.beginQue();
    if(mCurrentCanvas) {
        mCurrentCanvas->scheduleWaitingTasks();
        mCurrentCanvas->queScheduledTasks();
    }
    while(!mScheduledCPUTasks.isEmpty())
        queCPUTask(mScheduledCPUTasks.takeLast());
    mQuedCPUTasks.endQue();
    mCPUQueing = false;

    if(!mQuedCPUTasks.isEmpty()) processNextQuedCPUTask();
}

void TaskScheduler::queScheduledHDDTasks() {
    if(mHDDThreadBusy) return;
    for(int i = 0; i < mScheduledHDDTasks.count(); i++) {
        const auto task = mScheduledHDDTasks.takeAt(i);
        if(!task->isQued()) task->taskQued();

        mQuedHDDTasks << task;
        tryProcessingNextQuedHDDTask();
    }
}

void TaskScheduler::switchToBackupHDDExecutor() {
    if(!mHDDThreadBusy) return;
    disconnect(mHDDExecutor, &HDDExecController::HDDPartFinished,
               this, &TaskScheduler::switchToBackupHDDExecutor);

    if(mFreeBackupHDDExecs.isEmpty()) {
        mHDDExecutor = createNewBackupHDDExecutor();
    } else {
        mHDDExecutor = mFreeBackupHDDExecs.takeFirst();
    }
    mHDDThreadBusy = false;

    connect(mHDDExecutor, &HDDExecController::HDDPartFinished,
            this, &TaskScheduler::switchToBackupHDDExecutor);
    processNextQuedHDDTask();
}

void TaskScheduler::tryProcessingNextQuedHDDTask() {
    if(!mHDDThreadBusy) processNextQuedHDDTask();
}

void TaskScheduler::tryProcessingNextQuedCPUTask() {
    if(!mFreeCPUExecs.isEmpty()) processNextQuedCPUTask();
}

void TaskScheduler::afterHDDTaskFinished(
        const stdsptr<Task>& finishedTask,
        ExecController * const controller) {
    if(controller == mHDDExecutor)
        mHDDThreadBusy = false;
    else {
        const auto hddExec = static_cast<HDDExecController*>(controller);
        mFreeBackupHDDExecs << hddExec;
    }
    finishedTask->finishedProcessing();
    processNextTasks();
    if(!HDDTaskBeingProcessed()) queTasks();
    callAllTasksFinishedFunc();
}
#include "GUI/usagewidget.h"
#include "GUI/mainwindow.h"
void TaskScheduler::processNextQuedHDDTask() {
    if(!mHDDThreadBusy) {
        for(int i = 0; i < mQuedHDDTasks.count(); i++) {
            const auto task = mQuedHDDTasks.at(i);
            if(task->readyToBeProcessed()) {
                task->aboutToProcess();
                const auto hddTask = dynamic_cast<HDDTask*>(task.get());
                if(hddTask) hddTask->setController(mHDDExecutor);
                mQuedHDDTasks.removeAt(i--);
                mHDDThreadBusy = true;
                mHDDExecutor->processTask(task);
                break;
            }
        }
    }
    const auto usageWidget = MainWindow::getInstance()->getUsageWidget();
    if(!usageWidget) return;
    usageWidget->setHddUsage(mHDDThreadBusy);
}

void TaskScheduler::processNextTasks() {
    processNextQuedHDDTask();
    processNextQuedCPUTask();
    if(shouldQueMoreCPUTasks())
        callFreeThreadsForCPUTasksAvailableFunc();
}

void TaskScheduler::afterCPUTaskFinished(
        const stdsptr<Task>& task,
        ExecController * const controller) {
    mFreeCPUExecs << static_cast<CPUExecController*>(controller);
    if(task->getState() != Task::CANCELED) {
        if(task->needsGpuProcessing()) {
            const auto sTask = GetAsSPtr(task, BoundingBoxRenderData);
            const auto gpuProcess = SPtrCreate(BoxRenderDataScheduledPostProcess)(sTask);
            mGpuPostProcessor.addToProcess(gpuProcess);
        } else {
            task->finishedProcessing();
        }
    }
    processNextTasks();
    if(!CPUTasksBeingProcessed()) queTasks();
    callAllTasksFinishedFunc();
}

void TaskScheduler::processNextQuedCPUTask() {
    while(!mFreeCPUExecs.isEmpty() && !mQuedCPUTasks.isEmpty()) {
        const auto task = mQuedCPUTasks.takeQuedForProcessing();
        if(task) {
            task->aboutToProcess();
            const auto executor = mFreeCPUExecs.takeLast();
            executor->processTask(task);
        } else break;
    }
    const auto usageWidget = MainWindow::getInstance()->getUsageWidget();
    if(!usageWidget) return;
    const int cUsed = mCPUExecutors.count() - mFreeCPUExecs.count();
    usageWidget->setThreadsUsage(cUsed);
}
