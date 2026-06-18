// MainWindowTheme.cpp — 7-theme QSS system for NekoEcat Studio.
//
// Themes: Dark, Light, Nord, Catppuccin, Monokai, Dracula, Solarized.
// All QSS uses /* */ comments (QSS does not support // comments).
// Each theme function returns a complete QSS string.

#include "MainWindowIncludes.h"

#include <QFont>

namespace {

/* ── Dark Theme ──────────────────────────────────────────────────────
   Deep navy backgrounds, slate text, blue accent.
   The default industrial theme. */
QString qssDark()
{
    return QStringLiteral(R"QSS(
        QWidget {
            color: #e6edf5;
            selection-background-color: #3b82f6;
            selection-color: #ffffff;
        }
        QMainWindow, QDialog, QMessageBox, QDockWidget {
            background: #0e1117;
        }
        QMenuBar {
            background: #121722;
            border-bottom: 1px solid #263242;
            padding: 3px 8px;
        }
        QMenuBar::item {
            background: transparent;
            border-radius: 6px;
            padding: 5px 10px;
            margin: 2px;
        }
        QMenuBar::item:selected {
            background: #1e293b;
            color: #e2e8f0;
        }
        QMenu {
            background: #151b26;
            border: 1px solid #263242;
            border-radius: 8px;
            padding: 6px;
        }
        QMenu::item {
            border-radius: 5px;
            padding: 6px 28px 6px 12px;
        }
        QMenu::item:selected {
            background: #1e3a5f;
            color: #93c5fd;
        }
        QMenu::separator {
            height: 1px;
            background: #263242;
            margin: 5px 8px;
        }
        QToolBar {
            background: #121722;
            border: 0;
            border-bottom: 1px solid #263242;
            spacing: 6px;
            padding: 6px 10px;
        }
        QToolBar::separator {
            background: #263242;
            width: 1px;
            margin: 5px 4px;
        }
        QLabel#toolbarLabel {
            color: #64748b;
            font-size: 11px;
            font-weight: 700;
            padding: 0 2px 0 8px;
        }
        QLabel#sectionTitle {
            color: #94a3b8;
            font-size: 11px;
            font-weight: 700;
            padding: 8px 0 2px 0;
        }
        QLabel#dialogTitle {
            color: #f1f5f9;
            font-size: 18px;
            font-weight: 700;
        }
        QLabel#statusSummary {
            color: #94a3b8;
            font-size: 12px;
        }
        QToolButton, QPushButton {
            background: #1a2233;
            color: #e2e8f0;
            border: 1px solid #2d3a50;
            border-radius: 7px;
            padding: 6px 12px;
            min-height: 26px;
        }
        QToolButton:hover, QPushButton:hover {
            background: #1e293b;
            border-color: #3b82f6;
        }
        QToolButton:pressed, QPushButton:pressed {
            background: #1e3a5f;
            border-color: #3b82f6;
        }
        QToolButton:checked {
            background: #3b82f6;
            border-color: #3b82f6;
            color: #ffffff;
        }
        QPushButton#nextBestActionButton {
            background: #3b82f6;
            color: #ffffff;
            border-color: #2563eb;
            font-weight: 700;
        }
        QPushButton#nextBestActionButton[severity="ok"] {
            background: #22c55e;
            border-color: #16a34a;
        }
        QPushButton#nextBestActionButton[severity="action"] {
            background: #3b82f6;
            border-color: #2563eb;
        }
        QPushButton#nextBestActionButton[severity="warning"] {
            background: #f59e0b;
            color: #111827;
            border-color: #d97706;
        }
        QPushButton#nextBestActionButton[severity="error"] {
            background: #ef4444;
            border-color: #dc2626;
        }
        QPushButton#nextBestActionButton[severity="neutral"] {
            background: #475569;
            border-color: #334155;
        }
        QPushButton#nextBestActionButton:hover {
            background: #2563eb;
            border-color: #1d4ed8;
        }
        QPushButton:disabled, QToolButton:disabled {
            background: #1a2233;
            color: #475569;
            border-color: #263242;
        }
        QComboBox, QLineEdit, QSpinBox, QDoubleSpinBox {
            background: #151b26;
            color: #e2e8f0;
            border: 1px solid #2d3a50;
            border-radius: 7px;
            padding: 5px 10px;
            min-height: 26px;
        }
        QComboBox:hover, QLineEdit:hover, QSpinBox:hover, QDoubleSpinBox:hover {
            border-color: #3b82f6;
        }
        QComboBox:focus, QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
            border-color: #3b82f6;
            background: #151b26;
        }
        QLineEdit:read-only {
            background: #0e1117;
            color: #64748b;
        }
        QComboBox::drop-down {
            border: 0;
            width: 24px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #64748b;
            margin-right: 8px;
        }
        QTabWidget::pane {
            background: #0e1117;
            border: 1px solid #263242;
            border-top: 0;
        }
        QTabBar::tab {
            background: #121722;
            color: #94a3b8;
            border: 1px solid #263242;
            border-bottom: 0;
            padding: 7px 16px;
            margin-right: 2px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
        }
        QTabBar::tab:selected {
            background: #0e1117;
            color: #e2e8f0;
            border-bottom: 2px solid #3b82f6;
        }
        QTabBar::tab:hover:!selected {
            background: #1a2233;
            color: #cbd5e1;
        }
        QTableWidget, QTableView {
            background: #0e1117;
            alternate-background-color: #121722;
            gridline-color: #1e293b;
            border: 1px solid #263242;
            selection-background-color: #1e3a5f;
            selection-color: #93c5fd;
        }
        QTableWidget::item, QTableView::item {
            padding: 4px 8px;
            border: 0;
        }
        QTableWidget::item:selected, QTableView::item:selected {
            background: #1e3a5f;
            color: #93c5fd;
        }
        QHeaderView::section {
            background: #121722;
            color: #94a3b8;
            border: 0;
            border-bottom: 1px solid #263242;
            border-right: 1px solid #1e293b;
            padding: 6px 8px;
            font-weight: 600;
            font-size: 11px;
        }
        QScrollBar:vertical {
            background: #0e1117;
            width: 10px;
            border: 0;
        }
        QScrollBar::handle:vertical {
            background: #2d3a50;
            border-radius: 5px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background: #3b82f6;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar:horizontal {
            background: #0e1117;
            height: 10px;
            border: 0;
        }
        QScrollBar::handle:horizontal {
            background: #2d3a50;
            border-radius: 5px;
            min-width: 30px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #3b82f6;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
        QCheckBox {
            color: #e2e8f0;
            spacing: 6px;
        }
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 1px solid #2d3a50;
            border-radius: 4px;
            background: #151b26;
        }
        QCheckBox::indicator:checked {
            background: #3b82f6;
            border-color: #3b82f6;
        }
        QGroupBox {
            border: 1px solid #263242;
            border-radius: 8px;
            margin-top: 8px;
            padding-top: 16px;
            color: #94a3b8;
            font-weight: 600;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            padding: 0 6px;
        }
        QSplitter::handle {
            background: #263242;
        }
        QSplitter::handle:horizontal {
            width: 2px;
        }
        QSplitter::handle:vertical {
            height: 2px;
        }
        QStatusBar {
            background: #121722;
            border-top: 1px solid #263242;
            color: #64748b;
            font-size: 11px;
        }
        QTreeWidget, QTreeView {
            background: #0e1117;
            alternate-background-color: #121722;
            border: 1px solid #263242;
            color: #e2e8f0;
        }
        QTreeWidget::item:selected, QTreeView::item:selected {
            background: #1e3a5f;
            color: #93c5fd;
        }
        QTreeWidget::branch:hover, QTreeView::branch:hover {
            background: #1a2233;
        }
        QListWidget {
            background: #0e1117;
            border: 1px solid #263242;
            color: #e2e8f0;
        }
        QListWidget::item:selected {
            background: #1e3a5f;
            color: #93c5fd;
        }
        QTextBrowser, QTextEdit, QPlainTextEdit {
            background: #0e1117;
            color: #e2e8f0;
            border: 1px solid #263242;
            border-radius: 6px;
        }
        QProgressBar {
            background: #1a2233;
            border: 1px solid #263242;
            border-radius: 4px;
            text-align: center;
            color: #e2e8f0;
        }
        QProgressBar::chunk {
            background: #3b82f6;
            border-radius: 3px;
        }
        QToolTip {
            background: #1e293b;
            color: #e2e8f0;
            border: 1px solid #3b82f6;
            border-radius: 4px;
            padding: 4px 8px;
        }
    
        QFrame {
            background: #0e1117;
            border: none;
            border-radius: 0px;
        }
        QFrame#metricCard {
            background: #121722;
            border: 1px solid #263242;
            border-radius: 8px;
        }
        QWidget {
            background: #0e1117;
        }
)QSS");
}

/* ── Light Theme ─────────────────────────────────────────────────────
   Clean white background, blue accent, high contrast text. */
QString qssLight()
{
    return QStringLiteral(R"QSS(
        QWidget {
            color: #172033;
            selection-background-color: #2563eb;
            selection-color: #ffffff;
        }
        QMainWindow, QDialog, QMessageBox, QDockWidget {
            background: #f4f7fb;
        }
        QMenuBar {
            background: #ffffff;
            border-bottom: 1px solid #d9e1ec;
            padding: 3px 8px;
        }
        QMenuBar::item {
            background: transparent;
            border-radius: 7px;
            padding: 6px 10px;
            margin: 2px;
        }
        QMenuBar::item:selected {
            background: #eef4fb;
            color: #0f172a;
        }
        QMenu {
            background: #ffffff;
            border: 1px solid #d9e1ec;
            border-radius: 8px;
            padding: 6px;
        }
        QMenu::item {
            border-radius: 6px;
            padding: 7px 30px 7px 12px;
        }
        QMenu::item:selected {
            background: #e8f1ff;
            color: #12376f;
        }
        QMenu::separator {
            height: 1px;
            background: #d9e1ec;
            margin: 5px 8px;
        }
        QToolBar {
            background: #ffffff;
            border: 0;
            border-bottom: 1px solid #d9e1ec;
            spacing: 6px;
            padding: 8px 10px;
        }
        QToolBar::separator {
            background: #d9e1ec;
            width: 1px;
            margin: 6px 5px;
        }
        QLabel#toolbarLabel {
            color: #64748b;
            font-size: 11px;
            font-weight: 700;
            padding: 0 2px 0 8px;
        }
        QLabel#sectionTitle {
            color: #475569;
            font-size: 11px;
            font-weight: 700;
            padding: 8px 0 2px 0;
        }
        QLabel#dialogTitle {
            color: #0f172a;
            font-size: 18px;
            font-weight: 700;
        }
        QLabel#statusSummary {
            color: #64748b;
            font-size: 12px;
        }
        QToolButton, QPushButton {
            background: #f7f9fc;
            color: #172033;
            border: 1px solid #d4deeb;
            border-radius: 8px;
            padding: 7px 12px;
            min-height: 28px;
        }
        QToolButton:hover, QPushButton:hover {
            background: #edf4ff;
            border-color: #9bb9e8;
        }
        QToolButton:pressed, QPushButton:pressed {
            background: #dbeafe;
            border-color: #2563eb;
        }
        QToolButton:checked {
            background: #2563eb;
            border-color: #2563eb;
            color: #ffffff;
        }
        QPushButton#nextBestActionButton {
            background: #2563eb;
            color: #ffffff;
            border-color: #1d4ed8;
            font-weight: 700;
        }
        QPushButton#nextBestActionButton[severity="ok"] {
            background: #16a34a;
            border-color: #15803d;
        }
        QPushButton#nextBestActionButton[severity="action"] {
            background: #2563eb;
            border-color: #1d4ed8;
        }
        QPushButton#nextBestActionButton[severity="warning"] {
            background: #f59e0b;
            color: #111827;
            border-color: #d97706;
        }
        QPushButton#nextBestActionButton[severity="error"] {
            background: #dc2626;
            border-color: #b91c1c;
        }
        QPushButton#nextBestActionButton[severity="neutral"] {
            background: #475569;
            border-color: #334155;
        }
        QPushButton:disabled, QToolButton:disabled {
            background: #eef2f7;
            color: #94a3b8;
            border-color: #d9e1ec;
        }
        QComboBox, QLineEdit, QSpinBox, QDoubleSpinBox {
            background: #ffffff;
            color: #172033;
            border: 1px solid #d4deeb;
            border-radius: 8px;
            padding: 6px 10px;
            min-height: 28px;
        }
        QComboBox:hover, QLineEdit:hover, QSpinBox:hover, QDoubleSpinBox:hover {
            border-color: #a8b8cf;
        }
        QComboBox:focus, QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
            border-color: #2563eb;
            background: #ffffff;
        }
        QLineEdit:read-only {
            background: #f8fafc;
            color: #64748b;
        }
        QComboBox::drop-down {
            border: 0;
            width: 24px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #64748b;
            margin-right: 8px;
        }
        QTabWidget::pane {
            background: #ffffff;
            border: 1px solid #d9e1ec;
            border-top: 0;
        }
        QTabBar::tab {
            background: #f0f4f9;
            color: #64748b;
            border: 1px solid #d9e1ec;
            border-bottom: 0;
            padding: 7px 16px;
            margin-right: 2px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
        }
        QTabBar::tab:selected {
            background: #ffffff;
            color: #0f172a;
            border-bottom: 2px solid #2563eb;
        }
        QTabBar::tab:hover:!selected {
            background: #eef4fb;
            color: #334155;
        }
        QTableWidget, QTableView {
            background: #ffffff;
            alternate-background-color: #f8fafc;
            gridline-color: #e8edf4;
            border: 1px solid #d9e1ec;
            selection-background-color: #dbeafe;
            selection-color: #1e40af;
        }
        QTableWidget::item, QTableView::item {
            padding: 4px 8px;
            border: 0;
        }
        QTableWidget::item:selected, QTableView::item:selected {
            background: #dbeafe;
            color: #1e40af;
        }
        QHeaderView::section {
            background: #f0f4f9;
            color: #475569;
            border: 0;
            border-bottom: 1px solid #d9e1ec;
            border-right: 1px solid #e8edf4;
            padding: 6px 8px;
            font-weight: 600;
            font-size: 11px;
        }
        QScrollBar:vertical {
            background: #f4f7fb;
            width: 10px;
            border: 0;
        }
        QScrollBar::handle:vertical {
            background: #c5cdd9;
            border-radius: 5px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background: #2563eb;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar:horizontal {
            background: #f4f7fb;
            height: 10px;
            border: 0;
        }
        QScrollBar::handle:horizontal {
            background: #c5cdd9;
            border-radius: 5px;
            min-width: 30px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #2563eb;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
        QCheckBox {
            color: #172033;
            spacing: 6px;
        }
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 1px solid #d4deeb;
            border-radius: 4px;
            background: #ffffff;
        }
        QCheckBox::indicator:checked {
            background: #2563eb;
            border-color: #2563eb;
        }
        QGroupBox {
            border: 1px solid #d9e1ec;
            border-radius: 8px;
            margin-top: 8px;
            padding-top: 16px;
            color: #475569;
            font-weight: 600;
        }
        QSplitter::handle {
            background: #d9e1ec;
        }
        QSplitter::handle:horizontal {
            width: 2px;
        }
        QSplitter::handle:vertical {
            height: 2px;
        }
        QStatusBar {
            background: #ffffff;
            border-top: 1px solid #d9e1ec;
            color: #64748b;
            font-size: 11px;
        }
        QTreeWidget, QTreeView {
            background: #ffffff;
            alternate-background-color: #f8fafc;
            border: 1px solid #d9e1ec;
            color: #172033;
        }
        QTreeWidget::item:selected, QTreeView::item:selected {
            background: #dbeafe;
            color: #1e40af;
        }
        QListWidget {
            background: #ffffff;
            border: 1px solid #d9e1ec;
            color: #172033;
        }
        QListWidget::item:selected {
            background: #dbeafe;
            color: #1e40af;
        }
        QTextBrowser, QTextEdit, QPlainTextEdit {
            background: #ffffff;
            color: #172033;
            border: 1px solid #d9e1ec;
            border-radius: 6px;
        }
        QProgressBar {
            background: #eef2f7;
            border: 1px solid #d9e1ec;
            border-radius: 4px;
            text-align: center;
            color: #172033;
        }
        QProgressBar::chunk {
            background: #2563eb;
            border-radius: 3px;
        }
        QToolTip {
            background: #0f172a;
            color: #e2e8f0;
            border: 1px solid #2563eb;
            border-radius: 4px;
            padding: 4px 8px;
        }
    
        QFrame {
            background: #ffffff;
            border: none;
            border-radius: 0px;
        }
        QFrame#metricCard {
            background: #f4f7fb;
            border: 1px solid #d9e1ec;
            border-radius: 8px;
        }
        QWidget {
            background: #ffffff;
        }
)QSS");
}

/* ── Nord Theme ──────────────────────────────────────────────────────
   Arctic blue-gray palette from nordtheme.com.
   Polar Night + Snow Storm + Frost accents. */
QString qssNord()
{
    return QStringLiteral(R"QSS(
        QWidget {
            color: #d8dee9;
            selection-background-color: #5e81ac;
            selection-color: #eceff4;
        }
        QMainWindow, QDialog, QMessageBox, QDockWidget {
            background: #2e3440;
        }
        QMenuBar {
            background: #3b4252;
            border-bottom: 1px solid #434c5e;
            padding: 3px 8px;
        }
        QMenuBar::item {
            background: transparent;
            border-radius: 6px;
            padding: 5px 10px;
            margin: 2px;
        }
        QMenuBar::item:selected {
            background: #434c5e;
            color: #eceff4;
        }
        QMenu {
            background: #3b4252;
            border: 1px solid #434c5e;
            border-radius: 8px;
            padding: 6px;
        }
        QMenu::item {
            border-radius: 5px;
            padding: 6px 28px 6px 12px;
        }
        QMenu::item:selected {
            background: #5e81ac;
            color: #eceff4;
        }
        QMenu::separator {
            height: 1px;
            background: #434c5e;
            margin: 5px 8px;
        }
        QToolBar {
            background: #3b4252;
            border: 0;
            border-bottom: 1px solid #434c5e;
            spacing: 6px;
            padding: 6px 10px;
        }
        QToolBar::separator {
            background: #434c5e;
            width: 1px;
            margin: 5px 4px;
        }
        QLabel#toolbarLabel {
            color: #7b88a1;
            font-size: 11px;
            font-weight: 700;
            padding: 0 2px 0 8px;
        }
        QLabel#sectionTitle {
            color: #81a1c1;
            font-size: 11px;
            font-weight: 700;
            padding: 8px 0 2px 0;
        }
        QLabel#dialogTitle {
            color: #eceff4;
            font-size: 18px;
            font-weight: 700;
        }
        QLabel#statusSummary {
            color: #7b88a1;
            font-size: 12px;
        }
        QToolButton, QPushButton {
            background: #3b4252;
            color: #d8dee9;
            border: 1px solid #4c566a;
            border-radius: 7px;
            padding: 6px 12px;
            min-height: 26px;
        }
        QToolButton:hover, QPushButton:hover {
            background: #434c5e;
            border-color: #5e81ac;
        }
        QToolButton:pressed, QPushButton:pressed {
            background: #4c566a;
            border-color: #5e81ac;
        }
        QToolButton:checked {
            background: #5e81ac;
            border-color: #5e81ac;
            color: #eceff4;
        }
        QPushButton#nextBestActionButton {
            background: #5e81ac;
            color: #eceff4;
            border-color: #4c6a8e;
            font-weight: 700;
        }
        QPushButton#nextBestActionButton[severity="ok"] {
            background: #a3be8c;
            border-color: #8fad72;
            color: #2e3440;
        }
        QPushButton#nextBestActionButton[severity="action"] {
            background: #5e81ac;
            border-color: #4c6a8e;
        }
        QPushButton#nextBestActionButton[severity="warning"] {
            background: #ebcb8b;
            color: #2e3440;
            border-color: #d4a954;
        }
        QPushButton#nextBestActionButton[severity="error"] {
            background: #bf616a;
            border-color: #a94e57;
            color: #eceff4;
        }
        QPushButton#nextBestActionButton[severity="neutral"] {
            background: #4c566a;
            border-color: #434c5e;
        }
        QPushButton:disabled, QToolButton:disabled {
            background: #3b4252;
            color: #4c566a;
            border-color: #434c5e;
        }
        QComboBox, QLineEdit, QSpinBox, QDoubleSpinBox {
            background: #3b4252;
            color: #d8dee9;
            border: 1px solid #4c566a;
            border-radius: 7px;
            padding: 5px 10px;
            min-height: 26px;
        }
        QComboBox:hover, QLineEdit:hover, QSpinBox:hover, QDoubleSpinBox:hover {
            border-color: #5e81ac;
        }
        QComboBox:focus, QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
            border-color: #88c0d0;
            background: #3b4252;
        }
        QLineEdit:read-only {
            background: #2e3440;
            color: #7b88a1;
        }
        QComboBox::drop-down {
            border: 0;
            width: 24px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #7b88a1;
            margin-right: 8px;
        }
        QTabWidget::pane {
            background: #2e3440;
            border: 1px solid #434c5e;
            border-top: 0;
        }
        QTabBar::tab {
            background: #3b4252;
            color: #7b88a1;
            border: 1px solid #434c5e;
            border-bottom: 0;
            padding: 7px 16px;
            margin-right: 2px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
        }
        QTabBar::tab:selected {
            background: #2e3440;
            color: #eceff4;
            border-bottom: 2px solid #88c0d0;
        }
        QTabBar::tab:hover:!selected {
            background: #434c5e;
            color: #d8dee9;
        }
        QTableWidget, QTableView {
            background: #2e3440;
            alternate-background-color: #3b4252;
            gridline-color: #3b4252;
            border: 1px solid #434c5e;
            selection-background-color: #4c566a;
            selection-color: #88c0d0;
        }
        QTableWidget::item, QTableView::item {
            padding: 4px 8px;
            border: 0;
        }
        QTableWidget::item:selected, QTableView::item:selected {
            background: #4c566a;
            color: #88c0d0;
        }
        QHeaderView::section {
            background: #3b4252;
            color: #81a1c1;
            border: 0;
            border-bottom: 1px solid #434c5e;
            border-right: 1px solid #434c5e;
            padding: 6px 8px;
            font-weight: 600;
            font-size: 11px;
        }
        QScrollBar:vertical {
            background: #2e3440;
            width: 10px;
            border: 0;
        }
        QScrollBar::handle:vertical {
            background: #4c566a;
            border-radius: 5px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background: #88c0d0;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar:horizontal {
            background: #2e3440;
            height: 10px;
            border: 0;
        }
        QScrollBar::handle:horizontal {
            background: #4c566a;
            border-radius: 5px;
            min-width: 30px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #88c0d0;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
        QCheckBox {
            color: #d8dee9;
            spacing: 6px;
        }
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 1px solid #4c566a;
            border-radius: 4px;
            background: #3b4252;
        }
        QCheckBox::indicator:checked {
            background: #88c0d0;
            border-color: #88c0d0;
        }
        QGroupBox {
            border: 1px solid #434c5e;
            border-radius: 8px;
            margin-top: 8px;
            padding-top: 16px;
            color: #81a1c1;
            font-weight: 600;
        }
        QSplitter::handle {
            background: #434c5e;
        }
        QStatusBar {
            background: #3b4252;
            border-top: 1px solid #434c5e;
            color: #7b88a1;
            font-size: 11px;
        }
        QTreeWidget, QTreeView {
            background: #2e3440;
            alternate-background-color: #3b4252;
            border: 1px solid #434c5e;
            color: #d8dee9;
        }
        QTreeWidget::item:selected, QTreeView::item:selected {
            background: #4c566a;
            color: #88c0d0;
        }
        QListWidget {
            background: #2e3440;
            border: 1px solid #434c5e;
            color: #d8dee9;
        }
        QListWidget::item:selected {
            background: #4c566a;
            color: #88c0d0;
        }
        QTextBrowser, QTextEdit, QPlainTextEdit {
            background: #2e3440;
            color: #d8dee9;
            border: 1px solid #434c5e;
            border-radius: 6px;
        }
        QProgressBar {
            background: #3b4252;
            border: 1px solid #434c5e;
            border-radius: 4px;
            text-align: center;
            color: #d8dee9;
        }
        QProgressBar::chunk {
            background: #88c0d0;
            border-radius: 3px;
        }
        QToolTip {
            background: #3b4252;
            color: #eceff4;
            border: 1px solid #88c0d0;
            border-radius: 4px;
            padding: 4px 8px;
        }
    
        QFrame {
            background: #2e3440;
            border: none;
            border-radius: 0px;
        }
        QFrame#metricCard {
            background: #3b4252;
            border: 1px solid #434c5e;
            border-radius: 8px;
        }
        QWidget {
            background: #2e3440;
        }
)QSS");
}

/* ── Catppuccin Mocha Theme ──────────────────────────────────────────
   Warm dark theme from catppuccin.com. Mocha variant.
   Mauve accent on dark crust/mantle backgrounds. */
QString qssCatppuccin()
{
    return QStringLiteral(R"QSS(
        QWidget {
            color: #cdd6f4;
            selection-background-color: #cba6f7;
            selection-color: #1e1e2e;
        }
        QMainWindow, QDialog, QMessageBox, QDockWidget {
            background: #1e1e2e;
        }
        QMenuBar {
            background: #181825;
            border-bottom: 1px solid #313244;
            padding: 3px 8px;
        }
        QMenuBar::item {
            background: transparent;
            border-radius: 6px;
            padding: 5px 10px;
            margin: 2px;
        }
        QMenuBar::item:selected {
            background: #313244;
            color: #cdd6f4;
        }
        QMenu {
            background: #181825;
            border: 1px solid #313244;
            border-radius: 8px;
            padding: 6px;
        }
        QMenu::item {
            border-radius: 5px;
            padding: 6px 28px 6px 12px;
        }
        QMenu::item:selected {
            background: #45475a;
            color: #cba6f7;
        }
        QMenu::separator {
            height: 1px;
            background: #313244;
            margin: 5px 8px;
        }
        QToolBar {
            background: #181825;
            border: 0;
            border-bottom: 1px solid #313244;
            spacing: 6px;
            padding: 6px 10px;
        }
        QToolBar::separator {
            background: #313244;
            width: 1px;
            margin: 5px 4px;
        }
        QLabel#toolbarLabel {
            color: #6c7086;
            font-size: 11px;
            font-weight: 700;
            padding: 0 2px 0 8px;
        }
        QLabel#sectionTitle {
            color: #a6adc8;
            font-size: 11px;
            font-weight: 700;
            padding: 8px 0 2px 0;
        }
        QLabel#dialogTitle {
            color: #cdd6f4;
            font-size: 18px;
            font-weight: 700;
        }
        QLabel#statusSummary {
            color: #6c7086;
            font-size: 12px;
        }
        QToolButton, QPushButton {
            background: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 7px;
            padding: 6px 12px;
            min-height: 26px;
        }
        QToolButton:hover, QPushButton:hover {
            background: #45475a;
            border-color: #cba6f7;
        }
        QToolButton:pressed, QPushButton:pressed {
            background: #585b70;
            border-color: #cba6f7;
        }
        QToolButton:checked {
            background: #cba6f7;
            border-color: #cba6f7;
            color: #1e1e2e;
        }
        QPushButton#nextBestActionButton {
            background: #cba6f7;
            color: #1e1e2e;
            border-color: #b4befe;
            font-weight: 700;
        }
        QPushButton#nextBestActionButton[severity="ok"] {
            background: #a6e3a1;
            border-color: #94e2d5;
            color: #1e1e2e;
        }
        QPushButton#nextBestActionButton[severity="action"] {
            background: #89b4fa;
            border-color: #74c7ec;
            color: #1e1e2e;
        }
        QPushButton#nextBestActionButton[severity="warning"] {
            background: #f9e2af;
            color: #1e1e2e;
            border-color: #fab387;
        }
        QPushButton#nextBestActionButton[severity="error"] {
            background: #f38ba8;
            border-color: #eba0ac;
            color: #1e1e2e;
        }
        QPushButton#nextBestActionButton[severity="neutral"] {
            background: #585b70;
            border-color: #45475a;
        }
        QPushButton:disabled, QToolButton:disabled {
            background: #313244;
            color: #585b70;
            border-color: #313244;
        }
        QComboBox, QLineEdit, QSpinBox, QDoubleSpinBox {
            background: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 7px;
            padding: 5px 10px;
            min-height: 26px;
        }
        QComboBox:hover, QLineEdit:hover, QSpinBox:hover, QDoubleSpinBox:hover {
            border-color: #cba6f7;
        }
        QComboBox:focus, QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
            border-color: #cba6f7;
            background: #313244;
        }
        QLineEdit:read-only {
            background: #1e1e2e;
            color: #6c7086;
        }
        QComboBox::drop-down {
            border: 0;
            width: 24px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #6c7086;
            margin-right: 8px;
        }
        QTabWidget::pane {
            background: #1e1e2e;
            border: 1px solid #313244;
            border-top: 0;
        }
        QTabBar::tab {
            background: #181825;
            color: #6c7086;
            border: 1px solid #313244;
            border-bottom: 0;
            padding: 7px 16px;
            margin-right: 2px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
        }
        QTabBar::tab:selected {
            background: #1e1e2e;
            color: #cdd6f4;
            border-bottom: 2px solid #cba6f7;
        }
        QTabBar::tab:hover:!selected {
            background: #313244;
            color: #bac2de;
        }
        QTableWidget, QTableView {
            background: #1e1e2e;
            alternate-background-color: #181825;
            gridline-color: #313244;
            border: 1px solid #313244;
            selection-background-color: #45475a;
            selection-color: #cba6f7;
        }
        QTableWidget::item, QTableView::item {
            padding: 4px 8px;
            border: 0;
        }
        QTableWidget::item:selected, QTableView::item:selected {
            background: #45475a;
            color: #cba6f7;
        }
        QHeaderView::section {
            background: #181825;
            color: #a6adc8;
            border: 0;
            border-bottom: 1px solid #313244;
            border-right: 1px solid #313244;
            padding: 6px 8px;
            font-weight: 600;
            font-size: 11px;
        }
        QScrollBar:vertical {
            background: #1e1e2e;
            width: 10px;
            border: 0;
        }
        QScrollBar::handle:vertical {
            background: #45475a;
            border-radius: 5px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background: #cba6f7;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar:horizontal {
            background: #1e1e2e;
            height: 10px;
            border: 0;
        }
        QScrollBar::handle:horizontal {
            background: #45475a;
            border-radius: 5px;
            min-width: 30px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #cba6f7;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
        QCheckBox {
            color: #cdd6f4;
            spacing: 6px;
        }
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 1px solid #45475a;
            border-radius: 4px;
            background: #313244;
        }
        QCheckBox::indicator:checked {
            background: #cba6f7;
            border-color: #cba6f7;
        }
        QGroupBox {
            border: 1px solid #313244;
            border-radius: 8px;
            margin-top: 8px;
            padding-top: 16px;
            color: #a6adc8;
            font-weight: 600;
        }
        QSplitter::handle {
            background: #313244;
        }
        QStatusBar {
            background: #181825;
            border-top: 1px solid #313244;
            color: #6c7086;
            font-size: 11px;
        }
        QTreeWidget, QTreeView {
            background: #1e1e2e;
            alternate-background-color: #181825;
            border: 1px solid #313244;
            color: #cdd6f4;
        }
        QTreeWidget::item:selected, QTreeView::item:selected {
            background: #45475a;
            color: #cba6f7;
        }
        QListWidget {
            background: #1e1e2e;
            border: 1px solid #313244;
            color: #cdd6f4;
        }
        QListWidget::item:selected {
            background: #45475a;
            color: #cba6f7;
        }
        QTextBrowser, QTextEdit, QPlainTextEdit {
            background: #1e1e2e;
            color: #cdd6f4;
            border: 1px solid #313244;
            border-radius: 6px;
        }
        QProgressBar {
            background: #313244;
            border: 1px solid #45475a;
            border-radius: 4px;
            text-align: center;
            color: #cdd6f4;
        }
        QProgressBar::chunk {
            background: #cba6f7;
            border-radius: 3px;
        }
        QToolTip {
            background: #313244;
            color: #cdd6f4;
            border: 1px solid #cba6f7;
            border-radius: 4px;
            padding: 4px 8px;
        }
    
        QFrame {
            background: #1e1e2e;
            border: none;
            border-radius: 0px;
        }
        QFrame#metricCard {
            background: #181825;
            border: 1px solid #313244;
            border-radius: 8px;
        }
        QWidget {
            background: #1e1e2e;
        }
)QSS");
}

/* ── Dracula Theme ───────────────────────────────────────────────────
   Classic Dracula color scheme. Purple-tinted dark theme.
   Based on draculatheme.com. */
QString qssDracula()
{
    return QStringLiteral(R"QSS(
        QWidget {
            color: #f8f8f2;
            selection-background-color: #bd93f9;
            selection-color: #282a36;
        }
        QMainWindow, QDialog, QMessageBox, QDockWidget {
            background: #282a36;
        }
        QMenuBar {
            background: #21222c;
            border-bottom: 1px solid #44475a;
            padding: 3px 8px;
        }
        QMenuBar::item {
            background: transparent;
            border-radius: 6px;
            padding: 5px 10px;
            margin: 2px;
        }
        QMenuBar::item:selected {
            background: #44475a;
            color: #f8f8f2;
        }
        QMenu {
            background: #21222c;
            border: 1px solid #44475a;
            border-radius: 8px;
            padding: 6px;
        }
        QMenu::item {
            border-radius: 5px;
            padding: 6px 28px 6px 12px;
        }
        QMenu::item:selected {
            background: #44475a;
            color: #bd93f9;
        }
        QMenu::separator {
            height: 1px;
            background: #44475a;
            margin: 5px 8px;
        }
        QToolBar {
            background: #21222c;
            border: 0;
            border-bottom: 1px solid #44475a;
            spacing: 6px;
            padding: 6px 10px;
        }
        QToolBar::separator {
            background: #44475a;
            width: 1px;
            margin: 5px 4px;
        }
        QLabel#toolbarLabel {
            color: #6272a4;
            font-size: 11px;
            font-weight: 700;
            padding: 0 2px 0 8px;
        }
        QLabel#sectionTitle {
            color: #8be9fd;
            font-size: 11px;
            font-weight: 700;
            padding: 8px 0 2px 0;
        }
        QLabel#dialogTitle {
            color: #f8f8f2;
            font-size: 18px;
            font-weight: 700;
        }
        QLabel#statusSummary {
            color: #6272a4;
            font-size: 12px;
        }
        QToolButton, QPushButton {
            background: #282a36;
            color: #f8f8f2;
            border: 1px solid #44475a;
            border-radius: 7px;
            padding: 6px 12px;
            min-height: 26px;
        }
        QToolButton:hover, QPushButton:hover {
            background: #343746;
            border-color: #bd93f9;
        }
        QToolButton:pressed, QPushButton:pressed {
            background: #44475a;
            border-color: #bd93f9;
        }
        QToolButton:checked {
            background: #bd93f9;
            border-color: #bd93f9;
            color: #282a36;
        }
        QPushButton#nextBestActionButton {
            background: #bd93f9;
            color: #282a36;
            border-color: #a77bef;
            font-weight: 700;
        }
        QPushButton#nextBestActionButton[severity="ok"] {
            background: #50fa7b;
            border-color: #3dd673;
            color: #282a36;
        }
        QPushButton#nextBestActionButton[severity="action"] {
            background: #8be9fd;
            border-color: #6ed6ef;
            color: #282a36;
        }
        QPushButton#nextBestActionButton[severity="warning"] {
            background: #f1fa8c;
            color: #282a36;
            border-color: #e8f06a;
        }
        QPushButton#nextBestActionButton[severity="error"] {
            background: #ff5555;
            border-color: #e64545;
            color: #f8f8f2;
        }
        QPushButton#nextBestActionButton[severity="neutral"] {
            background: #44475a;
            border-color: #343746;
        }
        QPushButton:disabled, QToolButton:disabled {
            background: #282a36;
            color: #44475a;
            border-color: #343746;
        }
        QComboBox, QLineEdit, QSpinBox, QDoubleSpinBox {
            background: #282a36;
            color: #f8f8f2;
            border: 1px solid #44475a;
            border-radius: 7px;
            padding: 5px 10px;
            min-height: 26px;
        }
        QComboBox:hover, QLineEdit:hover, QSpinBox:hover, QDoubleSpinBox:hover {
            border-color: #bd93f9;
        }
        QComboBox:focus, QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
            border-color: #8be9fd;
            background: #282a36;
        }
        QLineEdit:read-only {
            background: #21222c;
            color: #6272a4;
        }
        QComboBox::drop-down {
            border: 0;
            width: 24px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #6272a4;
            margin-right: 8px;
        }
        QTabWidget::pane {
            background: #282a36;
            border: 1px solid #44475a;
            border-top: 0;
        }
        QTabBar::tab {
            background: #21222c;
            color: #6272a4;
            border: 1px solid #44475a;
            border-bottom: 0;
            padding: 7px 16px;
            margin-right: 2px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
        }
        QTabBar::tab:selected {
            background: #282a36;
            color: #f8f8f2;
            border-bottom: 2px solid #bd93f9;
        }
        QTabBar::tab:hover:!selected {
            background: #343746;
            color: #f8f8f2;
        }
        QTableWidget, QTableView {
            background: #282a36;
            alternate-background-color: #21222c;
            gridline-color: #343746;
            border: 1px solid #44475a;
            selection-background-color: #44475a;
            selection-color: #bd93f9;
        }
        QTableWidget::item, QTableView::item {
            padding: 4px 8px;
            border: 0;
        }
        QTableWidget::item:selected, QTableView::item:selected {
            background: #44475a;
            color: #bd93f9;
        }
        QHeaderView::section {
            background: #21222c;
            color: #8be9fd;
            border: 0;
            border-bottom: 1px solid #44475a;
            border-right: 1px solid #343746;
            padding: 6px 8px;
            font-weight: 600;
            font-size: 11px;
        }
        QScrollBar:vertical {
            background: #282a36;
            width: 10px;
            border: 0;
        }
        QScrollBar::handle:vertical {
            background: #44475a;
            border-radius: 5px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background: #bd93f9;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar:horizontal {
            background: #282a36;
            height: 10px;
            border: 0;
        }
        QScrollBar::handle:horizontal {
            background: #44475a;
            border-radius: 5px;
            min-width: 30px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #bd93f9;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
        QCheckBox {
            color: #f8f8f2;
            spacing: 6px;
        }
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 1px solid #44475a;
            border-radius: 4px;
            background: #282a36;
        }
        QCheckBox::indicator:checked {
            background: #bd93f9;
            border-color: #bd93f9;
        }
        QGroupBox {
            border: 1px solid #44475a;
            border-radius: 8px;
            margin-top: 8px;
            padding-top: 16px;
            color: #8be9fd;
            font-weight: 600;
        }
        QSplitter::handle {
            background: #44475a;
        }
        QStatusBar {
            background: #21222c;
            border-top: 1px solid #44475a;
            color: #6272a4;
            font-size: 11px;
        }
        QTreeWidget, QTreeView {
            background: #282a36;
            alternate-background-color: #21222c;
            border: 1px solid #44475a;
            color: #f8f8f2;
        }
        QTreeWidget::item:selected, QTreeView::item:selected {
            background: #44475a;
            color: #bd93f9;
        }
        QListWidget {
            background: #282a36;
            border: 1px solid #44475a;
            color: #f8f8f2;
        }
        QListWidget::item:selected {
            background: #44475a;
            color: #bd93f9;
        }
        QTextBrowser, QTextEdit, QPlainTextEdit {
            background: #282a36;
            color: #f8f8f2;
            border: 1px solid #44475a;
            border-radius: 6px;
        }
        QProgressBar {
            background: #343746;
            border: 1px solid #44475a;
            border-radius: 4px;
            text-align: center;
            color: #f8f8f2;
        }
        QProgressBar::chunk {
            background: #bd93f9;
            border-radius: 3px;
        }
        QToolTip {
            background: #343746;
            color: #f8f8f2;
            border: 1px solid #bd93f9;
            border-radius: 4px;
            padding: 4px 8px;
        }
    
        QFrame {
            background: #282a36;
            border: none;
            border-radius: 0px;
        }
        QFrame#metricCard {
            background: #21222c;
            border: 1px solid #44475a;
            border-radius: 8px;
        }
        QWidget {
            background: #282a36;
        }
)QSS");
}

/* ── Solarized Dark Theme ────────────────────────────────────────────
   Ethan Schoonover's Solarized palette, dark variant.
   Base03 background with solar yellow/orange accents. */
QString qssSolarized()
{
    return QStringLiteral(R"QSS(
        QWidget {
            color: #93a1a1;
            selection-background-color: #b58900;
            selection-color: #fdf6e3;
        }
        QMainWindow, QDialog, QMessageBox, QDockWidget {
            background: #002b36;
        }
        QMenuBar {
            background: #073642;
            border-bottom: 1px solid #586e75;
            padding: 3px 8px;
        }
        QMenuBar::item {
            background: transparent;
            border-radius: 6px;
            padding: 5px 10px;
            margin: 2px;
        }
        QMenuBar::item:selected {
            background: #586e75;
            color: #fdf6e3;
        }
        QMenu {
            background: #073642;
            border: 1px solid #586e75;
            border-radius: 8px;
            padding: 6px;
        }
        QMenu::item {
            border-radius: 5px;
            padding: 6px 28px 6px 12px;
        }
        QMenu::item:selected {
            background: #586e75;
            color: #b58900;
        }
        QMenu::separator {
            height: 1px;
            background: #586e75;
            margin: 5px 8px;
        }
        QToolBar {
            background: #073642;
            border: 0;
            border-bottom: 1px solid #586e75;
            spacing: 6px;
            padding: 6px 10px;
        }
        QToolBar::separator {
            background: #586e75;
            width: 1px;
            margin: 5px 4px;
        }
        QLabel#toolbarLabel {
            color: #657b83;
            font-size: 11px;
            font-weight: 700;
            padding: 0 2px 0 8px;
        }
        QLabel#sectionTitle {
            color: #268bd2;
            font-size: 11px;
            font-weight: 700;
            padding: 8px 0 2px 0;
        }
        QLabel#dialogTitle {
            color: #fdf6e3;
            font-size: 18px;
            font-weight: 700;
        }
        QLabel#statusSummary {
            color: #839496;
            font-size: 12px;
        }
        QToolButton, QPushButton {
            background: #073642;
            color: #93a1a1;
            border: 1px solid #586e75;
            border-radius: 7px;
            padding: 6px 12px;
            min-height: 26px;
        }
        QToolButton:hover, QPushButton:hover {
            background: #586e75;
            border-color: #b58900;
            color: #fdf6e3;
        }
        QToolButton:pressed, QPushButton:pressed {
            background: #657b83;
            border-color: #b58900;
        }
        QToolButton:checked {
            background: #b58900;
            border-color: #b58900;
            color: #fdf6e3;
        }
        QPushButton#nextBestActionButton {
            background: #b58900;
            color: #fdf6e3;
            border-color: #9e7c00;
            font-weight: 700;
        }
        QPushButton#nextBestActionButton[severity="ok"] {
            background: #859900;
            border-color: #738400;
            color: #fdf6e3;
        }
        QPushButton#nextBestActionButton[severity="action"] {
            background: #268bd2;
            border-color: #1a6da0;
            color: #fdf6e3;
        }
        QPushButton#nextBestActionButton[severity="warning"] {
            background: #cb4b16;
            border-color: #a94010;
            color: #fdf6e3;
        }
        QPushButton#nextBestActionButton[severity="error"] {
            background: #dc322f;
            border-color: #c02b28;
            color: #fdf6e3;
        }
        QPushButton#nextBestActionButton[severity="neutral"] {
            background: #586e75;
            border-color: #073642;
        }
        QPushButton:disabled, QToolButton:disabled {
            background: #073642;
            color: #586e75;
            border-color: #073642;
        }
        QComboBox, QLineEdit, QSpinBox, QDoubleSpinBox {
            background: #073642;
            color: #93a1a1;
            border: 1px solid #586e75;
            border-radius: 7px;
            padding: 5px 10px;
            min-height: 26px;
        }
        QComboBox:hover, QLineEdit:hover, QSpinBox:hover, QDoubleSpinBox:hover {
            border-color: #b58900;
        }
        QComboBox:focus, QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
            border-color: #268bd2;
            background: #073642;
        }
        QLineEdit:read-only {
            background: #002b36;
            color: #657b83;
        }
        QComboBox::drop-down {
            border: 0;
            width: 24px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #657b83;
            margin-right: 8px;
        }
        QTabWidget::pane {
            background: #002b36;
            border: 1px solid #586e75;
            border-top: 0;
        }
        QTabBar::tab {
            background: #073642;
            color: #657b83;
            border: 1px solid #586e75;
            border-bottom: 0;
            padding: 7px 16px;
            margin-right: 2px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
        }
        QTabBar::tab:selected {
            background: #002b36;
            color: #fdf6e3;
            border-bottom: 2px solid #b58900;
        }
        QTabBar::tab:hover:!selected {
            background: #586e75;
            color: #eee8d5;
        }
        QTableWidget, QTableView {
            background: #002b36;
            alternate-background-color: #073642;
            gridline-color: #073642;
            border: 1px solid #586e75;
            selection-background-color: #586e75;
            selection-color: #b58900;
        }
        QTableWidget::item, QTableView::item {
            padding: 4px 8px;
            border: 0;
        }
        QTableWidget::item:selected, QTableView::item:selected {
            background: #586e75;
            color: #b58900;
        }
        QHeaderView::section {
            background: #073642;
            color: #268bd2;
            border: 0;
            border-bottom: 1px solid #586e75;
            border-right: 1px solid #586e75;
            padding: 6px 8px;
            font-weight: 600;
            font-size: 11px;
        }
        QScrollBar:vertical {
            background: #002b36;
            width: 10px;
            border: 0;
        }
        QScrollBar::handle:vertical {
            background: #586e75;
            border-radius: 5px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background: #b58900;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar:horizontal {
            background: #002b36;
            height: 10px;
            border: 0;
        }
        QScrollBar::handle:horizontal {
            background: #586e75;
            border-radius: 5px;
            min-width: 30px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #b58900;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
        QCheckBox {
            color: #93a1a1;
            spacing: 6px;
        }
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 1px solid #586e75;
            border-radius: 4px;
            background: #073642;
        }
        QCheckBox::indicator:checked {
            background: #b58900;
            border-color: #b58900;
        }
        QGroupBox {
            border: 1px solid #586e75;
            border-radius: 8px;
            margin-top: 8px;
            padding-top: 16px;
            color: #268bd2;
            font-weight: 600;
        }
        QSplitter::handle {
            background: #586e75;
        }
        QStatusBar {
            background: #073642;
            border-top: 1px solid #586e75;
            color: #657b83;
            font-size: 11px;
        }
        QTreeWidget, QTreeView {
            background: #002b36;
            alternate-background-color: #073642;
            border: 1px solid #586e75;
            color: #93a1a1;
        }
        QTreeWidget::item:selected, QTreeView::item:selected {
            background: #586e75;
            color: #b58900;
        }
        QListWidget {
            background: #002b36;
            border: 1px solid #586e75;
            color: #93a1a1;
        }
        QListWidget::item:selected {
            background: #586e75;
            color: #b58900;
        }
        QTextBrowser, QTextEdit, QPlainTextEdit {
            background: #002b36;
            color: #93a1a1;
            border: 1px solid #586e75;
            border-radius: 6px;
        }
        QProgressBar {
            background: #073642;
            border: 1px solid #586e75;
            border-radius: 4px;
            text-align: center;
            color: #93a1a1;
        }
        QProgressBar::chunk {
            background: #b58900;
            border-radius: 3px;
        }
        QToolTip {
            background: #073642;
            color: #fdf6e3;
            border: 1px solid #b58900;
            border-radius: 4px;
            padding: 4px 8px;
        }
    
        QFrame {
            background: #002b36;
            border: none;
            border-radius: 0px;
        }
        QFrame#metricCard {
            background: #073642;
            border: 1px solid #586e75;
            border-radius: 8px;
        }
        QWidget {
            background: #002b36;
        }
)QSS");
}


/* ── Gruvbox Theme ───────────────────────────────────────────────────
   Retro groove warm theme. Popular with Vim/Neovim users.
   Warm dark background with orange/green/yellow accents. */
QString qssGruvbox()
{
    return QStringLiteral(R"QSS(
        QWidget {
            color: #ebdbb2;
            selection-background-color: #d65d0e;
            selection-color: #fbf1c7;
        }
        QMainWindow, QDialog, QMessageBox, QDockWidget {
            background: #282828;
        }
        QMenuBar {
            background: #1d2021;
            border-bottom: 1px solid #3c3836;
            padding: 3px 8px;
        }
        QMenuBar::item {
            background: transparent;
            border-radius: 6px;
            padding: 5px 10px;
            margin: 2px;
        }
        QMenuBar::item:selected {
            background: #3c3836;
            color: #ebdbb2;
        }
        QMenu {
            background: #1d2021;
            border: 1px solid #3c3836;
            border-radius: 8px;
            padding: 6px;
        }
        QMenu::item {
            border-radius: 5px;
            padding: 6px 28px 6px 12px;
        }
        QMenu::item:selected {
            background: #504945;
            color: #fabd2f;
        }
        QMenu::separator {
            height: 1px;
            background: #3c3836;
            margin: 5px 8px;
        }
        QToolBar {
            background: #1d2021;
            border: 0;
            border-bottom: 1px solid #3c3836;
            spacing: 6px;
            padding: 6px 10px;
        }
        QToolBar::separator {
            background: #3c3836;
            width: 1px;
            margin: 5px 4px;
        }
        QLabel#toolbarLabel {
            color: #928374;
            font-size: 11px;
            font-weight: 700;
            padding: 0 2px 0 8px;
        }
        QLabel#sectionTitle {
            color: #b8bb26;
            font-size: 11px;
            font-weight: 700;
            padding: 8px 0 2px 0;
        }
        QLabel#dialogTitle {
            color: #fbf1c7;
            font-size: 18px;
            font-weight: 700;
        }
        QLabel#statusSummary {
            color: #928374;
            font-size: 12px;
        }
        QToolButton, QPushButton {
            background: #3c3836;
            color: #ebdbb2;
            border: 1px solid #504945;
            border-radius: 7px;
            padding: 6px 12px;
            min-height: 26px;
        }
        QToolButton:hover, QPushButton:hover {
            background: #504945;
            border-color: #d65d0e;
        }
        QToolButton:pressed, QPushButton:pressed {
            background: #665c54;
            border-color: #d65d0e;
        }
        QToolButton:checked {
            background: #d65d0e;
            border-color: #d65d0e;
            color: #fbf1c7;
        }
        QPushButton#nextBestActionButton {
            background: #d65d0e;
            color: #fbf1c7;
            border-color: #b54900;
            font-weight: 700;
        }
        QPushButton#nextBestActionButton[severity="ok"] {
            background: #b8bb26;
            border-color: #98971a;
            color: #282828;
        }
        QPushButton#nextBestActionButton[severity="action"] {
            background: #458588;
            border-color: #076678;
            color: #fbf1c7;
        }
        QPushButton#nextBestActionButton[severity="warning"] {
            background: #fabd2f;
            color: #282828;
            border-color: #d79921;
        }
        QPushButton#nextBestActionButton[severity="error"] {
            background: #cc241d;
            border-color: #9d0006;
            color: #fbf1c7;
        }
        QPushButton#nextBestActionButton[severity="neutral"] {
            background: #504945;
            border-color: #3c3836;
        }
        QPushButton:disabled, QToolButton:disabled {
            background: #3c3836;
            color: #504945;
            border-color: #3c3836;
        }
        QComboBox, QLineEdit, QSpinBox, QDoubleSpinBox {
            background: #3c3836;
            color: #ebdbb2;
            border: 1px solid #504945;
            border-radius: 7px;
            padding: 5px 10px;
            min-height: 26px;
        }
        QComboBox:hover, QLineEdit:hover, QSpinBox:hover, QDoubleSpinBox:hover {
            border-color: #d65d0e;
        }
        QComboBox:focus, QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
            border-color: #d65d0e;
            background: #3c3836;
        }
        QLineEdit:read-only {
            background: #282828;
            color: #928374;
        }
        QComboBox::drop-down { border: 0; width: 24px; }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #928374;
            margin-right: 8px;
        }
        QTabWidget::pane {
            background: #282828;
            border: 1px solid #3c3836;
            border-top: 0;
        }
        QTabBar::tab {
            background: #1d2021;
            color: #928374;
            border: 1px solid #3c3836;
            border-bottom: 0;
            padding: 7px 16px;
            margin-right: 2px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
        }
        QTabBar::tab:selected {
            background: #282828;
            color: #ebdbb2;
            border-bottom: 2px solid #d65d0e;
        }
        QTabBar::tab:hover:!selected {
            background: #3c3836;
            color: #ebdbb2;
        }
        QTableWidget, QTableView {
            background: #282828;
            alternate-background-color: #1d2021;
            gridline-color: #3c3836;
            border: 1px solid #3c3836;
            selection-background-color: #504945;
            selection-color: #fabd2f;
        }
        QTableWidget::item, QTableView::item { padding: 4px 8px; border: 0; }
        QTableWidget::item:selected, QTableView::item:selected {
            background: #504945;
            color: #fabd2f;
        }
        QHeaderView::section {
            background: #1d2021;
            color: #b8bb26;
            border: 0;
            border-bottom: 1px solid #3c3836;
            border-right: 1px solid #3c3836;
            padding: 6px 8px;
            font-weight: 600;
            font-size: 11px;
        }
        QScrollBar:vertical { background: #282828; width: 10px; border: 0; }
        QScrollBar::handle:vertical { background: #504945; border-radius: 5px; min-height: 30px; }
        QScrollBar::handle:vertical:hover { background: #d65d0e; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
        QScrollBar:horizontal { background: #282828; height: 10px; border: 0; }
        QScrollBar::handle:horizontal { background: #504945; border-radius: 5px; min-width: 30px; }
        QScrollBar::handle:horizontal:hover { background: #d65d0e; }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
        QCheckBox { color: #ebdbb2; spacing: 6px; }
        QCheckBox::indicator {
            width: 16px; height: 16px;
            border: 1px solid #504945; border-radius: 4px; background: #3c3836;
        }
        QCheckBox::indicator:checked { background: #d65d0e; border-color: #d65d0e; }
        QGroupBox {
            border: 1px solid #3c3836; border-radius: 8px;
            margin-top: 8px; padding-top: 16px;
            color: #b8bb26; font-weight: 600;
        }
        QSplitter::handle { background: #3c3836; }
        QStatusBar {
            background: #1d2021; border-top: 1px solid #3c3836;
            color: #928374; font-size: 11px;
        }
        QTreeWidget, QTreeView {
            background: #282828; alternate-background-color: #1d2021;
            border: 1px solid #3c3836; color: #ebdbb2;
        }
        QTreeWidget::item:selected, QTreeView::item:selected {
            background: #504945; color: #fabd2f;
        }
        QListWidget {
            background: #282828; border: 1px solid #3c3836; color: #ebdbb2;
        }
        QListWidget::item:selected { background: #504945; color: #fabd2f; }
        QTextBrowser, QTextEdit, QPlainTextEdit {
            background: #282828; color: #ebdbb2;
            border: 1px solid #3c3836; border-radius: 6px;
        }
        QProgressBar {
            background: #3c3836; border: 1px solid #504945;
            border-radius: 4px; text-align: center; color: #ebdbb2;
        }
        QProgressBar::chunk { background: #d65d0e; border-radius: 3px; }
        QToolTip {
            background: #3c3836; color: #ebdbb2;
            border: 1px solid #d65d0e; border-radius: 4px; padding: 4px 8px;
        }
    
        QFrame {
            background: #282828;
            border: none;
            border-radius: 0px;
        }
        QFrame#metricCard {
            background: #1d2021;
            border: 1px solid #3c3836;
            border-radius: 8px;
        }
        QWidget {
            background: #282828;
        }
)QSS");
}

/* ── Tokyo Night Theme ───────────────────────────────────────────────
   Clean dark theme inspired by Tokyo city lights at night.
   Deep blue-purple background with soft blue/purple accents. */
QString qssTokyoNight()
{
    return QStringLiteral(R"QSS(
        QWidget {
            color: #c0caf5;
            selection-background-color: #7aa2f7;
            selection-color: #1a1b26;
        }
        QMainWindow, QDialog, QMessageBox, QDockWidget {
            background: #1a1b26;
        }
        QMenuBar {
            background: #16161e;
            border-bottom: 1px solid #292e42;
            padding: 3px 8px;
        }
        QMenuBar::item { background: transparent; border-radius: 6px; padding: 5px 10px; margin: 2px; }
        QMenuBar::item:selected { background: #292e42; color: #c0caf5; }
        QMenu {
            background: #16161e; border: 1px solid #292e42;
            border-radius: 8px; padding: 6px;
        }
        QMenu::item { border-radius: 5px; padding: 6px 28px 6px 12px; }
        QMenu::item:selected { background: #292e42; color: #7aa2f7; }
        QMenu::separator { height: 1px; background: #292e42; margin: 5px 8px; }
        QToolBar {
            background: #16161e; border: 0;
            border-bottom: 1px solid #292e42; spacing: 6px; padding: 6px 10px;
        }
        QToolBar::separator { background: #292e42; width: 1px; margin: 5px 4px; }
        QLabel#toolbarLabel { color: #565f89; font-size: 11px; font-weight: 700; padding: 0 2px 0 8px; }
        QLabel#sectionTitle { color: #7dcfff; font-size: 11px; font-weight: 700; padding: 8px 0 2px 0; }
        QLabel#dialogTitle { color: #c0caf5; font-size: 18px; font-weight: 700; }
        QLabel#statusSummary { color: #565f89; font-size: 12px; }
        QToolButton, QPushButton {
            background: #1a1b26; color: #c0caf5;
            border: 1px solid #292e42; border-radius: 7px;
            padding: 6px 12px; min-height: 26px;
        }
        QToolButton:hover, QPushButton:hover { background: #292e42; border-color: #7aa2f7; }
        QToolButton:pressed, QPushButton:pressed { background: #3b4261; border-color: #7aa2f7; }
        QToolButton:checked { background: #7aa2f7; border-color: #7aa2f7; color: #1a1b26; }
        QPushButton#nextBestActionButton {
            background: #7aa2f7; color: #1a1b26;
            border-color: #5d87e8; font-weight: 700;
        }
        QPushButton#nextBestActionButton[severity="ok"] {
            background: #9ece6a; border-color: #7fbe4a; color: #1a1b26;
        }
        QPushButton#nextBestActionButton[severity="action"] {
            background: #7aa2f7; border-color: #5d87e8; color: #1a1b26;
        }
        QPushButton#nextBestActionButton[severity="warning"] {
            background: #e0af68; color: #1a1b26; border-color: #c99a50;
        }
        QPushButton#nextBestActionButton[severity="error"] {
            background: #f7768e; border-color: #e05c74; color: #1a1b26;
        }
        QPushButton#nextBestActionButton[severity="neutral"] {
            background: #3b4261; border-color: #292e42;
        }
        QPushButton:disabled, QToolButton:disabled {
            background: #1a1b26; color: #3b4261; border-color: #292e42;
        }
        QComboBox, QLineEdit, QSpinBox, QDoubleSpinBox {
            background: #1a1b26; color: #c0caf5;
            border: 1px solid #292e42; border-radius: 7px;
            padding: 5px 10px; min-height: 26px;
        }
        QComboBox:hover, QLineEdit:hover, QSpinBox:hover, QDoubleSpinBox:hover {
            border-color: #7aa2f7;
        }
        QComboBox:focus, QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
            border-color: #7aa2f7; background: #1a1b26;
        }
        QLineEdit:read-only { background: #16161e; color: #565f89; }
        QComboBox::drop-down { border: 0; width: 24px; }
        QComboBox::down-arrow {
            image: none; border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #565f89; margin-right: 8px;
        }
        QTabWidget::pane { background: #1a1b26; border: 1px solid #292e42; border-top: 0; }
        QTabBar::tab {
            background: #16161e; color: #565f89;
            border: 1px solid #292e42; border-bottom: 0;
            padding: 7px 16px; margin-right: 2px;
            border-top-left-radius: 6px; border-top-right-radius: 6px;
        }
        QTabBar::tab:selected {
            background: #1a1b26; color: #c0caf5;
            border-bottom: 2px solid #7aa2f7;
        }
        QTabBar::tab:hover:!selected { background: #292e42; color: #a9b1d6; }
        QTableWidget, QTableView {
            background: #1a1b26; alternate-background-color: #16161e;
            gridline-color: #292e42; border: 1px solid #292e42;
            selection-background-color: #3b4261; selection-color: #7aa2f7;
        }
        QTableWidget::item, QTableView::item { padding: 4px 8px; border: 0; }
        QTableWidget::item:selected, QTableView::item:selected {
            background: #3b4261; color: #7aa2f7;
        }
        QHeaderView::section {
            background: #16161e; color: #7dcfff;
            border: 0; border-bottom: 1px solid #292e42;
            border-right: 1px solid #292e42;
            padding: 6px 8px; font-weight: 600; font-size: 11px;
        }
        QScrollBar:vertical { background: #1a1b26; width: 10px; border: 0; }
        QScrollBar::handle:vertical { background: #3b4261; border-radius: 5px; min-height: 30px; }
        QScrollBar::handle:vertical:hover { background: #7aa2f7; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
        QScrollBar:horizontal { background: #1a1b26; height: 10px; border: 0; }
        QScrollBar::handle:horizontal { background: #3b4261; border-radius: 5px; min-width: 30px; }
        QScrollBar::handle:horizontal:hover { background: #7aa2f7; }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
        QCheckBox { color: #c0caf5; spacing: 6px; }
        QCheckBox::indicator {
            width: 16px; height: 16px;
            border: 1px solid #292e42; border-radius: 4px; background: #1a1b26;
        }
        QCheckBox::indicator:checked { background: #7aa2f7; border-color: #7aa2f7; }
        QGroupBox {
            border: 1px solid #292e42; border-radius: 8px;
            margin-top: 8px; padding-top: 16px;
            color: #7dcfff; font-weight: 600;
        }
        QSplitter::handle { background: #292e42; }
        QStatusBar {
            background: #16161e; border-top: 1px solid #292e42;
            color: #565f89; font-size: 11px;
        }
        QTreeWidget, QTreeView {
            background: #1a1b26; alternate-background-color: #16161e;
            border: 1px solid #292e42; color: #c0caf5;
        }
        QTreeWidget::item:selected, QTreeView::item:selected {
            background: #3b4261; color: #7aa2f7;
        }
        QListWidget { background: #1a1b26; border: 1px solid #292e42; color: #c0caf5; }
        QListWidget::item:selected { background: #3b4261; color: #7aa2f7; }
        QTextBrowser, QTextEdit, QPlainTextEdit {
            background: #1a1b26; color: #c0caf5;
            border: 1px solid #292e42; border-radius: 6px;
        }
        QProgressBar {
            background: #292e42; border: 1px solid #3b4261;
            border-radius: 4px; text-align: center; color: #c0caf5;
        }
        QProgressBar::chunk { background: #7aa2f7; border-radius: 3px; }
        QToolTip {
            background: #292e42; color: #c0caf5;
            border: 1px solid #7aa2f7; border-radius: 4px; padding: 4px 8px;
        }
    
        QFrame {
            background: #1a1b26;
            border: none;
            border-radius: 0px;
        }
        QFrame#metricCard {
            background: #16161e;
            border: 1px solid #292e42;
            border-radius: 8px;
        }
        QWidget {
            background: #1a1b26;
        }
)QSS");
}

/* ── One Dark Theme ──────────────────────────────────────────────────
   Atom One Dark color scheme. Balanced dark theme with
   red/green/yellow/blue/purple accents on neutral gray. */
QString qssOneDark()
{
    return QStringLiteral(R"QSS(
        QWidget {
            color: #abb2bf;
            selection-background-color: #3e4451;
            selection-color: #528bff;
        }
        QMainWindow, QDialog, QMessageBox, QDockWidget {
            background: #282c34;
        }
        QMenuBar {
            background: #21252b;
            border-bottom: 1px solid #3e4451;
            padding: 3px 8px;
        }
        QMenuBar::item { background: transparent; border-radius: 6px; padding: 5px 10px; margin: 2px; }
        QMenuBar::item:selected { background: #3e4451; color: #abb2bf; }
        QMenu {
            background: #21252b; border: 1px solid #3e4451;
            border-radius: 8px; padding: 6px;
        }
        QMenu::item { border-radius: 5px; padding: 6px 28px 6px 12px; }
        QMenu::item:selected { background: #3e4451; color: #528bff; }
        QMenu::separator { height: 1px; background: #3e4451; margin: 5px 8px; }
        QToolBar {
            background: #21252b; border: 0;
            border-bottom: 1px solid #3e4451; spacing: 6px; padding: 6px 10px;
        }
        QToolBar::separator { background: #3e4451; width: 1px; margin: 5px 4px; }
        QLabel#toolbarLabel { color: #5c6370; font-size: 11px; font-weight: 700; padding: 0 2px 0 8px; }
        QLabel#sectionTitle { color: #56b6c2; font-size: 11px; font-weight: 700; padding: 8px 0 2px 0; }
        QLabel#dialogTitle { color: #abb2bf; font-size: 18px; font-weight: 700; }
        QLabel#statusSummary { color: #5c6370; font-size: 12px; }
        QToolButton, QPushButton {
            background: #2c313a; color: #abb2bf;
            border: 1px solid #3e4451; border-radius: 7px;
            padding: 6px 12px; min-height: 26px;
        }
        QToolButton:hover, QPushButton:hover { background: #3e4451; border-color: #528bff; }
        QToolButton:pressed, QPushButton:pressed { background: #4b5263; border-color: #528bff; }
        QToolButton:checked { background: #528bff; border-color: #528bff; color: #ffffff; }
        QPushButton#nextBestActionButton {
            background: #528bff; color: #ffffff;
            border-color: #3d7aed; font-weight: 700;
        }
        QPushButton#nextBestActionButton[severity="ok"] {
            background: #98c379; border-color: #7db35c; color: #282c34;
        }
        QPushButton#nextBestActionButton[severity="action"] {
            background: #528bff; border-color: #3d7aed; color: #ffffff;
        }
        QPushButton#nextBestActionButton[severity="warning"] {
            background: #e5c07b; color: #282c34; border-color: #d1a95d;
        }
        QPushButton#nextBestActionButton[severity="error"] {
            background: #e06c75; border-color: #c85a63; color: #ffffff;
        }
        QPushButton#nextBestActionButton[severity="neutral"] {
            background: #3e4451; border-color: #2c313a;
        }
        QPushButton:disabled, QToolButton:disabled {
            background: #282c34; color: #3e4451; border-color: #2c313a;
        }
        QComboBox, QLineEdit, QSpinBox, QDoubleSpinBox {
            background: #282c34; color: #abb2bf;
            border: 1px solid #3e4451; border-radius: 7px;
            padding: 5px 10px; min-height: 26px;
        }
        QComboBox:hover, QLineEdit:hover, QSpinBox:hover, QDoubleSpinBox:hover {
            border-color: #528bff;
        }
        QComboBox:focus, QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
            border-color: #528bff; background: #282c34;
        }
        QLineEdit:read-only { background: #21252b; color: #5c6370; }
        QComboBox::drop-down { border: 0; width: 24px; }
        QComboBox::down-arrow {
            image: none; border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #5c6370; margin-right: 8px;
        }
        QTabWidget::pane { background: #282c34; border: 1px solid #3e4451; border-top: 0; }
        QTabBar::tab {
            background: #21252b; color: #5c6370;
            border: 1px solid #3e4451; border-bottom: 0;
            padding: 7px 16px; margin-right: 2px;
            border-top-left-radius: 6px; border-top-right-radius: 6px;
        }
        QTabBar::tab:selected {
            background: #282c34; color: #abb2bf;
            border-bottom: 2px solid #528bff;
        }
        QTabBar::tab:hover:!selected { background: #3e4451; color: #abb2bf; }
        QTableWidget, QTableView {
            background: #282c34; alternate-background-color: #21252b;
            gridline-color: #3e4451; border: 1px solid #3e4451;
            selection-background-color: #3e4451; selection-color: #528bff;
        }
        QTableWidget::item, QTableView::item { padding: 4px 8px; border: 0; }
        QTableWidget::item:selected, QTableView::item:selected {
            background: #3e4451; color: #528bff;
        }
        QHeaderView::section {
            background: #21252b; color: #56b6c2;
            border: 0; border-bottom: 1px solid #3e4451;
            border-right: 1px solid #3e4451;
            padding: 6px 8px; font-weight: 600; font-size: 11px;
        }
        QScrollBar:vertical { background: #282c34; width: 10px; border: 0; }
        QScrollBar::handle:vertical { background: #3e4451; border-radius: 5px; min-height: 30px; }
        QScrollBar::handle:vertical:hover { background: #528bff; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
        QScrollBar:horizontal { background: #282c34; height: 10px; border: 0; }
        QScrollBar::handle:horizontal { background: #3e4451; border-radius: 5px; min-width: 30px; }
        QScrollBar::handle:horizontal:hover { background: #528bff; }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
        QCheckBox { color: #abb2bf; spacing: 6px; }
        QCheckBox::indicator {
            width: 16px; height: 16px;
            border: 1px solid #3e4451; border-radius: 4px; background: #282c34;
        }
        QCheckBox::indicator:checked { background: #528bff; border-color: #528bff; }
        QGroupBox {
            border: 1px solid #3e4451; border-radius: 8px;
            margin-top: 8px; padding-top: 16px;
            color: #56b6c2; font-weight: 600;
        }
        QSplitter::handle { background: #3e4451; }
        QStatusBar {
            background: #21252b; border-top: 1px solid #3e4451;
            color: #5c6370; font-size: 11px;
        }
        QTreeWidget, QTreeView {
            background: #282c34; alternate-background-color: #21252b;
            border: 1px solid #3e4451; color: #abb2bf;
        }
        QTreeWidget::item:selected, QTreeView::item:selected {
            background: #3e4451; color: #528bff;
        }
        QListWidget { background: #282c34; border: 1px solid #3e4451; color: #abb2bf; }
        QListWidget::item:selected { background: #3e4451; color: #528bff; }
        QTextBrowser, QTextEdit, QPlainTextEdit {
            background: #282c34; color: #abb2bf;
            border: 1px solid #3e4451; border-radius: 6px;
        }
        QProgressBar {
            background: #3e4451; border: 1px solid #4b5263;
            border-radius: 4px; text-align: center; color: #abb2bf;
        }
        QProgressBar::chunk { background: #528bff; border-radius: 3px; }
        QToolTip {
            background: #3e4451; color: #abb2bf;
            border: 1px solid #528bff; border-radius: 4px; padding: 4px 8px;
        }
    
        QFrame {
            background: #282c34;
            border: none;
            border-radius: 0px;
        }
        QFrame#metricCard {
            background: #21252b;
            border: 1px solid #3e4451;
            border-radius: 8px;
        }
        QWidget {
            background: #282c34;
        }
)QSS");
}

/* ── Monokai Pro Theme ───────────────────────────────────────────────
   Classic Sublime Text Monokai palette. Warm dark background
   with signature pink/green/yellow/orange accent colors. */
QString qssMonokai()
{
    return QStringLiteral(R"QSS(
        QWidget {
            color: #fcfcfa;
            selection-background-color: #75715e;
            selection-color: #f8f8f2;
        }
        QMainWindow, QDialog, QMessageBox, QDockWidget {
            background: #272822;
        }
        QMenuBar {
            background: #1e1f1c;
            border-bottom: 1px solid #49483e;
            padding: 3px 8px;
        }
        QMenuBar::item { background: transparent; border-radius: 6px; padding: 5px 10px; margin: 2px; }
        QMenuBar::item:selected { background: #49483e; color: #f8f8f2; }
        QMenu {
            background: #1e1f1c; border: 1px solid #49483e;
            border-radius: 8px; padding: 6px;
        }
        QMenu::item { border-radius: 5px; padding: 6px 28px 6px 12px; }
        QMenu::item:selected { background: #49483e; color: #f92672; }
        QMenu::separator { height: 1px; background: #49483e; margin: 5px 8px; }
        QToolBar {
            background: #1e1f1c; border: 0;
            border-bottom: 1px solid #49483e; spacing: 6px; padding: 6px 10px;
        }
        QToolBar::separator { background: #49483e; width: 1px; margin: 5px 4px; }
        QLabel#toolbarLabel { color: #75715e; font-size: 11px; font-weight: 700; padding: 0 2px 0 8px; }
        QLabel#sectionTitle { color: #a6e22e; font-size: 11px; font-weight: 700; padding: 8px 0 2px 0; }
        QLabel#dialogTitle { color: #f8f8f2; font-size: 18px; font-weight: 700; }
        QLabel#statusSummary { color: #75715e; font-size: 12px; }
        QToolButton, QPushButton {
            background: #3e3d32; color: #f8f8f2;
            border: 1px solid #49483e; border-radius: 7px;
            padding: 6px 12px; min-height: 26px;
        }
        QToolButton:hover, QPushButton:hover { background: #49483e; border-color: #f92672; }
        QToolButton:pressed, QPushButton:pressed { background: #75715e; border-color: #f92672; }
        QToolButton:checked { background: #f92672; border-color: #f92672; color: #272822; }
        QPushButton#nextBestActionButton {
            background: #f92672; color: #272822;
            border-color: #d41a5e; font-weight: 700;
        }
        QPushButton#nextBestActionButton[severity="ok"] {
            background: #a6e22e; border-color: #8cc417; color: #272822;
        }
        QPushButton#nextBestActionButton[severity="action"] {
            background: #66d9ef; color: #272822; border-color: #4dc9e0;
        }
        QPushButton#nextBestActionButton[severity="warning"] {
            background: #e6db74; color: #272822; border-color: #d4ca5a;
        }
        QPushButton#nextBestActionButton[severity="error"] {
            background: #f92672; border-color: #d41a5e; color: #272822;
        }
        QPushButton#nextBestActionButton[severity="neutral"] {
            background: #75715e; border-color: #49483e; color: #f8f8f2;
        }
        QPushButton:disabled, QToolButton:disabled {
            background: #272822; color: #49483e; border-color: #3e3d32;
        }
        QComboBox, QLineEdit, QSpinBox, QDoubleSpinBox {
            background: #3e3d32; color: #f8f8f2;
            border: 1px solid #49483e; border-radius: 7px;
            padding: 5px 10px; min-height: 26px;
        }
        QComboBox:hover, QLineEdit:hover, QSpinBox:hover, QDoubleSpinBox:hover {
            border-color: #f92672;
        }
        QComboBox:focus, QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
            border-color: #66d9ef; background: #3e3d32;
        }
        QLineEdit:read-only { background: #272822; color: #75715e; }
        QComboBox::drop-down { border: 0; width: 24px; }
        QComboBox::down-arrow {
            image: none; border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #75715e; margin-right: 8px;
        }
        QTabWidget::pane { background: #272822; border: 1px solid #49483e; border-top: 0; }
        QTabBar::tab {
            background: #1e1f1c; color: #75715e;
            border: 1px solid #49483e; border-bottom: 0;
            padding: 7px 16px; margin-right: 2px;
            border-top-left-radius: 6px; border-top-right-radius: 6px;
        }
        QTabBar::tab:selected {
            background: #272822; color: #f8f8f2;
            border-bottom: 2px solid #f92672;
        }
        QTabBar::tab:hover:!selected { background: #3e3d32; color: #f8f8f2; }
        QTableWidget, QTableView {
            background: #272822; alternate-background-color: #1e1f1c;
            gridline-color: #3e3d32; border: 1px solid #49483e;
            selection-background-color: #49483e; selection-color: #e6db74;
        }
        QTableWidget::item, QTableView::item { padding: 4px 8px; border: 0; }
        QTableWidget::item:selected, QTableView::item:selected {
            background: #49483e; color: #e6db74;
        }
        QHeaderView::section {
            background: #1e1f1c; color: #a6e22e;
            border: 0; border-bottom: 1px solid #49483e;
            border-right: 1px solid #49483e;
            padding: 6px 8px; font-weight: 600; font-size: 11px;
        }
        QScrollBar:vertical { background: #272822; width: 10px; border: 0; }
        QScrollBar::handle:vertical { background: #49483e; border-radius: 5px; min-height: 30px; }
        QScrollBar::handle:vertical:hover { background: #f92672; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
        QScrollBar:horizontal { background: #272822; height: 10px; border: 0; }
        QScrollBar::handle:horizontal { background: #49483e; border-radius: 5px; min-width: 30px; }
        QScrollBar::handle:horizontal:hover { background: #f92672; }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
        QCheckBox { color: #f8f8f2; spacing: 6px; }
        QCheckBox::indicator {
            width: 16px; height: 16px;
            border: 1px solid #49483e; border-radius: 4px; background: #3e3d32;
        }
        QCheckBox::indicator:checked { background: #a6e22e; border-color: #a6e22e; }
        QGroupBox {
            border: 1px solid #49483e; border-radius: 8px;
            margin-top: 8px; padding-top: 16px;
            color: #a6e22e; font-weight: 600;
        }
        QSplitter::handle { background: #49483e; }
        QStatusBar {
            background: #1e1f1c; border-top: 1px solid #49483e;
            color: #75715e; font-size: 11px;
        }
        QTreeWidget, QTreeView {
            background: #272822; alternate-background-color: #1e1f1c;
            border: 1px solid #49483e; color: #f8f8f2;
        }
        QTreeWidget::item:selected, QTreeView::item:selected {
            background: #49483e; color: #e6db74;
        }
        QListWidget { background: #272822; border: 1px solid #49483e; color: #f8f8f2; }
        QListWidget::item:selected { background: #49483e; color: #e6db74; }
        QTextBrowser, QTextEdit, QPlainTextEdit {
            background: #272822; color: #f8f8f2;
            border: 1px solid #49483e; border-radius: 6px;
        }
        QProgressBar {
            background: #3e3d32; border: 1px solid #49483e;
            border-radius: 4px; text-align: center; color: #f8f8f2;
        }
        QProgressBar::chunk { background: #f92672; border-radius: 3px; }
        QToolTip {
            background: #3e3d32; color: #f8f8f2;
            border: 1px solid #f92672; border-radius: 4px; padding: 4px 8px;
        }
    
        QFrame {
            background: #272822;
            border: none;
            border-radius: 0px;
        }
        QFrame#metricCard {
            background: #1e1f1c;
            border: 1px solid #49483e;
            border-radius: 8px;
        }
        QWidget {
            background: #272822;
        }
)QSS");
}

/* ── Cyberpunk Theme ─────────────────────────────────────────────────
   Neon-heavy futuristic dark theme. Deep black background
   with vivid cyan/magenta/yellow neon accents. */
QString qssCyberpunk()
{
    return QStringLiteral(R"QSS(
        QWidget {
            color: #e0e0e0;
            selection-background-color: #ff2a6d;
            selection-color: #0d0221;
        }
        QMainWindow, QDialog, QMessageBox, QDockWidget {
            background: #0d0221;
        }
        QMenuBar {
            background: #0a0118;
            border-bottom: 1px solid #1a0a3e;
            padding: 3px 8px;
        }
        QMenuBar::item { background: transparent; border-radius: 6px; padding: 5px 10px; margin: 2px; }
        QMenuBar::item:selected { background: #1a0a3e; color: #05d9e8; }
        QMenu {
            background: #0a0118; border: 1px solid #1a0a3e;
            border-radius: 8px; padding: 6px;
        }
        QMenu::item { border-radius: 5px; padding: 6px 28px 6px 12px; }
        QMenu::item:selected { background: #1a0a3e; color: #ff2a6d; }
        QMenu::separator { height: 1px; background: #1a0a3e; margin: 5px 8px; }
        QToolBar {
            background: #0a0118; border: 0;
            border-bottom: 1px solid #1a0a3e; spacing: 6px; padding: 6px 10px;
        }
        QToolBar::separator { background: #1a0a3e; width: 1px; margin: 5px 4px; }
        QLabel#toolbarLabel { color: #4a3f6b; font-size: 11px; font-weight: 700; padding: 0 2px 0 8px; }
        QLabel#sectionTitle { color: #05d9e8; font-size: 11px; font-weight: 700; padding: 8px 0 2px 0; }
        QLabel#dialogTitle { color: #05d9e8; font-size: 18px; font-weight: 700; }
        QLabel#statusSummary { color: #7b6fa0; font-size: 12px; }
        QToolButton, QPushButton {
            background: #1a0a3e; color: #e0e0e0;
            border: 1px solid #2a1a5e; border-radius: 7px;
            padding: 6px 12px; min-height: 26px;
        }
        QToolButton:hover, QPushButton:hover {
            background: #2a1a5e; border-color: #05d9e8;
        }
        QToolButton:pressed, QPushButton:pressed {
            background: #3a2a6e; border-color: #05d9e8;
        }
        QToolButton:checked {
            background: #05d9e8; border-color: #05d9e8; color: #0d0221;
        }
        QPushButton#nextBestActionButton {
            background: #ff2a6d; color: #0d0221;
            border-color: #d41f5a; font-weight: 700;
        }
        QPushButton#nextBestActionButton[severity="ok"] {
            background: #05d9e8; color: #0d0221; border-color: #04b8c5;
        }
        QPushButton#nextBestActionButton[severity="action"] {
            background: #05d9e8; color: #0d0221; border-color: #04b8c5;
        }
        QPushButton#nextBestActionButton[severity="warning"] {
            background: #ffe814; color: #0d0221; border-color: #d4c210;
        }
        QPushButton#nextBestActionButton[severity="error"] {
            background: #ff2a6d; border-color: #d41f5a; color: #0d0221;
        }
        QPushButton#nextBestActionButton[severity="neutral"] {
            background: #2a1a5e; border-color: #1a0a3e; color: #7b6fa0;
        }
        QPushButton:disabled, QToolButton:disabled {
            background: #0d0221; color: #2a1a5e; border-color: #1a0a3e;
        }
        QComboBox, QLineEdit, QSpinBox, QDoubleSpinBox {
            background: #1a0a3e; color: #e0e0e0;
            border: 1px solid #2a1a5e; border-radius: 7px;
            padding: 5px 10px; min-height: 26px;
        }
        QComboBox:hover, QLineEdit:hover, QSpinBox:hover, QDoubleSpinBox:hover {
            border-color: #05d9e8;
        }
        QComboBox:focus, QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
            border-color: #ff2a6d; background: #1a0a3e;
        }
        QLineEdit:read-only { background: #0d0221; color: #4a3f6b; }
        QComboBox::drop-down { border: 0; width: 24px; }
        QComboBox::down-arrow {
            image: none; border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #4a3f6b; margin-right: 8px;
        }
        QTabWidget::pane { background: #0d0221; border: 1px solid #1a0a3e; border-top: 0; }
        QTabBar::tab {
            background: #0a0118; color: #4a3f6b;
            border: 1px solid #1a0a3e; border-bottom: 0;
            padding: 7px 16px; margin-right: 2px;
            border-top-left-radius: 6px; border-top-right-radius: 6px;
        }
        QTabBar::tab:selected {
            background: #0d0221; color: #05d9e8;
            border-bottom: 2px solid #ff2a6d;
        }
        QTabBar::tab:hover:!selected { background: #1a0a3e; color: #05d9e8; }
        QTableWidget, QTableView {
            background: #0d0221; alternate-background-color: #0a0118;
            gridline-color: #1a0a3e; border: 1px solid #1a0a3e;
            selection-background-color: #2a1a5e; selection-color: #05d9e8;
        }
        QTableWidget::item, QTableView::item { padding: 4px 8px; border: 0; }
        QTableWidget::item:selected, QTableView::item:selected {
            background: #2a1a5e; color: #05d9e8;
        }
        QHeaderView::section {
            background: #0a0118; color: #05d9e8;
            border: 0; border-bottom: 1px solid #1a0a3e;
            border-right: 1px solid #1a0a3e;
            padding: 6px 8px; font-weight: 600; font-size: 11px;
        }
        QScrollBar:vertical { background: #0d0221; width: 10px; border: 0; }
        QScrollBar::handle:vertical { background: #2a1a5e; border-radius: 5px; min-height: 30px; }
        QScrollBar::handle:vertical:hover { background: #ff2a6d; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
        QScrollBar:horizontal { background: #0d0221; height: 10px; border: 0; }
        QScrollBar::handle:horizontal { background: #2a1a5e; border-radius: 5px; min-width: 30px; }
        QScrollBar::handle:horizontal:hover { background: #ff2a6d; }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
        QCheckBox { color: #e0e0e0; spacing: 6px; }
        QCheckBox::indicator {
            width: 16px; height: 16px;
            border: 1px solid #2a1a5e; border-radius: 4px; background: #1a0a3e;
        }
        QCheckBox::indicator:checked { background: #05d9e8; border-color: #05d9e8; }
        QGroupBox {
            border: 1px solid #1a0a3e; border-radius: 8px;
            margin-top: 8px; padding-top: 16px;
            color: #05d9e8; font-weight: 600;
        }
        QSplitter::handle { background: #1a0a3e; }
        QStatusBar {
            background: #0a0118; border-top: 1px solid #1a0a3e;
            color: #4a3f6b; font-size: 11px;
        }
        QTreeWidget, QTreeView {
            background: #0d0221; alternate-background-color: #0a0118;
            border: 1px solid #1a0a3e; color: #e0e0e0;
        }
        QTreeWidget::item:selected, QTreeView::item:selected {
            background: #2a1a5e; color: #05d9e8;
        }
        QListWidget { background: #0d0221; border: 1px solid #1a0a3e; color: #e0e0e0; }
        QListWidget::item:selected { background: #2a1a5e; color: #05d9e8; }
        QTextBrowser, QTextEdit, QPlainTextEdit {
            background: #0d0221; color: #e0e0e0;
            border: 1px solid #1a0a3e; border-radius: 6px;
        }
        QProgressBar {
            background: #1a0a3e; border: 1px solid #2a1a5e;
            border-radius: 4px; text-align: center; color: #e0e0e0;
        }
        QProgressBar::chunk { background: #ff2a6d; border-radius: 3px; }
        QToolTip {
            background: #1a0a3e; color: #05d9e8;
            border: 1px solid #ff2a6d; border-radius: 4px; padding: 4px 8px;
        }
    
        QFrame {
            background: #0d0221;
            border: none;
            border-radius: 0px;
        }
        QFrame#metricCard {
            background: #0a0118;
            border: 1px solid #1a0a3e;
            border-radius: 8px;
        }
        QWidget {
            background: #0d0221;
        }
)QSS");
}


} /* anonymous namespace */

/* ── Theme application ───────────────────────────────────────────────
   Dispatches to the correct theme function based on settings_.theme.
   Falls back to Dark for unknown theme names. */
void MainWindow::applyTheme()
{
    const QString &t = settings_.theme;

    if (t == QStringLiteral("Light"))
        qApp->setStyleSheet(qssLight());
    else if (t == QStringLiteral("Nord"))
        qApp->setStyleSheet(qssNord());
    else if (t == QStringLiteral("Catppuccin"))
        qApp->setStyleSheet(qssCatppuccin());
    else if (t == QStringLiteral("Dracula"))
        qApp->setStyleSheet(qssDracula());
    else if (t == QStringLiteral("Solarized"))
        qApp->setStyleSheet(qssSolarized());
    else if (t == QStringLiteral("Gruvbox"))
        qApp->setStyleSheet(qssGruvbox());
    else if (t == QStringLiteral("Tokyo Night"))
        qApp->setStyleSheet(qssTokyoNight());
    else if (t == QStringLiteral("One Dark"))
        qApp->setStyleSheet(qssOneDark());
    else if (t == QStringLiteral("Monokai"))
        qApp->setStyleSheet(qssMonokai());
    else if (t == QStringLiteral("Cyberpunk"))
        qApp->setStyleSheet(qssCyberpunk());
    else /* Dark or unknown */
        qApp->setStyleSheet(qssDark());
}

/* ── Apply all user settings ─────────────────────────────────────────
   Theme, font scale, master selector, action availability, status bar. */
void MainWindow::applySettings()
{
    LanguageManager::instance().setCurrentLanguage(settings_.language);
    applyTheme();
    QFont font = qApp->font();
    font.setPointSizeF(10.0 * settings_.scale);
    qApp->setFont(font);
    refreshMasterSelector();
    updateActionAvailability();
    updateStatusBar();
}
