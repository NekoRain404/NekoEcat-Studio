// MainWindowAbout.cpp — About dialog with version, license, and credits.
#include "MainWindowIncludes.h"
#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QDate>

void MainWindow::showAboutDialog() {
    QDialog dlg(this);
    dlg.setWindowTitle(tr("About NekoEcat Studio"));
    dlg.setFixedSize(480, 360);

    auto *layout = new QVBoxLayout(&dlg);
    auto *tabs = new QTabWidget;

    // ── About tab ──
    auto *aboutPage = new QWidget;
    auto *aboutLayout = new QVBoxLayout(aboutPage);
    auto *nameLabel = new QLabel(QStringLiteral(
        "<h2>NekoEcat Studio</h2>"
        "<p>EtherCAT commissioning and diagnostics workstation</p>"
        "<p>Version %1</p>"
        "<p>Built with Qt %2 on %3</p>"
    ).arg(QApplication::applicationVersion(),
          QStringLiteral(QT_VERSION_STR),
          QStringLiteral(__DATE__)));
    nameLabel->setWordWrap(true);
    nameLabel->setAlignment(Qt::AlignCenter);
    aboutLayout->addWidget(nameLabel);
    aboutLayout->addStretch();
    auto *licenseLabel = new QLabel(QStringLiteral(
        "<p>Copyright \xC2\xA9 2026 NekoRain</p>"
        "<p>Licensed under the GPL v3.0</p>"
        "<p><a href='https://github.com/NekoRain/nekoecat-studio'>"
        "github.com/NekoRain/nekoecat-studio</a></p>"));
    licenseLabel->setOpenExternalLinks(true);
    licenseLabel->setWordWrap(true);
    licenseLabel->setAlignment(Qt::AlignCenter);
    aboutLayout->addWidget(licenseLabel);
    tabs->addTab(aboutPage, tr("About"));

    // ── License tab ──
    auto *licensePage = new QWidget;
    auto *licenseLayout = new QVBoxLayout(licensePage);
    auto *licenseText = new QTextBrowser;
    licenseText->setOpenExternalLinks(true);
    QFile f(QStringLiteral(":/LICENSE"));
    if (f.open(QIODevice::ReadOnly))
        licenseText->setText(f.readAll());
    else
        licenseText->setText(tr("GNU General Public License v3.0 \xE2\x80\x94 see LICENSE file"));
    licenseLayout->addWidget(licenseText);
    tabs->addTab(licensePage, tr("License"));

    // ── Credits tab ──
    auto *creditsPage = new QWidget;
    auto *creditsLayout = new QVBoxLayout(creditsPage);
    auto *creditsText = new QLabel(QStringLiteral(
        "<h3>Contributors</h3>"
        "<p>NekoRain \xE2\x80\x94 Lead Developer</p>"
        "<h3>Technologies</h3>"
        "<p>Qt 6 \xC2\xB7 IgH EtherCAT Master \xC2\xB7 CMake</p>"));
    creditsText->setWordWrap(true);
    creditsLayout->addWidget(creditsText);
    creditsLayout->addStretch();
    tabs->addTab(creditsPage, tr("Credits"));

    layout->addWidget(tabs);

    auto *closeBtn = new QPushButton(tr("Close"));
    connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    layout->addWidget(closeBtn);

    dlg.exec();
}
