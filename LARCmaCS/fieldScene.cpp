#include "fieldScene.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#define _USE_MATH_DEFINES
#include <math.h>
#include <QTextStream>

Robot::Robot()
{
    Robot(0, 0, 0, teamUnknown, 0, 0, 0);
}

Robot::Robot(double _x, double _y, double _orientation, int _teamID, int _id, int _key, double _conf)
{
  conf = _conf;
  key = _key;
  setVisible(true);
  setZValue(1);
  x = _x/10;
  y = _y/10;
  orientation = _orientation;
  teamID = _teamID;
  id = _id;

  robotOutline.moveTo(14, 0);
  robotOutline.arcTo(-14, -14, 28, 28, 0, 270);
  robotOutline.closeSubpath();
  robotOutlineCircle.addEllipse(-7, -7, 14, 14);

  if (id != NA)
    robotLabel = QString(QByteArray((char *)&id, 4).toHex()).mid(1, 1).toUpper();
  else
    robotLabel = "?";

  drawFont = QFont("Courier", 8, 2, false);
  robotID.addText(-25, 25, QFont("Courier", 8, 2, false), robotLabel);
  switch (teamID) {
    case teamBlue:
    {
        brush = new QBrush(QColor(0x41, 0x7e, 0xff, 255), Qt::SolidPattern);
        pen = new QPen(QColor(0x12, 0x3b, 0xa0, 255));
        break;
    }
    case teamYellow:
    {
        brush = new QBrush(QColor(0xff, 0xf3, 0x3e, 255), Qt::SolidPattern);
        pen = new QPen(QColor(0xcc, 0x9d, 0x00, 255));
        break;
    }
    default:
    {
        brush = new QBrush(QColor(150, 150, 150, 255), Qt::SolidPattern);
        pen = new QPen(QColor(70, 70, 70, 255));
        break;
    }
  }
  pen->setWidth(2);
  idPen = new QPen(Qt::black);
  idPen->setWidth(0);
  confPen = new QPen(Qt::white);
  confPen->setWidth(1);
  tStamp = GetTimeUSec();
}

Robot::~Robot()
{
  delete brush;
  delete pen;
  delete idPen;
}

QRectF Robot::boundingRect() const
{
  return QRectF ( -7.5+x,-7.5-y,15,15 );
}

void Robot::paint ( QPainter *painter, const QStyleOptionGraphicsItem* , QWidget* )
{
  if ( conf==0.0 )
    return;
  painter->translate ( x, y );
  painter->setPen ( *pen );
  painter->setBrush ( *brush );
  if ( fabs ( orientation ) <360 )
  {
    painter->rotate ( -45+orientation );
    painter->drawPath ( robotOutline );
    painter->rotate ( 45-orientation );
  }
  else
    painter->drawPath ( robotOutlineCircle );

  painter->setPen ( *idPen );
  painter->setBrush ( Qt::black );
  painter->setFont ( drawFont );
  //painter->drawPath(robotID);
  painter->drawText ( -4.5,-6,100,100,0,robotLabel );

  painter->setPen ( Qt::NoPen );
  painter->setBrush ( *brush );
  painter->drawRect ( -9,-15, ( int ) ( ( ( double ) 3 ) *conf ),3 );
  painter->setPen ( *pen );
  painter->setBrush ( QBrush ( Qt::white, Qt::NoBrush ) );
  painter->drawRect ( -9,-15,18,3 );
}

QPainterPath Robot::shape() const
{
  QPainterPath path;
  path.addEllipse ( -10, -10, 20, 20 );
  return path;
}

void Robot::SetPose ( double _x, double _y, double _orientation, double _conf )
{
  x = _x/10;
  y = _y/10;
  orientation = _orientation;
  conf = _conf;
  tStamp = GetTimeUSec();
}

FieldScene::FieldScene(QObject *parent) :
    QGraphicsScene(parent)
{
    setBackgroundBrush ( QBrush ( QColor ( 0,0x91,0x19,255 ),Qt::SolidPattern ) );
    //drawMutex = _drawMutex;
    shutdownSoccerView = false;
//    glWidget = new QGLWidget ( QGLFormat ( QGL::DoubleBuffer ) );
//    setViewport ( glWidget );
    ksize = 10;
    LoadFieldGeometry();

    ConstructField();
    fieldBrush = new QBrush ( Qt::NoBrush );
    fieldLinePen = new QPen();
    fieldLinePen->setColor ( Qt::white );
    fieldLinePen->setWidth ( 2 );
    fieldLinePen->setJoinStyle ( Qt::MiterJoin );
    fieldItem = this->addPath ( *field,*fieldLinePen,*fieldBrush );
    fieldItem->setZValue(0);
//    drawScale = 0.15;

//    connect(this, SIGNAL(scaleView(qreal)),rc,SLOT(scaleView(qreal)));
//    scalingRequested = true;
}

void FieldScene::AddRobot ( Robot *robot )
{
  robots.append ( robot );
  this->addItem ( robot );
}

void FieldScene::UpdateField(SSL_GeometryFieldSize field) {
    LoadFieldGeometry(field);
}

void FieldScene::UpdateRobots ( SSL_DetectionFrame &detection )
{
  int robots_blue_n =  detection.robots_blue_size();
  int robots_yellow_n =  detection.robots_yellow_size();
  //cout << robots_blue_n << " " << robots_yellow_n << endl;
  int i,j,yellowj=0,bluej=0;
  int team=teamBlue;

  SSL_DetectionRobot robot;
  for ( i = 0; i < robots_blue_n+robots_yellow_n; i++ )
  {
    //  cout << "i=" << i << endl;
    if ( i<robots_blue_n )
    {
      robot = detection.robots_blue ( i );
      team = teamBlue;
      j=bluej;
    }
    else
    {
      robot = detection.robots_yellow ( i-robots_blue_n );
      team = teamYellow;
      j=yellowj;
    }

    double x,y,orientation,conf =robot.confidence();
    int id=NA;
    if ( robot.has_robot_id() )
      id = robot.robot_id();
    else
      id = NA;
    x = robot.x();
    y = -robot.y();
    if ( robot.has_orientation() )
      orientation = robot.orientation() *180.0/M_PI;
    else
      orientation = NAOrientation;

    //seek to the next robot of the same camera and team colour
    while ( j<robots.size() && ( robots[j]->key!=detection.camera_id() || robots[j]->teamID!=team ) )
      j++;
//    cout << "robot size " << robots.size() << endl;

    if ( j+1>robots.size() )
      AddRobot ( new Robot ( x,y,orientation,team,id,detection.camera_id(),conf ) );

    //cout << i << " " << id << " " << x << " " << y << " " << orientation << " " << conf << endl;

    robots[j]->SetPose ( x, -y,orientation,conf );
    QString label;

    if ( id!=NA )
      label.setNum ( id+1,10 );
    else
      label = "?";
    label = label.toUpper();
    if ( label!=robots[j]->robotLabel )
      robots[j]->robotLabel = label;

    j++;

    if ( i<robots_blue_n )
      bluej=j;
    else
      yellowj=j;
  }
  for ( j=bluej;j<robots.size();j++ )
  {
    if ( robots[j]->key==detection.camera_id() && robots[j]->teamID==teamBlue )
      robots[j]->conf=0.0;
  }
  for ( j=yellowj;j<robots.size();j++ )
  {
    if ( robots[j]->key==detection.camera_id() && robots[j]->teamID==teamYellow )
      robots[j]->conf=0.0;
  }

  QVector<QGraphicsEllipseItem*> tmp;
  int cameraID=detection.camera_id();
  while(cameraID+1>ballItems.size())
    ballItems.append(tmp);
  if ( ballItems[cameraID].size() < detection.balls_size() ){
    //need to allocate some space for the new balls
    QPen pen ( QColor ( 0xcd,0x59,0x00,0xff ) );
    pen.setWidth ( 2 );
    QBrush brush ( QColor ( 0xff,0x81,0x00,0xff ),Qt::SolidPattern );
    while(detection.balls_size()>ballItems[cameraID].size()){
      ballItems[cameraID].append ( this->addEllipse ( 0,0,12,12,pen,brush ) );
      ballItems[cameraID][ballItems[cameraID].size()-1]->setZValue(2);
    }
  }
  else if ( ballItems[cameraID].size() >detection.balls_size() ){
    //need to delete some balls
    while(ballItems[cameraID].size()>detection.balls_size()){
        this->removeItem ( ballItems[cameraID][0] );
        ballItems[cameraID].remove(0);
    }
  }
  for ( int i=0;i<detection.balls_size();i++ )
  {
    ballItems[cameraID][i]->setPos ( detection.balls(i).x()/ksize-6,detection.balls(i).y()/ksize-6 );
  }
  return;
}

void FieldScene::UpdateGeometry(SSL_GeometryFieldSize fieldSize) {
    LoadFieldGeometry(fieldSize);
}

int FieldScene::UpdateBalls ( QVector<QPointF> &_balls, int cameraID )
{
  QVector<QGraphicsEllipseItem*> tmp;
  while(cameraID+1>ballItems.size())
    ballItems.append(tmp);

  if ( ballItems[cameraID].size() < _balls.size() ){
    //need to allocate some space for the new balls
    QPen pen ( QColor ( 0xcd,0x59,0x00,0xff ) );
    pen.setWidth ( 2 );
    QBrush brush ( QColor ( 0xff,0x81,0x00,0xff ),Qt::SolidPattern );
    while(_balls.size()>ballItems[cameraID].size()){
      ballItems[cameraID].append ( this->addEllipse ( 0,0,12,12,pen,brush ) );
      ballItems[cameraID][ballItems[cameraID].size()-1]->setZValue(2);
    }
  }
  else if ( ballItems[cameraID].size() >_balls.size() ){
    //need to delete some balls
    while(ballItems[cameraID].size()>_balls.size()){
      this->removeItem ( ballItems[cameraID][0] );
      ballItems[cameraID].remove(0);
    }
  }

  for ( int i=0;i<_balls.size();i++ ){
    //Let's update the ball positions now
//      cout << _balls[i].x()/ksize << " " << _balls[i].y()/ksize << endl;
    ballItems[cameraID][i]->setPos ( _balls[i].x()/ksize-6,_balls[i].y()/ksize-6 );
  }

  int balls = ballItems[cameraID].size();
  return balls;
}

void FieldScene::ConstructField()
{

   //scene->removeItem(fieldItem);
  field = new QPainterPath();
  QFont qfont;
  qfont.setPixelSize(20);
  field->addText(QPoint(field_length/(2*ksize) + 100 / ksize, field_width/(2*ksize)+200/ksize), qfont , "(" + QString::number(field_length / 2) + ", " + QString::number(-field_width / 2) + ")");
  field->addText(QPoint(-field_length/(2*ksize)-1400/ksize, field_width/(2*ksize)+200/ksize), qfont , "(" + QString::number(-field_length / 2) + ", " + QString::number(-field_width / 2) + ")");
  field->addText(QPoint(-field_length/(2*ksize)-1300/ksize, -field_width/(2*ksize)-100/ksize), qfont , "(" + QString::number(-field_length / 2) + ", " + QString::number(field_width / 2) + ")");
  field->addText(QPoint(field_length/(2*ksize)+100/ksize, -field_width/(2*ksize)-100/ksize), qfont , "(" + QString::number(field_length / 2) + ", " + QString::number(field_width / 2) + ")");

  if (field_lines.empty() || field_arcs.empty()) {
    field->moveTo ( 0,-field_width/(2*ksize) );
    field->lineTo ( 0,field_width/(2*ksize) );

    field->addEllipse ( -2*center_circle_radius/(2*ksize),-center_circle_radius/ksize, 4*center_circle_radius/(2*ksize),2*center_circle_radius/ksize );

    field->moveTo ( field_length/(2*ksize),-field_width/(2*ksize) );
    field->lineTo ( field_length/(2*ksize),field_width/(2*ksize) );

    field->moveTo ( -field_length/(2*ksize),-field_width/(2*ksize) );
    field->lineTo ( -field_length/(2*ksize),field_width/(2*ksize) );

    field->moveTo ( -field_length/(2*ksize),-field_width/(2*ksize) );
    field->lineTo ( field_length/(2*ksize),-field_width/(2*ksize) );

    field->moveTo ( -field_length/(2*ksize),field_width/(2*ksize) );
    field->lineTo ( field_length/(2*ksize),field_width/(2*ksize) );

    field->moveTo ( field_length/(2*ksize),goal_width/(2*ksize) );
    field->lineTo ( ( field_length/(2*ksize)+goal_depth/ksize ),goal_width/(2*ksize) );
    field->lineTo ( ( field_length/(2*ksize)+goal_depth/ksize ),-goal_width/(2*ksize) );
    field->lineTo ( field_length/(2*ksize),-goal_width/(2*ksize) );
    field->moveTo (( field_length/(2*ksize)-penalty_area_depth/ksize ),penalty_area_width/(2*ksize) );
    field->lineTo (( field_length/(2*ksize)-penalty_area_depth/ksize ),-penalty_area_width/(2*ksize) );
    field->moveTo((field_length/(2*ksize)), penalty_area_width/(2*ksize));
    field->lineTo (( field_length/(2*ksize)-penalty_area_depth/ksize ),penalty_area_width/(2*ksize) );
    field->moveTo((field_length/(2*ksize)), -penalty_area_width/(2*ksize));
    field->lineTo (( field_length/(2*ksize)-penalty_area_depth/ksize ),-penalty_area_width/(2*ksize) );

    field->moveTo ( -field_length/(2*ksize),goal_width/(2*ksize) );
    field->lineTo ( - ( field_length/(2*ksize)+goal_depth/ksize ),goal_width/(2*ksize) );
    field->lineTo ( - ( field_length/(2*ksize)+goal_depth/ksize ),-goal_width/(2*ksize) );
    field->lineTo ( -field_length/(2*ksize),-goal_width/(2*ksize) );
    field->moveTo ( - ( field_length/(2*ksize)-penalty_area_depth/ksize ),penalty_area_width/(2*ksize) );
    field->lineTo ( - ( field_length/(2*ksize)-penalty_area_depth/ksize ),-penalty_area_width/(2*ksize) );
    field->moveTo(- (field_length/(2*ksize)), penalty_area_width/(2*ksize));
    field->lineTo ( - ( field_length/(2*ksize)-penalty_area_depth/ksize ),penalty_area_width/(2*ksize) );
    field->moveTo(- (field_length/(2*ksize)), -penalty_area_width/(2*ksize));
    field->lineTo ( - ( field_length/(2*ksize)-penalty_area_depth/ksize ),-penalty_area_width/(2*ksize) );
  } else {
      for (int i = 0; i < field_lines.size(); i++) {
          field->moveTo(field_lines[i].p1().x() / ksize, field_lines[i].p1().y() / ksize);
          field->lineTo(field_lines[i].p2().x() / ksize, field_lines[i].p2().y() / ksize);
      }
      for (int i = 0; i < field_arcs.size(); i++) {
          double tmp;
          if (field_arcs[i].a2() < field_arcs[i].a1()) {
              tmp = 2 * M_PI + field_arcs[i].a2() - field_arcs[i].a1();
          } else {
              tmp = field_arcs[i].a2() - field_arcs[i].a1();
          }
          field->moveTo(field_arcs[i].center().x() / ksize + field_arcs[i].radius() / ksize, field_arcs[i].center().y() / ksize);
          field->arcTo(field_arcs[i].center().x() / ksize - field_arcs[i].radius() / ksize, field_arcs[i].center().y() / ksize - field_arcs[i].radius() / ksize, 2 * field_arcs[i].radius() / ksize, 2 * field_arcs[i].radius() / ksize, field_arcs[i].a1(), tmp/0.0175);
      }
  }
}

void FieldScene::LoadFieldGeometry()
{
  this->line_width = FieldConstantsRoboCup2018A::kLineThickness;
  this->field_length = FieldConstantsRoboCup2018A::kFieldLength;
  this->field_width = FieldConstantsRoboCup2018A::kFieldWidth;
  this->boundary_width = FieldConstantsRoboCup2018A::kBoundaryWidth;
  this->goal_width = FieldConstantsRoboCup2018A::kGoalWidth;
  this->goal_depth = FieldConstantsRoboCup2018A::kGoalDepth;
  this->center_circle_radius = FieldConstantsRoboCup2018A::kCenterCircleRadius;
  this->penalty_area_depth = FieldConstantsRoboCup2018A::kPenaltyAreaDepth;
  this->penalty_area_width = FieldConstantsRoboCup2018A::kPenaltyAreaWidth;
}

void FieldScene::LoadFieldGeometry ( SSL_GeometryFieldSize &fieldSize )
{
  this->line_width = FieldConstantsRoboCup2018A::kLineThickness;//fieldSize.line_width();
  field_lines.clear();
  field_arcs.clear();
  for (int i = 0; i < fieldSize.field_lines_size(); i++) {
    field_lines.push_back(fieldSize.field_lines(i));
  }
  for (int i = 0; i < fieldSize.field_arcs_size(); i++) {
    field_arcs.push_back(fieldSize.field_arcs(i));
  }
  this->field_length = fieldSize.field_length();
  this->field_width = fieldSize.field_width();
  this->boundary_width = fieldSize.boundary_width();
  this->goal_width = fieldSize.goal_width();
  this->goal_depth = fieldSize.goal_depth();
  this->penalty_area_depth = FieldConstantsRoboCup2018A::kPenaltyAreaDepth;
  this->penalty_area_width = FieldConstantsRoboCup2018A::kPenaltyAreaWidth;
  this->center_circle_radius = FieldConstantsRoboCup2018A::kCenterCircleRadius;//fieldSize.center_circle_radius();

  this->removeItem ( fieldItem );
  ConstructField();
  fieldItem = this->addPath ( *field,*fieldLinePen,*fieldBrush );
}

//void SoccerScene::setSizeScene(int _size)
//{
////    ksize = _size;
//    qreal sview = (qreal)_size;
//    emit scaleView(sview);
//    this->removeItem( fieldItem );
////    drawMutex->lock();
//    ConstructField();
//    fieldItem = this->addPath ( *field,*fieldLinePen,*fieldBrush );
////    drawMutex->unlock();
//}
