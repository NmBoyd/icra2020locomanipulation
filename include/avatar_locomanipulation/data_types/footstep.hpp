#ifndef FOOTSTEP_OBJECT_H
#define FOOTSTEP_OBJECT_H

#include <Eigen/Dense>
#include <iostream>
#include <string>

#define LEFT_FOOTSTEP 0
#define RIGHT_FOOTSTEP 1
#define MID_FOOTSTEP 2

// Data container for a footstep object
 

class Footstep{
public:
  Footstep();
  Footstep(const Eigen::Vector3d & pos_in, const Eigen::Quaterniond & quat_in, const int & robot_side_in);

  ~Footstep(); 

  // Position and orientation of the sole of the footstep
  Eigen::Vector3d position;
  Eigen::Quaternion<double> orientation;

  Eigen::Matrix3d R_ori;

  // Setters
  void setPosOriSide(const Eigen::Vector3d & pos_in, const Eigen::Quaterniond & quat_in, const int & robot_side_in);
  void setPosOri(const Eigen::Vector3d & pos_in, const Eigen::Quaterniond & quat_in);
  void setRightSide();
  void setLeftSide();

  // Left or right side
  int robot_side;

  void computeMidfeet(const Footstep & footstep1, const Footstep & footstep2, Footstep & midfeet);

  int getSide();
  void printInfo();

  // length and width of the soles
  double sole_length = 0.2;
  double sole_width = 0.1;

  std::vector<Eigen::Vector3d> local_contact_point_list;
  std::vector<Eigen::Vector3d> global_contact_point_list;


private:
  void common_initialization();
  void updateContactLocations();

};

#endif