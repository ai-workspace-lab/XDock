#include <QGuiApplication>
#include <QImage>
#include <QPainter>

int main(int argc, char **argv) {
    QGuiApplication app(argc, argv);
    if (argc != 4) return 2;
    const QImage reference(argv[1]), actual(argv[2]);
    if (reference.size() != QSize(1204, 78) || actual.size() != reference.size()) return 2;
    QImage sheet(1204, 430, QImage::Format_RGB32);
    sheet.fill(QColor("#f3f0e9"));
    QPainter painter(&sheet);
    painter.setPen(QColor("#35322e"));
    painter.setFont(QFont("Arial", 12));
    painter.drawText(12, 19, "REFERENCE / supplied dock strip / 1204 x 78");
    painter.drawImage(0, 26, reference);
    painter.drawText(12, 126, "IMPLEMENTATION / native Qt Quick / neutral preview backdrop / 1204 x 78");
    painter.drawImage(0, 133, actual);
    painter.drawText(12, 240, "REFERENCE / first icons / 2x");
    painter.drawText(614, 240, "IMPLEMENTATION / first icons / 2x");
    painter.drawImage(QRect(0, 253, 600, 156), reference, QRect(0, 0, 300, 78));
    painter.drawImage(QRect(604, 253, 600, 156), actual, QRect(0, 0, 300, 78));
    painter.end();
    return sheet.save(argv[3]) ? 0 : 1;
}
