#include <QtMath>
#include <QPainter>
#include "MapIconPainter.h"
#include "MapRobotObject.h"
#include "MapWaypointObject.h"
#include "MapIcons.h"
#include "RobotStates.h"

using namespace MapAbstraction;

QImage MapIconPainter::paintIcon(QImage icon, MapObjectConstPtr object, bool simplified)
{
    //Transform icon if necessary
    if (object->category() == PlacemarkRobot && !simplified)
    {   //Paint over icon if robot is connected or disconnected
        MapRobotObjectConstPtr robot = object.staticCast<const MapRobotObject>();
        if (robot->state() == RobotStateConnected)
        {
            QPainter painter(&icon);
            QImage connectedIcon(MapIcons::defaultIconPath(MapIcons::ConnectedIcon));
            QPoint position(icon.width() - connectedIcon.width(),
                            icon.height() - connectedIcon.height());
            painter.drawImage(position, connectedIcon);
        }
        else if (robot->state() == RobotStateDisconnected)
        {
            QPainter painter(&icon);
            QImage disconnectedIcon(MapIcons::defaultIconPath(MapIcons::DisconnectedIcon));
            QPoint position(icon.width() - disconnectedIcon.width(),
                            icon.height() - disconnectedIcon.height());
            painter.drawImage(position, disconnectedIcon);
        }
    }
    else if (object->category() == PlacemarkRobot && simplified)
    {   //Rotate image of default triangle to show orientation
        QTransform rotation;
        MapRobotObjectConstPtr robot = object.staticCast<const MapRobotObject>();
        rotation.rotateRadians(- robot->orientation() + M_PI/2 );
        return icon.transformed(rotation, Qt::SmoothTransformation);
    }
    else if (object->category() == PlacemarkWaypoint)
    {   //Draw index on waypoint
        MapWaypointObjectConstPtr waypoint = object.staticCast<const MapWaypointObject>();
        QPainter painter(&icon);
        QRgb color = icon.pixel(3, icon.height()/2);
        painter.setPen(color);
        painter.setBrush(QBrush(color));
        int radius = icon.width()/2 - 5;
        QPen oldPen = painter.pen();
        QPen pen(painter.brush(), 2);
        painter.setPen(pen);
        painter.drawLine(QPointF(icon.width()/2, icon.height()/2),
                         QPointF(icon.width()/2 + radius*cos(waypoint->orientation()),
                                 icon.height()/2 + radius*sin(waypoint->orientation())));
        painter.setPen(oldPen);
        QPainterPath path;
        QFont f = painter.font();
        f.setPixelSize(16);
        f.setBold(true);
        painter.setFont(f);
        path.addText(icon.rect().bottomLeft(), f, QString::number(waypoint->number()));
        painter.drawPath(path);
        painter.strokePath(path, QPen(Qt::white, 1));
    }
    return icon;
}
