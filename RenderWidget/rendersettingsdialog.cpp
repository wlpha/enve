#include "rendersettingsdialog.h"
#include "mainwindow.h"
#define COMPLIENCE FF_COMPLIANCE_NORMAL

static const QList<FormatCodecs> SUPPORTED_FORMATS = {
    FormatCodecs(QList<AVCodecID>() << AV_CODEC_ID_PNG << AV_CODEC_ID_TIFF << AV_CODEC_ID_MJPEG,
                 QList<AVCodecID>(),
                 "*.jpg"),
    FormatCodecs(QList<AVCodecID>() << AV_CODEC_ID_PNG << AV_CODEC_ID_QTRLE << AV_CODEC_ID_H264,
                 QList<AVCodecID>() << AV_CODEC_ID_MP3 << AV_CODEC_ID_AAC << AV_CODEC_ID_AC3 << AV_CODEC_ID_FLAC << AV_CODEC_ID_VORBIS << AV_CODEC_ID_WAVPACK,
                 "*.mov"),
    FormatCodecs(QList<AVCodecID>() << AV_CODEC_ID_H264 << AV_CODEC_ID_MPEG4 << AV_CODEC_ID_MPEG2VIDEO << AV_CODEC_ID_HEVC,
                 QList<AVCodecID>() << AV_CODEC_ID_MP3 << AV_CODEC_ID_AAC << AV_CODEC_ID_AC3 << AV_CODEC_ID_FLAC << AV_CODEC_ID_VORBIS << AV_CODEC_ID_WAVPACK,
                 "*.mp4"),
    FormatCodecs(QList<AVCodecID>() << AV_CODEC_ID_H264 << AV_CODEC_ID_MPEG4 << AV_CODEC_ID_MPEG2VIDEO << AV_CODEC_ID_HEVC,
                 QList<AVCodecID>() << AV_CODEC_ID_MP3 << AV_CODEC_ID_AAC << AV_CODEC_ID_AC3 << AV_CODEC_ID_FLAC << AV_CODEC_ID_VORBIS << AV_CODEC_ID_WAVPACK,
    "*.mkv"),
    FormatCodecs(QList<AVCodecID>() << AV_CODEC_ID_H264 << AV_CODEC_ID_MPEG4 << AV_CODEC_ID_MPEG2VIDEO << AV_CODEC_ID_HEVC << AV_CODEC_ID_RAWVIDEO << AV_CODEC_ID_MJPEG << AV_CODEC_ID_JPEG2000 << AV_CODEC_ID_JPEGLS,
                 QList<AVCodecID>() << AV_CODEC_ID_MP3 << AV_CODEC_ID_AAC << AV_CODEC_ID_AC3 << AV_CODEC_ID_FLAC << AV_CODEC_ID_VORBIS << AV_CODEC_ID_WAVPACK,
    "*.avi"),
    FormatCodecs(QList<AVCodecID>(),
                 QList<AVCodecID>() << AV_CODEC_ID_MP3 << AV_CODEC_ID_AAC << AV_CODEC_ID_AC3 << AV_CODEC_ID_FLAC << AV_CODEC_ID_VORBIS << AV_CODEC_ID_WAVPACK,
    "*.mp3"),
    FormatCodecs(QList<AVCodecID>(),
                 QList<AVCodecID>() << AV_CODEC_ID_FLAC,
    "*.flac"),
    FormatCodecs(QList<AVCodecID>(),
                 QList<AVCodecID>() << AV_CODEC_ID_MP3 << AV_CODEC_ID_AAC << AV_CODEC_ID_AC3 << AV_CODEC_ID_FLAC << AV_CODEC_ID_VORBIS << AV_CODEC_ID_WAVPACK,
    "*.wav"),
    FormatCodecs(QList<AVCodecID>(),
                 QList<AVCodecID>() << AV_CODEC_ID_MP3 << AV_CODEC_ID_AAC << AV_CODEC_ID_AC3 << AV_CODEC_ID_FLAC << AV_CODEC_ID_VORBIS << AV_CODEC_ID_WAVPACK,
    "*.ogg"),
    FormatCodecs(QList<AVCodecID>(),
                 QList<AVCodecID>() << AV_CODEC_ID_MP3 << AV_CODEC_ID_AAC << AV_CODEC_ID_AC3 << AV_CODEC_ID_FLAC << AV_CODEC_ID_VORBIS << AV_CODEC_ID_WAVPACK,
    "*.aiff")
};

RenderSettingsDialog::RenderSettingsDialog(const RenderInstanceSettings &settings,
                                           QWidget *parent) :
    QDialog(parent) {
    setStyleSheet(MainWindow::getInstance()->styleSheet());
    mInitialSettings = settings;

    mMainLayout = new QVBoxLayout(this);
    setLayout(mMainLayout);

    mOutputFormatsLayout = new QHBoxLayout();
    mOutputFormatsLabel = new QLabel("Format:", this);
    mOutputFormatsComboBox = new QComboBox(this);
    mOutputFormatsLayout->addWidget(mOutputFormatsLabel);
    mOutputFormatsLayout->addWidget(mOutputFormatsComboBox);

    mVideoGroupBox = new QGroupBox("Video", this);
    mVideoGroupBox->setCheckable(true);
    mVideoGroupBox->setChecked(true);
    mVideoSettingsLayout = new TwoColumnLayout();
    mVideoCodecsLabel = new QLabel("Codec:", this);
    mPixelFormatsLabel = new QLabel("Pixel format:", this);
    mBitrateLabel = new QLabel("Bitrate:", this);

    mVideoCodecsComboBox = new QComboBox(this);
    mPixelFormatsComboBox = new QComboBox(this);
    mBitrateSpinBox = new QDoubleSpinBox(this);
    mBitrateSpinBox->setRange(0.1, 100.);
    mBitrateSpinBox->setSuffix(" Mbps");

    mVideoSettingsLayout->addPair(mVideoCodecsLabel,
                                  mVideoCodecsComboBox);
    mVideoSettingsLayout->addPair(mPixelFormatsLabel,
                                  mPixelFormatsComboBox);
    mVideoSettingsLayout->addPair(mBitrateLabel,
                                  mBitrateSpinBox);

    mAudioGroupBox = new QGroupBox("Audio", this);
    mAudioGroupBox->setCheckable(true);
    mAudioSettingsLayout = new TwoColumnLayout();
    mAudioCodecsLabel = new QLabel("Codec:", this);
    mSampleRateLabel = new QLabel("Sample rate:", this);
    mSampleFormatsLabel = new QLabel("Sample format:", this);
    mAudioBitrateLabel = new QLabel("Bitrate:", this);
    mAudioChannelLayoutLabel = new QLabel("Channels:", this);

    mAudioCodecsComboBox = new QComboBox(this);
    mSampleRateComboBox = new QComboBox(this);
    mSampleFormatsComboBox = new QComboBox(this);
    mAudioBitrateComboBox = new QComboBox(this);
    mAudioChannelLayoutsComboBox = new QComboBox(this);

    mAudioSettingsLayout->addPair(mAudioCodecsLabel,
                                  mAudioCodecsComboBox);
    mAudioSettingsLayout->addPair(mSampleRateLabel,
                                  mSampleRateComboBox);
    mAudioSettingsLayout->addPair(mSampleFormatsLabel,
                                  mSampleFormatsComboBox);
    mAudioSettingsLayout->addPair(mAudioBitrateLabel,
                                  mAudioBitrateComboBox);
    mAudioSettingsLayout->addPair(mAudioChannelLayoutLabel,
                                  mAudioChannelLayoutsComboBox);

    mButtonsLayout = new QHBoxLayout();
    mOkButton = new QPushButton("Ok", this);
    mCancelButton = new QPushButton("Cancel", this);
    mResetButton = new QPushButton("Reset", this);
    connect(mOkButton, SIGNAL(released()),
            this, SLOT(accept()));
    connect(mCancelButton, SIGNAL(released()),
            this, SLOT(reject()));
    connect(mResetButton, SIGNAL(released()),
            this, SLOT(restoreInitialSettings()));
    mButtonsLayout->addWidget(mResetButton, Qt::AlignLeft);
    mButtonsLayout->addWidget(mCancelButton, Qt::AlignLeft);
    mButtonsLayout->addWidget(mOkButton, Qt::AlignRight);

    mMainLayout->addLayout(mOutputFormatsLayout);
    mVideoGroupBox->setLayout(mVideoSettingsLayout);
    mMainLayout->addWidget(mVideoGroupBox);
    mAudioGroupBox->setLayout(mAudioSettingsLayout);
    mMainLayout->addWidget(mAudioGroupBox);
    mMainLayout->addLayout(mButtonsLayout);

    connect(mOutputFormatsComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(updateAvailableVideoCodecs()));
    connect(mVideoCodecsComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(updateAvailablePixelFormats()));

    connect(mOutputFormatsComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(updateAvailableAudioCodecs()));
    connect(mAudioCodecsComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(updateAvailableSampleFormats()));
    connect(mAudioCodecsComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(updateAvailableSampleRates()));
    connect(mAudioCodecsComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(updateAvailableAudioBitrates()));
    connect(mAudioCodecsComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(updateAvailableAudioChannelLayouts()));

    updateAvailableOutputFormats();
    restoreInitialSettings();
}

void RenderSettingsDialog::updateAvailablePixelFormats() {
    AVCodec *currentCodec = NULL;
    if(mVideoCodecsComboBox->count() > 0) {
        currentCodec = mVideoCodecsList.at(mVideoCodecsComboBox->currentIndex());
    }
    QString currentFormatName = mPixelFormatsComboBox->currentText();
    mPixelFormatsComboBox->clear();
    mPixelFormatsList.clear();
    if(currentCodec != NULL) {
        const AVPixelFormat *format = currentCodec->pix_fmts;
        if(format == NULL) return;
        while(*format != -1) {
            mPixelFormatsList << *format;
            QString formatName = QString(av_get_pix_fmt_name(*format));
            mPixelFormatsComboBox->addItem(formatName);
            if(formatName == currentFormatName) {
                mPixelFormatsComboBox->setCurrentText(formatName);
            }
            format++;
        }
    }
}

void RenderSettingsDialog::addVideoCodec(const AVCodecID &codecId,
                                         AVOutputFormat *outputFormat,
                                         const QString &currentCodecName) {
    AVCodec *currentCodec = avcodec_find_encoder(codecId);
    if(currentCodec == NULL) return;
    if(currentCodec->type != AVMEDIA_TYPE_VIDEO) return;
    if(currentCodec->capabilities & AV_CODEC_CAP_EXPERIMENTAL) return;
    if(currentCodec->pix_fmts == NULL) return;
    if(avformat_query_codec(outputFormat, codecId, COMPLIENCE) <= 0) return;
    mVideoCodecsList << currentCodec;
    QString codecName = QString(currentCodec->long_name);
    mVideoCodecsComboBox->addItem(codecName);
    if(codecName == currentCodecName) {
        mVideoCodecsComboBox->setCurrentText(codecName);
    }
}

void RenderSettingsDialog::addAudioCodec(const AVCodecID &codecId,
                                         AVOutputFormat *outputFormat,
                                         const QString &currentCodecName) {
    AVCodec *currentCodec = avcodec_find_encoder(codecId);
    if(currentCodec == NULL) return;
    if(currentCodec->type != AVMEDIA_TYPE_AUDIO) return;
    if(currentCodec->capabilities & AV_CODEC_CAP_EXPERIMENTAL) return;
    if(currentCodec->sample_fmts == NULL) return;
    if(avformat_query_codec(outputFormat, codecId, COMPLIENCE) <= 0) return;
    mAudioCodecsList << currentCodec;
    QString codecName = QString(currentCodec->long_name);
    mAudioCodecsComboBox->addItem(codecName);
    if(codecName == currentCodecName) {
        mAudioCodecsComboBox->setCurrentText(codecName);
    }
}

static const QList<AVCodecID> VIDEO_CODECS = { AV_CODEC_ID_H264,
                                         AV_CODEC_ID_HEVC,
                                         AV_CODEC_ID_MPEG4,
                                         AV_CODEC_ID_MPEG2VIDEO,
                                         AV_CODEC_ID_QTRLE,
                                         AV_CODEC_ID_RAWVIDEO,
                                         AV_CODEC_ID_MJPEG,
                                         AV_CODEC_ID_PNG,
                                         AV_CODEC_ID_TIFF,
                                         AV_CODEC_ID_GIF };
static const QList<AVCodecID> AUDIO_CODECS = { AV_CODEC_ID_MP2,
                                         AV_CODEC_ID_MP3,
                                         AV_CODEC_ID_AAC,
                                         AV_CODEC_ID_AC3,
                                         AV_CODEC_ID_FLAC,
                                         AV_CODEC_ID_VORBIS,
                                         AV_CODEC_ID_WAVPACK  };

void RenderSettingsDialog::updateAvailableVideoCodecs() {
    mVideoCodecsComboBox->clear();
    mVideoCodecsList.clear();
    AVOutputFormat *outputFormat = NULL;
    if(mOutputFormatsComboBox->count() > 0) {
        int outputId = mOutputFormatsComboBox->currentIndex();
        outputFormat = mOutputFormatsList.at(outputId);
    }
    if(outputFormat == NULL) return;
    QString currentCodecName = mVideoCodecsComboBox->currentText();
    foreach(const AVCodecID &codecId, VIDEO_CODECS) {
        addVideoCodec(codecId,
                      outputFormat,
                      currentCodecName);
    }

//    AVCodec *currentCodec = NULL;
//    do {
//        currentCodec = av_codec_next(currentCodec);
//        if(currentCodec == NULL) break;
//        if(currentCodec->type != AVMEDIA_TYPE_VIDEO) continue;
//        if(currentCodec->capabilities & AV_CODEC_CAP_EXPERIMENTAL) {
//            continue;
//        }
//        if(currentCodec->pix_fmts == NULL) continue;
//        addVideoCodec(currentCodec);
//    } while(currentCodec != NULL);
    bool noCodecs = mVideoCodecsComboBox->count() == 0;
    mVideoGroupBox->setChecked(!noCodecs);
    mVideoGroupBox->setDisabled(noCodecs);
}

void RenderSettingsDialog::updateAvailableAudioCodecs() {
    mAudioCodecsComboBox->clear();
    mAudioCodecsList.clear();
    AVOutputFormat *outputFormat = NULL;
    if(mOutputFormatsComboBox->count() > 0) {
        int outputId = mOutputFormatsComboBox->currentIndex();
        outputFormat = mOutputFormatsList.at(outputId);
    }
    if(outputFormat == NULL) return;
    QString currentCodecName = mAudioCodecsComboBox->currentText();
    foreach(const AVCodecID &codecId, AUDIO_CODECS) {
        addAudioCodec(codecId,
                      outputFormat,
                      currentCodecName);
    }
//    AVCodec *currentCodec = NULL;
//    do {
//        currentCodec = av_codec_next(currentCodec);
//        if(currentCodec == NULL) break;
//        if(currentCodec->type != AVMEDIA_TYPE_AUDIO) continue;
//        if(currentCodec->sample_fmts == NULL) continue;
//        if(currentCodec->capabilities & AV_CODEC_CAP_EXPERIMENTAL) continue;
//        AVCodecID codecId = currentCodec->id;
//        if(avformat_query_codec(outputFormat, codecId, COMPLIENCE) == 0) continue;
//        mAudioCodecsList << currentCodec;
//        QString codecName = QString(currentCodec->long_name);
//        mAudioCodecsComboBox->addItem(codecName);
//        if(codecName == currentCodecName) {
//            mAudioCodecsComboBox->setCurrentText(codecName);
//        }
//    } while(currentCodec != NULL);
    bool noCodecs = mAudioCodecsComboBox->count() == 0;
    mAudioGroupBox->setChecked(!noCodecs);
    mAudioGroupBox->setDisabled(noCodecs);
}

struct FormatCodecs {
    FormatCodecs(const QList<AVCodecID> &vidCodecs,
                 const QList<AVCodecID> &audioCodec,
                 const char *name) {
        mFormat = av_guess_format(NULL, name, NULL);
        foreach(const AVCodecID &vidCodec, vidCodecs) {
            if(avformat_query_codec(mFormat, vidCodec, COMPLIENCE) == 0) continue;
            mVidCodecs << vidCodec;
        }
        foreach(const AVCodecID &audCodec, audioCodec) {
            if(avformat_query_codec(mFormat, audCodec, COMPLIENCE) == 0) continue;
            mAudioCodecs << audCodec;
        }
    }

    AVOutputFormat *mFormat = NULL;
    QList<AVCodecID> mVidCodecs;
    QList<AVCodecID> mAudioCodecs;
};

void RenderSettingsDialog::updateAvailableOutputFormats() {
    QString currentCodecName = mOutputFormatsComboBox->currentText();
    mOutputFormatsComboBox->clear();

    foreach(const char *chars, OUTPUT_FORMATS) {
        AVOutputFormat *currentFormat = av_guess_format(NULL, chars, NULL);
        if(currentFormat == NULL) break;
        //if(!currentFormat->codec_tag || !currentFormat->codec_tag[0]) continue;
        int supportedCodecs = 0;
        foreach(const AVCodecID &codecId, VIDEO_CODECS) {
            if(avformat_query_codec(currentFormat, codecId, COMPLIENCE) <= 0) continue;
            supportedCodecs++;
            break;
        }
        foreach(const AVCodecID &codecId, AUDIO_CODECS) {
            if(avformat_query_codec(currentFormat, codecId, COMPLIENCE) <= 0) continue;
            supportedCodecs++;
            break;
        }
        if(supportedCodecs == 0) continue;
        //if(currentFormat->query_codec == NULL) continue;
        mOutputFormatsList << currentFormat;
        QString formatName = QString(currentFormat->long_name);
        mOutputFormatsComboBox->addItem(formatName);
        if(formatName == currentCodecName) {
            mOutputFormatsComboBox->setCurrentText(formatName);
        }
    }

//    AVOutputFormat *currentFormat = NULL;
//    do {
//        currentFormat = av_oformat_next(currentFormat);
//        if(currentFormat == NULL) break;
//        if(!currentFormat->codec_tag || !currentFormat->codec_tag[0]) continue;
//        int supportedCodecs = 0;
//        foreach(const AVCodecID &codecId, VIDEO_CODECS) {
//            if(avformat_query_codec(currentFormat, codecId, COMPLIENCE) == 0) continue;
//            supportedCodecs++;
//            break;
//        }
//        foreach(const AVCodecID &codecId, AUDIO_CODECS) {
//            if(avformat_query_codec(currentFormat, codecId, COMPLIENCE) == 0) continue;
//            supportedCodecs++;
//            break;
//        }
//        if(supportedCodecs == 0) continue;
//        //if(currentFormat->query_codec == NULL) continue;
//        mOutputFormatsList << currentFormat;
//        QString formatName = QString(currentFormat->long_name);
//        mOutputFormatsComboBox->addItem(formatName);
//        if(formatName == currentCodecName) {
//            mOutputFormatsComboBox->setCurrentText(formatName);
//        }
//    } while(currentFormat != NULL);
}

static const QStringList sampleFormatsNames = { "8 bits unsigned",
                                                "16 bits signed",
                                                "32 bits signed",
                                                "32 bits float",
                                                "64 bits double",
                                                "8 bits unsigned, planar",
                                                "16 bits signed, planar",
                                                "32 bits signed, planar",
                                                "32 bits float, planar",
                                                "64 bits double, planar",
                                                "64 bits signed",
                                                "64 bits signed, planar",
                                                "" };

void RenderSettingsDialog::updateAvailableSampleFormats() {
    QString currentFormatName = mSampleFormatsComboBox->currentText();
    mSampleFormatsComboBox->clear();
    AVCodec *currentCodec = NULL;
    if(mAudioCodecsComboBox->count() > 0) {
        int codecId = mAudioCodecsComboBox->currentIndex();
        currentCodec = mAudioCodecsList.at(codecId);
    }
    if(currentCodec == NULL) return;
    const AVSampleFormat *sampleFormats = currentCodec->sample_fmts;
    if(sampleFormats == NULL) return;
    while(*sampleFormats != -1) {
        mSampleFormatsList << *sampleFormats;
        QString formatName = sampleFormatsNames.at(*sampleFormats);
        mSampleFormatsComboBox->addItem(formatName);
        if(formatName == currentFormatName) {
            mSampleFormatsComboBox->setCurrentText(formatName);
        }
        sampleFormats++;
    }
}

void RenderSettingsDialog::updateAvailableAudioBitrates() {
    mAudioBitrateComboBox->clear();
    mAudioBitrateList.clear();
    AVCodec *currentCodec = NULL;
    if(mAudioCodecsComboBox->count() > 0) {
        int codecId = mAudioCodecsComboBox->currentIndex();
        currentCodec = mAudioCodecsList.at(codecId);
    }
    if(currentCodec == NULL) return;
    if(currentCodec->capabilities & AV_CODEC_CAP_LOSSLESS) {
        mAudioBitrateList << 384000;
        mAudioBitrateComboBox->addItem("Loseless");
    } else {
        QList<int> ratesT = { 24, 32, 48, 64, 128, 160, 192, 320, 384 };
        foreach(const int &rateT, ratesT) {
            mAudioBitrateComboBox->addItem(QString::number(rateT) + " kbps");
            mAudioBitrateList << rateT*1000;
        }
    }
}

void RenderSettingsDialog::updateAvailableSampleRates() {
    mSampleRateComboBox->clear();
    mSampleRatesList.clear();
    AVCodec *currentCodec = NULL;
    if(mAudioCodecsComboBox->count() > 0) {
        int codecId = mAudioCodecsComboBox->currentIndex();
        currentCodec = mAudioCodecsList.at(codecId);
    }
    if(currentCodec == NULL) return;
    const int *sampleRates = currentCodec->supported_samplerates;
    if(sampleRates == NULL) {
        QList<int> ratesT = { 96000, 88200, 64000, 48000, 44100,
                              32000, 22050, 11025, 8000 };
        foreach(const int &rateT, ratesT) {
            mSampleRateComboBox->addItem(QString::number(rateT) + " Hz");
            mSampleRatesList << rateT;
        }
    } else {
        int rateT = *sampleRates;
        while(rateT != 0) {
            mSampleRateComboBox->addItem(QString::number(rateT) + " Hz");
            mSampleRatesList << rateT;
            sampleRates++;
            rateT = *sampleRates;
        }
    }
}

const static QString getChannelLayoutName(const uint64_t &layout) {
    if(layout == AV_CH_LAYOUT_MONO) {
        return "Mono";
    } else if(layout == AV_CH_LAYOUT_STEREO) {
        return "Stereo";
    } else if(layout == AV_CH_LAYOUT_2POINT1) {
        return "2.1";
    } else if(layout == AV_CH_LAYOUT_SURROUND) {
        return "3.0";
    } else if(layout == AV_CH_LAYOUT_2_1) {
        return "3.0(back)";
    } else if(layout == AV_CH_LAYOUT_4POINT0) {
        return "4.0";
    } else if(layout == AV_CH_LAYOUT_QUAD) {
        return "Quad";
    } else if(layout == AV_CH_LAYOUT_2_2) {
        return "Quad(side)";
    } else if(layout == AV_CH_LAYOUT_3POINT1) {
        return "3.1";
    } else if(layout == AV_CH_LAYOUT_5POINT0_BACK) {
        return "5.0";
    } else if(layout == AV_CH_LAYOUT_5POINT0) {
        return "5.0(side)";
    } else if(layout == AV_CH_LAYOUT_4POINT1) {
        return "4.1";
    } else if(layout == AV_CH_LAYOUT_5POINT1_BACK) {
        return "5.1";
    } else if(layout == AV_CH_LAYOUT_5POINT1) {
        return "5.1(side)";
    } else if(layout == AV_CH_LAYOUT_6POINT0) {
        return "6.0";
    } else if(layout == AV_CH_LAYOUT_6POINT0_FRONT) {
        return "6.0(front)";
    } else if(layout == AV_CH_LAYOUT_HEXAGONAL) {
        return "Hexagonal";
    } else if(layout == AV_CH_LAYOUT_6POINT1) {
        return "6.1";
    } else if(layout == AV_CH_LAYOUT_6POINT1_BACK) {
        return "6.1(back)";
    } else if(layout == AV_CH_LAYOUT_6POINT1_FRONT) {
        return "6.1(front)";
    } else if(layout == AV_CH_LAYOUT_7POINT0) {
        return "7.0";
    } else if(layout == AV_CH_LAYOUT_7POINT0_FRONT) {
        return "7.0(front)";
    } else if(layout == AV_CH_LAYOUT_7POINT1) {
        return "7.1";
    } else if(layout == AV_CH_LAYOUT_7POINT1_WIDE_BACK) {
        return "7.1(wide)";
    } else if(layout == AV_CH_LAYOUT_7POINT1_WIDE) {
        return "7.1(wide-side)";
    } else if(layout == AV_CH_LAYOUT_OCTAGONAL) {
        return "Octagonal";
    } else if(layout == AV_CH_LAYOUT_HEXADECAGONAL) {
        return "Hexadecagonal";
    } else if(layout == AV_CH_LAYOUT_STEREO_DOWNMIX) {
        return "Downmix";
    }
    return "Invalid";
}

void RenderSettingsDialog::updateAvailableAudioChannelLayouts() {
    mAudioChannelLayoutsComboBox->clear();
    mAudioChannelLayoutsList.clear();
    AVCodec *currentCodec = NULL;
    if(mAudioCodecsComboBox->count() > 0) {
        int codecId = mAudioCodecsComboBox->currentIndex();
        currentCodec = mAudioCodecsList.at(codecId);
    }
    if(currentCodec == NULL) return;
    const uint64_t *layouts = currentCodec->channel_layouts;
    if(layouts == NULL) {
        mAudioChannelLayoutsList << AV_CH_LAYOUT_MONO;
        mAudioChannelLayoutsList << AV_CH_LAYOUT_STEREO;
        mAudioChannelLayoutsComboBox->addItem("Mono");
        mAudioChannelLayoutsComboBox->addItem("Stereo");
    } else {
        uint64_t layout = *layouts;
        while(layout != 0) {
            QString layoutName = getChannelLayoutName(layout);
            mAudioChannelLayoutsList << layout;
            mAudioChannelLayoutsComboBox->addItem(layoutName);
            layouts++;
            layout = *layouts;
        }
    }
}

void RenderSettingsDialog::restoreInitialSettings() {
    AVOutputFormat *currentOutputFormat = mInitialSettings.getOutputFormat();
    if(currentOutputFormat == NULL) {
        mOutputFormatsComboBox->setCurrentIndex(0);
    } else {
        QString currentOutputFormatName = QString(currentOutputFormat->long_name);
        mOutputFormatsComboBox->setCurrentText(currentOutputFormatName);
    }
    AVCodec *currentVideoCodec = mInitialSettings.getVideoCodec();
    if(currentVideoCodec == NULL) {
        mVideoCodecsComboBox->setCurrentIndex(0);
    } else {
        QString currentCodecName = QString(currentVideoCodec->long_name);
        mVideoCodecsComboBox->setCurrentText(currentCodecName);
    }
    AVPixelFormat currentFormat = mInitialSettings.getVideoPixelFormat();
    if(currentFormat == AV_PIX_FMT_NONE) {
        mPixelFormatsComboBox->setCurrentIndex(0);
    } else {
        QString currentFormatName = QString(av_get_pix_fmt_name(currentFormat));
        mPixelFormatsComboBox->setCurrentText(currentFormatName);
    }
    int currentBitrate = mInitialSettings.getVideoBitrate();
    if(currentBitrate <= 0) {
        mBitrateSpinBox->setValue(9.);
    } else {
        mBitrateSpinBox->setValue(currentBitrate/1000000.);
    }
    bool noVideoCodecs = mVideoCodecsComboBox->count() == 0;
    mVideoGroupBox->setChecked(mInitialSettings.getVideoEnabled() &&
                               !noVideoCodecs);

    AVCodec *currentAudioCodec = mInitialSettings.getAudioCodec();
    if(currentAudioCodec == NULL) {
        mAudioCodecsComboBox->setCurrentIndex(0);
    } else {
        QString currentCodecName = QString(currentAudioCodec->long_name);
        mAudioCodecsComboBox->setCurrentText(currentCodecName);
    }

    AVSampleFormat currentSampleFormat = mInitialSettings.getAudioSampleFormat();
    if(currentSampleFormat == AV_SAMPLE_FMT_NONE) {
        mSampleFormatsComboBox->setCurrentIndex(0);
    } else {
        QString currentFormatName = sampleFormatsNames.at(currentSampleFormat);
        mSampleFormatsComboBox->setCurrentText(currentFormatName);
    }

    int currentSampleRate = mInitialSettings.getAudioSampleRate();
    if(currentSampleRate <= 0) {
        int i48000Id = mSampleRateComboBox->findText("48000 Hz");
        if(i48000Id == -1) {
            int i44100Id = mSampleRateComboBox->findText("44100 Hz");
            if(i44100Id == -1) {
                int lastId = mSampleRateComboBox->count() - 1;
                mSampleRateComboBox->setCurrentIndex(lastId);
            } else {
                mSampleRateComboBox->setCurrentIndex(i44100Id);
            }
        } else {
            mSampleRateComboBox->setCurrentIndex(i48000Id);
        }
    } else {
        QString currentSampleRateStr = QString::number(currentSampleRate) + " Hz";
        mSampleRateComboBox->setCurrentText(currentSampleRateStr);
    }

    int currentAudioBitrate = mInitialSettings.getAudioBitrate();
    if(currentAudioBitrate <= 0) {
        int i320Id = mAudioBitrateComboBox->findText("320 kbps");
        if(i320Id == -1) {
            int lastId = mAudioBitrateComboBox->count() - 1;
            mAudioBitrateComboBox->setCurrentIndex(lastId);
        } else {
            mAudioBitrateComboBox->setCurrentIndex(i320Id);
        }
    } else {
        QString currentAudioBitrateStr = QString::number(currentAudioBitrate/1000) + " kbps";
        mAudioBitrateComboBox->setCurrentText(currentAudioBitrateStr);
    }

    uint64_t currentChannelsLayout = mInitialSettings.getAudioChannelsLayout();
    if(currentChannelsLayout == 0) {
        mAudioChannelLayoutsComboBox->setCurrentIndex(0);
    } else {
        QString currentChannelsLayoutStr =
                getChannelLayoutName(currentChannelsLayout);
        if(currentChannelsLayoutStr == "Invalid") {
            mAudioChannelLayoutsComboBox->setCurrentIndex(0);
        } else {
            mAudioBitrateComboBox->setCurrentText(currentChannelsLayoutStr);
        }
    }

    bool noAudioCodecs = mAudioCodecsComboBox->count() == 0;
    mAudioGroupBox->setChecked(mInitialSettings.getAudioEnabled() &&
                               !noAudioCodecs);
}
