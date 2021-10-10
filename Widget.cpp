#include "Widget.h"
#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QShortcut>
#include <QMouseEvent>
#include <QPainter>
#include <QClipboard>
#include <QCursor>

Widget::Widget()
{
    connect(new QShortcut(Qt::Key_Escape, this), &QShortcut::activated, this, &Widget::close);

    m_screenshot = qApp->screenAt(QCursor::pos())->grabWindow(0);

    showFullScreen();
    setCursor(Qt::CrossCursor);
    setMouseTracking(true);
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

void Widget::mouseReleaseEvent(QMouseEvent* /*event*/)
{
    hide();


    QClipboard *clip = qApp->clipboard();
    if (!m_selectionRect.isEmpty()) {
        clip->setPixmap(m_screenshot.copy(m_selectionRect));
    } else {
        clip->setPixmap(m_screenshot);
    }

    connect(clip, &QClipboard::dataChanged, this, &Widget::onClipboardChanged);
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons()) {
        m_selectionRect = QRect(m_clickedPos, event->pos()).normalized();
    }
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

    p.setCompositionMode(QPainter::CompositionMode_Difference);
    p.setPen(Qt::white);

    if (m_selectionRect.isEmpty()) {
        QPoint cursor = QCursor::pos();
        p.drawLine(0, cursor.y(), m_screenshot.width(), cursor.y());
        p.drawLine(cursor.x(), 0, cursor.x(), m_screenshot.height());
    } else {
        p.drawRect(m_selectionRect);
    }
}

