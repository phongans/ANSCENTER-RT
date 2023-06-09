#include "FrameLabel.h"

#include <QPainter>
#include <QMouseEvent>
#include <QRect>
#include <QMenu>

FrameLabel::FrameLabel(QWidget *parent) : 
    QLabel(parent)
{
    m_startPoint.setX(0);
    m_startPoint.setY(0);
    m_mouseCursorPos.setX(0);
    m_mouseCursorPos.setY(0);
    m_drawBox = false;
    m_mouseData.leftButtonRelease = false;
    m_mouseData.rightButtonRelease = false;
    createContextMenu();
}

void FrameLabel::mouseMoveEvent(QMouseEvent *ev)
{
    // Save mouse cursor position
    setMouseCursorPos(ev->pos());
    // Update box width and height if box drawing is in progress
    if (m_drawBox)
    {
        m_box->setWidth(getMouseCursorPos().x() - m_startPoint.x());
        m_box->setHeight(getMouseCursorPos().y() - m_startPoint.y());
    }
    // Inform main window of mouse move event
    emit onMouseMoveEvent();
}

void FrameLabel::setMouseCursorPos(QPoint input)
{
    m_mouseCursorPos = input;
}

QPoint FrameLabel::getMouseCursorPos()
{
    return m_mouseCursorPos;
}

void FrameLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    // Update cursor position
    setMouseCursorPos(ev->pos());
    // On left mouse button release
    if(ev->button() == Qt::LeftButton)
    {
        // Set leftButtonRelease flag to TRUE
        m_mouseData.leftButtonRelease = true;
        if (m_drawBox)
        {
            // Stop drawing box
            m_drawBox = false;
            // Save box dimensions
            m_mouseData.selectionBox.setX(m_box->left());
            m_mouseData.selectionBox.setY(m_box->top());
            m_mouseData.selectionBox.setWidth(m_box->width());
            m_mouseData.selectionBox.setHeight(m_box->height());
            // Set leftButtonRelease flag to TRUE
            m_mouseData.leftButtonRelease = true;
            // Inform main window of event
            emit newMouseData(m_mouseData);
        }
        // Set leftButtonRelease flag to FALSE
        m_mouseData.leftButtonRelease = false;
    }
    // On right mouse button release
    else if(ev->button() == Qt::RightButton)
    {
        // If user presses (and then releases) the right mouse button while drawing box, stop drawing box
        if (m_drawBox)
        {
            m_drawBox = false;
        }
        else
        {
            // Show context menu
            menu->exec(ev->globalPos());
        }
    }
}

void FrameLabel::mousePressEvent(QMouseEvent *ev)
{
    // Update cursor position
    setMouseCursorPos(ev->pos());;
    if(ev->button() == Qt::LeftButton)
    {
        // Start drawing box
        m_startPoint = ev->pos();
        m_box = new QRect(m_startPoint.x(), m_startPoint.y(), 0, 0);
        m_drawBox = true;
    }
}

void FrameLabel::paintEvent(QPaintEvent *ev)
{
    QLabel::paintEvent(ev);
    QPainter painter(this);
    // Draw box
    if (m_drawBox)
    {
        painter.setPen(Qt::blue);
        painter.drawRect(*m_box);
    }
}

void FrameLabel::createContextMenu()
{
    // Create top-level menu object
    menu = new QMenu(this);
    // Add actions
    QAction *action;
    action = new QAction(this);
    action->setText(tr("Reset ROI"));
    menu->addAction(action);
    action = new QAction(this);
    action->setText(tr("Scale to Fit Frame"));
    action->setCheckable(true);
    menu->addAction(action);
    menu->addSeparator();
    // Create image processing menu object
    QMenu* menu_imgProc = new QMenu(this);
    menu_imgProc->setTitle(tr("Image Processing"));
    menu->addMenu(menu_imgProc);
    // Add actions
    action = new QAction(this);
    action->setText(tr("Grayscale"));
    action->setCheckable(true);
    menu_imgProc->addAction(action);
    action = new QAction(this);
    action->setText(tr("Smooth"));
    action->setCheckable(true);
    menu_imgProc->addAction(action);
    action = new QAction(this);
    action->setText(tr("Dilate"));
    action->setCheckable(true);
    menu_imgProc->addAction(action);
    action = new QAction(this);
    action->setText(tr("Erode"));
    action->setCheckable(true);
    menu_imgProc->addAction(action);
    action = new QAction(this);
    action->setText(tr("Detector"));
    action->setCheckable(true);
    menu_imgProc->addAction(action);
    action = new QAction(this);
    action->setText(tr("Tracker"));
    action->setCheckable(true);
    menu_imgProc->addAction(action);
    menu_imgProc->addSeparator();
    action = new QAction(this);
    action->setText(tr("Settings..."));
    menu_imgProc->addAction(action);
}
