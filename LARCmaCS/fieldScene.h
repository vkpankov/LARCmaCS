#pragma once

#include <iostream>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include "field_default_constants.h"
#include "robocup_ssl_client.h"
#include "timer.h"

const int teamUnknown = 0;
const int teamBlue = 1;
const int teamYellow = 2;
const int NA = 0xffff;
const double NAOrientation = 1000.0;

class Robot : public QGraphicsPathItem
{
  public:
    double orientation;     //In degrees
    int teamID;             //Team ID. 0 = blue, 1 = yellow
    int id;                 //ID of the robot in its team
    double x,y;
    double conf;
    int key;
    QString robotLabel;

  private:
    QBrush *brush;
    QPen *pen, *idPen, *confPen;
    QPainterPath robotOutline, robotOutlineCircle, robotID;
    QFont drawFont;

  public:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    unsigned long int tStamp;
    void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
//    void update ( qreal x, qreal y, qreal width, qreal height ) { return;}
//    void update ( const QRectF & rect = QRectF() ) {return;}

    Robot();
    Robot ( double _x, double _y, double _orientation, int _teamID, int _id, int _key, double _conf );
    void SetPose ( double _x, double _y, double _orientation, double _conf );
    ~Robot();
};

class FieldScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit FieldScene(QObject *parent = 0);

    void AddRobot ( Robot* robot );
    void UpdateRobots ( SSL_DetectionFrame &detection );
    int UpdateBalls ( QVector<QPointF> &_balls, int cameraID );
    void UpdateField(SSL_GeometryFieldSize field);

    void UpdateGeometry(SSL_GeometryFieldSize fieldSize);
    void LoadFieldGeometry();
    void LoadFieldGeometry ( SSL_GeometryFieldSize &fieldSize );

signals:

public slots:

private:

    //Robots
    QVector<Robot*> blueRobots,yellowRobots, robots;
    //balls
    QVector < QVector<QGraphicsEllipseItem*> > ballItems;
    //field
    QPainterPath *field;
    QGraphicsPathItem *fieldItem;
    //brushes and pens
    QBrush *fieldBrush, *ballBrush;
    QPen *fieldPen, *fieldLinePen, *ballPen;
    bool shutdownSoccerView;

    void ConstructField();

    //gain size
    double ksize;

    //Field dimensions and geometry
    vector<SSL_FieldLineSegment> field_lines;
    vector<SSL_FieldCicularArc> field_arcs;
    double line_width;
    double field_length;
    double field_width;
    double boundary_width;
    double goal_width;
    double goal_depth;
    double center_circle_radius;
    double penalty_area_width;
    double penalty_area_depth;

};

