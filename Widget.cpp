#include "Widget.h"
#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QShortcut>
#include <QMouseEvent>
#include <QPainter>
#include <QClipboard>

Widget::Widget()
{
    connect(new QShortcut(Qt::Key_Escape, this), &QShortcut::activated, this, &Widget::close);
    QScreen *screen = windowHandle() ? windowHandle()->screen() : QGuiApplication::primaryScreen();

    m_screenshot = screen->grabWindow(0);

    showFullScreen();
    setCursor(Qt::CrossCursor);
}

void Widget::onClipboardChanged()
{
    if (!qApp->clipboard()->ownsClipboard()) {
        close();
    }
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    m_clickedPos = event->pos();
}

void Widget::mouseReleaseEvent(QMouseEvent */*event*/)
{
    hide();

    QClipboard *clip = qApp->clipboard();
    clip->setPixmap(m_screenshot.copy(m_selectionRect));

    connect(clip, &QClipboard::dataChanged, this, &Widget::onClipboardChanged);
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    m_selectionRect = QRect(m_clickedPos, event->pos()).normalized();
    update();
}

void Widget::paintEvent(QPaintEvent *event)
{
    if (m_screenshot.isNull()) {
        return;
    }

    QPainter p(this);
    p.setClipRegion(event->region());
    p.drawPixmap(0, 0, m_screenshot);

    if (!m_selectionRect.isEmpty()) {
        p.setCompositionMode(QPainter::CompositionMode_Difference);
        p.setPen(Qt::white);
        p.drawRect(m_selectionRect);
    }
}

