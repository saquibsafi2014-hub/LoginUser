#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QDialog>
#include <QPointer>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QVector>
#include <QTimer>
#include <QLineEdit>
#include <QComboBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QtMath>
#include <cmath>
#include <utility>
#include <QMap>

// =========================================================================
// UPDATED SECONDARY POP-UP WINDOW
// =========================================================================
class SecondaryWindow : public QDialog {
    Q_OBJECT

public:
    explicit SecondaryWindow(QWidget *parent = nullptr);

signals:
    void modeSelected(const QString &modeName);

private slots:
    void onComboClicked();
    void onTopUpClicked();
    void onAddOnClicked();

private:
    QPushButton *m_btnCombo = nullptr;
    QPushButton *m_btnTopUp = nullptr;
    QPushButton *m_btnAddOn = nullptr;
};

// =========================================================================
// CUSTOM RADAR VIEWPORT CANVAS WIDGET (MAIN OPERATIONAL SCREEN)
// =========================================================================
class RadarPPIWidget : public QWidget {
    Q_OBJECT

public:
    explicit RadarPPIWidget(QWidget *parent = nullptr);

    void setMapEnabled(bool enabled);
    bool isMapEnabled() const;
    void setMapCoordinates(double lat, double lon);
    void setPenMode(bool enabled);
    bool isPenMode() const;
    void clearDrawings();
    void setPpiMaxRange(int range);
    void setScopeMaxRange(int range, int maxVal, const QString &scopeMode = "Range Vs Azimuth");
    void setSelectedMode(const QString &mode);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QRect getTargetBounds(QPoint center, double offsetX, double offsetY);
    void drawMapOverlay(QPainter &painter, QPoint center, int outerRadius);
    void drawTargetBlip(QPainter &painter, QPoint pos, const QString &label);
    void drawScopeWaveform(QPainter &painter, int x, int y, int w, int h);

    int m_ppiMaxRange = 100;
    int m_scopeMaxRange = 100;
    int m_scopeMaxVal = 360;
    QString m_scopeModeText = "Range Vs Azimuth";
    QString m_selectedMode = "";
    bool m_isScopeMode = false;
    double m_zoomFactor = 1.0;

    // Map Configuration State
    bool m_mapEnabled = false;
    double m_latitude = 9.4723;
    double m_longitude = 77.4560;

    // Real-Time Rotating Radar Sweep Animation
    double m_sweepAngle = 55.0;
    QTimer *m_sweepTimer = nullptr;

    // Freehand Drawing State Variables
    bool m_penMode = false;
    bool m_isDrawing = false;
    QPainterPath m_currentPath;
    QVector<QPainterPath> m_drawnPaths;
    QPen m_drawPen;

    // Target position offsets relative to radar center
    double m_rectOffsetX1 = 35.0;   double m_rectOffsetY1 = -135.0;
    double m_rectOffsetX2 = -140.0; double m_rectOffsetY2 = -80.0;
    double m_rectOffsetX3 = -60.0;  double m_rectOffsetY3 = 60.0;
    double m_rectOffsetX4 = 110.0;  double m_rectOffsetY4 = 40.0;

    int m_activeDragIndex = -1;
    QPoint m_lastMousePos;
};

// =========================================================================
// REALISTIC HIGH-DETAIL GEOGRAPHIC GIS WORLD MAP CANVAS WIDGET
// =========================================================================
class TacticalMapWidget : public QWidget {
    Q_OBJECT

public:
    explicit TacticalMapWidget(QWidget *parent = nullptr);
    void setMapData(double lat, double lon, int maxRange);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onSearchSubmitted();

private:
    QPointF latLonToPixel(double lat, double lon, QPointF centerPixel) const;
    QPainterPath createPolyPath(const QVector<QPointF>& coords, QPointF centerPixel) const;
    void setupSearchOverlay();

    double m_latitude = 9.4723;
    double m_longitude = 77.4560;
    int m_maxRange = 100;
    double m_zoomFactor = 1.0;
    QPoint m_panOffset{0, 0};
    QPoint m_lastMousePos;
    bool m_isPanning = false;

    QLineEdit *m_searchInput = nullptr;
    QMap<QString, QPointF> m_searchLocations;
};

// =========================================================================
// STANDALONE TACTICAL MAP WINDOW SCREEN
// =========================================================================
class MapWindow : public QDialog {
    Q_OBJECT

public:
    explicit MapWindow(double lat, double lon, int maxRange, QWidget* parent = nullptr);
    void updateMapData(double lat, double lon, int maxRange);

private:
    void updateHeaderTitle();

    double m_lat;
    double m_lon;
    int m_maxRange;
    TacticalMapWidget* m_mapCanvas = nullptr;
    QLabel* m_titleLbl = nullptr;
};

// =========================================================================
// MAIN WINDOW CLASS DECLARATION
// =========================================================================
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void showHelpDialog();
    void openSecondaryWindow();
    void onLoginSubmitted();
    void onProceedToRadarWorkspace();
    void onMenuRowClicked();
    void openMapInNewScreen();
    void updateRadarRange();
    void onModeSelected(const QString &modeName);

private:
    void createLoginPageView(QWidget* parentWidget);
    void createSuccessPageView(QWidget* parentWidget);
    void createPpiScopeView(QWidget* parentWidget);
    void applyCustomStyle();

    // UI Widgets & Navigation
    QStackedWidget* m_masterStack = nullptr;
    QStackedWidget* m_stackedWidget = nullptr;
    RadarPPIWidget* m_radarView = nullptr;
    QLabel* m_lblPpiTitle = nullptr;
    QPointer<MapWindow> m_mapWindow = nullptr;
    QPointer<SecondaryWindow> m_secondaryWindow = nullptr;

    // Login Form Elements
    QLineEdit* m_usernameInput = nullptr;
    QLineEdit* m_passwordInput = nullptr;

    // Control View Inputs
    QComboBox* m_trackCombo = nullptr;
    QVector<QRadioButton*> m_scopeRadioButtons;
    QLineEdit* m_scopeMaxRng = nullptr;
    QLineEdit* m_scopeMaxAng = nullptr;
    QLineEdit* m_ppiMaxRngInput = nullptr;
    QLineEdit* m_ppiStepSize = nullptr;
    QLineEdit* m_ppiAngStep = nullptr;
    QRadioButton* m_rbMapOn = nullptr;
    QLineEdit* m_leLat = nullptr;
    QLineEdit* m_leLon = nullptr;
};

#endif // MAINWINDOW_H