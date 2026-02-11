#include "ui/main_window.h"

#include "io/bms_parser.h"
#include "ui/chart_editor_widget.h"
#include "ui/dialogs/compare_method_dialog.h"
#include "ui/dialogs/diag_find_dialog.h"
#include "ui/dialogs/import_sm_dialog.h"
#include "ui/dialogs/op_general_dialog.h"
#include "ui/dialogs/op_player_dialog.h"
#include "ui/dialogs/op_visual_dialog.h"
#include "ui/resource_catalog.h"

#include <QAction>
#include <QActionGroup>
#include <QAudioBuffer>
#include <QAudioDecoder>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QImageReader>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QInputDialog>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QScrollArea>
#include <QScrollBar>
#include <QSizePolicy>
#include <QShortcut>
#include <QSpinBox>
#include <QSplitter>
#include <QStatusBar>
#include <QSettings>
#include <QSignalBlocker>
#include <QDoubleSpinBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QEventLoop>
#include <QTimer>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>
#include <QCloseEvent>

#include <algorithm>
#include <cmath>
#include <memory>

namespace ibmsc {

namespace {
QWidget* makeHeaderTab(MainWindow* self,
                       QLineEdit*& title,
                       QLineEdit*& artist,
                       QLineEdit*& genre,
                       QLineEdit*& subtitle,
                       QLineEdit*& subartist,
                       QLineEdit*& playLevel,
                       QLineEdit*& total,
                       QComboBox*& player,
                       QLineEdit*& stageFile,
                       QLineEdit*& banner,
                       QLineEdit*& backBmp,
                       QSpinBox*& bpm) {
    QWidget* tab = new QWidget(self);
    auto* layout = new QFormLayout(tab);

    title = new QLineEdit(tab);
    artist = new QLineEdit(tab);
    genre = new QLineEdit(tab);
    subtitle = new QLineEdit(tab);
    subartist = new QLineEdit(tab);
    playLevel = new QLineEdit(tab);
    total = new QLineEdit(tab);
    player = new QComboBox(tab);
    player->addItem("1 - Single Play", 1);
    player->addItem("2 - Couple Play", 2);
    player->addItem("3 - Double Play / PMS", 3);
    stageFile = new QLineEdit(tab);
    banner = new QLineEdit(tab);
    backBmp = new QLineEdit(tab);
    bpm = new QSpinBox(tab);
    bpm->setRange(1, 1000000);
    bpm->setMaximumWidth(140);

    layout->setContentsMargins(8, 6, 8, 6);
    layout->setHorizontalSpacing(10);
    layout->setVerticalSpacing(6);
    layout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    auto tighten = [](QLineEdit* e) {
        e->setMinimumWidth(180);
        e->setMaximumWidth(300);
    };
    tighten(title);
    tighten(artist);
    tighten(genre);
    tighten(subtitle);
    tighten(subartist);
    tighten(playLevel);
    tighten(total);
    player->setMinimumWidth(180);
    player->setMaximumWidth(300);
    tighten(stageFile);
    tighten(banner);
    tighten(backBmp);

    layout->addRow("Title", title);
    layout->addRow("Artist", artist);
    layout->addRow("Genre", genre);
    layout->addRow("SubTitle", subtitle);
    layout->addRow("SubArtist", subartist);
    layout->addRow("PlayLevel", playLevel);
    layout->addRow("TOTAL", total);
    layout->addRow("Player", player);
    layout->addRow("StageFile", stageFile);
    layout->addRow("Banner", banner);
    layout->addRow("BackBMP", backBmp);
    layout->addRow("BPM", bpm);

    return tab;
}

QString c10To36(int value) {
    static const char* digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (value <= 0) {
        return "00";
    }
    QString out;
    int v = value;
    while (v > 0) {
        out.prepend(QChar(digits[v % 36]));
        v /= 36;
    }
    while (out.size() < 2) {
        out.prepend('0');
    }
    return out.right(2);
}

QString encodeBase36_00_ZZ(int value) {
    return c10To36(std::clamp(value, 0, 1295));
}

bool decodeBase36_00_ZZ(const QString& raw, int* out) {
    if (!out) {
        return false;
    }
    const QString s = raw.trimmed().toUpper();
    if (s.isEmpty() || s.size() > 2) {
        return false;
    }
    int v = 0;
    for (QChar c : s) {
        if (c >= '0' && c <= '9') {
            v = v * 36 + (c.unicode() - '0');
        } else if (c >= 'A' && c <= 'Z') {
            v = v * 36 + (c.unicode() - 'A' + 10);
        } else {
            return false;
        }
    }
    if (v < 0 || v > 1295) {
        return false;
    }
    *out = v;
    return true;
}

int c36To10Fast(const QString& raw) {
    const QString s = raw.trimmed().toUpper();
    int out = 0;
    for (QChar c : s) {
        int v = -1;
        if (c >= '0' && c <= '9') v = c.unicode() - '0';
        else if (c >= 'A' && c <= 'Z') v = c.unicode() - 'A' + 10;
        if (v < 0) return 0;
        out = out * 36 + v;
    }
    return out;
}

QString normalizedChannel(const BmsNote& n) {
    QString ch = n.channelId.trimmed().toUpper();
    if (ch.isEmpty()) ch = BmsDocument::columnToDefaultChannel(n.columnIndex);
    if (ch.size() == 1) ch.prepend('0');
    return ch;
}

bool isLongChannel(const QString& ch) {
    const int v = c36To10Fast(ch);
    return v >= 50 && v < 90;
}

bool isHiddenChannel(const QString& ch) {
    const int v = c36To10Fast(ch);
    return (v >= 30 && v < 50) || (v >= 70 && v < 90);
}

int gcdInt(int a, int b) {
    a = std::abs(a);
    b = std::abs(b);
    while (b != 0) {
        const int t = a % b;
        a = b;
        b = t;
    }
    return a;
}

QVector<float> decodeAudioFileMono(const QString& absPath, QString* errorOut) {
    QVector<float> out;
    QAudioDecoder decoder;
    decoder.setSource(QUrl::fromLocalFile(absPath));

    bool failed = false;
    QString errText;
    QEventLoop loop;
    QTimer timeout;
    timeout.setSingleShot(true);
    timeout.setInterval(6000);

    QObject::connect(&timeout, &QTimer::timeout, &loop, [&]() {
        failed = true;
        errText = "Waveform decode timed out.";
        loop.quit();
    });
    QObject::connect(&decoder, QOverload<QAudioDecoder::Error>::of(&QAudioDecoder::error), &loop, [&](QAudioDecoder::Error err) {
        if (err == QAudioDecoder::NoError) {
            return;
        }
        failed = true;
        errText = decoder.errorString();
        loop.quit();
    });
    QObject::connect(&decoder, &QAudioDecoder::bufferReady, &loop, [&]() {
        const QAudioBuffer buffer = decoder.read();
        if (!buffer.isValid()) {
            return;
        }
        const QAudioFormat fmt = buffer.format();
        const int channels = std::max(1, fmt.channelCount());
        const int frames = buffer.frameCount();
        if (frames <= 0) {
            return;
        }

        if (fmt.sampleFormat() == QAudioFormat::Int16) {
            const qint16* data = buffer.constData<qint16>();
            for (int i = 0; i < frames; ++i) {
                float sum = 0.0f;
                for (int c = 0; c < channels; ++c) {
                    sum += static_cast<float>(data[i * channels + c]) / 32768.0f;
                }
                out.push_back(sum / static_cast<float>(channels));
            }
        } else if (fmt.sampleFormat() == QAudioFormat::Int32) {
            const qint32* data = buffer.constData<qint32>();
            for (int i = 0; i < frames; ++i) {
                float sum = 0.0f;
                for (int c = 0; c < channels; ++c) {
                    sum += static_cast<float>(data[i * channels + c] / 2147483648.0);
                }
                out.push_back(sum / static_cast<float>(channels));
            }
        } else if (fmt.sampleFormat() == QAudioFormat::Float) {
            const float* data = buffer.constData<float>();
            for (int i = 0; i < frames; ++i) {
                float sum = 0.0f;
                for (int c = 0; c < channels; ++c) {
                    sum += data[i * channels + c];
                }
                out.push_back(sum / static_cast<float>(channels));
            }
        }
    });
    QObject::connect(&decoder, &QAudioDecoder::finished, &loop, [&]() { loop.quit(); });

    timeout.start();
    decoder.start();
    loop.exec();
    timeout.stop();

    if (failed || out.isEmpty()) {
        if (errorOut) {
            *errorOut = errText.isEmpty() ? "Unsupported or empty audio format." : errText;
        }
        return {};
    }

    const int maxPoints = 16000;
    if (out.size() > maxPoints) {
        QVector<float> compact;
        compact.reserve(maxPoints);
        const int outSize = static_cast<int>(out.size());
        const double step = static_cast<double>(out.size()) / static_cast<double>(maxPoints);
        for (int i = 0; i < maxPoints; ++i) {
            const int idx = std::min(static_cast<int>(std::floor(i * step)), outSize - 1);
            compact.push_back(out[idx]);
        }
        out.swap(compact);
    }

    float maxAbs = 0.0001f;
    for (float v : out) {
        maxAbs = std::max(maxAbs, std::abs(v));
    }
    for (float& v : out) {
        v = std::clamp(v / maxAbs, -1.0f, 1.0f);
    }

    return out;
}

QStringList candidateThemeDirs() {
    QStringList dirs;
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString cwd = QDir::currentPath();

    const QStringList seeds = {
        appDir,
        QDir(appDir).absoluteFilePath(".."),
        QDir(appDir).absoluteFilePath("../.."),
        QDir(appDir).absoluteFilePath("../../.."),
        cwd,
        QDir(cwd).absoluteFilePath(".."),
        QDir(cwd).absoluteFilePath("../.."),
        QDir(cwd).absoluteFilePath("../../..")
    };

    for (const QString& seed : seeds) {
        const QString path = QDir(seed).absoluteFilePath("iBMSC/misc/Data");
        if (QDir(path).exists()) {
            dirs << QDir(path).absolutePath();
        }
    }

    // Fallback for this workspace layout.
    const QString wsFallback = "/Users/guanhao/VSCodeAlgorithmTrain/CPP/ibmsc/iBMSCAgent/iBMSC/misc/Data";
    if (QDir(wsFallback).exists()) {
        dirs << QDir(wsFallback).absolutePath();
    }

    dirs.removeDuplicates();
    return dirs;
}
} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    m_theme = ThemeLoader::defaultTheme();
    setupUi();
    buildMenus();
    setupEditorShortcuts();
    loadThemes();
    applyTheme(m_theme);
    syncHeaderToUi();
    refreshResourceTables();
    refreshBeatTable();
    m_expansionEdit->setPlainText(m_doc.expansion);
    applyDocumentToEditors();
    setMode(ChartEditorWidget::EditMode::Write);
    loadPersistentSettings();
    updateWindowTitle();
    resize(1780, 1000);

    m_autoSaveTimer = new QTimer(this);
    m_autoSaveTimer->setInterval(120000);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &MainWindow::onAutoSaveTick);
    m_autoSaveTimer->start();

    m_errorCheckTimer = new QTimer(this);
    m_errorCheckTimer->setSingleShot(true);
    m_errorCheckTimer->setInterval(90);
    connect(m_errorCheckTimer, &QTimer::timeout, this, [this]() {
        m_errorCheckQueued = false;
        runBasicErrorCheck();
    });
}

void MainWindow::setupUi() {
    auto* rootSplitter = new QSplitter(this);
    setCentralWidget(rootSplitter);

    auto* editorsSplitter = new QSplitter(Qt::Horizontal, rootSplitter);
    editorsSplitter->setChildrenCollapsible(false);

    for (int i = 0; i < 3; ++i) {
        m_editors[i] = new ChartEditorWidget(this);
        m_editors[i]->setDocument(&m_doc);
        m_editors[i]->setTheme(&m_theme);

        m_editorScrolls[i] = new QScrollArea(this);
        m_editorScrolls[i]->setWidgetResizable(false);
        m_editorScrolls[i]->setWidget(m_editors[i]);
        m_editorScrolls[i]->setFrameShape(QFrame::Box);
        m_editorScrolls[i]->setFrameShadow(QFrame::Plain);
        m_editorScrolls[i]->setLineWidth(1);
        editorsSplitter->addWidget(m_editorScrolls[i]);

        connect(m_editors[i], &ChartEditorWidget::documentEdited, this, &MainWindow::onChartEdited);
        connect(m_editors[i], &ChartEditorWidget::focusEntered, this, &MainWindow::onEditorFocus);
        connect(m_editors[i], &ChartEditorWidget::selectionChanged, this, &MainWindow::onEditorSelectionChanged);
        connect(m_editors[i], &ChartEditorWidget::timeSelectionChanged, this, &MainWindow::onEditorTimeSelectionChanged);
        connect(m_editors[i], &ChartEditorWidget::aboutToEdit, this, &MainWindow::onEditorAboutToEdit);
        connect(m_editors[i], &ChartEditorWidget::notePicked, this, [this](const BmsNote& note) {
            const int v = std::clamp(note.value / 10000, 0, 1295);
            m_lastValidNoteValue = std::max(1, v);
            if (m_noteValueEdit) {
                m_noteValueEdit->setText(encodeBase36_00_ZZ(v));
            }
            if (!m_previewOnClick) return;
            const int idx = note.value / 10000;
            if (idx <= 0 || idx >= m_doc.wavTable.size()) return;
            const QString wav = m_doc.wavTable[idx];
            if (wav.trimmed().isEmpty()) return;
            m_audio.play(wav, chartBaseDir());
        });
    }

    m_activeEditor = m_editors[1];
    syncEditorScrollBars();
    updateActiveEditorStyles();
    m_editorScrolls[0]->setVisible(false);
    m_editorScrolls[2]->setVisible(false);

    rootSplitter->addWidget(editorsSplitter);

    QWidget* rightPanel = new QWidget(this);
    rightPanel->setObjectName("RightPanel");
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(10, 10, 10, 10);
    rightLayout->setSpacing(8);

    auto* topBar = new QGridLayout();
    topBar->setHorizontalSpacing(8);
    topBar->setVerticalSpacing(6);
    m_themeCombo = new QComboBox(rightPanel);
    m_modeCombo = new QComboBox(rightPanel);
    m_modeCombo->addItems({"Write", "Select", "Time Select"});
    auto* skinLabel = new QLabel("Skin", rightPanel);
    auto* modeLabel = new QLabel("Mode", rightPanel);
    auto* channelLabel = new QLabel("Channel", rightPanel);
    auto* valueLabel = new QLabel("Value", rightPanel);

    m_noteChannelCombo = new QComboBox(rightPanel);
    m_noteValueEdit = new QLineEdit(rightPanel);
    m_noteValueEdit->setMaxLength(2);
    m_noteValueEdit->setFixedWidth(48);
    m_noteValueEdit->setAlignment(Qt::AlignCenter);
    m_noteValueEdit->setText(encodeBase36_00_ZZ(m_lastValidNoteValue));
    m_noteValueEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9A-Za-z]{1,2}$"), m_noteValueEdit));

    m_snapSpin = new QSpinBox(rightPanel);
    m_snapSpin->setRange(1, 192);
    m_snapSpin->setValue(16);
    m_snapCheck = new QCheckBox("Snap", rightPanel);
    m_snapCheck->setChecked(true);
    m_ntInputCheck = new QCheckBox("NT", rightPanel);
    m_ntInputCheck->setChecked(false);
    m_longNoteCheck = new QCheckBox("Long", rightPanel);
    m_hiddenCheck = new QCheckBox("Hidden", rightPanel);
    m_landmineCheck = new QCheckBox("Landmine", rightPanel);

    topBar->addWidget(skinLabel, 0, 0);
    topBar->addWidget(m_themeCombo, 0, 1);
    topBar->addWidget(modeLabel, 0, 2);
    topBar->addWidget(m_modeCombo, 0, 3);
    topBar->addWidget(channelLabel, 0, 4);
    topBar->addWidget(m_noteChannelCombo, 0, 5);
    topBar->addWidget(valueLabel, 0, 6);
    topBar->addWidget(m_noteValueEdit, 0, 7);
    topBar->addWidget(m_snapCheck, 1, 0);
    topBar->addWidget(m_snapSpin, 1, 1);
    topBar->addWidget(m_ntInputCheck, 1, 2);
    topBar->addWidget(m_longNoteCheck, 1, 3);
    topBar->addWidget(m_hiddenCheck, 1, 4);
    topBar->addWidget(m_landmineCheck, 1, 5);
    topBar->setColumnStretch(1, 1);
    topBar->setColumnStretch(3, 1);
    topBar->setColumnStretch(5, 1);

    rightLayout->addLayout(topBar);

    auto* poScroll = new QScrollArea(rightPanel);
    poScroll->setWidgetResizable(true);
    poScroll->setFrameShape(QFrame::NoFrame);
    auto* poRoot = new QWidget(poScroll);
    poRoot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    auto* poLayout = new QVBoxLayout(poRoot);
    poLayout->setContentsMargins(0, 0, 0, 0);
    poLayout->setSpacing(6);
    poLayout->setSizeConstraint(QLayout::SetMinimumSize);
    poScroll->setWidget(poRoot);
    rightLayout->addWidget(poScroll, 1);

    auto addPoSection = [this, poLayout](const QString& key, const QString& title, QWidget* body, bool withExpander) {
        auto* section = new QFrame();
        section->setObjectName("PoSection");
        section->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        auto* sectionLayout = new QVBoxLayout(section);
        sectionLayout->setContentsMargins(6, 6, 6, 6);
        sectionLayout->setSpacing(4);

        auto* switchBox = new QCheckBox(title, section);
        switchBox->setObjectName(key + "_switch");
        switchBox->setProperty("poRole", "switch");
        switchBox->setChecked(true);
        switchBox->setTristate(false);
        sectionLayout->addWidget(switchBox);

        auto* inner = new QWidget(section);
        auto* innerLayout = new QVBoxLayout(inner);
        innerLayout->setContentsMargins(6, 2, 6, 4);
        innerLayout->setSpacing(4);

        QCheckBox* expander = nullptr;
        QWidget* detail = body;
        if (withExpander) {
            expander = new QCheckBox("Expand...", inner);
            expander->setObjectName(key + "_expander");
            expander->setProperty("poRole", "expander");
            expander->setChecked(true);
            innerLayout->addWidget(expander);
        }
        body->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        innerLayout->addWidget(body);
        sectionLayout->addWidget(inner);

        int sectionMin = 0;
        if (key == "POWAV") {
            sectionMin = 420;
        } else if (key == "POBMP") {
            sectionMin = 560;
        }
        m_poPanelManager.registerSection({key, section, inner, detail, switchBox, expander, sectionMin});
        connect(switchBox, &QCheckBox::toggled, this, [this, key](bool checked) {
            m_poPanelManager.applySwitchState(key, checked);
        });
        if (expander) {
            connect(expander, &QCheckBox::toggled, this, [this, key](bool expanded) {
                m_poPanelManager.applyExpandState(key, expanded);
            });
        }
        poLayout->addWidget(section);
    };

    QWidget* headerTab = makeHeaderTab(this,
                                       m_titleEdit,
                                       m_artistEdit,
                                       m_genreEdit,
                                       m_subtitleEdit,
                                       m_subartistEdit,
                                       m_playLevelEdit,
                                       m_totalEdit,
                                       m_playerCombo,
                                       m_stageFileEdit,
                                       m_bannerEdit,
                                       m_backBmpEdit,
                                       m_bpmEdit);
    addPoSection("POHeader", "Header", headerTab, true);

    QWidget* gridTab = new QWidget(this);
    auto* gridLayout = new QVBoxLayout(gridTab);
    m_cgShow = new QCheckBox("Show notes/grid layers", gridTab);
    m_cgShow->setChecked(true);
    m_cgShowBG = new QCheckBox("Show column backgrounds", gridTab);
    m_cgShowBG->setChecked(true);
    m_cgShowV = new QCheckBox("Show vertical lines", gridTab);
    m_cgShowV->setChecked(true);
    m_cgBpm = new QCheckBox("Show BPM channels", gridTab);
    m_cgBpm->setChecked(true);
    m_cgStop = new QCheckBox("Show STOP channels", gridTab);
    m_cgStop->setChecked(true);
    m_cgScroll = new QCheckBox("Show SCROLL channels", gridTab);
    m_cgScroll->setChecked(true);
    m_cgBlp = new QCheckBox("Show BLP channels", gridTab);
    m_cgBlp->setChecked(true);
    auto* bColsRow = new QHBoxLayout();
    bColsRow->addWidget(new QLabel("B Columns", gridTab));
    m_bColumnsSpin = new QSpinBox(gridTab);
    m_bColumnsSpin->setRange(1, 999);
    m_bColumnsSpin->setValue(m_bColumnCount);
    bColsRow->addWidget(m_bColumnsSpin);
    bColsRow->addStretch(1);
    gridLayout->addWidget(m_cgShow);
    gridLayout->addWidget(m_cgShowBG);
    gridLayout->addWidget(m_cgShowV);
    gridLayout->addWidget(m_cgBpm);
    gridLayout->addWidget(m_cgStop);
    gridLayout->addWidget(m_cgScroll);
    gridLayout->addWidget(m_cgBlp);
    gridLayout->addLayout(bColsRow);
    gridLayout->addStretch(1);
    addPoSection("POGrid", "Grid", gridTab, true);

    QWidget* waveformTab = new QWidget(this);
    auto* waveLayout = new QVBoxLayout(waveformTab);
    auto* waveTools = new QHBoxLayout();
    auto* bwLoad = new QPushButton("Load", waveformTab);
    auto* bwClear = new QPushButton("Clear", waveformTab);
    m_waveLockCheck = new QCheckBox("Lock", waveformTab);
    waveTools->addWidget(bwLoad);
    waveTools->addWidget(bwClear);
    waveTools->addWidget(m_waveLockCheck);
    waveLayout->addLayout(waveTools);
    m_waveWidthSpin = new QSpinBox(waveformTab);
    m_waveWidthSpin->setRange(1, 4096);
    m_waveWidthSpin->setValue(512);
    m_waveOffsetSpin = new QSpinBox(waveformTab);
    m_waveOffsetSpin->setRange(-4096, 4096);
    m_waveOffsetSpin->setValue(0);
    m_waveAlphaSpin = new QSpinBox(waveformTab);
    m_waveAlphaSpin->setRange(0, 255);
    m_waveAlphaSpin->setValue(110);
    m_wavePrecisionSpin = new QSpinBox(waveformTab);
    m_wavePrecisionSpin->setRange(1, 64);
    m_wavePrecisionSpin->setValue(8);
    auto* waveForm = new QFormLayout();
    waveForm->addRow("Width", m_waveWidthSpin);
    waveForm->addRow("Offset", m_waveOffsetSpin);
    waveForm->addRow("Transparency", m_waveAlphaSpin);
    waveForm->addRow("Precision", m_wavePrecisionSpin);
    waveLayout->addLayout(waveForm);
    waveLayout->addStretch(1);
    connect(bwLoad, &QPushButton::clicked, this, [this]() { loadWaveformOverlayFromWavSelection(); });
    connect(bwClear, &QPushButton::clicked, this, [this]() { clearWaveformOverlay(); });
    connect(m_waveLockCheck, &QCheckBox::toggled, this, [this](bool locked) {
        applyWaveformOptionsToEditors();
        statusBar()->showMessage(QString("WaveForm lock: %1").arg(locked ? "ON" : "OFF"), 1500);
    });
    connect(m_waveWidthSpin, &QSpinBox::valueChanged, this, [this](int) { applyWaveformOptionsToEditors(); });
    connect(m_waveOffsetSpin, &QSpinBox::valueChanged, this, [this](int) { applyWaveformOptionsToEditors(); });
    connect(m_waveAlphaSpin, &QSpinBox::valueChanged, this, [this](int) { applyWaveformOptionsToEditors(); });
    connect(m_wavePrecisionSpin, &QSpinBox::valueChanged, this, [this](int) { applyWaveformOptionsToEditors(); });
    addPoSection("POWaveForm", "WaveForm", waveformTab, true);

    QWidget* wavTab = new QWidget(this);
    auto* wavLayout = new QVBoxLayout(wavTab);
    m_wavTableWidget = new QTableWidget(wavTab);
    m_wavTableWidget->setColumnCount(2);
    m_wavTableWidget->setHorizontalHeaderLabels({"Code", "Path (.wav/.ogg)"});
    m_wavTableWidget->verticalHeader()->setVisible(false);
    m_wavTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_wavTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_wavTableWidget->setMinimumHeight(260);
    m_wavTableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wavLayout->addWidget(m_wavTableWidget);

    auto* wavButtons = new QHBoxLayout();
    auto* addWav = new QPushButton("Add/Update", wavTab);
    auto* removeWav = new QPushButton("Remove", wavTab);
    auto* playWav = new QPushButton("Play", wavTab);
    auto* stopWav = new QPushButton("Stop", wavTab);
    wavButtons->addWidget(addWav);
    wavButtons->addWidget(removeWav);
    wavButtons->addWidget(playWav);
    wavButtons->addWidget(stopWav);
    wavLayout->addLayout(wavButtons);
    wavLayout->setStretch(0, 1);
    wavTab->setMinimumHeight(360);
    addPoSection("POWAV", "WAV/OGG", wavTab, true);

    QWidget* bmpTab = new QWidget(this);
    auto* bmpLayout = new QVBoxLayout(bmpTab);
    m_bmpTableWidget = new QTableWidget(bmpTab);
    m_bmpTableWidget->setColumnCount(2);
    m_bmpTableWidget->setHorizontalHeaderLabels({"Code", "BGI Path"});
    m_bmpTableWidget->verticalHeader()->setVisible(false);
    m_bmpTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_bmpTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_bmpTableWidget->setMinimumHeight(240);
    m_bmpTableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bmpLayout->addWidget(m_bmpTableWidget);

    m_bgiPreview = new QLabel(bmpTab);
    m_bgiPreview->setMinimumHeight(220);
    m_bgiPreview->setAlignment(Qt::AlignCenter);
    m_bgiPreview->setText("BGI Preview");
    bmpLayout->addWidget(m_bgiPreview);

    auto* bmpButtons = new QHBoxLayout();
    auto* addBmp = new QPushButton("Add/Update", bmpTab);
    auto* removeBmp = new QPushButton("Remove", bmpTab);
    bmpButtons->addWidget(addBmp);
    bmpButtons->addWidget(removeBmp);
    bmpLayout->addLayout(bmpButtons);
    bmpLayout->setStretch(0, 1);
    bmpTab->setMinimumHeight(520);
    addPoSection("POBMP", "BGI/BMP", bmpTab, false);

    QWidget* beatTab = new QWidget(this);
    auto* beatLayout = new QVBoxLayout(beatTab);
    m_beatTableWidget = new QTableWidget(beatTab);
    m_beatTableWidget->setColumnCount(2);
    m_beatTableWidget->setHorizontalHeaderLabels({"Measure", "Ratio"});
    m_beatTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_beatTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    beatLayout->addWidget(m_beatTableWidget);
    addPoSection("POBeat", "Beat", beatTab, true);

    QWidget* expansionTab = new QWidget(this);
    auto* expansionLayout = new QVBoxLayout(expansionTab);
    m_expansionEdit = new QTextEdit(expansionTab);
    m_expansionEdit->setPlaceholderText("Raw expansion lines (#IF/#ENDIF/#SWITCH/unknown commands)...");
    expansionLayout->addWidget(m_expansionEdit);
    addPoSection("POExpansion", "Expansion", expansionTab, false);
    poLayout->addStretch(1);

    rootSplitter->addWidget(rightPanel);
    rootSplitter->setSizes({1400, 420});
    editorsSplitter->setSizes({0, 1200, 0});

    rightPanel->setStyleSheet(
        "QWidget#RightPanel { background: #2f3136; }"
        "QWidget#RightPanel QLabel { color: #e6e6e6; }"
        "QWidget#RightPanel QLineEdit, QWidget#RightPanel QSpinBox, QWidget#RightPanel QComboBox, QWidget#RightPanel QTextEdit, QWidget#RightPanel QTableWidget {"
        "  background: #1f2124; color: #efefef; border: 1px solid #4b4f57; border-radius: 4px; padding: 2px 4px; }"
        "QWidget#RightPanel QHeaderView::section { background: #3a3d44; color: #f0f0f0; border: 0; padding: 4px; }"
        "QFrame#PoSection { background: #363941; border: 1px solid #4a4e57; border-radius: 6px; }"
        "QCheckBox[poRole=\"switch\"] { font-weight: 700; color: #f2f4f7; padding: 2px 4px; }"
        "QCheckBox[poRole=\"expander\"] { color: #c8ccd4; padding: 1px 4px; }"
        "QWidget#RightPanel QPushButton { background: #4a5c8a; color: #f8f8f8; border: 0; border-radius: 5px; padding: 4px 8px; }"
        "QWidget#RightPanel QPushButton:hover { background: #5870a8; }"
    );

    connect(m_themeCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onThemeChanged);
    connect(m_modeCombo, &QComboBox::currentIndexChanged, this, [this](int idx) {
        if (idx == 0) onModeWrite();
        else if (idx == 1) onModeSelect();
        else onModeTimeSelect();
    });
    connect(m_noteChannelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        const QString text = m_noteChannelCombo->itemData(index).toString();
        if (text.isEmpty()) {
            return;
        }
        for (const auto& editor : m_editors) {
            if (!editor) continue;
            editor->setDefaultChannel(text);
        }
    });
    connect(m_noteValueEdit, &QLineEdit::editingFinished, this, [this]() {
        int decoded = 0;
        if (!decodeBase36_00_ZZ(m_noteValueEdit->text(), &decoded)) {
            m_noteValueEdit->setText(encodeBase36_00_ZZ(m_lastValidNoteValue));
            return;
        }
        m_lastValidNoteValue = std::max(1, decoded);
        m_noteValueEdit->setText(encodeBase36_00_ZZ(decoded));
        for (const auto& editor : m_editors) {
            if (!editor) continue;
            editor->setDefaultValue(std::max(1, decoded) * 10000);
        }
    });
    connect(m_noteValueEdit, &QLineEdit::textEdited, this, [this](const QString& text) {
        const QString normalized = text.trimmed().toUpper();
        if (normalized == text) {
            return;
        }
        m_noteValueEdit->blockSignals(true);
        m_noteValueEdit->setText(normalized);
        m_noteValueEdit->blockSignals(false);
    });
    connect(m_snapSpin, &QSpinBox::valueChanged, this, [this](int value) {
        for (const auto& editor : m_editors) {
            if (!editor) continue;
            editor->setSnapDivision(value);
        }
    });
    connect(m_snapCheck, &QCheckBox::toggled, this, [this](bool checked) {
        for (const auto& editor : m_editors) {
            if (!editor) continue;
            editor->setSnapEnabled(checked);
        }
    });
    connect(m_ntInputCheck, &QCheckBox::toggled, this, [this](bool checked) {
        for (const auto& editor : m_editors) {
            if (!editor) continue;
            editor->setNtInput(checked);
        }
    });
    auto applyDisplayOptions = [this]() {
        ChartEditorWidget::DisplayOptions opt;
        opt.showNotesGridLayers = m_cgShow ? m_cgShow->isChecked() : true;
        opt.showColumnBackgrounds = m_cgShowBG ? m_cgShowBG->isChecked() : true;
        opt.showVerticalLines = m_cgShowV ? m_cgShowV->isChecked() : true;
        opt.showBpmChannels = m_cgBpm ? m_cgBpm->isChecked() : true;
        opt.showStopChannels = m_cgStop ? m_cgStop->isChecked() : true;
        opt.showScrollChannels = m_cgScroll ? m_cgScroll->isChecked() : true;
        opt.showBlpChannels = m_cgBlp ? m_cgBlp->isChecked() : true;
        for (const auto& editor : m_editors) {
            if (!editor) continue;
            editor->setDisplayOptions(opt);
        }
    };
    auto onGridOptionChanged = [this, applyDisplayOptions]() {
        applyDisplayOptions();
        applyRuntimeColumnVisibility();
        for (const auto& editor : m_editors) {
            if (!editor) continue;
            editor->setTheme(&m_theme);
            editor->updateGeometry();
            editor->update();
        }
        rebuildNoteChannelCombo();
    };
    connect(m_cgShow, &QCheckBox::toggled, this, [onGridOptionChanged](bool) { onGridOptionChanged(); });
    connect(m_cgShowBG, &QCheckBox::toggled, this, [onGridOptionChanged](bool) { onGridOptionChanged(); });
    connect(m_cgShowV, &QCheckBox::toggled, this, [onGridOptionChanged](bool) { onGridOptionChanged(); });
    connect(m_cgBpm, &QCheckBox::toggled, this, [onGridOptionChanged](bool) { onGridOptionChanged(); });
    connect(m_cgStop, &QCheckBox::toggled, this, [onGridOptionChanged](bool) { onGridOptionChanged(); });
    connect(m_cgScroll, &QCheckBox::toggled, this, [onGridOptionChanged](bool) { onGridOptionChanged(); });
    connect(m_cgBlp, &QCheckBox::toggled, this, [onGridOptionChanged](bool) { onGridOptionChanged(); });
    connect(m_bColumnsSpin, &QSpinBox::valueChanged, this, [this, onGridOptionChanged](int value) {
        m_bColumnCount = std::clamp(value, 1, 999);
        onGridOptionChanged();
    });
    applyDisplayOptions();
    applyWaveformOptionsToEditors();
    auto syncWriteMods = [this]() {
        for (const auto& editor : m_editors) {
            if (!editor) continue;
            editor->setWriteModifiers(m_longNoteCheck->isChecked(), m_hiddenCheck->isChecked(), m_landmineCheck->isChecked());
        }
    };
    connect(m_longNoteCheck, &QCheckBox::toggled, this, [syncWriteMods](bool) { syncWriteMods(); });
    connect(m_hiddenCheck, &QCheckBox::toggled, this, [syncWriteMods](bool) { syncWriteMods(); });
    connect(m_landmineCheck, &QCheckBox::toggled, this, [syncWriteMods](bool) { syncWriteMods(); });
    syncWriteMods();
    for (const auto& editor : m_editors) {
            if (!editor) continue;
        editor->setSnapEnabled(m_snapCheck->isChecked());
        editor->setNtInput(m_ntInputCheck->isChecked());
    }

    auto connectHeader = [this](QLineEdit* edit) {
        connect(edit, &QLineEdit::editingFinished, this, &MainWindow::onHeaderEdited);
    };
    connectHeader(m_titleEdit);
    connectHeader(m_artistEdit);
    connectHeader(m_genreEdit);
    connectHeader(m_subtitleEdit);
    connectHeader(m_subartistEdit);
    connectHeader(m_playLevelEdit);
    connectHeader(m_totalEdit);
    connect(m_playerCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) { onHeaderEdited(); });
    connectHeader(m_stageFileEdit);
    connectHeader(m_bannerEdit);
    connectHeader(m_backBmpEdit);
    connect(m_bpmEdit, &QSpinBox::valueChanged, this, [this](int) { onHeaderEdited(); });

    connect(addWav, &QPushButton::clicked, this, [this]() {
        pushUndoSnapshot();
        const QString path = QFileDialog::getOpenFileName(this, "Select WAV/OGG", chartBaseDir(), "Audio (*.wav *.ogg)");
        if (path.isEmpty()) return;
        const QString code = nextCodeForTable(m_doc.wavTable);
        const int row = m_wavTableWidget->rowCount();
        m_wavTableWidget->insertRow(row);
        m_wavTableWidget->setItem(row, 0, new QTableWidgetItem(code));
        m_wavTableWidget->setItem(row, 1, new QTableWidgetItem(QDir(chartBaseDir()).relativeFilePath(path)));
        onChartEdited();
    });
    connect(removeWav, &QPushButton::clicked, this, [this]() {
        const int row = m_wavTableWidget->currentRow();
        if (row >= 0) {
            pushUndoSnapshot();
            m_wavTableWidget->removeRow(row);
            onChartEdited();
        }
    });
    connect(playWav, &QPushButton::clicked, this, &MainWindow::onPlaySelectedAudio);
    connect(stopWav, &QPushButton::clicked, this, &MainWindow::onStopAudio);

    connect(addBmp, &QPushButton::clicked, this, [this]() {
        pushUndoSnapshot();
        const QString path = QFileDialog::getOpenFileName(this, "Select BGI", chartBaseDir(), "Images/Video (*.png *.jpg *.jpeg *.bmp *.gif *.mp4 *.webm *.avi)");
        if (path.isEmpty()) return;
        const QString code = nextCodeForTable(m_doc.bmpTable);
        const int row = m_bmpTableWidget->rowCount();
        m_bmpTableWidget->insertRow(row);
        m_bmpTableWidget->setItem(row, 0, new QTableWidgetItem(code));
        m_bmpTableWidget->setItem(row, 1, new QTableWidgetItem(QDir(chartBaseDir()).relativeFilePath(path)));
        onChartEdited();
    });
    connect(removeBmp, &QPushButton::clicked, this, [this]() {
        const int row = m_bmpTableWidget->currentRow();
        if (row >= 0) {
            pushUndoSnapshot();
            m_bmpTableWidget->removeRow(row);
            onChartEdited();
        }
    });

    connect(m_wavTableWidget, &QTableWidget::itemSelectionChanged, this, &MainWindow::onWavSelectionChanged);
    connect(m_wavTableWidget, &QTableWidget::cellChanged, this, [this](int, int) { onChartEdited(); });
    connect(m_bmpTableWidget, &QTableWidget::itemSelectionChanged, this, &MainWindow::onBmpSelectionChanged);
    connect(m_bmpTableWidget, &QTableWidget::cellChanged, this, [this](int, int) { onChartEdited(); });
    connect(m_beatTableWidget, &QTableWidget::cellChanged, this, [this](int, int) { onChartEdited(); });
    connect(m_expansionEdit, &QTextEdit::textChanged, this, &MainWindow::onChartEdited);

    m_statusSelection = new QLabel("Sel: 0", this);
    m_statusTimeSelection = new QLabel("TS: -", this);
    statusBar()->addPermanentWidget(m_statusSelection);
    statusBar()->addPermanentWidget(m_statusTimeSelection);
    statusBar()->showMessage("Ready");
}

void MainWindow::syncEditorScrollBars() {
    for (int i = 0; i < 3; ++i) {
        if (!m_editorScrolls[i]) continue;
        auto* srcV = m_editorScrolls[i]->verticalScrollBar();
        auto* srcH = m_editorScrolls[i]->horizontalScrollBar();

        connect(srcV, &QScrollBar::valueChanged, this, [this, i](int value) {
            if (m_blockEditorSync) return;
            m_blockEditorSync = true;
            for (int j = 0; j < 3; ++j) {
                if (i == j) continue;
                if (!m_editorScrolls[j]) continue;
                m_editorScrolls[j]->verticalScrollBar()->setValue(value);
            }
            m_blockEditorSync = false;
        });
        connect(srcH, &QScrollBar::valueChanged, this, [this, i](int value) {
            if (m_blockEditorSync) return;
            m_blockEditorSync = true;
            for (int j = 0; j < 3; ++j) {
                if (i == j) continue;
                if (!m_editorScrolls[j]) continue;
                m_editorScrolls[j]->horizontalScrollBar()->setValue(value);
            }
            m_blockEditorSync = false;
        });
    }
}

void MainWindow::setupEditorShortcuts() {
    auto* undoSc = new QShortcut(QKeySequence::Undo, this);
    auto* redoSc = new QShortcut(QKeySequence::Redo, this);
    auto* ntSc = new QShortcut(QKeySequence(Qt::Key_F8), this);
    connect(undoSc, &QShortcut::activated, this, &MainWindow::onUndo);
    connect(redoSc, &QShortcut::activated, this, &MainWindow::onRedo);
    connect(ntSc, &QShortcut::activated, this, [this]() {
        if (m_ntInputCheck) {
            m_ntInputCheck->setChecked(!m_ntInputCheck->isChecked());
        }
    });
}

void MainWindow::buildMenus() {
    auto icon = [](const char* vbName) { return ResourceCatalog::iconByVbName(vbName); };

    QMenu* fileMenu = menuBar()->addMenu("File");
    QAction* openAction = fileMenu->addAction(icon("x16Open"), "Open", this, &MainWindow::onOpen);
    openAction->setShortcut(QKeySequence::Open);
    QAction* saveAction = fileMenu->addAction(icon("x16Save"), "Save", this, &MainWindow::onSave);
    saveAction->setShortcut(QKeySequence::Save);
    QAction* saveAsAction = fileMenu->addAction(icon("x16SaveAs"), "Save As", this, &MainWindow::onSaveAs);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    fileMenu->addAction(icon("x16Import2"), "Import from .SM file", this, [this]() {
        ImportSmDialog dlg(this);
        dlg.exec();
    });
    m_recentMenu = fileMenu->addMenu("Open Recent");
    rebuildRecentMenu();
    fileMenu->addSeparator();
    fileMenu->addAction("Exit", this, &QWidget::close);

    QMenu* editMenu = menuBar()->addMenu("Edit");
    m_undoAction = editMenu->addAction(icon("x16Undo"), "Undo", this, &MainWindow::onUndo);
    m_undoAction->setShortcut(QKeySequence::Undo);
    m_redoAction = editMenu->addAction(icon("x16Redo"), "Redo", this, &MainWindow::onRedo);
    m_redoAction->setShortcut(QKeySequence::Redo);

    QMenu* modeMenu = menuBar()->addMenu("Mode");
    m_modeTimeSelectAction = modeMenu->addAction(icon("x16TimeSelection"), "Time Select", this, &MainWindow::onModeTimeSelect);
    m_modeTimeSelectAction->setShortcut(Qt::Key_F1);
    m_modeSelectAction = modeMenu->addAction(icon("x16Select"), "Select", this, &MainWindow::onModeSelect);
    m_modeSelectAction->setShortcut(Qt::Key_F2);
    m_modeWriteAction = modeMenu->addAction(icon("x16Pen"), "Write", this, &MainWindow::onModeWrite);
    m_modeWriteAction->setShortcut(Qt::Key_F3);

    auto* modeGroup = new QActionGroup(this);
    m_modeSelectAction->setCheckable(true);
    m_modeWriteAction->setCheckable(true);
    m_modeTimeSelectAction->setCheckable(true);
    modeGroup->addAction(m_modeSelectAction);
    modeGroup->addAction(m_modeWriteAction);
    modeGroup->addAction(m_modeTimeSelectAction);

    auto* tb = addToolBar("Main");
    tb->addAction(icon("x16Open"), "Open", this, &MainWindow::onOpen);
    tb->addAction(icon("x16Save"), "Save", this, &MainWindow::onSave);
    tb->addSeparator();
    tb->addAction(m_undoAction);
    tb->addAction(m_redoAction);
    tb->addSeparator();
    tb->addAction(m_modeSelectAction);
    tb->addAction(m_modeWriteAction);
    tb->addAction(m_modeTimeSelectAction);
    tb->addSeparator();
    tb->addAction(icon("x16Play"), "Play", this, &MainWindow::onPlaySelectedAudio);
    tb->addAction(icon("x16Stop"), "Stop", this, &MainWindow::onStopAudio);

    QMenu* toolsMenu = menuBar()->addMenu("Tools");
    toolsMenu->addAction(icon("x16Statistics"), "Statistics", this, &MainWindow::onStatistics);
    QAction* findAction = toolsMenu->addAction(icon("x16Find"), "Find By Value", this, &MainWindow::onFindByValue);
    findAction->setShortcut(QKeySequence::Find);
    toolsMenu->addAction(icon("x16ModifyLabel"), "Replace Value", this, &MainWindow::onReplaceValue);
    toolsMenu->addAction(icon("x16MyO2"), "MyO2", this, &MainWindow::onMyO2);
    toolsMenu->addAction("Compare Method", this, [this]() {
        CompareMethodDialog dlg(this);
        dlg.exec();
    });

    QMenu* viewMenu = menuBar()->addMenu("View");
    m_toggleLeftSplitAction = viewMenu->addAction("Show Left Split");
    m_toggleLeftSplitAction->setCheckable(true);
    m_toggleLeftSplitAction->setChecked(false);
    connect(m_toggleLeftSplitAction, &QAction::toggled, this, &MainWindow::onToggleLeftSplit);

    m_toggleRightSplitAction = viewMenu->addAction("Show Right Split");
    m_toggleRightSplitAction->setCheckable(true);
    m_toggleRightSplitAction->setChecked(false);
    connect(m_toggleRightSplitAction, &QAction::toggled, this, &MainWindow::onToggleRightSplit);

    QMenu* optionsMenu = menuBar()->addMenu("Options");
    QAction* previewAction = optionsMenu->addAction(icon("x16PreviewOnClick"), "Preview On Click");
    previewAction->setCheckable(true);
    previewAction->setChecked(m_previewOnClick);
    connect(previewAction, &QAction::toggled, this, [this](bool checked) { m_previewOnClick = checked; });

    QAction* errorCheckAction = optionsMenu->addAction(icon("x16CheckError"), "Error Check");
    errorCheckAction->setCheckable(true);
    errorCheckAction->setChecked(m_errorCheckEnabled);
    connect(errorCheckAction, &QAction::toggled, this, [this](bool checked) { m_errorCheckEnabled = checked; });

    QAction* autoSaveAction = optionsMenu->addAction(icon("x16settings"), "Auto Save");
    autoSaveAction->setCheckable(true);
    autoSaveAction->setChecked(m_autoSaveEnabled);
    connect(autoSaveAction, &QAction::toggled, this, [this](bool checked) { m_autoSaveEnabled = checked; });

    QAction* ntInputAction = optionsMenu->addAction(icon("x16NTInput"), "NT Input Style");
    ntInputAction->setCheckable(true);
    ntInputAction->setShortcut(Qt::Key_F8);
    ntInputAction->setChecked(m_ntInputCheck ? m_ntInputCheck->isChecked() : false);
    connect(ntInputAction, &QAction::toggled, this, [this](bool checked) {
        if (m_ntInputCheck) {
            m_ntInputCheck->setChecked(checked);
        }
    });
    if (m_ntInputCheck) {
        connect(m_ntInputCheck, &QCheckBox::toggled, this, [ntInputAction](bool checked) {
            if (ntInputAction->isChecked() != checked) {
                ntInputAction->setChecked(checked);
            }
        });
    }

    optionsMenu->addSeparator();
    optionsMenu->addAction(icon("x16GeneralOptions"), "General Settings", this, [this]() {
        OpGeneralDialog dlg(this);
        dlg.exec();
    });
    optionsMenu->addAction(icon("x16VisualOptions"), "Visual Settings", this, [this]() {
        OpVisualDialog dlg(this);
        dlg.exec();
    });
    optionsMenu->addAction(icon("x16PlayerOptions"), "Player Options", this, [this]() {
        OpPlayerDialog dlg(this);
        dlg.exec();
    });
}

void MainWindow::loadThemes() {
    m_themeCombo->blockSignals(true);
    m_themeCombo->clear();

    QStringList themePaths;
    const QStringList dirs = candidateThemeDirs();
    for (const QString& dirPath : dirs) {
        QDir d(dirPath);
        const QFileInfoList files = d.entryInfoList({"*.Theme.xml"}, QDir::Files | QDir::Readable, QDir::Name);
        for (const QFileInfo& fi : files) {
            if (!themePaths.contains(fi.absoluteFilePath())) {
                themePaths << fi.absoluteFilePath();
            }
        }
    }

    if (themePaths.isEmpty()) {
        m_themeCombo->addItem("Default", QString());
    } else {
        for (const QString& path : themePaths) {
            m_themeCombo->addItem(QFileInfo(path).baseName(), path);
        }
    }

    m_themeCombo->setCurrentIndex(0);
    m_themeCombo->blockSignals(false);
    onThemeChanged(0);
}

void MainWindow::applyTheme(const Theme& theme) {
    m_baseTheme = theme;
    m_theme = theme;
    applyRuntimeColumnVisibility();
    for (const auto& editor : m_editors) {
            if (!editor) continue;
        editor->setTheme(&m_theme);
        editor->updateGeometry();
        editor->update();
    }
    rebuildNoteChannelCombo();
}

void MainWindow::applyRuntimeColumnVisibility() {
    if (m_baseTheme.columns.isEmpty()) {
        return;
    }

    m_theme = m_baseTheme;

    if (m_theme.columns.size() < 28) {
        m_theme.columns.resize(28);
    }

    auto setVisible = [this](int index, bool visible) {
        if (index < 0 || index >= m_theme.columns.size()) {
            return;
        }
        m_theme.columns[index].isVisible = visible;
    };

    const bool showGp2 = m_doc.header.player != 1;
    setVisible(14, showGp2); // D1
    setVisible(15, showGp2); // D2
    setVisible(16, showGp2); // D3
    setVisible(17, showGp2); // D4
    setVisible(18, showGp2); // D5
    setVisible(19, showGp2); // D6
    setVisible(20, showGp2); // D7
    setVisible(21, showGp2); // D8
    setVisible(22, showGp2); // S3

    const bool showBlp = m_cgBlp ? m_cgBlp->isChecked() : true;
    setVisible(23, showBlp); // BGA
    setVisible(24, showBlp); // LAYER
    setVisible(25, showBlp); // POOR
    setVisible(26, showBlp); // S4

    const bool showScroll = m_cgScroll ? m_cgScroll->isChecked() : true;
    setVisible(1, showScroll);
    const bool showBpm = m_cgBpm ? m_cgBpm->isChecked() : true;
    setVisible(2, showBpm);
    const bool showStop = m_cgStop ? m_cgStop->isChecked() : true;
    setVisible(3, showStop);

    const int bCount = std::clamp(m_bColumnCount, 1, 999);
    const int targetSize = 27 + bCount;
    if (m_theme.columns.size() < targetSize) {
        m_theme.columns.resize(targetSize);
    }
    ThemeColumn baseB = m_theme.columns[27];
    baseB.index = 27;
    baseB.isVisible = true;
    m_theme.columns[27] = baseB;
    for (int i = 28; i < targetSize; ++i) {
        ThemeColumn b = baseB;
        b.index = i;
        m_theme.columns[i] = b;
    }
}

void MainWindow::rebuildNoteChannelCombo() {
    if (!m_noteChannelCombo) {
        return;
    }
    const QString previous = m_noteChannelCombo->currentData().toString();
    m_noteChannelCombo->blockSignals(true);
    m_noteChannelCombo->clear();

    const int columnCount = m_theme.columns.size();
    for (int i = 0; i < columnCount; ++i) {
        const ThemeColumn& c = m_theme.columns[i];
        if (!c.isEnabledAfterAll()) {
            continue;
        }
        const QString channel = BmsDocument::columnToDefaultChannel(i);
        if (channel.isEmpty()) {
            continue;
        }
        const QString label = QString("%1 (%2)").arg(BmsDocument::columnTitle(i)).arg(channel);
        m_noteChannelCombo->addItem(label, channel);
    }
    if (m_noteChannelCombo->count() == 0) {
        m_noteChannelCombo->addItem("A2 (11)", "11");
    }

    int targetIndex = 0;
    if (!previous.isEmpty()) {
        for (int i = 0; i < m_noteChannelCombo->count(); ++i) {
            if (m_noteChannelCombo->itemData(i).toString() == previous) {
                targetIndex = i;
                break;
            }
        }
    }
    m_noteChannelCombo->setCurrentIndex(targetIndex);
    m_noteChannelCombo->blockSignals(false);

    const QString currentChannel = m_noteChannelCombo->currentData().toString();
    for (const auto& editor : m_editors) {
        if (!editor) continue;
        editor->setDefaultChannel(currentChannel.isEmpty() ? QStringLiteral("11") : currentChannel);
    }
}

void MainWindow::syncHeaderToUi() {
    const QSignalBlocker b1(m_titleEdit);
    const QSignalBlocker b2(m_artistEdit);
    const QSignalBlocker b3(m_genreEdit);
    const QSignalBlocker b4(m_subtitleEdit);
    const QSignalBlocker b5(m_subartistEdit);
    const QSignalBlocker b6(m_playLevelEdit);
    const QSignalBlocker b7(m_totalEdit);
    const QSignalBlocker b8(m_playerCombo);
    const QSignalBlocker b9(m_stageFileEdit);
    const QSignalBlocker b10(m_bannerEdit);
    const QSignalBlocker b11(m_backBmpEdit);
    const QSignalBlocker b12(m_bpmEdit);
    m_titleEdit->setText(m_doc.header.title);
    m_artistEdit->setText(m_doc.header.artist);
    m_genreEdit->setText(m_doc.header.genre);
    m_subtitleEdit->setText(m_doc.header.subtitle);
    m_subartistEdit->setText(m_doc.header.subartist);
    m_playLevelEdit->setText(m_doc.header.playLevel);
    m_totalEdit->setText(m_doc.header.total);
    const int player = std::clamp(m_doc.header.player, 1, 3);
    const int idx = std::max(0, m_playerCombo->findData(player));
    m_playerCombo->setCurrentIndex(idx);
    m_stageFileEdit->setText(m_doc.header.stageFile);
    m_bannerEdit->setText(m_doc.header.banner);
    m_backBmpEdit->setText(m_doc.header.backBmp);
    m_bpmEdit->setValue(static_cast<int>(m_doc.header.bpm));
}

void MainWindow::syncHeaderFromUi() {
    m_doc.header.title = m_titleEdit->text();
    m_doc.header.artist = m_artistEdit->text();
    m_doc.header.genre = m_genreEdit->text();
    m_doc.header.subtitle = m_subtitleEdit->text();
    m_doc.header.subartist = m_subartistEdit->text();
    m_doc.header.playLevel = m_playLevelEdit->text();
    m_doc.header.total = m_totalEdit->text();
    m_doc.header.player = std::clamp(m_playerCombo->currentData().toInt(), 1, 3);
    m_doc.header.stageFile = m_stageFileEdit->text();
    m_doc.header.banner = m_bannerEdit->text();
    m_doc.header.backBmp = m_backBmpEdit->text();
    m_doc.header.bpm = m_bpmEdit->value();
}

QString MainWindow::chartBaseDir() const {
    if (!m_doc.sourcePath.isEmpty()) {
        return QFileInfo(m_doc.sourcePath).absolutePath();
    }
    return QDir::currentPath();
}

QString MainWindow::nextCodeForTable(const QVector<QString>& table) const {
    for (int i = 1; i < table.size(); ++i) {
        if (table[i].trimmed().isEmpty()) {
            return c10To36(i);
        }
    }
    return "ZZ";
}

void MainWindow::refreshResourceTables() {
    m_wavTableWidget->blockSignals(true);
    m_wavTableWidget->setRowCount(0);
    for (int i = 1; i < m_doc.wavTable.size(); ++i) {
        if (m_doc.wavTable[i].trimmed().isEmpty()) {
            continue;
        }
        const int row = m_wavTableWidget->rowCount();
        m_wavTableWidget->insertRow(row);
        m_wavTableWidget->setItem(row, 0, new QTableWidgetItem(c10To36(i)));
        m_wavTableWidget->setItem(row, 1, new QTableWidgetItem(m_doc.wavTable[i]));
    }
    m_wavTableWidget->blockSignals(false);

    m_bmpTableWidget->blockSignals(true);
    m_bmpTableWidget->setRowCount(0);
    for (int i = 1; i < m_doc.bmpTable.size(); ++i) {
        if (m_doc.bmpTable[i].trimmed().isEmpty()) {
            continue;
        }
        const int row = m_bmpTableWidget->rowCount();
        m_bmpTableWidget->insertRow(row);
        m_bmpTableWidget->setItem(row, 0, new QTableWidgetItem(c10To36(i)));
        m_bmpTableWidget->setItem(row, 1, new QTableWidgetItem(m_doc.bmpTable[i]));
    }
    m_bmpTableWidget->blockSignals(false);
}

void MainWindow::refreshBeatTable() {
    m_beatTableWidget->blockSignals(true);
    m_beatTableWidget->setRowCount(0);
    for (int i = 0; i < m_doc.measureLengths.size(); ++i) {
        const double ratio = m_doc.measureLengthAt(i) / 192.0;
        if (std::abs(ratio - 1.0) < 0.000001) {
            continue;
        }
        const int row = m_beatTableWidget->rowCount();
        m_beatTableWidget->insertRow(row);
        m_beatTableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(i)));
        m_beatTableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(ratio, 'f', 8)));
    }
    m_beatTableWidget->blockSignals(false);
}

void MainWindow::syncResourceTablesToDocument() {
    m_doc.wavTable.fill(QString());
    for (int i = 0; i < m_wavTableWidget->rowCount(); ++i) {
        auto* code = m_wavTableWidget->item(i, 0);
        auto* val = m_wavTableWidget->item(i, 1);
        if (!code || !val) {
            continue;
        }
        const int idx = code->text().toInt(nullptr, 36);
        if (idx > 0 && idx < m_doc.wavTable.size()) {
            m_doc.wavTable[idx] = val->text().trimmed();
        }
    }

    m_doc.bmpTable.fill(QString());
    for (int i = 0; i < m_bmpTableWidget->rowCount(); ++i) {
        auto* code = m_bmpTableWidget->item(i, 0);
        auto* val = m_bmpTableWidget->item(i, 1);
        if (!code || !val) {
            continue;
        }
        const int idx = code->text().toInt(nullptr, 36);
        if (idx > 0 && idx < m_doc.bmpTable.size()) {
            m_doc.bmpTable[idx] = val->text().trimmed();
        }
    }

    m_doc.measureLengths.fill(192.0);
    for (int i = 0; i < m_beatTableWidget->rowCount(); ++i) {
        auto* mItem = m_beatTableWidget->item(i, 0);
        auto* rItem = m_beatTableWidget->item(i, 1);
        if (!mItem || !rItem) {
            continue;
        }
        bool okMeasure = false;
        bool okRatio = false;
        const int measure = mItem->text().toInt(&okMeasure);
        const double ratio = rItem->text().toDouble(&okRatio);
        if (!okMeasure || !okRatio) {
            continue;
        }
        if (measure >= 0 && measure < m_doc.measureLengths.size() && ratio > 0.0) {
            m_doc.measureLengths[measure] = ratio * 192.0;
        }
    }

    m_doc.recomputeMeasureBottom();
    m_doc.expansion = m_expansionEdit->toPlainText();
}

void MainWindow::scheduleErrorCheck() {
    if (!m_errorCheckEnabled) {
        return;
    }
    m_errorCheckQueued = true;
    if (m_errorCheckTimer && !m_errorCheckTimer->isActive()) {
        m_errorCheckTimer->start();
    }
}

void MainWindow::applyDocumentToEditors() {
    m_suppressUndoCapture = true;
    syncHeaderToUi();
    applyRuntimeColumnVisibility();
    rebuildNoteChannelCombo();
    refreshResourceTables();
    refreshBeatTable();
    m_expansionEdit->blockSignals(true);
    m_expansionEdit->setPlainText(m_doc.expansion);
    m_expansionEdit->blockSignals(false);
    for (const auto& editor : m_editors) {
            if (!editor) continue;
        editor->setTheme(&m_theme);
        editor->setDocument(&m_doc);
        editor->updateGeometry();
        editor->update();
    }
    scrollEditorsToChartStart();
    m_suppressUndoCapture = false;
}

void MainWindow::scrollEditorsToChartStart() {
    QTimer::singleShot(0, this, [this]() {
        if (m_blockEditorSync) {
            return;
        }
        m_blockEditorSync = true;
        for (const auto& scroll : m_editorScrolls) {
            if (!scroll) {
                continue;
            }
            if (auto* vbar = scroll->verticalScrollBar()) {
                vbar->setValue(vbar->maximum());
            }
        }
        m_blockEditorSync = false;
    });
}

void MainWindow::updateWindowTitle() {
    const QString fileName = m_doc.sourcePath.isEmpty() ? "Untitled.bms" : QFileInfo(m_doc.sourcePath).fileName();
    setWindowTitle(QString("iBMSC Qt Rewrite - %1%2").arg(fileName).arg(m_dirty ? " *" : ""));
    if (m_undoAction) m_undoAction->setEnabled(m_undoStack.canUndo());
    if (m_redoAction) m_redoAction->setEnabled(m_undoStack.canRedo());
}

void MainWindow::pushUndoSnapshot() {
    if (m_suppressUndoCapture) {
        return;
    }
    m_pendingUndoSnapshot = m_doc;
    updateWindowTitle();
}

ChartEditorWidget* MainWindow::activeEditor() const {
    if (m_activeEditor) {
        return m_activeEditor;
    }
    return m_editors[1];
}

void MainWindow::setMode(ChartEditorWidget::EditMode mode) {
    for (const auto& editor : m_editors) {
            if (!editor) continue;
        editor->setEditMode(mode);
    }
    m_modeSelectAction->setChecked(mode == ChartEditorWidget::EditMode::Select);
    m_modeWriteAction->setChecked(mode == ChartEditorWidget::EditMode::Write);
    m_modeTimeSelectAction->setChecked(mode == ChartEditorWidget::EditMode::TimeSelect);
    if (m_modeCombo) {
        const int idx = (mode == ChartEditorWidget::EditMode::Write) ? 0 : (mode == ChartEditorWidget::EditMode::Select ? 1 : 2);
        if (m_modeCombo->currentIndex() != idx) {
            m_modeCombo->blockSignals(true);
            m_modeCombo->setCurrentIndex(idx);
            m_modeCombo->blockSignals(false);
        }
    }
    statusBar()->showMessage(QString("Mode: %1")
                                 .arg(mode == ChartEditorWidget::EditMode::Write
                                          ? "Write"
                                          : (mode == ChartEditorWidget::EditMode::Select ? "Select" : "Time Select")),
                             1500);
}

bool MainWindow::openFilePath(const QString& path) {
    if (path.isEmpty()) {
        return false;
    }
    QString err;
    BmsDocument loaded;
    if (!BmsParser::loadFromFile(path, loaded, &err)) {
        QMessageBox::critical(this, "Open Failed", err);
        return false;
    }

    m_doc = loaded;
    m_undoStack.clear();
    m_pendingUndoSnapshot.reset();
    m_dirty = false;
    applyDocumentToEditors();
    updateRecentFiles(path);
    updateWindowTitle();
    statusBar()->showMessage(QString("Opened %1").arg(path), 3000);
    return true;
}

void MainWindow::onOpen() {
    const QString path = QFileDialog::getOpenFileName(this, "Open BMS", chartBaseDir(), "BMS (*.bms *.bme *.bml *.pms *.txt)");
    if (path.isEmpty()) {
        return;
    }
    openFilePath(path);
}

void MainWindow::onSave() {
    if (m_doc.sourcePath.isEmpty()) {
        onSaveAs();
        return;
    }

    syncHeaderFromUi();
    syncResourceTablesToDocument();

    QString err;
    if (!BmsParser::saveToFile(m_doc.sourcePath, m_doc, &err)) {
        QMessageBox::critical(this, "Save Failed", err);
        return;
    }
    m_dirty = false;
    updateWindowTitle();
    statusBar()->showMessage("Saved", 2000);
}

void MainWindow::onSaveAs() {
    const QString path = QFileDialog::getSaveFileName(this, "Save BMS", chartBaseDir(), "BMS (*.bms)");
    if (path.isEmpty()) {
        return;
    }
    m_doc.sourcePath = path;
    onSave();
}

void MainWindow::onThemeChanged(int index) {
    if (index < 0) {
        return;
    }

    const QString path = m_themeCombo->itemData(index).toString();
    if (path.isEmpty()) {
        applyTheme(ThemeLoader::defaultTheme());
        return;
    }

    Theme t;
    QString err;
    if (!ThemeLoader::loadTheme(path, t, &err)) {
        QMessageBox::warning(this, "Theme", err);
        applyTheme(ThemeLoader::defaultTheme());
        return;
    }
    applyTheme(t);
    statusBar()->showMessage(QString("Theme applied: %1").arg(t.name), 2000);
}

void MainWindow::onPlaySelectedAudio() {
    const int row = m_wavTableWidget->currentRow();
    if (row < 0) {
        return;
    }
    auto* item = m_wavTableWidget->item(row, 1);
    if (!item) {
        return;
    }
    m_audio.play(item->text(), chartBaseDir());
}

void MainWindow::onStopAudio() {
    m_audio.stop();
}

void MainWindow::onWavSelectionChanged() {
    const int row = m_wavTableWidget->currentRow();
    if (row < 0) {
        return;
    }
    auto* codeItem = m_wavTableWidget->item(row, 0);
    if (!codeItem) {
        return;
    }
    int decoded = 0;
    if (!decodeBase36_00_ZZ(codeItem->text(), &decoded)) {
        return;
    }
    m_lastValidNoteValue = std::max(1, decoded);
    m_noteValueEdit->setText(encodeBase36_00_ZZ(decoded));
    for (const auto& editor : m_editors) {
        if (!editor) continue;
        editor->setDefaultValue(std::max(1, decoded) * 10000);
    }
    // Keep VB-like behavior: waveform overlay updates only when user clicks "Load".
}

void MainWindow::onBmpSelectionChanged() {
    const int row = m_bmpTableWidget->currentRow();
    if (row < 0) {
        m_bgiPreview->setText("BGI Preview");
        return;
    }
    auto* pathItem = m_bmpTableWidget->item(row, 1);
    if (!pathItem) {
        return;
    }

    const QString absPath = QFileInfo(QDir(chartBaseDir()), pathItem->text()).absoluteFilePath();
    QImageReader reader(absPath);
    const QImage img = reader.read();
    if (img.isNull()) {
        m_bgiPreview->setText(QFileInfo(absPath).fileName());
        return;
    }
    m_bgiPreview->setPixmap(QPixmap::fromImage(img).scaled(m_bgiPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::onHeaderEdited() {
    syncHeaderFromUi();
    applyRuntimeColumnVisibility();
    for (const auto& editor : m_editors) {
        if (!editor) continue;
        editor->setTheme(&m_theme);
        editor->updateGeometry();
        editor->update();
    }
    rebuildNoteChannelCombo();
    onChartEdited();
}

void MainWindow::onChartEdited() {
    if (m_suppressUndoCapture) {
        return;
    }
    const QObject* src = sender();
    const bool fromEditor = src && qobject_cast<const ChartEditorWidget*>(src);
    if (!fromEditor) {
        syncResourceTablesToDocument();
    }

    if (m_pendingUndoSnapshot.has_value()) {
        m_undoStack.push(std::make_unique<SnapshotCommand>(*m_pendingUndoSnapshot, m_doc));
        m_pendingUndoSnapshot.reset();
    }

    for (const auto& editor : m_editors) {
            if (!editor) continue;
        if (!fromEditor) {
            editor->updateGeometry();
        }
        editor->update();
    }

    m_dirty = true;
    scheduleErrorCheck();
    updateWindowTitle();
}

void MainWindow::updateRecentFiles(const QString& filePath) {
    m_recentFiles.erase(std::remove(m_recentFiles.begin(), m_recentFiles.end(), filePath), m_recentFiles.end());
    m_recentFiles.insert(m_recentFiles.begin(), filePath);
    if (m_recentFiles.size() > 5) {
        m_recentFiles.resize(5);
    }
    rebuildRecentMenu();
}

void MainWindow::rebuildRecentMenu() {
    if (!m_recentMenu) {
        return;
    }
    m_recentMenu->clear();
    if (m_recentFiles.empty()) {
        QAction* a = m_recentMenu->addAction("(Empty)");
        a->setEnabled(false);
        return;
    }
    for (const QString& path : m_recentFiles) {
        QAction* action = m_recentMenu->addAction(path);
        connect(action, &QAction::triggered, this, &MainWindow::onOpenRecent);
    }
}

void MainWindow::onOpenRecent() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action) {
        return;
    }
    openFilePath(action->text());
}

void MainWindow::onStatistics() {
    int total = 0;
    int ln = 0;
    int hidden = 0;
    int landmine = 0;
    for (const BmsNote& n : m_doc.notes) {
        if (n.vPosition < 0) continue;
        ++total;
        if (n.longNote) ++ln;
        if (n.hidden) ++hidden;
        if (n.landmine) ++landmine;
    }
    QMessageBox::information(this,
                             "Statistics",
                             QString("Total notes: %1\nLong notes: %2\nHidden notes: %3\nLandmine notes: %4")
                                 .arg(total)
                                 .arg(ln)
                                 .arg(hidden)
                                 .arg(landmine));
}

void MainWindow::onFindByValue() {
    DiagFindDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }
    const DiagFindDialog::Criteria criteria = dlg.criteria();
    int found = 0;

    auto channelMatched = [&criteria](const QString& ch) {
        if (criteria.channels.isEmpty()) {
            return true;
        }
        if (criteria.channels.contains(ch)) {
            return true;
        }
        if ((ch == "03" && criteria.channels.contains("08")) || (ch == "08" && criteria.channels.contains("03"))) {
            return true;
        }
        return false;
    };

    for (BmsNote& n : m_doc.notes) {
        if (n.vPosition < 0) {
            continue;
        }
        const int measure = m_doc.measureAtPosition(n.vPosition);
        const int label = std::clamp(n.value / 10000, 0, 1295);
        const QString ch = normalizedChannel(n);
        const bool okMeasure = measure >= criteria.measureMin && measure <= criteria.measureMax;
        const bool okLabel = label >= criteria.labelMin && label <= criteria.labelMax;
        const bool okValue = label >= criteria.valueMin && label <= criteria.valueMax;
        const bool okChannel = channelMatched(ch);
        n.selected = okMeasure && okLabel && okValue && okChannel;
        if (n.selected) {
            ++found;
        }
    }
    onEditorSelectionChanged(found);
    for (const auto& editor : m_editors) {
            if (!editor) continue;
        editor->update();
    }
    statusBar()->showMessage(QString("Find matched %1 notes").arg(found), 3000);
}

void MainWindow::onReplaceValue() {
    bool okOld = false;
    const int oldValue = QInputDialog::getInt(this, "Replace Value", "Old value (1..1295):", 1, 1, 1295, 1, &okOld);
    if (!okOld) {
        return;
    }
    bool okNew = false;
    const int newValue = QInputDialog::getInt(this, "Replace Value", "New value (1..1295):", oldValue, 1, 1295, 1, &okNew);
    if (!okNew) {
        return;
    }

    pushUndoSnapshot();
    int changed = 0;
    for (BmsNote& n : m_doc.notes) {
        if (n.vPosition < 0) continue;
        if ((n.value / 10000) == oldValue) {
            n.value = newValue * 10000;
            ++changed;
        }
    }
    for (const auto& editor : m_editors) {
            if (!editor) continue;
        editor->update();
    }
    onChartEdited();
    statusBar()->showMessage(QString("Replaced %1 notes").arg(changed), 3000);
}

void MainWindow::applyDocumentChanged() {
    for (const auto& editor : m_editors) {
        if (!editor) continue;
        editor->updateGeometry();
        editor->update();
    }
    m_dirty = true;
    runBasicErrorCheck();
    updateWindowTitle();
}

void MainWindow::applyWaveformOptionsToEditors() {
    ChartEditorWidget::WaveformOptions opt;
    opt.width = m_waveWidthSpin ? m_waveWidthSpin->value() : 512;
    opt.offset = m_waveOffsetSpin ? m_waveOffsetSpin->value() : 0;
    opt.alpha = m_waveAlphaSpin ? m_waveAlphaSpin->value() : 110;
    opt.precision = m_wavePrecisionSpin ? m_wavePrecisionSpin->value() : 8;
    opt.locked = m_waveLockCheck ? m_waveLockCheck->isChecked() : false;
    for (const auto& editor : m_editors) {
        if (!editor) continue;
        editor->setWaveformOptions(opt);
    }
}

void MainWindow::clearWaveformOverlay() {
    for (const auto& editor : m_editors) {
        if (!editor) continue;
        editor->clearWaveformData();
    }
    statusBar()->showMessage("WaveForm cleared", 1800);
}

void MainWindow::loadWaveformOverlayFromWavSelection() {
    const int row = m_wavTableWidget ? m_wavTableWidget->currentRow() : -1;
    if (row < 0) {
        statusBar()->showMessage("WaveForm: select WAV/OGG row first", 2000);
        return;
    }
    auto* item = m_wavTableWidget->item(row, 1);
    if (!item) {
        return;
    }
    const QString rel = item->text().trimmed();
    if (rel.isEmpty()) {
        return;
    }
    const QString absPath = QFileInfo(QDir(chartBaseDir()), rel).absoluteFilePath();
    QString err;
    const QVector<float> samples = decodeAudioFileMono(absPath, &err);
    if (samples.isEmpty()) {
        statusBar()->showMessage(QString("WaveForm decode failed: %1").arg(err), 2600);
        return;
    }
    for (const auto& editor : m_editors) {
        if (!editor) continue;
        editor->setWaveformData(samples);
    }
    applyWaveformOptionsToEditors();
    statusBar()->showMessage(QString("WaveForm loaded: %1 samples").arg(samples.size()), 2200);
}

void MainWindow::myO2ConstBpm(int bpmX10000) {
    const int targetBpm = std::max(1, bpmX10000);
    QVector<QPair<double, int>> bpmEvents;
    bpmEvents.push_back({0.0, std::max(1, static_cast<int>(m_doc.header.bpm * 10000.0))});
    for (const BmsNote& n : m_doc.notes) {
        const QString ch = normalizedChannel(n);
        if (n.vPosition >= 0.0 && (ch == "03" || ch == "08") && n.value > 0) {
            bpmEvents.push_back({n.vPosition, n.value});
        }
    }
    std::sort(bpmEvents.begin(), bpmEvents.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
    QVector<QPair<double, int>> compact;
    for (const auto& e : bpmEvents) {
        if (compact.isEmpty() || std::abs(compact.back().first - e.first) > 0.000001) compact.push_back(e);
        else compact.back().second = e.second;
    }

    auto timeAt = [&compact](double vPos) {
        if (vPos <= 0.0) return 0.0;
        double t = 0.0;
        double prevV = 0.0;
        int bpm = compact.isEmpty() ? 1200000 : std::max(1, compact.first().second);
        for (const auto& e : compact) {
            if (e.first <= prevV) {
                bpm = std::max(1, e.second);
                continue;
            }
            if (vPos <= e.first) {
                t += (vPos - prevV) / bpm;
                return t;
            }
            t += (e.first - prevV) / bpm;
            prevV = e.first;
            bpm = std::max(1, e.second);
        }
        t += (vPos - prevV) / bpm;
        return t;
    };

    for (BmsNote& n : m_doc.notes) {
        if (n.vPosition < 0.0) continue;
        const QString ch = normalizedChannel(n);
        if (ch == "03" || ch == "08") continue;
        const double t0 = timeAt(n.vPosition);
        if (n.length > 0.0 && m_ntInputCheck && m_ntInputCheck->isChecked()) {
            const double t1 = timeAt(n.vPosition + n.length);
            n.vPosition = targetBpm * t0;
            n.length = std::max(0.0, targetBpm * t1 - n.vPosition);
        } else {
            n.vPosition = targetBpm * t0;
        }
    }

    m_doc.notes.erase(std::remove_if(m_doc.notes.begin(), m_doc.notes.end(), [](const BmsNote& n) {
                         if (n.vPosition < 0.0) return false;
                         const QString ch = normalizedChannel(n);
                         return ch == "03" || ch == "08";
                     }),
                      m_doc.notes.end());
    std::sort(m_doc.notes.begin(), m_doc.notes.end(), [](const BmsNote& a, const BmsNote& b) {
        if (a.vPosition == b.vPosition) return a.columnIndex < b.columnIndex;
        return a.vPosition < b.vPosition;
    });
    m_doc.header.bpm = targetBpm / 10000.0;
    if (!m_doc.notes.isEmpty() && m_doc.notes[0].vPosition < 0.0) {
        m_doc.notes[0].value = targetBpm;
    }
}

QVector<MainWindow::MyO2Adjustment> MainWindow::myO2GridCheck() const {
    QVector<MyO2Adjustment> out;
    static const QStringList ids = {
        "01","03","04","06","07","08","09","16","11","12","13","14","15","18","19",
        "26","21","22","23","24","25","28","29","36","31","32","33","34","35","38","39",
        "46","41","42","43","44","45","48","49","56","51","52","53","54","55","58","59",
        "66","61","62","63","64","65","68","69","76","71","72","73","74","75","78","79",
        "86","81","82","83","84","85","88","89"
    };

    double greatest = 0.0;
    for (const BmsNote& n : m_doc.notes) {
        if (n.vPosition < 0.0) continue;
        greatest = std::max(greatest, n.vPosition + std::max(0.0, n.length));
    }
    const int maxMeasure = std::max(0, m_doc.measureAtPosition(greatest));
    const bool nt = m_ntInputCheck && m_ntInputCheck->isChecked();

    for (int measure = 0; measure <= maxMeasure; ++measure) {
        const double mb = m_doc.measureBottomAt(measure);
        const double mt = mb + m_doc.measureLengthAt(measure);
        for (const QString& id : ids) {
            QVector<int> offsets;
            for (const BmsNote& n : m_doc.notes) {
                if (n.vPosition < 0.0) continue;
                const QString ch = normalizedChannel(n);
                if (ch != id) continue;
                if (nt && (n.length > 0.0) != isLongChannel(id)) continue;
                if (n.vPosition >= mb && n.vPosition < mt) {
                    const int off = static_cast<int>(std::llround(n.vPosition - mb));
                    if (off > 0) offsets.push_back(off);
                }
                if (nt && n.length > 0.0) {
                    const double end = n.vPosition + n.length;
                    if (end >= mb && end < mt) {
                        const int off = static_cast<int>(std::llround(end - mb));
                        if (off > 0) offsets.push_back(off);
                    }
                }
            }
            int g = 192;
            for (int x : offsets) g = gcdInt(g, x);
            if (g < 3) {
                int d48 = 0;
                int d64 = 0;
                for (int x : offsets) {
                    d48 += std::abs(x - (x / 4) * 4);
                    d64 += std::abs(x - (x / 3) * 3);
                }
                MyO2Adjustment a;
                a.measure = measure;
                a.columnIndex = BmsDocument::channelToColumn(id);
                a.columnName = (a.columnIndex > 0) ? BmsDocument::columnToDefaultChannel(a.columnIndex) : id;
                a.grid = g > 0 ? (192 / g) : 0;
                a.longNote = isLongChannel(id);
                a.hidden = isHiddenChannel(id);
                a.adjTo64 = d48 > d64;
                a.d64 = d64;
                a.d48 = d48;
                out.push_back(a);
            }
        }
    }
    return out;
}

void MainWindow::myO2GridAdjust(const QVector<MyO2Adjustment>& adjustments) {
    const bool nt = m_ntInputCheck && m_ntInputCheck->isChecked();
    for (const MyO2Adjustment& a : adjustments) {
        const int step = a.adjTo64 ? 3 : 4;
        if (step <= 0) continue;
        const double mb = m_doc.measureBottomAt(a.measure);
        const double mt = mb + m_doc.measureLengthAt(a.measure);
        for (BmsNote& n : m_doc.notes) {
            if (n.vPosition < 0.0) continue;
            if (n.columnIndex != a.columnIndex) continue;
            if (n.hidden != a.hidden) continue;
            if (nt && ((n.length > 0.0) != a.longNote)) continue;

            if (n.vPosition >= mb && n.vPosition < mt) {
                const double rel = n.vPosition - mb;
                n.vPosition = mb + std::floor(rel / step) * step;
            }
            if (nt && n.length > 0.0) {
                double end = n.vPosition + n.length;
                if (end >= mb && end < mt) {
                    const double relEnd = end - mb;
                    end = mb + std::floor(relEnd / step) * step;
                    n.length = std::max(0.0, end - n.vPosition);
                }
            }
        }
    }
    std::sort(m_doc.notes.begin(), m_doc.notes.end(), [](const BmsNote& a, const BmsNote& b) {
        if (a.vPosition == b.vPosition) return a.columnIndex < b.columnIndex;
        return a.vPosition < b.vPosition;
    });
}

void MainWindow::onMyO2() {
    QDialog dlg(this);
    dlg.setWindowTitle("MyO2");
    dlg.resize(980, 620);
    auto* layout = new QVBoxLayout(&dlg);

    auto* row1 = new QHBoxLayout();
    row1->addWidget(new QLabel("Constant BPM:"));
    auto* bpmSpin = new QDoubleSpinBox(&dlg);
    bpmSpin->setDecimals(4);
    bpmSpin->setRange(0.0001, 65535.9999);
    bpmSpin->setValue(std::max(0.0001, m_doc.header.bpm));
    auto* apply1 = new QPushButton("Apply Constant BPM", &dlg);
    row1->addWidget(bpmSpin);
    row1->addWidget(apply1);
    row1->addStretch(1);
    layout->addLayout(row1);

    auto* row2 = new QHBoxLayout();
    auto* checkBtn = new QPushButton("Grid Check", &dlg);
    auto* applyAdjustBtn = new QPushButton("Apply Grid Adjust", &dlg);
    row2->addWidget(checkBtn);
    row2->addWidget(applyAdjustBtn);
    row2->addStretch(1);
    layout->addLayout(row2);

    auto* table = new QTableWidget(&dlg);
    table->setColumnCount(9);
    table->setHorizontalHeaderLabels({"#", "Measure", "Column", "Grid", "Long", "Hidden", "AdjTo64", "D64", "D48"});
    table->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(table, 1);

    QVector<MyO2Adjustment> adjs;
    connect(checkBtn, &QPushButton::clicked, &dlg, [&, this]() {
        adjs = myO2GridCheck();
        table->setRowCount(adjs.size());
        for (int i = 0; i < adjs.size(); ++i) {
            const MyO2Adjustment& a = adjs[i];
            table->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
            table->setItem(i, 1, new QTableWidgetItem(QString::number(a.measure)));
            table->setItem(i, 2, new QTableWidgetItem(a.columnName));
            table->setItem(i, 3, new QTableWidgetItem(QString::number(a.grid)));
            table->setItem(i, 4, new QTableWidgetItem(a.longNote ? "1" : "0"));
            table->setItem(i, 5, new QTableWidgetItem(a.hidden ? "1" : "0"));
            auto* cboxItem = new QTableWidgetItem();
            cboxItem->setFlags(cboxItem->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
            cboxItem->setCheckState(a.adjTo64 ? Qt::Checked : Qt::Unchecked);
            table->setItem(i, 6, cboxItem);
            table->setItem(i, 7, new QTableWidgetItem(QString::number(a.d64)));
            table->setItem(i, 8, new QTableWidgetItem(QString::number(a.d48)));
        }
    });

    connect(apply1, &QPushButton::clicked, &dlg, [&, this]() {
        pushUndoSnapshot();
        myO2ConstBpm(static_cast<int>(std::llround(bpmSpin->value() * 10000.0)));
        applyDocumentChanged();
        statusBar()->showMessage("MyO2: Constant BPM applied", 2500);
    });

    connect(applyAdjustBtn, &QPushButton::clicked, &dlg, [&, this]() {
        if (adjs.isEmpty()) return;
        for (int i = 0; i < adjs.size() && i < table->rowCount(); ++i) {
            if (auto* it = table->item(i, 6)) {
                adjs[i].adjTo64 = (it->checkState() == Qt::Checked);
            }
        }
        pushUndoSnapshot();
        myO2GridAdjust(adjs);
        applyDocumentChanged();
        statusBar()->showMessage("MyO2: Grid adjustment applied", 2500);
    });

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dlg);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    layout->addWidget(buttons);

    dlg.exec();
}

void MainWindow::loadPersistentSettings() {
    QSettings settings("iBMSCQtRewrite", "iBMSCQt");
    restoreGeometry(settings.value("window/geometry").toByteArray());
    const int snap = settings.value("editor/snap", 16).toInt();
    m_snapSpin->setValue(snap);
    m_ntInputCheck->setChecked(settings.value("editor/nt_input", false).toBool());
    const int mode = settings.value("editor/mode", static_cast<int>(ChartEditorWidget::EditMode::Write)).toInt();
    setMode(static_cast<ChartEditorWidget::EditMode>(std::clamp(mode, 0, 2)));
    m_autoSaveEnabled = settings.value("editor/autosave", false).toBool();
    m_previewOnClick = settings.value("editor/preview_on_click", false).toBool();
    m_errorCheckEnabled = settings.value("editor/error_check", true).toBool();
    if (m_waveWidthSpin) {
        m_waveWidthSpin->setValue(settings.value("waveform/width", 512).toInt());
    }
    if (m_waveOffsetSpin) {
        m_waveOffsetSpin->setValue(settings.value("waveform/offset", 0).toInt());
    }
    if (m_waveAlphaSpin) {
        m_waveAlphaSpin->setValue(settings.value("waveform/alpha", 110).toInt());
    }
    if (m_wavePrecisionSpin) {
        m_wavePrecisionSpin->setValue(settings.value("waveform/precision", 8).toInt());
    }
    if (m_waveLockCheck) {
        m_waveLockCheck->setChecked(settings.value("waveform/lock", false).toBool());
    }
    m_bColumnCount = std::clamp(settings.value("editor/b_columns", 20).toInt(), 1, 999);
    if (m_bColumnsSpin) {
        m_bColumnsSpin->setValue(m_bColumnCount);
    }
    applyRuntimeColumnVisibility();
    for (const auto& editor : m_editors) {
        if (!editor) continue;
        editor->setTheme(&m_theme);
        editor->updateGeometry();
        editor->update();
    }
    rebuildNoteChannelCombo();
    applyWaveformOptionsToEditors();

    const QStringList poKeys = {"POHeader", "POGrid", "POWaveForm", "POWAV", "POBMP", "POBeat", "POExpansion"};
    for (const QString& key : poKeys) {
        if (auto* sw = findChild<QCheckBox*>(key + "_switch")) {
            sw->setChecked(settings.value(QString("po/%1/switch").arg(key), true).toBool());
        }
        if (auto* ex = findChild<QCheckBox*>(key + "_expander")) {
            ex->setChecked(settings.value(QString("po/%1/expander").arg(key), true).toBool());
        }
    }

    m_recentFiles.clear();
    const QStringList recents = settings.value("file/recent").toStringList();
    for (const QString& p : recents) {
        if (!p.trimmed().isEmpty()) {
            m_recentFiles.push_back(p);
        }
    }
    rebuildRecentMenu();
}

void MainWindow::savePersistentSettings() {
    QSettings settings("iBMSCQtRewrite", "iBMSCQt");
    settings.setValue("window/geometry", saveGeometry());
    settings.setValue("editor/snap", m_snapSpin->value());
    settings.setValue("editor/nt_input", m_ntInputCheck->isChecked());
    settings.setValue("editor/mode", static_cast<int>(activeEditor()->editMode()));
    settings.setValue("editor/autosave", m_autoSaveEnabled);
    settings.setValue("editor/preview_on_click", m_previewOnClick);
    settings.setValue("editor/error_check", m_errorCheckEnabled);
    settings.setValue("editor/b_columns", std::clamp(m_bColumnCount, 1, 999));
    settings.setValue("waveform/width", m_waveWidthSpin ? m_waveWidthSpin->value() : 512);
    settings.setValue("waveform/offset", m_waveOffsetSpin ? m_waveOffsetSpin->value() : 0);
    settings.setValue("waveform/alpha", m_waveAlphaSpin ? m_waveAlphaSpin->value() : 110);
    settings.setValue("waveform/precision", m_wavePrecisionSpin ? m_wavePrecisionSpin->value() : 8);
    settings.setValue("waveform/lock", m_waveLockCheck ? m_waveLockCheck->isChecked() : false);
    QStringList recents;
    for (const QString& p : m_recentFiles) {
        recents << p;
    }
    settings.setValue("file/recent", recents);

    const QStringList poKeys = {"POHeader", "POGrid", "POWaveForm", "POWAV", "POBMP", "POBeat", "POExpansion"};
    for (const QString& key : poKeys) {
        if (auto* sw = findChild<QCheckBox*>(key + "_switch")) {
            settings.setValue(QString("po/%1/switch").arg(key), sw->isChecked());
        }
        if (auto* ex = findChild<QCheckBox*>(key + "_expander")) {
            settings.setValue(QString("po/%1/expander").arg(key), ex->isChecked());
        }
    }
}

void MainWindow::onUndo() {
    if (!m_undoStack.canUndo()) {
        return;
    }
    m_pendingUndoSnapshot.reset();
    m_undoStack.undo(m_doc);
    m_dirty = true;
    applyDocumentToEditors();
    updateWindowTitle();
}

void MainWindow::onRedo() {
    if (!m_undoStack.canRedo()) {
        return;
    }
    m_pendingUndoSnapshot.reset();
    m_undoStack.redo(m_doc);
    m_dirty = true;
    applyDocumentToEditors();
    updateWindowTitle();
}

void MainWindow::onModeSelect() {
    setMode(ChartEditorWidget::EditMode::Select);
}

void MainWindow::onModeWrite() {
    setMode(ChartEditorWidget::EditMode::Write);
}

void MainWindow::onModeTimeSelect() {
    setMode(ChartEditorWidget::EditMode::TimeSelect);
}

void MainWindow::onEditorFocus(ibmsc::ChartEditorWidget* editor) {
    if (m_closing) {
        return;
    }
    bool knownEditor = false;
    for (const auto& e : m_editors) {
        if (e == editor) {
            knownEditor = true;
            break;
        }
    }
    if (!knownEditor) {
        return;
    }
    m_activeEditor = editor;
    updateActiveEditorStyles();
}

void MainWindow::onEditorSelectionChanged(int count) {
    m_statusSelection->setText(QString("Sel: %1").arg(count));
}

void MainWindow::onEditorTimeSelectionChanged(double start, double length) {
    if (length <= 0.0) {
        m_statusTimeSelection->setText("TS: -");
    } else {
        m_statusTimeSelection->setText(QString("TS: %1 -> %2")
                                           .arg(start / 192.0, 0, 'f', 2)
                                           .arg((start + length) / 192.0, 0, 'f', 2));
    }
}

void MainWindow::onEditorAboutToEdit() {
    pushUndoSnapshot();
}

void MainWindow::onToggleLeftSplit(bool checked) {
    if (!m_editorScrolls[0]) return;
    m_editorScrolls[0]->setVisible(checked);
}

void MainWindow::onToggleRightSplit(bool checked) {
    if (!m_editorScrolls[2]) return;
    m_editorScrolls[2]->setVisible(checked);
}

void MainWindow::updateActiveEditorStyles() {
    if (m_closing) {
        return;
    }
    for (int i = 0; i < 3; ++i) {
        if (!m_editorScrolls[i] || !m_editors[i]) continue;
        m_editorScrolls[i]->setLineWidth(m_editors[i] == m_activeEditor ? 2 : 1);
    }
}

void MainWindow::onAutoSaveTick() {
    if (!m_autoSaveEnabled || !m_dirty) {
        return;
    }
    const QString base = m_doc.sourcePath.isEmpty()
                             ? QDir::current().absoluteFilePath("Untitled.autosave.bms")
                             : m_doc.sourcePath + ".autosave.bms";
    QString err;
    BmsDocument copy = m_doc;
    BmsParser::saveToFile(base, copy, &err);
}

void MainWindow::runBasicErrorCheck() {
    if (!m_errorCheckEnabled) {
        return;
    }
    QHash<QString, int> counter;
    counter.reserve(m_doc.notes.size());
    int overlaps = 0;
    for (const BmsNote& n : m_doc.notes) {
        if (n.vPosition < 0) {
            continue;
        }
        const qint64 vq = qRound64(n.vPosition * 1000000.0);
        const QString key = QString::number(n.columnIndex) + ":" + QString::number(vq);
        const int prev = counter.value(key, 0);
        overlaps += prev;
        counter.insert(key, prev + 1);
    }
    if (overlaps > 0) {
        statusBar()->showMessage(QString("ErrorCheck: %1 overlapping notes").arg(overlaps), 4000);
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    m_closing = true;
    if (m_autoSaveTimer) {
        m_autoSaveTimer->stop();
    }
    if (m_dirty) {
        const QMessageBox::StandardButton btn = QMessageBox::question(
            this,
            "Unsaved Changes",
            "Chart has unsaved changes.\nSave before closing?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Cancel);
        if (btn == QMessageBox::Cancel) {
            m_closing = false;
            event->ignore();
            return;
        }
        if (btn == QMessageBox::Save) {
            onSave();
            if (m_dirty) {
                m_closing = false;
                event->ignore();
                return;
            }
        }
    }
    savePersistentSettings();
    QMainWindow::closeEvent(event);
}

} // namespace ibmsc
