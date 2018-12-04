//========================================================================
//  This software is free: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License Version 3,
//  as published by the Free Software Foundation.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  Version 3 in the file COPYING that came with this distribution.
//  If not, see <http://www.gnu.org/licenses/>.
//========================================================================
/*!
  \file    field.h
  \brief   Definition of field dimensions
  \author  Stefan Zickler / Tim Laue, (C) 2009
*/
//========================================================================

#pragma once

#include "field_default_constants.h"

#include "VarTypes.h"
#include <QObject>
#include <QReadWriteLock>


#ifndef NO_PROTOBUFFERS
  #include "messages_robocup_ssl_geometry.pb.h"
#endif

using namespace VarTypes;

/*!
  \class RoboCupField

  \brief Definition of all variables for a symmetric, regulation-style RoboCup SSL field

  \author Stefan Zickler , (C) 2009
**/


class FieldLine : public QObject {
Q_OBJECT
protected:
  FieldLine(VarString* name_,
            VarDouble* p1_x_,
            VarDouble* p1_y_,
            VarDouble* p2_x_,
            VarDouble* p2_y_,
            VarDouble* thickness_,
            VarList* list_);
private:
  // Disable assignment operator.
  const FieldLine& operator=(const FieldLine& other);
public:
  VarString* name;
  VarDouble* p1_x;
  VarDouble* p1_y;
  VarDouble* p2_x;
  VarDouble* p2_y;
  VarDouble* thickness;
  VarList* list;

  FieldLine(const FieldLine& other);
  FieldLine(const std::string& marking_name);
  FieldLine(const std::string& marking_name,
              double p1_x_, double p1_y_, double p2_x_, double p2_y_,
              double thickness_);
  ~FieldLine();

  // Reads the entries from the specified VarList and attempts to create a
  // FieldLine object from the entries. If succesful, it returns a valid pointer
  // to a newly created FieldLine object. If it fails, it returns NULL.
  static FieldLine* FromVarList(VarList* list);

protected slots:
  void Rename();
};

void FieldLine::Rename() {
  list->setName(name->getString());
}

class FieldCircularArc : public QObject {
Q_OBJECT
protected:
  FieldCircularArc(VarString* name_,
                   VarDouble* center_x_,
                   VarDouble* center_y_,
                   VarDouble* radius_,
                   VarDouble* a1_,
                   VarDouble* a2_,
                   VarDouble* thickness_,
                   VarList* list_);

private:
  // Disable assignment operator.
  const FieldCircularArc& operator=(const FieldCircularArc& other);

public:
  VarString* name;
  VarDouble* center_x;
  VarDouble* center_y;
  VarDouble* radius;
  VarDouble* a1;
  VarDouble* a2;
  VarDouble* thickness;
  VarList* list;

  FieldCircularArc(const FieldCircularArc& other);
  FieldCircularArc(const std::string& marking_name);
  FieldCircularArc(const std::string& marking_name,
              double center_x_, double center_y_, double radius_,
              double a1_, double a2_, double thickness_);
  ~FieldCircularArc();

  // Reads the entries from the specified VarList and attempts to create a
  // FieldCircularArc object from the entries. If succesful, it returns a valid
  // pointer to a newly created FieldCircularArc object. If it fails, it returns
  // NULL.
  static FieldCircularArc* FromVarList(VarList* list);

private slots:
  void Rename();
};
void FieldCircularArc::Rename() {
  list->setName(name->getString());
}

class RoboCupField : public QObject
{
Q_OBJECT

protected:
  vector<VarType *> field_params;
  vector<VarType *> derived_params;
  VarList * settings;
  VarTrigger * restore;
public:
  VarList * getSettings() const {
    return settings;
  }
  VarInt * line_width;
  /*VarInt * field_length;
  VarInt * field_width;
  VarInt * boundary_width;*/
  VarInt * referee_width;/*
  VarInt * goal_width;
  VarInt * goal_depth;*/
  VarInt * goal_wall_width;
  VarInt * center_circle_radius;
  VarInt * defense_radius;
  VarInt * defense_stretch;
  VarInt * free_kick_from_defense_dist;
  VarInt * penalty_spot_from_field_line_dist;
  VarInt * penalty_line_from_spot_dist;

  mutable QReadWriteLock field_markings_mutex;
  VarDouble* field_length;
  VarDouble* field_width;
  VarDouble* goal_width;
  VarDouble* goal_depth;
  VarDouble* boundary_width;
  VarDouble* line_thickness;
  VarDouble* penalty_area_depth;
  VarDouble* penalty_area_width;
  VarInt* num_cameras_total;
  VarInt* num_cameras_local;
  VarInt* var_num_lines;
  VarInt* var_num_arcs;
  VarList* field_lines_list;
  VarList* field_arcs_list;
  vector<FieldLine*> field_lines;
  vector<FieldCircularArc*> field_arcs;

  //derived:
  VarInt * field_total_playable_length;
  VarInt * field_total_playable_width;
  VarInt * field_total_surface_length;
  VarInt * field_total_surface_width;
  VarInt * half_field_length;
  VarInt * half_field_width;
  VarInt * half_line_width;
  VarInt * half_goal_width;
  VarInt * half_defense_stretch;
  VarInt * half_field_total_playable_length;
  VarInt * half_field_total_playable_width;
  VarInt * half_field_total_surface_length;
  VarInt * half_field_total_surface_width;

  #ifndef NO_PROTOBUFFERS
  /*void toProtoBuffer(SSL_GeometryFieldSize & buffer) const {
    buffer.set_line_width(line_width->getInt());
    buffer.set_field_length(field_length->getInt());
    buffer.set_field_width(field_width->getInt());
    buffer.set_boundary_width(boundary_width->getInt());
    buffer.set_referee_width(referee_width->getInt());
    buffer.set_goal_width(goal_width->getInt());
    buffer.set_goal_depth(goal_depth->getInt());
    buffer.set_goal_wall_width(goal_wall_width->getInt());
    buffer.set_center_circle_radius(center_circle_radius->getInt());
    buffer.set_defense_radius(defense_radius->getInt());
    buffer.set_defense_stretch(defense_stretch->getInt());
    buffer.set_free_kick_from_defense_dist(free_kick_from_defense_dist->getInt());
    buffer.set_penalty_spot_from_field_line_dist(penalty_spot_from_field_line_dist->getInt());
    buffer.set_penalty_line_from_spot_dist(penalty_line_from_spot_dist->getInt());
  }*/



  void toProtoBuffer(SSL_GeometryFieldSize& buffer) const{
    field_markings_mutex.lockForRead();
    buffer.Clear();
    buffer.set_field_length(field_length->getDouble());
    buffer.set_field_width(field_width->getDouble());
    buffer.set_goal_width(goal_width->getDouble());
    buffer.set_goal_depth(goal_depth->getDouble());
    buffer.set_boundary_width(boundary_width->getDouble());
    for (size_t i = 0; i < field_lines.size(); ++i) {
      const FieldLine& line = *(field_lines[i]);
      SSL_FieldLineSegment proto_line;
      proto_line.set_name(line.name->getString());
      proto_line.mutable_p1()->set_x(line.p1_x->getDouble());
      proto_line.mutable_p1()->set_y(line.p1_y->getDouble());
      proto_line.mutable_p2()->set_x(line.p2_x->getDouble());
      proto_line.mutable_p2()->set_y(line.p2_y->getDouble());
      proto_line.set_thickness(line.thickness->getDouble());
      *(buffer.add_field_lines()) = proto_line;
    }
    for (size_t i = 0; i < field_arcs.size(); ++i) {
      const FieldCircularArc& arc = *(field_arcs[i]);
      SSL_FieldCicularArc proto_arc;
      proto_arc.set_name(arc.name->getString());
      proto_arc.mutable_center()->set_x(arc.center_x->getDouble());
      proto_arc.mutable_center()->set_y(arc.center_y->getDouble());
      proto_arc.set_radius(arc.radius->getDouble());
      proto_arc.set_a1(arc.a1->getDouble());
      proto_arc.set_a2(arc.a2->getDouble());
      proto_arc.set_thickness(arc.thickness->getDouble());
      *(buffer.add_field_arcs()) = proto_arc;
    }
    field_markings_mutex.unlock();
  }


  /*void fromProtoBuffer(const SSL_GeometryFieldSize & buffer) {
    line_width->setInt(buffer.line_width());
    field_length->setInt(buffer.field_length());
    field_width->setInt(buffer.field_width());
    boundary_width->setInt(buffer.boundary_width());
    referee_width->setInt(buffer.referee_width());
    goal_width->setInt(buffer.goal_width());
    goal_depth->setInt(buffer.goal_depth());
    goal_wall_width->setInt(buffer.goal_wall_width());
    center_circle_radius->setInt(buffer.center_circle_radius());
    defense_radius->setInt(buffer.defense_radius());
    defense_stretch->setInt(buffer.defense_stretch());
    free_kick_from_defense_dist->setInt(buffer.free_kick_from_defense_dist());
    penalty_spot_from_field_line_dist->setInt(buffer.penalty_spot_from_field_line_dist());
    penalty_line_from_spot_dist->setInt(buffer.penalty_line_from_spot_dist());
    updateDerivedParameters();
  }*/
  #endif

  void loadDefaultsRoboCup2009() {
    line_width->setDouble(FieldConstantsRoboCup2018A::kLineThickness);
    penalty_area_depth->setDouble(FieldConstantsRoboCup2018A::kPenaltyAreaDepth);
    penalty_area_width->setDouble(FieldConstantsRoboCup2018A::kPenaltyAreaWidth);
    field_length->setDouble(FieldConstantsRoboCup2018A::kFieldLength);
    field_width->setDouble(FieldConstantsRoboCup2018A::kFieldWidth);
    boundary_width->setDouble(FieldConstantsRoboCup2018A::kBoundaryWidth);
    referee_width->setInt(FieldConstantsRoboCup2009::referee_width);
    goal_width->setDouble(FieldConstantsRoboCup2018A::kGoalWidth);
    goal_depth->setDouble(FieldConstantsRoboCup2018A::kGoalDepth);
    goal_wall_width->setInt(FieldConstantsRoboCup2009::goal_wall_width);
    center_circle_radius->setDouble(FieldConstantsRoboCup2018A::kCenterCircleRadius);
    defense_radius->setInt(FieldConstantsRoboCup2009::defense_radius);
    defense_stretch->setInt(FieldConstantsRoboCup2009::defense_stretch);
    free_kick_from_defense_dist->setInt(FieldConstantsRoboCup2009::free_kick_from_defense_dist);
    penalty_spot_from_field_line_dist->setInt(FieldConstantsRoboCup2009::penalty_spot_from_field_line_dist);
    penalty_line_from_spot_dist->setInt(FieldConstantsRoboCup2009::penalty_line_from_spot_dist);
    penalty_area_depth->setDouble(FieldConstantsRoboCup2018A::kPenaltyAreaDepth);
    penalty_area_width->setDouble(FieldConstantsRoboCup2018A::kPenaltyAreaWidth);
    updateDerivedParameters();
  }
  void updateDerivedParameters() {
    field_total_playable_length->setInt(field_length->getInt() + (2 * boundary_width->getInt()));
    field_total_playable_width ->setInt(field_width->getInt() + (2 * boundary_width->getInt()));
    field_total_surface_length->setInt(field_length->getInt() + (2 * (boundary_width->getInt() + referee_width->getInt())));
    field_total_surface_width->setInt(field_width->getInt() + (2 * (boundary_width->getInt() + referee_width->getInt())));
    half_field_length->setInt(field_length->getInt() / 2);
    half_field_width->setInt(field_width->getInt() / 2);
    half_line_width->setInt(line_width->getInt() / 2);
    half_goal_width->setInt(goal_width->getInt() / 2);
    half_defense_stretch->setInt(defense_stretch->getInt() / 2);
    half_field_total_playable_length->setInt(field_total_playable_length->getInt() / 2);
    half_field_total_playable_width->setInt(field_total_playable_width->getInt() / 2);
    half_field_total_surface_length->setInt(field_total_surface_length->getInt() / 2);
    half_field_total_surface_width->setInt(field_total_surface_width->getInt() / 2);
  }

  RoboCupField()
  {
    settings = new VarList("Field Configuration");
    settings->addChild(restore = new VarTrigger("Reset SSL 2009","Reset SSL 2009"));

    connect(restore,SIGNAL(wasEdited(VarType*)),this,SLOT(restoreRoboCup()));
    //regulation-based symmetric field:
    field_params.push_back(line_width             = new VarInt("Line Width"));

    field_params.push_back(field_length           = new VarDouble("Field Length")); //including lines (outside to outside)
    field_params.push_back(field_width            = new VarDouble("Field Height")); //including lines (outside to outside)

    field_params.push_back(boundary_width         = new VarDouble("Boundary Width")); //width of the boundary
    field_params.push_back(referee_width          = new VarInt("Referee Width")); //width of the ref-walking area

    field_params.push_back(goal_width             = new VarDouble("Goal Width")); //inside width of the goal
    field_params.push_back(goal_depth             = new VarDouble("Goal Depth")); //inside depth of the goal
    field_params.push_back(goal_wall_width        = new VarInt("Goal Wall Width")); //goal wall thickness

    field_params.push_back(center_circle_radius   = new VarInt("Center Radius")); //radius of defense quarter circles

    field_params.push_back(defense_radius         = new VarInt("Defense Radius")); //radius of defense quarter circles

    //total length of the line connecting the two quarter circles of the defense area:
    field_params.push_back(defense_stretch        = new VarInt("Defense Stretch"));

    //distance that freekickers have to be from the defense line:
    field_params.push_back(free_kick_from_defense_dist = new VarInt("Freekick Defense Dist"));

    //distance of the penalty spot's center from the outside of the field line
    field_params.push_back(penalty_spot_from_field_line_dist = new VarInt("Penalty Spot Dist"));

    //distance between the penalty spot and the line where all other robots must be behind during penalty
    field_params.push_back(penalty_line_from_spot_dist = new VarInt("Penalty Line From Spot Dist"));
    field_params.push_back(penalty_area_depth = new VarDouble("Penalty area depth"));
    field_params.push_back(penalty_area_width = new VarDouble("Penalty are width"));

    //---------------------------------------------------------------
    //auto-derived variables:
    //---------------------------------------------------------------

    //total length of field (including boundary, BUT NOT INCLUDING REFEREE WALKING AREA)
    derived_params.push_back(field_total_playable_length        = new VarInt("Total Playable Length"));
    derived_params.push_back(field_total_playable_width         = new VarInt("Total Playable Width"));

    //total length from the outer walls (including playable boundary and referee walking area):
    derived_params.push_back(field_total_surface_length         = new VarInt("Total Surface Length"));
    derived_params.push_back(field_total_surface_width          = new VarInt("Total Surface Width"));

    derived_params.push_back(half_field_length                  = new VarInt("Half Field Length"));
    derived_params.push_back(half_field_width                   = new VarInt("Half Field Width"));
    derived_params.push_back(half_line_width                    = new VarInt("Half Line Width"));
    derived_params.push_back(half_goal_width                    = new VarInt("Half Goal Width"));
    derived_params.push_back(half_defense_stretch               = new VarInt("Half Defense Stretch"));
    derived_params.push_back(half_field_total_playable_length   = new VarInt("Half Total Playable Length"));
    derived_params.push_back(half_field_total_playable_width    = new VarInt("Half Total Playable Width"));
    derived_params.push_back(half_field_total_surface_length    = new VarInt("Half Total Surface Length"));
    derived_params.push_back(half_field_total_surface_width     = new VarInt("Half Total Surface Width"));

    for (unsigned int i=0;i<field_params.size();i++) {
      connect(field_params[i],SIGNAL(hasChanged(VarType *)),this,SLOT(changed()));
      settings->addChild(field_params[i]);
    }
    for (unsigned int i=0;i<derived_params.size();i++) {
      derived_params[i]->addFlags( VARTYPE_FLAG_READONLY );
      settings->addChild(derived_params[i]);
    }

    loadDefaultsRoboCup2009();
    emit calibrationChanged();
    //setup the derived parameters to auto-update when the non-derived parameters change.


  }

  ~RoboCupField() {
    field_params.clear();
    derived_params.clear();
    delete line_width;
    delete field_length;
    delete field_width;
    delete boundary_width;
    delete referee_width;
    delete goal_width;
    delete goal_depth;
    delete goal_wall_width;
    delete center_circle_radius;
    delete defense_radius;
    delete free_kick_from_defense_dist;
    delete penalty_spot_from_field_line_dist;
    delete penalty_line_from_spot_dist;

    //derived:
    delete field_total_playable_length;
    delete field_total_playable_width;
    delete field_total_surface_length;
    delete field_total_surface_width;
    delete half_field_length;
    delete half_field_width;
    delete half_line_width;
    delete half_goal_width;
    delete half_defense_stretch;
    delete half_field_total_playable_length;
    delete half_field_total_playable_width;
    delete half_field_total_surface_length;
    delete half_field_total_surface_width;

    delete restore;
    delete settings;
  }
 public:
  signals:
  void calibrationChanged();

protected slots:
  void changed() {
    updateDerivedParameters();
    calibrationChanged();
  }
  void restoreRoboCup() {
    loadDefaultsRoboCup2009();
  }
};

