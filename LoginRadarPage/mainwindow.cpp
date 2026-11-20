#include "mainwindow.h"
#include <QConicalGradient>
#include <QRadialGradient>
#include <QLinearGradient>

// =========================================================================
// SecondaryWindow IMPLEMENTATION
// =========================================================================
SecondaryWindow::SecondaryWindow(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Secondary Diagnostics & Operational Control");
    resize(520, 420);
    setStyleSheet("QDialog { background-color: #0c131c; border: 1px solid #1c2a39; }");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    QLabel *headerLabel = new QLabel("Secondary Telemetry & Monitoring System", this);
    headerLabel->setStyleSheet("color: #00f0ff; font-weight: bold; font-size: 15px;");
    layout->addWidget(headerLabel);

    // System Status Group Box
    QGroupBox *infoGroup = new QGroupBox("System Status", this);
    infoGroup->setStyleSheet("QGroupBox { color: white; font-weight: bold; border: 1px solid #233549; border-radius: 4px; margin-top: 10px; padding: 10px; }");
    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);

    QLabel *status1 = new QLabel("• Radar Signal Processing Unit: ACTIVE (Nominal)", infoGroup);
    QLabel *status2 = new QLabel("• Primary Feed Frequency: 3.45 GHz", infoGroup);
    QLabel *status3 = new QLabel("• Target Tracking Loop: LOCK ESTABLISHED", infoGroup);
    status1->setStyleSheet("color: #00ff66; font-size: 12px;");
    status2->setStyleSheet("color: #a4b3c6; font-size: 12px;");
    status3->setStyleSheet("color: #00f0ff; font-size: 12px;");

    infoLayout->addWidget(status1);
    infoLayout->addWidget(status2);
    infoLayout->addWidget(status3);
    layout->addWidget(infoGroup);

    // =========================================================================
    // NEW OPERATIONAL CONTROLS: COMBO, TOP UP, ADD-ON PUSH BUTTONS
    // =========================================================================
    QGroupBox *controlsGroup = new QGroupBox("Operational Controls", this);
    controlsGroup->setStyleSheet("QGroupBox { color: white; font-weight: bold; border: 1px solid #233549; border-radius: 4px; margin-top: 6px; padding: 10px; }");

    QHBoxLayout *btnLayout = new QHBoxLayout(controlsGroup);
    btnLayout->setSpacing(10);
    btnLayout->setContentsMargins(8, 12, 8, 8);

    QString actionBtnStyle =
        "QPushButton { "
        "  background-color: #111a24; "
        "  color: #00f0ff; "
        "  border: 1px solid #00f0ff; "
        "  font-weight: bold; "
        "  border-radius: 3px; "
        "  font-size: 12px; "
        "  padding: 6px 12px; "
        "}"
        "QPushButton:hover { "
        "  background-color: #00f0ff; "
        "  color: #0c131c; "
        "}"
        "QPushButton:pressed { "
        "  background-color: #00b8c4; "
        "  color: #0c131c; "
        "}";

    m_btnCombo = new QPushButton("Combo", controlsGroup);
    m_btnCombo->setStyleSheet(actionBtnStyle);
    m_btnCombo->setCursor(Qt::PointingHandCursor);

    m_btnTopUp = new QPushButton("Top Up", controlsGroup);
    m_btnTopUp->setStyleSheet(actionBtnStyle);
    m_btnTopUp->setCursor(Qt::PointingHandCursor);

    m_btnAddOn = new QPushButton("Add-On", controlsGroup);
    m_btnAddOn->setStyleSheet(actionBtnStyle);
    m_btnAddOn->setCursor(Qt::PointingHandCursor);

    connect(m_btnCombo, &QPushButton::clicked, this, &SecondaryWindow::onComboClicked);
    connect(m_btnTopUp, &QPushButton::clicked, this, &SecondaryWindow::onTopUpClicked);
    connect(m_btnAddOn, &QPushButton::clicked, this, &SecondaryWindow::onAddOnClicked);

    btnLayout->addWidget(m_btnCombo);
    btnLayout->addWidget(m_btnTopUp);
    btnLayout->addWidget(m_btnAddOn);

    layout->addWidget(controlsGroup);
    layout->addStretch();

    // Close Screen Button
    QPushButton *closeBtn = new QPushButton("Close Screen", this);
    closeBtn->setFixedSize(110, 30);
    closeBtn->setStyleSheet(
        "QPushButton { background-color: #111a24; color: #ff5555; border: 1px solid #ff5555; font-weight: bold; border-radius: 3px; font-size: 11px; }"
        "QPushButton:hover { background-color: #ff5555; color: black; }"
        );
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeBtn, 0, Qt::AlignRight);
}

void SecondaryWindow::onComboClicked() {
    emit modeSelected("Combo");
}

void SecondaryWindow::onTopUpClicked() {
    emit modeSelected("Top Up");
}

void SecondaryWindow::onAddOnClicked() {
    emit modeSelected("Add-On");
}

// =========================================================================
// RadarPPIWidget IMPLEMENTATION
// =========================================================================
RadarPPIWidget::RadarPPIWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: #040e1b;");
    setFocusPolicy(Qt::StrongFocus);

    m_drawPen.setColor(Qt::yellow);
    m_drawPen.setWidth(2);
    m_drawPen.setCapStyle(Qt::RoundCap);
    m_drawPen.setJoinStyle(Qt::RoundJoin);

    m_sweepTimer = new QTimer(this);
    connect(m_sweepTimer, &QTimer::timeout, this, [this]() {
        m_sweepAngle = fmod(m_sweepAngle + 1.2, 360.0);
        update();
    });
    m_sweepTimer->start(30);
}

void RadarPPIWidget::setSelectedMode(const QString &mode) {
    m_selectedMode = mode;
    update();
}

void RadarPPIWidget::setMapEnabled(bool enabled) {
    m_mapEnabled = enabled;
    update();
}

bool RadarPPIWidget::isMapEnabled() const { return m_mapEnabled; }

void RadarPPIWidget::setMapCoordinates(double lat, double lon) {
    m_latitude = lat;
    m_longitude = lon;
    update();
}

void RadarPPIWidget::setPenMode(bool enabled) {
    m_penMode = enabled;
    setCursor(m_penMode ? Qt::CrossCursor : Qt::ArrowCursor);
}

bool RadarPPIWidget::isPenMode() const { return m_penMode; }

void RadarPPIWidget::clearDrawings() {
    m_drawnPaths.clear();
    m_currentPath = QPainterPath();
    update();
}

void RadarPPIWidget::setPpiMaxRange(int range) {
    if (range > 0) {
        m_ppiMaxRange = range;
        m_isScopeMode = false;
        update();
    }
}

void RadarPPIWidget::setScopeMaxRange(int range, int maxVal, const QString &scopeMode) {
    if (range > 0 && maxVal > 0) {
        m_scopeMaxRange = range;
        m_scopeMaxVal = maxVal;
        m_scopeModeText = scopeMode;
        m_isScopeMode = true;
        update();
    }
}

QRect RadarPPIWidget::getTargetBounds(QPoint center, double offsetX, double offsetY) {
    int size = qRound(30 * m_zoomFactor);
    int x = center.x() + qRound(offsetX * m_zoomFactor);
    int y = center.y() + qRound(offsetY * m_zoomFactor);
    return QRect(x - size/2, y - size/2, size, size);
}

void RadarPPIWidget::drawMapOverlay(QPainter &painter, QPoint center, int outerRadius) {
    painter.save();

    QPainterPath clipCircle;
    clipCircle.addEllipse(center, outerRadius, outerRadius);
    painter.setClipPath(clipCircle);

    // Fill Ocean Background
    painter.fillRect(QRect(center.x() - outerRadius, center.y() - outerRadius, outerRadius * 2, outerRadius * 2), QColor(5, 12, 20, 220));

    double scale = outerRadius / 150.0;

    // Coastlines
    QPainterPath landmass1;
    landmass1.moveTo(center.x() - 300 * scale, center.y() - 300 * scale);
    landmass1.lineTo(center.x() - 300 * scale, center.y() + 300 * scale);
    landmass1.lineTo(center.x() - 100 * scale, center.y() + 300 * scale);
    landmass1.cubicTo(center.x() - 80 * scale, center.y() + 150 * scale, center.x() - 40 * scale, center.y() - 50 * scale, center.x() - 30 * scale, center.y() - 30 * scale);
    landmass1.cubicTo(center.x() - 20 * scale, center.y() - 100 * scale, center.x() - 90 * scale, center.y() - 200 * scale, center.x() - 100 * scale, center.y() - 300 * scale);
    landmass1.closeSubpath();

    QPainterPath landmass2;
    landmass2.moveTo(center.x() + 300 * scale, center.y() - 300 * scale);
    landmass2.lineTo(center.x() + 100 * scale, center.y() - 300 * scale);
    landmass2.cubicTo(center.x() + 50 * scale, center.y() - 150 * scale, center.x() + 30 * scale, center.y() + 50 * scale, center.x() + 40 * scale, center.y() + 100 * scale);
    landmass2.cubicTo(center.x() + 50 * scale, center.y() + 180 * scale, center.x() + 80 * scale, center.y() + 250 * scale, center.x() + 100 * scale, center.y() + 300 * scale);
    landmass2.lineTo(center.x() + 300 * scale, center.y() + 300 * scale);
    landmass2.closeSubpath();

    QColor landColor(18, 38, 56, 210);
    QPen coastPen(QColor(0, 200, 240, 180), 1.5, Qt::SolidLine);

    painter.setBrush(landColor);
    painter.setPen(coastPen);
    painter.drawPath(landmass1);
    painter.drawPath(landmass2);

    QPen latLonPen(QColor(0, 160, 210, 50), 1, Qt::DotLine);
    painter.setPen(latLonPen);

    int gridSpacing = qRound(40 * scale);
    for (int x = center.x() - outerRadius; x <= center.x() + outerRadius; x += gridSpacing) {
        painter.drawLine(x, center.y() - outerRadius, x, center.y() + outerRadius);
    }
    for (int y = center.y() - outerRadius; y <= center.y() + outerRadius; y += gridSpacing) {
        painter.drawLine(center.x() - outerRadius, y, center.x() + outerRadius, y);
    }

    QFont mapFont("Monospace", 8, QFont::Bold);
    painter.setFont(mapFont);
    painter.setPen(QColor(0, 200, 240, 120));
    QString coordText = QString("MAP: %1° N, %2° E").arg(m_latitude, 0, 'f', 4).arg(m_longitude, 0, 'f', 4);
    painter.drawText(center.x() - outerRadius + 15, center.y() + outerRadius - 15, coordText);

    painter.restore();
}

void RadarPPIWidget::drawTargetBlip(QPainter &painter, QPoint pos, const QString &label) {
    double s = m_zoomFactor;
    int radius = qMax(4, qRound(5 * s));

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);

    QRadialGradient glow(pos, radius * 3);
    glow.setColorAt(0.0, QColor(0, 240, 255, 220));
    glow.setColorAt(0.4, QColor(0, 200, 255, 100));
    glow.setColorAt(1.0, QColor(0, 180, 255, 0));
    painter.setBrush(glow);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(pos, radius * 3, radius * 3);

    painter.setBrush(QColor(220, 255, 255));
    painter.drawEllipse(pos, radius, radius);

    if (!label.isEmpty()) {
        QFont labelFont = painter.font();
        labelFont.setPointSize(qMax(7, qRound(8 * s)));
        labelFont.setBold(true);
        painter.setFont(labelFont);
        painter.setPen(QColor(0, 240, 255, 200));
        painter.drawText(pos.x() + radius * 2, pos.y() + radius, label);
    }

    painter.restore();
}

void RadarPPIWidget::drawScopeWaveform(QPainter &painter, int x, int y, int w, int h) {
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath wavePath;
    int baseLineY = y + h - 15;
    wavePath.moveTo(x, baseLineY);

    for (int px = 0; px < w; ++px) {
        double normX = (double)px / w;
        double noise = (std::rand() % 100) / 100.0 * 8.0;

        // Simulated target amplitude returns
        double peak1 = 0.85 * h * std::exp(-std::pow((normX - 0.28) / 0.02, 2));
        double peak2 = 0.55 * h * std::exp(-std::pow((normX - 0.62) / 0.025, 2));
        double peak3 = 0.35 * h * std::exp(-std::pow((normX - 0.82) / 0.018, 2));

        double py = baseLineY - (noise + peak1 + peak2 + peak3);
        py = qBound((double)y + 5, py, (double)baseLineY);

        wavePath.lineTo(x + px, py);
    }

    QPen wavePen(QColor(0, 255, 180, 230), 1.8);
    painter.setPen(wavePen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(wavePath);

    painter.restore();
}

void RadarPPIWidget::wheelEvent(QWheelEvent *event) {
    if (event->angleDelta().y() > 0) m_zoomFactor *= 1.15;
    else m_zoomFactor /= 1.15;

    if (m_zoomFactor > 6.0) m_zoomFactor = 6.0;
    if (m_zoomFactor < 0.25) m_zoomFactor = 0.25;

    event->accept();
    update();
}

void RadarPPIWidget::mousePressEvent(QMouseEvent *event) {
    if (m_penMode && event->button() == Qt::LeftButton) {
        m_isDrawing = true;
        m_currentPath = QPainterPath();
        m_currentPath.moveTo(event->pos());
        update();
        event->accept();
        return;
    }

    if (m_isScopeMode || event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }

    QPoint center(width() / 2, height() / 2);
    QRect t1 = getTargetBounds(center, m_rectOffsetX1, m_rectOffsetY1);
    QRect t2 = getTargetBounds(center, m_rectOffsetX2, m_rectOffsetY2);
    QRect t3 = getTargetBounds(center, m_rectOffsetX3, m_rectOffsetY3);
    QRect t4 = getTargetBounds(center, m_rectOffsetX4, m_rectOffsetY4);

    if (t1.contains(event->pos())) m_activeDragIndex = 0;
    else if (t2.contains(event->pos())) m_activeDragIndex = 1;
    else if (t3.contains(event->pos())) m_activeDragIndex = 2;
    else if (t4.contains(event->pos())) m_activeDragIndex = 3;
    else m_activeDragIndex = -1;

    if (m_activeDragIndex != -1) {
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    } else {
        QWidget::mousePressEvent(event);
    }
}

void RadarPPIWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_penMode && m_isDrawing && (event->buttons() & Qt::LeftButton)) {
        m_currentPath.lineTo(event->pos());
        update();
        event->accept();
        return;
    }

    if (m_activeDragIndex != -1) {
        QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();

        double dx = (double)delta.x() / m_zoomFactor;
        double dy = (double)delta.y() / m_zoomFactor;

        if (m_activeDragIndex == 0) { m_rectOffsetX1 += dx; m_rectOffsetY1 += dy; }
        else if (m_activeDragIndex == 1) { m_rectOffsetX2 += dx; m_rectOffsetY2 += dy; }
        else if (m_activeDragIndex == 2) { m_rectOffsetX3 += dx; m_rectOffsetY3 += dy; }
        else if (m_activeDragIndex == 3) { m_rectOffsetX4 += dx; m_rectOffsetY4 += dy; }

        update();
        event->accept();
    } else {
        QWidget::mouseMoveEvent(event);
    }
}

void RadarPPIWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (m_penMode && event->button() == Qt::LeftButton && m_isDrawing) {
        m_isDrawing = false;
        m_drawnPaths.append(m_currentPath);
        m_currentPath = QPainterPath();
        update();
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton && m_activeDragIndex != -1) {
        m_activeDragIndex = -1;
        setCursor(Qt::ArrowCursor);
        event->accept();
    } else {
        QWidget::mouseReleaseEvent(event);
    }
}

void RadarPPIWidget::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();
    QPoint center(w / 2, h / 2);

    if (m_isScopeMode) {
        int marginLeft = qRound(55 * m_zoomFactor);
        int marginRight = qRound(35 * m_zoomFactor);
        int marginTop = qRound(40 * m_zoomFactor);
        int marginBottom = qRound(45 * m_zoomFactor);

        int graphWidth = w - marginLeft - marginRight;
        int graphHeight = h - marginTop - marginBottom;
        if (graphWidth <= 0 || graphHeight <= 0) return;

        // Title Box
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.setPen(QColor(0, 240, 255));
        painter.drawText(marginLeft, marginTop - 12, QString("A-SCOPE DISPLAY [%1]").arg(m_scopeModeText.toUpper()));

        painter.setPen(QPen(QColor(0, 220, 255, 120), 1.5));
        painter.drawRect(marginLeft, marginTop, graphWidth, graphHeight);

        int yIntervals = 10;
        QPen gridPen(QColor(0, 200, 255, 40), 1, Qt::DashLine);

        for (int i = 0; i <= yIntervals; ++i) {
            double ratio = (double)i / yIntervals;
            int yPos = marginTop + qRound((1.0 - ratio) * graphHeight);
            if (i > 0 && i < yIntervals) {
                painter.setPen(gridPen);
                painter.drawLine(marginLeft, yPos, marginLeft + graphWidth, yPos);
            }
            QString yLabel = QString::number(qRound(ratio * m_scopeMaxRange)) + "km";
            painter.setPen(QPen(QColor(0, 240, 255, 200), 1));
            int textWidth = painter.fontMetrics().horizontalAdvance(yLabel);
            painter.drawText(marginLeft - textWidth - 8, yPos + 4, yLabel);
        }

        int xIntervals = 8;
        QString unitSymbol = m_scopeModeText.contains("Velocity") ? "m/s" : (m_scopeModeText.contains("Height") ? "m" : "°");
        for (int j = 0; j <= xIntervals; ++j) {
            double ratio = (double)j / xIntervals;
            int xPos = marginLeft + qRound(ratio * graphWidth);
            if (j > 0 && j < xIntervals) {
                painter.setPen(gridPen);
                painter.drawLine(xPos, marginTop, xPos, marginTop + graphHeight);
            }
            QString xLabel = QString::number(qRound(ratio * m_scopeMaxVal)) + unitSymbol;
            painter.setPen(QPen(QColor(0, 240, 255, 200), 1));
            int textWidth = painter.fontMetrics().horizontalAdvance(xLabel);
            painter.drawText(xPos - (textWidth / 2), marginTop + graphHeight + 18, xLabel);
        }

        // Draw dynamic signal returns waveform
        drawScopeWaveform(painter, marginLeft, marginTop, graphWidth, graphHeight);

        // Render Active Mode Banner Overlay
        if (!m_selectedMode.isEmpty()) {
            painter.save();
            QRect modeRect(marginLeft + graphWidth - 180, marginTop + 10, 170, 28);
            painter.setPen(QPen(QColor(0, 240, 255, 200), 1));
            painter.setBrush(QColor(12, 19, 28, 220));
            painter.drawRoundedRect(modeRect, 4, 4);

            painter.setFont(QFont("Monospace", 9, QFont::Bold));
            painter.setPen(QColor(0, 255, 136));
            painter.drawText(modeRect, Qt::AlignCenter, QString("MODE: %1").arg(m_selectedMode.toUpper()));
            painter.restore();
        }
        return;
    }

    int baseSize = qMin(w, h) - 70;
    int size = qRound(baseSize * m_zoomFactor);
    if (size <= 0) return;

    int outerRadius = size / 2;

    if (m_mapEnabled) {
        drawMapOverlay(painter, center, outerRadius);
    }

    int ringCount = 6;
    QPen ringPen(QColor(0, 220, 255, m_mapEnabled ? 90 : 60), 1);
    painter.setPen(ringPen);
    painter.setBrush(Qt::NoBrush);

    for (int i = 1; i <= ringCount; ++i) {
        int r = qRound((double)i / ringCount * outerRadius);
        painter.drawEllipse(center, r, r);
    }

    QPen spokePen(QColor(0, 220, 255, m_mapEnabled ? 70 : 45), 1);
    painter.setPen(spokePen);

    for (int deg = 0; deg < 360; deg += 30) {
        double rad = qDegreesToRadians((double)deg);
        QPoint endPt(center.x() + outerRadius * qCos(rad),
                     center.y() + outerRadius * qSin(rad));
        painter.drawLine(center, endPt);
    }

    QPen axisPen(QColor(0, 240, 255, 130), 1);
    painter.setPen(axisPen);
    painter.drawLine(center.x() - outerRadius, center.y(), center.x() + outerRadius, center.y());
    painter.drawLine(center.x(), center.y() - outerRadius, center.x(), center.y() + outerRadius);

    painter.save();
    QConicalGradient sweepGrad(center, -m_sweepAngle);
    sweepGrad.setColorAt(0.0, QColor(0, 240, 255, 150));
    sweepGrad.setColorAt(0.12, QColor(0, 200, 255, 45));
    sweepGrad.setColorAt(0.25, QColor(0, 180, 255, 0));
    sweepGrad.setColorAt(1.0, QColor(0, 180, 255, 0));

    painter.setBrush(sweepGrad);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(center, outerRadius, outerRadius);

    double sweepRad = qDegreesToRadians(m_sweepAngle);
    QPoint sweepEnd(center.x() + outerRadius * qCos(sweepRad),
                    center.y() - outerRadius * qSin(sweepRad));
    painter.setPen(QPen(QColor(220, 255, 255, 240), 1.5));
    painter.drawLine(center, sweepEnd);
    painter.restore();

    int bezelRadius = outerRadius + 8;
    painter.setPen(QPen(QColor(0, 220, 255, 120), 1));
    painter.drawEllipse(center, bezelRadius, bezelRadius);

    for (int angle = 0; angle < 360; angle += 2) {
        double rad = qDegreesToRadians((double)angle);
        bool isMajor = (angle % 10 == 0);
        int tickLength = isMajor ? 7 : 3;

        QPoint p1(center.x() + bezelRadius * qCos(rad),
                  center.y() + bezelRadius * qSin(rad));
        QPoint p2(center.x() + (bezelRadius + tickLength) * qCos(rad),
                  center.y() + (bezelRadius + tickLength) * qSin(rad));

        painter.setPen(QPen(QColor(0, 240, 255, isMajor ? 200 : 90), isMajor ? 1.2 : 0.8));
        painter.drawLine(p1, p2);
    }

    QPen arcPen(QColor(0, 240, 255, 220), 3);
    painter.setPen(arcPen);
    int arcMargin = bezelRadius + 14;
    QRect arcRect(center.x() - arcMargin, center.y() - arcMargin, arcMargin * 2, arcMargin * 2);

    painter.drawArc(arcRect, 25 * 16, 50 * 16);
    painter.drawArc(arcRect, 115 * 16, 50 * 16);
    painter.drawArc(arcRect, 205 * 16, 50 * 16);
    painter.drawArc(arcRect, 295 * 16, 50 * 16);

    QRadialGradient centerGlow(center, 8);
    centerGlow.setColorAt(0.0, QColor(255, 255, 255, 255));
    centerGlow.setColorAt(0.5, QColor(0, 240, 255, 255));
    centerGlow.setColorAt(1.0, QColor(0, 200, 255, 0));
    painter.setBrush(centerGlow);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(center, 6, 6);

    drawTargetBlip(painter, center + QPoint(qRound(m_rectOffsetX1 * m_zoomFactor), qRound(m_rectOffsetY1 * m_zoomFactor)), "TRK-1");
    drawTargetBlip(painter, center + QPoint(qRound(m_rectOffsetX2 * m_zoomFactor), qRound(m_rectOffsetY2 * m_zoomFactor)), "TRK-2");
    drawTargetBlip(painter, center + QPoint(qRound(m_rectOffsetX3 * m_zoomFactor), qRound(m_rectOffsetY3 * m_zoomFactor)), "");
    drawTargetBlip(painter, center + QPoint(qRound(m_rectOffsetX4 * m_zoomFactor), qRound(m_rectOffsetY4 * m_zoomFactor)), "TRK-4");

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(m_drawPen);
    painter.setBrush(Qt::NoBrush);

    for (const QPainterPath &path : std::as_const(m_drawnPaths)) {
        painter.drawPath(path);
    }
    if (m_isDrawing) {
        painter.drawPath(m_currentPath);
    }
    painter.restore();

    // Render Active Operational Mode HUD Banner Overlay
    if (!m_selectedMode.isEmpty()) {
        painter.save();
        QRect modeRect(15, 15, 180, 28);
        painter.setPen(QPen(QColor(0, 240, 255, 200), 1));
        painter.setBrush(QColor(12, 19, 28, 220));
        painter.drawRoundedRect(modeRect, 4, 4);

        painter.setFont(QFont("Monospace", 9, QFont::Bold));
        painter.setPen(QColor(0, 255, 136));
        painter.drawText(modeRect, Qt::AlignCenter, QString("MODE: %1").arg(m_selectedMode.toUpper()));
        painter.restore();
    }
}

// =========================================================================
// TacticalMapWidget IMPLEMENTATION
// =========================================================================
TacticalMapWidget::TacticalMapWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: #7baedc;");
    setCursor(Qt::OpenHandCursor);

    m_searchLocations["NORTH AMERICA"] = QPointF(40.0, -100.0);
    m_searchLocations["SOUTH AMERICA"] = QPointF(-15.0, -60.0);
    m_searchLocations["EUROPE"]        = QPointF(50.0, 15.0);
    m_searchLocations["AFRICA"]        = QPointF(5.0, 20.0);
    m_searchLocations["ASIA"]          = QPointF(45.0, 95.0);
    m_searchLocations["AUSTRALIA"]     = QPointF(-25.0, 135.0);
    m_searchLocations["INDIA"]         = QPointF(20.59, 78.96);

    setupSearchOverlay();
}

void TacticalMapWidget::setupSearchOverlay() {
    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText("Search location...");
    m_searchInput->setStyleSheet(
        "QLineEdit { background-color: rgba(255, 255, 255, 245); color: #202020; "
        "border: 1px solid #a0a0a0; border-radius: 3px; padding: 3px 6px; font-size: 11px; }"
        );
    connect(m_searchInput, &QLineEdit::returnPressed, this, &TacticalMapWidget::onSearchSubmitted);
}

void TacticalMapWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (m_searchInput) {
        int searchWidth = 180;
        int searchHeight = 26;
        m_searchInput->setGeometry(width() - searchWidth - 15, 12, searchWidth, searchHeight);
    }
}

void TacticalMapWidget::onSearchSubmitted() {
    if (!m_searchInput) return;
    QString query = m_searchInput->text().trimmed().toUpper();
    if (m_searchLocations.contains(query)) {
        QPointF targetGeo = m_searchLocations[query];
        m_latitude = targetGeo.x();
        m_longitude = targetGeo.y();
        m_panOffset = QPoint(0, 0);
        update();
    }
}

void TacticalMapWidget::setMapData(double lat, double lon, int maxRange) {
    m_latitude = lat;
    m_longitude = lon;
    m_maxRange = maxRange;
    update();
}

QPointF TacticalMapWidget::latLonToPixel(double lat, double lon, QPointF centerPixel) const {
    double scale = (260.0 * m_zoomFactor) / M_PI;

    double x = centerPixel.x() + (lon - m_longitude) * (M_PI / 180.0) * scale;

    double latRad = lat * M_PI / 180.0;
    double centerLatRad = m_latitude * M_PI / 180.0;

    latRad = qBound(-1.48, latRad, 1.48);
    centerLatRad = qBound(-1.48, centerLatRad, 1.48);

    double yProj = std::log(std::tan(M_PI / 4.0 + latRad / 2.0));
    double centerYProj = std::log(std::tan(M_PI / 4.0 + centerLatRad / 2.0));

    double y = centerPixel.y() - (yProj - centerYProj) * scale;

    return QPointF(x, y);
}

QPainterPath TacticalMapWidget::createPolyPath(const QVector<QPointF>& coords, QPointF centerPixel) const {
    QPainterPath path;
    for (int i = 0; i < coords.size(); ++i) {
        QPointF px = latLonToPixel(coords[i].x(), coords[i].y(), centerPixel);
        if (i == 0) path.moveTo(px);
        else path.lineTo(px);
    }
    path.closeSubpath();
    return path;
}

void TacticalMapWidget::wheelEvent(QWheelEvent *event) {
    if (event->angleDelta().y() > 0) m_zoomFactor *= 1.2;
    else m_zoomFactor /= 1.2;

    m_zoomFactor = qBound(0.1, m_zoomFactor, 25.0);
    event->accept();
    update();
}

void TacticalMapWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QRect zoomInRect(12, 12, 32, 30);
        QRect zoomOutRect(12, 42, 32, 30);

        if (zoomInRect.contains(event->pos())) {
            m_zoomFactor = qBound(0.1, m_zoomFactor * 1.25, 25.0);
            update();
            event->accept();
            return;
        } else if (zoomOutRect.contains(event->pos())) {
            m_zoomFactor = qBound(0.1, m_zoomFactor / 1.25, 25.0);
            update();
            event->accept();
            return;
        }

        m_isPanning = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    }
}

void TacticalMapWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_panOffset = QPoint(0, 0);
        update();
        event->accept();
    }
}

void TacticalMapWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_isPanning) {
        m_panOffset += (event->pos() - m_lastMousePos);
        m_lastMousePos = event->pos();
        event->accept();
        update();
    }
}

void TacticalMapWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_isPanning) {
        m_isPanning = false;
        setCursor(Qt::OpenHandCursor);
        event->accept();
    }
}

void TacticalMapWidget::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();
    QPointF mapCenter = QPointF(w / 2.0, h / 2.0) + m_panOffset;

    // Ocean Fill Gradient
    QLinearGradient oceanGrad(0, 0, 0, h);
    oceanGrad.setColorAt(0.0, QColor(118, 168, 212));
    oceanGrad.setColorAt(0.5, QColor(128, 180, 222));
    oceanGrad.setColorAt(1.0, QColor(112, 162, 208));
    painter.fillRect(rect(), oceanGrad);

    // Landmass & Border Styling
    QColor landBaseColor(218, 214, 188);
    QColor countryBorderColor(255, 255, 255, 210);
    QPen borderPen(countryBorderColor, 1.0, Qt::SolidLine);

    painter.setBrush(landBaseColor);
    painter.setPen(borderPen);

    // Continental Coastlines
    QVector<QPointF> africa = {
        {35.8, -5.8}, {36.8, 8.5}, {33.8, 11.0}, {31.0, 32.0}, {28.0, 34.5},
        {12.0, 43.5}, {11.8, 51.0}, {9.5, 50.5}, {-0.5, 42.5}, {-11.8, 40.5},
        {-26.0, 33.0}, {-34.8, 20.0}, {-34.0, 18.2}, {-22.5, 14.5}, {-15.5, 12.0},
        {-5.0, 12.0}, {4.5, 9.5}, {5.2, -3.5}, {4.3, -7.8}, {14.8, -17.2},
        {21.0, -17.0}, {28.0, -13.0}, {35.8, -5.8}
    };
    painter.drawPath(createPolyPath(africa, mapCenter));

    QVector<QPointF> eurasia = {
        {36.0, -9.5}, {43.5, -9.3}, {43.8, -1.8}, {48.5, -4.5}, {51.0, 1.8},
        {54.0, 5.0}, {57.0, 8.5}, {62.0, 5.0}, {70.0, 22.0}, {71.0, 28.0},
        {68.0, 45.0}, {73.0, 75.0}, {73.5, 125.0}, {68.0, 172.0}, {60.0, 163.0},
        {55.0, 135.0}, {43.0, 132.0}, {38.0, 118.0}, {22.0, 114.0}, {10.0, 104.0},
        {1.3, 103.8}, {16.0, 96.0}, {22.0, 89.0}, {16.0, 82.0}, {8.0, 77.5},
        {16.0, 73.0}, {23.0, 68.0}, {25.0, 62.0}, {12.8, 44.5}, {28.0, 34.5},
        {36.5, 36.0}, {41.0, 28.5}, {38.0, 20.0}, {36.0, -9.5}
    };
    painter.drawPath(createPolyPath(eurasia, mapCenter));

    QVector<QPointF> northAmerica = {
        {70.0, -165.0}, {71.0, -120.0}, {60.0, -64.0}, {47.0, -53.0},
        {30.0, -80.0}, {25.0, -80.0}, {22.0, -97.0}, {15.0, -88.0},
        {8.5, -78.5}, {16.0, -99.0}, {23.0, -110.0}, {32.0, -117.0},
        {48.0, -124.0}, {60.0, -140.0}, {65.0, -168.0}, {70.0, -165.0}
    };
    painter.drawPath(createPolyPath(northAmerica, mapCenter));

    QVector<QPointF> southAmerica = {
        {12.0, -72.0}, {7.0, -55.0}, {-5.0, -35.0}, {-23.0, -42.0},
        {-38.0, -57.0}, {-55.0, -68.0}, {-46.0, -75.0}, {-18.0, -70.0},
        {-2.0, -80.8}, {8.0, -77.2}, {12.0, -72.0}
    };
    painter.drawPath(createPolyPath(southAmerica, mapCenter));

    QVector<QPointF> australia = {
        {-12.0, 131.0}, {-12.0, 142.0}, {-25.0, 153.5}, {-38.0, 144.5},
        {-35.0, 117.0}, {-22.0, 114.0}, {-12.0, 131.0}
    };
    painter.drawPath(createPolyPath(australia, mapCenter));

    QVector<QPointF> greenland = {
        {78.0, -68.0}, {82.0, -30.0}, {60.0, -43.0}, {65.0, -52.0}, {78.0, -68.0}
    };
    painter.drawPath(createPolyPath(greenland, mapCenter));

    QVector<QPointF> antarctica = {
        {-68.0, -180.0}, {-70.0, -90.0}, {-68.0, 0.0}, {-70.0, 90.0}, {-68.0, 180.0},
        {-85.0, 180.0}, {-85.0, -180.0}, {-68.0, -180.0}
    };
    painter.drawPath(createPolyPath(antarctica, mapCenter));

    auto drawIsland = [&](const QVector<QPointF>& pts) {
        painter.drawPath(createPolyPath(pts, mapCenter));
    };

    drawIsland({{58.0, -6.0}, {50.0, 1.5}, {50.0, -5.0}, {58.0, -6.0}});
    drawIsland({{-12.0, 49.0}, {-16.0, 44.0}, {-25.0, 47.0}, {-12.0, 49.0}});
    drawIsland({{45.0, 142.0}, {35.0, 139.0}, {31.0, 130.0}, {45.0, 142.0}});
    drawIsland({{9.8, 80.2}, {8.5, 81.2}, {6.0, 80.8}, {6.9, 79.8}, {9.8, 80.2}});
    drawIsland({{-35.0, 173.0}, {-46.0, 168.0}, {-41.0, 175.0}, {-35.0, 173.0}});

    QPen innerBorderPen(QColor(255, 255, 255, 180), 0.9, Qt::DashLine);
    painter.setPen(innerBorderPen);

    painter.drawLine(latLonToPixel(49.0, -123.0, mapCenter), latLonToPixel(49.0, -89.0, mapCenter));
    painter.drawLine(latLonToPixel(32.0, -117.0, mapCenter), latLonToPixel(26.0, -97.0, mapCenter));
    painter.drawLine(latLonToPixel(22.0, -16.0, mapCenter), latLonToPixel(22.0, 36.0, mapCenter));
    painter.drawLine(latLonToPixel(-22.0, 14.0, mapCenter), latLonToPixel(-22.0, 33.0, mapCenter));
    painter.drawLine(latLonToPixel(50.0, 14.0, mapCenter), latLonToPixel(50.0, 80.0, mapCenter));
    painter.drawLine(latLonToPixel(30.0, 34.0, mapCenter), latLonToPixel(30.0, 75.0, mapCenter));

    QFont labelFont("Arial", qMax(8, qRound(11 * qSqrt(m_zoomFactor))), QFont::Bold);
    painter.setFont(labelFont);
    painter.setPen(QColor(20, 20, 20));

    auto drawContinentLabel = [&](double lat, double lon, const QString& text) {
        QPointF p = latLonToPixel(lat, lon, mapCenter);
        if (rect().contains(p.toPoint())) {
            QRectF textRect(p.x() - 110, p.y() - 15, 220, 30);
            painter.drawText(textRect, Qt::AlignCenter, text);
        }
    };

    drawContinentLabel(40.0, -100.0, "NORTH AMERICA");
    drawContinentLabel(-15.0, -60.0, "SOUTH AMERICA");
    drawContinentLabel(50.0, 15.0, "EUROPE");
    drawContinentLabel(5.0, 20.0, "AFRICA");
    drawContinentLabel(45.0, 95.0, "ASIA");
    drawContinentLabel(-25.0, 135.0, "AUSTRALIA");

    QFont oceanFont("Arial", qMax(7, qRound(10 * qSqrt(m_zoomFactor))), QFont::StyleItalic);
    painter.setFont(oceanFont);
    painter.setPen(QColor(255, 255, 255, 230));

    drawContinentLabel(25.0, -40.0, "Atlantic Ocean");
    drawContinentLabel(-10.0, 75.0, "Indian Ocean");
    drawContinentLabel(0.0, -140.0, "Pacific Ocean");
    drawContinentLabel(0.0, 160.0, "Pacific Ocean");
    drawContinentLabel(75.0, 0.0, "Arctic Ocean");

    // Radar Site Pinpoint Marker & Tactical Range Rings Overlay
    QPointF radarPixel = latLonToPixel(m_latitude, m_longitude, mapCenter);

    int baseRadius = qRound(160 * m_zoomFactor);
    QPen ringPen(QColor(0, 220, 255, 220), 1.5, Qt::DashLine);
    painter.setPen(ringPen);
    painter.setBrush(Qt::NoBrush);

    for (int i = 1; i <= 3; ++i) {
        int r = baseRadius * i / 3;
        painter.drawEllipse(radarPixel, r, r);
        painter.setPen(QColor(0, 120, 180, 240));
        QFont ringFont("Monospace", 8, QFont::Bold);
        painter.setFont(ringFont);
        painter.drawText(radarPixel.x() + 6, radarPixel.y() - r + 12, QString::number(m_maxRange * i / 3) + " km");
        painter.setPen(ringPen);
    }

    painter.setBrush(QColor(0, 255, 128));
    painter.setPen(QPen(Qt::white, 2));
    painter.drawEllipse(radarPixel, 7, 7);

    QFont fontBold("Monospace", 8, QFont::Bold);
    painter.setFont(fontBold);
    painter.setPen(QColor(0, 80, 40));
    painter.drawText(radarPixel.x() + 12, radarPixel.y() + 4,
                     QString("RADAR SITE [%1°N, %2°E]").arg(m_latitude, 0, 'f', 3).arg(m_longitude, 0, 'f', 3));

    auto drawMapTarget = [&](QPointF geoOffset, const QString& label) {
        QPointF tPos = latLonToPixel(m_latitude + geoOffset.x(), m_longitude + geoOffset.y(), mapCenter);
        if (rect().contains(tPos.toPoint())) {
            painter.setBrush(QColor(255, 50, 50));
            painter.setPen(QPen(Qt::white, 1));
            painter.drawRect(tPos.x() - 4, tPos.y() - 4, 8, 8);
            painter.setPen(QColor(180, 0, 0));
            painter.drawText(tPos.x() + 10, tPos.y() + 4, label);
        }
    };

    drawMapTarget(QPointF(0.45, 0.65), "TRK-1 (Air)");
    drawMapTarget(QPointF(-0.30, -0.45), "TRK-2 (Naval)");

    // Interactive Zoom UI Controls Overlay
    painter.setPen(QPen(QColor(160, 160, 160), 1));
    painter.setBrush(QColor(255, 255, 255, 245));
    painter.drawRoundedRect(12, 12, 32, 60, 3, 3);
    painter.drawLine(12, 42, 44, 42);

    QFont btnFont("Arial", 14, QFont::Bold);
    painter.setFont(btnFont);
    painter.setPen(QColor(40, 40, 40));
    painter.drawText(QRect(12, 12, 32, 30), Qt::AlignCenter, "+");
    painter.drawText(QRect(12, 42, 32, 30), Qt::AlignCenter, "−");
}

// =========================================================================
// MapWindow IMPLEMENTATION
// =========================================================================
MapWindow::MapWindow(double lat, double lon, int maxRange, QWidget* parent)
    : QDialog(parent), m_lat(lat), m_lon(lon), m_maxRange(maxRange)
{
    setWindowTitle("Tactical Map Screen");
    resize(950, 750);
    setStyleSheet("QDialog { background-color: #040e1b; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    QWidget* header = new QWidget(this);
    header->setStyleSheet("background-color: #0c131c; border: 1px solid #1c2a39; border-radius: 4px;");
    QHBoxLayout* hLayout = new QHBoxLayout(header);
    hLayout->setContentsMargins(12, 6, 12, 6);

    m_titleLbl = new QLabel(this);
    m_titleLbl->setStyleSheet("color: #00f0ff; font-weight: bold; font-size: 13px;");
    hLayout->addWidget(m_titleLbl);
    hLayout->addStretch();

    QPushButton* closeBtn = new QPushButton("Close Screen", this);
    closeBtn->setFixedSize(110, 28);
    closeBtn->setStyleSheet(
        "QPushButton { background-color: #111a24; color: #ff5555; border: 1px solid #ff5555; font-weight: bold; border-radius: 3px; font-size: 11px; }"
        "QPushButton:hover { background-color: #ff5555; color: black; }"
        );
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    hLayout->addWidget(closeBtn);

    mainLayout->addWidget(header);

    m_mapCanvas = new TacticalMapWidget(this);
    m_mapCanvas->setMapData(m_lat, m_lon, m_maxRange);
    mainLayout->addWidget(m_mapCanvas, 1);

    updateHeaderTitle();
}

void MapWindow::updateMapData(double lat, double lon, int maxRange) {
    m_lat = lat; m_lon = lon; m_maxRange = maxRange;
    updateHeaderTitle();
    if (m_mapCanvas) {
        m_mapCanvas->setMapData(lat, lon, maxRange);
    }
}

void MapWindow::updateHeaderTitle() {
    if (m_titleLbl) {
        m_titleLbl->setText(QString("🗺 TACTICAL MAP DISPLAY  |  LAT: %1° N   LON: %2° E   RANGE: %3 km")
                                .arg(m_lat, 0, 'f', 4)
                                .arg(m_lon, 0, 'f', 4)
                                .arg(m_maxRange));
    }
    setWindowTitle(QString("Tactical Map Screen - [%1° N, %2° E]").arg(m_lat, 0, 'f', 4).arg(m_lon, 0, 'f', 4));
}

// =========================================================================
// MainWindow IMPLEMENTATION
// =========================================================================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_masterStack = new QStackedWidget(this);
    setCentralWidget(m_masterStack);

    QWidget* loginContainer = new QWidget();
    createLoginPageView(loginContainer);
    m_masterStack->addWidget(loginContainer);

    QWidget* successContainer = new QWidget();
    createSuccessPageView(successContainer);
    m_masterStack->addWidget(successContainer);

    QWidget* operationalWorkspace = new QWidget();
    QVBoxLayout* masterVerticalLayout = new QVBoxLayout(operationalWorkspace);
    masterVerticalLayout->setContentsMargins(0, 0, 0, 0);
    masterVerticalLayout->setSpacing(0);

    QWidget* topNavBar = new QWidget(this);
    topNavBar->setObjectName("topNavBar");
    topNavBar->setFixedHeight(36);
    QHBoxLayout* topBarLayout = new QHBoxLayout(topNavBar);
    topBarLayout->setContentsMargins(10, 0, 10, 0);
    topBarLayout->setSpacing(8);

    QPushButton* btnRefresh = new QPushButton("⟳");
    btnRefresh->setFixedSize(24, 24);
    topBarLayout->addWidget(btnRefresh);

    QPushButton* btnHome = new QPushButton("⌂");
    btnHome->setFixedSize(24, 24);
    topBarLayout->addWidget(btnHome);

    m_lblPpiTitle = new QLabel("Radar Terminal Operations");
    m_lblPpiTitle->setStyleSheet("color: white; font-weight: bold; font-size: 14px; margin-left: 5px;");
    topBarLayout->addWidget(m_lblPpiTitle);

    topBarLayout->addStretch();

    m_radarView = new RadarPPIWidget(this);

    QPushButton* btnPen = new QPushButton("✏ Pen");
    btnPen->setCheckable(true);
    btnPen->setFixedSize(60, 24);
    btnPen->setStyleSheet(
        "QPushButton { background-color: #111a24; color: #ffcc00; border: 1px solid #3a536e; font-weight: bold; border-radius: 2px; font-size: 11px; }"
        "QPushButton:hover { background-color: #162433; border-color: #ffcc00; }"
        "QPushButton:checked { background-color: #ffcc00; color: #000000; border-color: #ffcc00; }"
        );
    connect(btnPen, &QPushButton::toggled, m_radarView, &RadarPPIWidget::setPenMode);
    topBarLayout->addWidget(btnPen);

    QPushButton* btnClearDraw = new QPushButton("Clear");
    btnClearDraw->setFixedSize(55, 24);
    btnClearDraw->setStyleSheet(
        "QPushButton { background-color: #111a24; color: #ff5555; border: 1px solid #3a536e; font-weight: bold; border-radius: 2px; font-size: 11px; }"
        "QPushButton:hover { background-color: #162433; border-color: #ff5555; }"
        "QPushButton:pressed { background-color: #ff5555; color: #000000; }"
        );
    connect(btnClearDraw, &QPushButton::clicked, m_radarView, &RadarPPIWidget::clearDrawings);
    topBarLayout->addWidget(btnClearDraw);

    QPushButton* btnHelp = new QPushButton("Help ?");
    btnHelp->setFixedSize(65, 24);
    btnHelp->setStyleSheet(
        "QPushButton { background-color: #111a24; color: #1fa3ff; border: 1px solid #3a536e; font-weight: bold; border-radius: 2px; font-size: 11px; }"
        "QPushButton:hover { background-color: #162433; border-color: #1fa3ff; }"
        "QPushButton:pressed { background-color: #1fa3ff; color: black; }"
        );
    connect(btnHelp, &QPushButton::clicked, this, &MainWindow::showHelpDialog);
    topBarLayout->addWidget(btnHelp);

    QPushButton* btnOpenWindow = new QPushButton("Open Window 🗔");
    btnOpenWindow->setFixedSize(110, 24);
    btnOpenWindow->setStyleSheet(
        "QPushButton { background-color: #111a24; color: #00f0ff; border: 1px solid #00f0ff; font-weight: bold; border-radius: 2px; font-size: 11px; }"
        "QPushButton:hover { background-color: #00f0ff; color: #000000; }"
        "QPushButton:pressed { background-color: #00b8c4; color: #000000; }"
        );
    connect(btnOpenWindow, &QPushButton::clicked, this, &MainWindow::openSecondaryWindow);
    topBarLayout->addWidget(btnOpenWindow);

    masterVerticalLayout->addWidget(topNavBar);

    QWidget* centralDisplayArea = new QWidget();
    QHBoxLayout* mainLayout = new QHBoxLayout(centralDisplayArea);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(m_radarView, 1);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFixedWidth(390);

    QWidget* panelContainer = new QWidget();
    QVBoxLayout* panelLayout = new QVBoxLayout(panelContainer);
    panelLayout->setContentsMargins(10, 10, 10, 10);
    panelLayout->setSpacing(6);

    QPushButton* ppiHeaderBtn = new QPushButton("PPI and Scope Analyze");
    ppiHeaderBtn->setObjectName("menuRow");
    connect(ppiHeaderBtn, &QPushButton::clicked, this, &MainWindow::onMenuRowClicked);
    panelLayout->addWidget(ppiHeaderBtn);

    m_stackedWidget = new QStackedWidget(this);
    QWidget* ppiScopePage = new QWidget();
    createPpiScopeView(ppiScopePage);
    m_stackedWidget->addWidget(ppiScopePage);
    panelLayout->addWidget(m_stackedWidget);

    QStringList systemMenuHeaders = {
        "Operational Mode", "Maintenance Mode", "Calibration Mode",
        "Site Adaptation", "Pedestal Msg", "Radar Coverage", "Target Info"
    };

    for (const QString& title : std::as_const(systemMenuHeaders)) {
        QPushButton* btn = new QPushButton(title);
        btn->setObjectName("menuRow");
        connect(btn, &QPushButton::clicked, this, &MainWindow::onMenuRowClicked);
        panelLayout->addWidget(btn);
    }

    panelLayout->addStretch();
    scrollArea->setWidget(panelContainer);
    mainLayout->addWidget(scrollArea, 0);

    masterVerticalLayout->addWidget(centralDisplayArea, 1);
    m_masterStack->addWidget(operationalWorkspace);

    m_masterStack->setCurrentIndex(0);

    applyCustomStyle();
}

MainWindow::~MainWindow() {
    if (m_mapWindow) {
        delete m_mapWindow;
    }
    if (m_secondaryWindow) {
        delete m_secondaryWindow;
    }
}

void MainWindow::onModeSelected(const QString &modeName) {
    if (m_radarView) {
        m_radarView->setSelectedMode(modeName);
    }
    if (m_lblPpiTitle) {
        m_lblPpiTitle->setText(QString("Radar Terminal Operations - [%1]").arg(modeName));
    }
}

void MainWindow::showHelpDialog() {
    QMessageBox helpBox(this);
    helpBox.setWindowTitle("Operational Manual: View Navigation");
    helpBox.setTextFormat(Qt::RichText);

    QString guideText =
        "<h3 style='color: #1fa3ff;'>Radar Workspace Navigation Guide</h3>"
        "<p><b>Map Screen:</b> Select <b>Map ON</b> and click <b>Submit</b> to pop open the Tactical Map in a new screen window.</p>"
        "<p><b>Map Coordinates:</b> Enter Latitude/Longitude under Map Config and click <b>Submit</b> to show updated coordinates in the map window and PPI display.</p>"
        "<p><b>Zooming & Panning:</b> Use the mouse scroll wheel or top-left <b>+ / −</b> buttons to zoom, click-and-drag to pan, and double-click to re-center.</p>";

    helpBox.setText(guideText);
    helpBox.setStyleSheet(
        "QMessageBox { background-color: #0c131c; border: 1px solid #3a536e; }"
        "QLabel { color: #ffffff; font-size: 12px; }"
        "QPushButton { background-color: #111a24; color: #ffffff; border: 1px solid #3a536e; min-width: 70px; min-height: 22px; font-weight: bold; }"
        "QPushButton:pressed { background-color: #1fa3ff; color: #000000; }"
        );
    helpBox.exec();
}

void MainWindow::openSecondaryWindow() {
    if (!m_secondaryWindow) {
        m_secondaryWindow = new SecondaryWindow(this);
        connect(m_secondaryWindow, &SecondaryWindow::modeSelected, this, &MainWindow::onModeSelected);
    }
    m_secondaryWindow->show();
    m_secondaryWindow->raise();
    m_secondaryWindow->activateWindow();
}

void MainWindow::onLoginSubmitted() {
    m_masterStack->setCurrentIndex(1);
}

void MainWindow::onProceedToRadarWorkspace() {
    m_masterStack->setCurrentIndex(2);
}

void MainWindow::onMenuRowClicked() {
    QPushButton* sourceBtn = qobject_cast<QPushButton*>(sender());
    if (!sourceBtn) return;
    if (sourceBtn->text() == "PPI and Scope Analyze") m_stackedWidget->setCurrentIndex(0);
}

void MainWindow::openMapInNewScreen() {
    bool okLat = false, okLon = false, okRng = false;
    double lat = m_leLat ? m_leLat->text().toDouble(&okLat) : 9.4723;
    double lon = m_leLon ? m_leLon->text().toDouble(&okLon) : 77.4560;
    int rng = m_ppiMaxRngInput ? m_ppiMaxRngInput->text().toInt(&okRng) : 100;

    if (!okLat) lat = 9.4723;
    if (!okLon) lon = 77.4560;
    if (!okRng || rng <= 0) rng = 100;

    if (m_radarView) {
        m_radarView->setMapCoordinates(lat, lon);
        m_radarView->setMapEnabled(m_rbMapOn && m_rbMapOn->isChecked());
    }

    if (!m_mapWindow) {
        m_mapWindow = new MapWindow(lat, lon, rng, this);
    } else {
        m_mapWindow->updateMapData(lat, lon, rng);
    }

    m_mapWindow->show();
    m_mapWindow->raise();
    m_mapWindow->activateWindow();
}

void MainWindow::updateRadarRange() {
    if (!m_radarView) return;

    QPushButton* explicitSender = qobject_cast<QPushButton*>(sender());
    if (!explicitSender) return;

    if (explicitSender->objectName() == "scopeSubmitBtn") {
        if (!m_scopeMaxRng || !m_scopeMaxAng) return;
        bool okRng, okAng;
        int parsedRange = m_scopeMaxRng->text().toInt(&okRng);
        int parsedValue = m_scopeMaxAng->text().toInt(&okAng);

        QString activeScopeMode = "Range Vs Azimuth";
        for (const auto &rb : m_scopeRadioButtons) {
            if (rb && rb->isChecked()) {
                activeScopeMode = rb->text();
                break;
            }
        }

        if (okRng && okAng && parsedRange > 0 && parsedValue > 0) {
            m_radarView->setScopeMaxRange(parsedRange, parsedValue, activeScopeMode);
        }
    }
    else if (explicitSender->objectName() == "ppiSubmitBtn") {
        if (m_ppiMaxRngInput) {
            bool success;
            int parsedValue = m_ppiMaxRngInput->text().toInt(&success);
            if (success && parsedValue > 0) {
                m_radarView->setPpiMaxRange(parsedValue);
            }
        }

        if (m_rbMapOn && m_rbMapOn->isChecked()) {
            openMapInNewScreen();
        } else {
            m_radarView->setMapEnabled(false);
            if (m_mapWindow && m_mapWindow->isVisible()) {
                m_mapWindow->hide();
            }
        }
    }
}

void MainWindow::createLoginPageView(QWidget* parentWidget) {
    QVBoxLayout* centerFrameWrapper = new QVBoxLayout(parentWidget);

    QGroupBox* loginBox = new QGroupBox("Radar Core Security Access");
    loginBox->setFixedSize(340, 240);
    QVBoxLayout* boxLayout = new QVBoxLayout(loginBox);
    boxLayout->setContentsMargins(20, 25, 20, 20);
    boxLayout->setSpacing(12);

    boxLayout->addWidget(new QLabel("Operator Username:"));
    m_usernameInput = new QLineEdit("admin");
    m_usernameInput->setEchoMode(QLineEdit::Normal);
    m_usernameInput->setStyleSheet("background-color: #111a24; color: #ffffff; border: 1px solid #3a536e; padding: 4px; text-align: left; font-weight: normal;");
    boxLayout->addWidget(m_usernameInput);

    boxLayout->addWidget(new QLabel("Security Key / Password:"));
    m_passwordInput = new QLineEdit("••••••••");
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_passwordInput->setStyleSheet("background-color: #111a24; color: #ffffff; border: 1px solid #3a536e; padding: 4px; text-align: left; font-weight: normal;");
    boxLayout->addWidget(m_passwordInput);

    boxLayout->addSpacing(5);

    QPushButton* loginBtn = new QPushButton("AUTHENTICATE SYSTEM");
    loginBtn->setMinimumHeight(34);
    loginBtn->setStyleSheet(
        "QPushButton { background-color: #101c2a; border: 1px solid #1fa3ff; color: #1fa3ff; font-weight: bold; border-radius: 2px; }"
        "QPushButton:pressed { background-color: #1fa3ff; color: #000000; }"
        );
    connect(loginBtn, &QPushButton::clicked, this, &MainWindow::onLoginSubmitted);
    boxLayout->addWidget(loginBtn);

    centerFrameWrapper->addWidget(loginBox, 0, Qt::AlignCenter);
}

void MainWindow::createSuccessPageView(QWidget* parentWidget) {
    QVBoxLayout* layout = new QVBoxLayout(parentWidget);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    layout->addStretch();

    QLabel* iconLabel = new QLabel("✓", parentWidget);
    iconLabel->setStyleSheet("color: #00ff66; font-size: 72px; font-weight: bold;");
    iconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(iconLabel);

    QLabel* headerLabel = new QLabel("INITIALIZATION SUCCESSFUL", parentWidget);
    headerLabel->setStyleSheet("color: #ffffff; font-size: 18px; font-weight: bold; letter-spacing: 2px;");
    headerLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(headerLabel);

    QLabel* subLabel = new QLabel("All secure datalink nodes online. Local console ready.", parentWidget);
    subLabel->setStyleSheet("color: #8fa0b5; font-size: 12px;");
    subLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(subLabel);

    layout->addSpacing(15);

    QPushButton* btnProceed = new QPushButton("LAUNCH RADAR WORKSPACE", parentWidget);
    btnProceed->setFixedSize(260, 42);
    btnProceed->setStyleSheet(
        "QPushButton { background-color: #00ff66; color: #060b11; font-weight: bold; font-size: 11px; border-radius: 4px; letter-spacing: 1px; }"
        "QPushButton:pressed { background-color: #00b347; }"
        );
    connect(btnProceed, &QPushButton::clicked, this, &MainWindow::onProceedToRadarWorkspace);
    layout->addWidget(btnProceed, 0, Qt::AlignCenter);

    layout->addStretch();
}

void MainWindow::createPpiScopeView(QWidget* parentWidget) {
    QVBoxLayout* pageLayout = new QVBoxLayout(parentWidget);
    pageLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox* masterGroup = new QGroupBox("Radar Channels");
    QVBoxLayout* masterLayout = new QVBoxLayout(masterGroup);
    masterLayout->setContentsMargins(8, 12, 8, 8);
    masterLayout->setSpacing(10);

    QHBoxLayout* columnsSplitter = new QHBoxLayout();
    columnsSplitter->setSpacing(10);

    QWidget* scopeWidget = new QWidget();
    QVBoxLayout* scopeVBox = new QVBoxLayout(scopeWidget);
    scopeVBox->setContentsMargins(0, 0, 0, 0);
    scopeVBox->setSpacing(6);

    QLabel* titleScope = new QLabel("Scope Analyze");
    titleScope->setStyleSheet("color: #ffffff; font-weight: bold; font-size: 12px;");
    scopeVBox->addWidget(titleScope);

    m_trackCombo = new QComboBox(); m_trackCombo->addItems({"Track", "Plot", "Burst"});
    scopeVBox->addWidget(m_trackCombo);

    m_scopeRadioButtons.clear();
    QStringList scopeOptions = {"Range Vs Azimuth", "Range Vs Elevation", "Range Vs Height", "Range Vs Velocity"};
    for (const QString& opt : std::as_const(scopeOptions)) {
        QRadioButton* rb = new QRadioButton(opt);
        if (opt == "Range Vs Azimuth") rb->setChecked(true);
        m_scopeRadioButtons.append(rb);
        scopeVBox->addWidget(rb);
    }

    QGridLayout* scopeGrid = new QGridLayout();
    scopeGrid->setContentsMargins(0, 0, 0, 0);
    scopeGrid->setSpacing(4);

    scopeGrid->addWidget(new QLabel("Max Rng"), 0, 0);
    m_scopeMaxRng = new QLineEdit("100"); m_scopeMaxRng->setFixedWidth(45);
    scopeGrid->addWidget(m_scopeMaxRng, 0, 1);
    scopeGrid->addWidget(new QLabel("km"), 0, 2);

    scopeGrid->addWidget(new QLabel("Max Ang"), 1, 0);
    m_scopeMaxAng = new QLineEdit("360"); m_scopeMaxAng->setFixedWidth(45);
    scopeGrid->addWidget(m_scopeMaxAng, 1, 1);
    scopeGrid->addWidget(new QLabel("deg"), 1, 2);
    scopeVBox->addLayout(scopeGrid);

    QGroupBox* openGlBox = new QGroupBox();
    openGlBox->setStyleSheet("QGroupBox { background-color: #080f16; border: 1px solid #1d2e40; padding: 4px; margin-top:0px; }");
    QVBoxLayout* oglLayout = new QVBoxLayout(openGlBox);
    oglLayout->setContentsMargins(4, 4, 4, 4);
    oglLayout->setSpacing(2);
    QRadioButton* rbOgl1 = new QRadioButton("OpenGL"); rbOgl1->setChecked(true);
    QRadioButton* rbOgl2 = new QRadioButton("Without OpenGL");
    oglLayout->addWidget(rbOgl1);
    oglLayout->addWidget(rbOgl2);
    scopeVBox->addWidget(openGlBox);

    QPushButton* scopeSubmit = new QPushButton("Submit");
    scopeSubmit->setObjectName("scopeSubmitBtn");
    scopeSubmit->setFixedWidth(85);
    scopeVBox->addWidget(scopeSubmit, 0, Qt::AlignCenter);
    scopeVBox->addStretch();

    QWidget* ppiWidget = new QWidget();
    QVBoxLayout* ppiVBox = new QVBoxLayout(ppiWidget);
    ppiVBox->setContentsMargins(0, 0, 0, 0);
    ppiVBox->setSpacing(6);

    QLabel* titlePpi = new QLabel("PPI Analyze");
    titlePpi->setStyleSheet("color: #ffffff; font-weight: bold; font-size: 12px;");
    ppiVBox->addWidget(titlePpi);

    QGridLayout* ppiGrid = new QGridLayout();
    ppiGrid->setContentsMargins(0, 0, 0, 0);
    ppiGrid->setSpacing(4);

    ppiGrid->addWidget(new QLabel("Max Rng"), 0, 0);
    m_ppiMaxRngInput = new QLineEdit("100"); m_ppiMaxRngInput->setFixedWidth(45);
    ppiGrid->addWidget(m_ppiMaxRngInput, 0, 1);
    ppiGrid->addWidget(new QLabel("km"), 0, 2);

    ppiGrid->addWidget(new QLabel("StepSize"), 1, 0);
    m_ppiStepSize = new QLineEdit("1"); m_ppiStepSize->setFixedWidth(45);
    ppiGrid->addWidget(m_ppiStepSize, 1, 1);
    ppiGrid->addWidget(new QLabel("km"), 1, 2);

    ppiGrid->addWidget(new QLabel("AngStep"), 2, 0);
    m_ppiAngStep = new QLineEdit("45"); m_ppiAngStep->setFixedWidth(45);
    ppiGrid->addWidget(m_ppiAngStep, 2, 1);
    ppiGrid->addWidget(new QLabel("deg"), 2, 2);
    ppiVBox->addLayout(ppiGrid);

    QGridLayout* checkGrid = new QGridLayout();
    checkGrid->setContentsMargins(0, 0, 0, 0);
    checkGrid->setSpacing(2);
    QCheckBox* cbPlot = new QCheckBox("Plot"); cbPlot->setChecked(true);
    QCheckBox* cbTrack = new QCheckBox("Track"); cbTrack->setChecked(true);
    checkGrid->addWidget(cbPlot, 0, 0);
    checkGrid->addWidget(cbTrack, 0, 1);

    QCheckBox* cbBurst = new QCheckBox("Burst Plot");
    QCheckBox* cbTrackInfo = new QCheckBox("Track Info");
    QCheckBox* cbScanner = new QCheckBox("Scanner");
    checkGrid->addWidget(cbBurst, 1, 0, 1, 2);
    checkGrid->addWidget(cbTrackInfo, 2, 0, 1, 2);
    checkGrid->addWidget(cbScanner, 3, 0, 1, 2);
    ppiVBox->addLayout(checkGrid);

    m_rbMapOn = new QRadioButton("Map ON"); m_rbMapOn->setChecked(true);
    QRadioButton* rbMapOff = new QRadioButton("Map OFF");
    QRadioButton* rbOgl = new QRadioButton("OpenGL");
    ppiVBox->addWidget(m_rbMapOn);
    ppiVBox->addWidget(rbMapOff);
    ppiVBox->addWidget(rbOgl);

    QLabel* lblAccum = new QLabel("Accumulation");
    lblAccum->setStyleSheet("color: #a4b3c6; font-weight: bold; margin-top: 2px;");
    ppiVBox->addWidget(lblAccum);

    QHBoxLayout* accumRow = new QHBoxLayout();
    accumRow->setContentsMargins(0, 0, 0, 0);
    QRadioButton* rbAccOn = new QRadioButton("On");
    QRadioButton* rbAccOff = new QRadioButton("Off"); rbAccOff->setChecked(true);
    accumRow->addWidget(rbAccOn); accumRow->addWidget(rbAccOff);
    ppiVBox->addLayout(accumRow);

    QPushButton* ppiSubmit = new QPushButton("Submit");
    ppiSubmit->setObjectName("ppiSubmitBtn");
    ppiSubmit->setFixedWidth(85);
    ppiVBox->addWidget(ppiSubmit, 0, Qt::AlignCenter);
    ppiVBox->addStretch();

    QGroupBox* mapConfigGroup = new QGroupBox("Map Config");
    mapConfigGroup->setStyleSheet("QGroupBox { background-color: transparent; border: 1px solid #233549; border-radius: 4px; margin-top: 4px; padding: 6px; } QGroupBox::title { color: #ffffff; font-size:11px; subcontrol-position: top left; left: 8px; }");
    QGridLayout* mapGrid = new QGridLayout();
    mapGrid->setContentsMargins(4, 8, 4, 4);
    mapGrid->setSpacing(4);

    mapGrid->addWidget(new QLabel("Latitude"), 0, 0, Qt::AlignRight);
    m_leLat = new QLineEdit("9.4723"); m_leLat->setFixedWidth(65);
    mapGrid->addWidget(m_leLat, 0, 1);
    mapGrid->addWidget(new QLabel("deg"), 0, 2);

    mapGrid->addWidget(new QLabel("Longitude"), 1, 0, Qt::AlignRight);
    m_leLon = new QLineEdit("77.4560"); m_leLon->setFixedWidth(65);
    mapGrid->addWidget(m_leLon, 1, 1);
    mapGrid->addWidget(new QLabel("deg"), 1, 2);

    QPushButton* mapSubmit = new QPushButton("Submit");
    mapSubmit->setObjectName("submitBtn");
    mapSubmit->setFixedWidth(80);
    mapGrid->addWidget(mapSubmit, 2, 0, 1, 3, Qt::AlignCenter);

    connect(mapSubmit, &QPushButton::clicked, this, &MainWindow::openMapInNewScreen);

    columnsSplitter->addWidget(scopeWidget);
    columnsSplitter->addWidget(ppiWidget);
    masterLayout->addLayout(columnsSplitter);

    masterLayout->addWidget(mapConfigGroup);
    mapConfigGroup->setLayout(mapGrid);

    pageLayout->addWidget(masterGroup);

    connect(scopeSubmit, &QPushButton::clicked, this, &MainWindow::updateRadarRange);
    connect(ppiSubmit, &QPushButton::clicked, this, &MainWindow::updateRadarRange);
}

void MainWindow::applyCustomStyle() {
    this->setStyleSheet(R"(
        QMainWindow { background-color: #060b11; }
        QScrollArea { border: none; background-color: #060b11; }
        QWidget#topNavBar { background-color: #0c131c; border-bottom: 1px solid #1c2a39; }
        QGroupBox { background-color: #0c131c; border: 2px solid #233549; border-radius: 4px; margin-top: 10px; padding-top: 15px; }
        QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 12px; color: #ffffff; font-weight: bold; }
        QLabel { color: #a4b3c6; font-size: 11px; }
        QComboBox { background-color: #111a24; border: 1px solid #3a536e; border-radius: 2px; padding: 2px; color: #ffffff; }
        QLineEdit { background-color: #ffffff; border: 1px solid #a0a0a0; color: #000000; font-weight: bold; text-align: center; }
        QRadioButton, QCheckBox { color: #ffffff; font-size: 11px; }
        QPushButton#scopeSubmitBtn, QPushButton#ppiSubmitBtn, QPushButton#submitBtn { background-color: #0c131c; border: 1px solid #ffffff; color: #ffffff; padding: 3px 12px; font-weight: bold; font-size: 11px; }
        QPushButton#scopeSubmitBtn:pressed, QPushButton#ppiSubmitBtn:pressed, QPushButton#submitBtn:pressed { background-color: #1fa3ff; color: black; }
        QPushButton#menuRow { background-color: #101c2a; border: 1px solid #283d53; border-right: 15px solid #283d53; color: #ffffff; font-weight: bold; text-align: left; padding: 8px; border-radius: 0px; }
    )");
}