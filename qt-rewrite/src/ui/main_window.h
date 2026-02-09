#pragma once

#include "audio/audio_preview_service.h"
#include "core/bms_document.h"
#include "core/undo/commands.h"
#include "theme/theme_loader.h"
#include "ui/chart_editor_widget.h"
#include "ui/po_panel_manager.h"

#include <QMainWindow>
#include <QPointer>
#include <QVector>
#include <array>
#include <optional>
#include <vector>

QT_BEGIN_NAMESPACE
class QAction;
class QCheckBox;
class QComboBox;
class QFormLayout;
class QLabel;
class QLineEdit;
class QScrollArea;
class QSpinBox;
class QTableWidget;
class QTabWidget;
class QTextEdit;
class QTimer;
QT_END_NAMESPACE

namespace ibmsc {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onOpen();
    void onSave();
    void onSaveAs();
    void onThemeChanged(int index);
    void onPlaySelectedAudio();
    void onStopAudio();
    void onWavSelectionChanged();
    void onBmpSelectionChanged();
    void onHeaderEdited();
    void onChartEdited();
    void onUndo();
    void onRedo();
    void onModeSelect();
    void onModeWrite();
    void onModeTimeSelect();
    void onEditorFocus(ibmsc::ChartEditorWidget* editor);
    void onEditorSelectionChanged(int count);
    void onEditorTimeSelectionChanged(double start, double length);
    void onEditorAboutToEdit();
    void onOpenRecent();
    void onStatistics();
    void onFindByValue();
    void onReplaceValue();
    void onMyO2();
    void onAutoSaveTick();
    void onToggleLeftSplit(bool checked);
    void onToggleRightSplit(bool checked);

private:
    struct MyO2Adjustment {
        int measure = 0;
        int columnIndex = 0;
        QString columnName;
        int grid = 0;
        bool longNote = false;
        bool hidden = false;
        bool adjTo64 = false;
        int d64 = 0;
        int d48 = 0;
    };

    void setupUi();
    void buildMenus();
    void loadThemes();
    void applyTheme(const Theme& theme);

    void syncHeaderToUi();
    void syncHeaderFromUi();
    void refreshResourceTables();
    void refreshBeatTable();
    void updateWindowTitle();
    void pushUndoSnapshot();
    void applyDocumentToEditors();
    void scrollEditorsToChartStart();
    void syncEditorScrollBars();
    void setMode(ChartEditorWidget::EditMode mode);
    ChartEditorWidget* activeEditor() const;
    void setupEditorShortcuts();
    void updateActiveEditorStyles();
    void loadPersistentSettings();
    void savePersistentSettings();
    void updateRecentFiles(const QString& filePath);
    void rebuildRecentMenu();
    bool openFilePath(const QString& path);
    void runBasicErrorCheck();
    void applyDocumentChanged();
    void myO2ConstBpm(int bpmX10000);
    QVector<MyO2Adjustment> myO2GridCheck() const;
    void myO2GridAdjust(const QVector<MyO2Adjustment>& adjustments);
    void applyWaveformOptionsToEditors();
    void clearWaveformOverlay();
    void loadWaveformOverlayFromWavSelection();

    QString chartBaseDir() const;
    QString nextCodeForTable(const QVector<QString>& table) const;

    BmsDocument m_doc;
    Theme m_theme;
    bool m_dirty = false;
    bool m_blockEditorSync = false;
    bool m_suppressUndoCapture = false;
    bool m_closing = false;
    UndoStack m_undoStack;
    std::optional<BmsDocument> m_pendingUndoSnapshot;
    PoPanelManager m_poPanelManager;

    std::array<QPointer<ChartEditorWidget>, 3> m_editors{};
    std::array<QPointer<QScrollArea>, 3> m_editorScrolls{};
    QPointer<ChartEditorWidget> m_activeEditor = nullptr;
    QTabWidget* m_rightTabs = nullptr;

    QLineEdit* m_titleEdit = nullptr;
    QLineEdit* m_artistEdit = nullptr;
    QLineEdit* m_genreEdit = nullptr;
    QLineEdit* m_subtitleEdit = nullptr;
    QLineEdit* m_subartistEdit = nullptr;
    QLineEdit* m_playLevelEdit = nullptr;
    QLineEdit* m_totalEdit = nullptr;
    QLineEdit* m_stageFileEdit = nullptr;
    QLineEdit* m_bannerEdit = nullptr;
    QLineEdit* m_backBmpEdit = nullptr;
    QSpinBox* m_bpmEdit = nullptr;

    QTableWidget* m_wavTableWidget = nullptr;
    QTableWidget* m_bmpTableWidget = nullptr;
    QTableWidget* m_beatTableWidget = nullptr;
    QTextEdit* m_expansionEdit = nullptr;
    QLabel* m_bgiPreview = nullptr;

    QComboBox* m_themeCombo = nullptr;
    QComboBox* m_modeCombo = nullptr;
    QComboBox* m_noteChannelCombo = nullptr;
    QLineEdit* m_noteValueEdit = nullptr;
    QSpinBox* m_snapSpin = nullptr;
    QCheckBox* m_snapCheck = nullptr;
    QCheckBox* m_ntInputCheck = nullptr;
    QCheckBox* m_longNoteCheck = nullptr;
    QCheckBox* m_hiddenCheck = nullptr;
    QCheckBox* m_landmineCheck = nullptr;
    QCheckBox* m_cgShow = nullptr;
    QCheckBox* m_cgShowBG = nullptr;
    QCheckBox* m_cgShowV = nullptr;
    QCheckBox* m_cgBpm = nullptr;
    QCheckBox* m_cgStop = nullptr;
    QCheckBox* m_cgScroll = nullptr;
    QCheckBox* m_cgBlp = nullptr;
    QCheckBox* m_waveLockCheck = nullptr;
    QSpinBox* m_waveWidthSpin = nullptr;
    QSpinBox* m_waveOffsetSpin = nullptr;
    QSpinBox* m_waveAlphaSpin = nullptr;
    QSpinBox* m_wavePrecisionSpin = nullptr;
    QLabel* m_statusSelection = nullptr;
    QLabel* m_statusTimeSelection = nullptr;
    QAction* m_modeSelectAction = nullptr;
    QAction* m_modeWriteAction = nullptr;
    QAction* m_modeTimeSelectAction = nullptr;
    QAction* m_undoAction = nullptr;
    QAction* m_redoAction = nullptr;
    QMenu* m_recentMenu = nullptr;
    std::vector<QString> m_recentFiles;
    QAction* m_toggleLeftSplitAction = nullptr;
    QAction* m_toggleRightSplitAction = nullptr;
    QTimer* m_autoSaveTimer = nullptr;
    bool m_autoSaveEnabled = true;
    bool m_previewOnClick = false;
    bool m_errorCheckEnabled = true;
    int m_lastValidNoteValue = 1;

    AudioPreviewService m_audio;

protected:
    void closeEvent(QCloseEvent* event) override;
};

} // namespace ibmsc
