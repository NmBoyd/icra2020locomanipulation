#ifndef WALKING_PATTERN_GENERATOR_H
#define WALKING_PATTERN_GENERATOR_H

#include <Eigen/Dense>
#include <cmath>
#include <string>
#include <avatar_locomanipulation/data_types/footstep.hpp>
#include <avatar_locomanipulation/data_types/trajectory_SE3.hpp>

#include <avatar_locomanipulation/helpers/hermite_curve_vec.hpp>
#include <avatar_locomanipulation/helpers/hermite_quaternion_curve.hpp>
#include <iostream>


class WalkingPatternGenerator{
public:
  WalkingPatternGenerator();
  ~WalkingPatternGenerator(); 

  static int const SWING_VRP_TYPE;
  static int const DOUBLE_SUPPORT_TRANSFER_VRP_TYPE;

  static int const STATE_INITIAL_TRANSFER;
  static int const STATE_SWING;
  static int const STATE_DOUBLE_SUPPORT;
  static int const STATE_FINAL_TRANSFER;

  std::vector<Footstep> footstep_list;

  std::vector<Eigen::Vector3d> rvrp_list; // List of virtual repelant points.
  std::vector<Eigen::Vector3d> dcm_ini_list; // List of initial DCM states 
  std::vector<Eigen::Vector3d> dcm_eos_list; // List of end-of-step DCM states

  std::vector<int> rvrp_type_list; // List of type of virtual repelant point


  Eigen::Vector3d start_stance_rvrp;
  Eigen::Vector3d start_stance_dcm_ini;  
  Eigen::Vector3d start_stance_dcm_eos;  

  // DCM parameters:
  double gravity = 9.81;
  double z_vrp = 0.95; //1.0; // desired VRP height / CoM height
  double b = std::sqrt(z_vrp/gravity); // time constant of DCM dynamics  

  double t_ds = 0.45; //0.9; // time in double support
  double t_ss = 1.0;//1.2; // time in single support
  double t_settle = -b*log(0.001); // settling time at the end of the full walking trajectory 
  double t_transfer = -b*log(1.0 - 0.01); // settling time at the end of the full walking trajectory 

  double swing_height = 0.1; //0.05; // swing height in meters.

  void setCoMHeight(double z_vrp_in); // Sets the desired CoM Height
  void setDoubleSupportTime(double t_ds_in); // Sets the desired double support time
  void setSingleSupportSwingTime(double t_ss_in); // Sets the desired single support swing time
  void setSettlingPercentage(double percentage_convergence); // Percentage to settle. Default 0.999
  void setSwingHeight(double swing_height_in); // Sets the swing height of the robot. Default 0.1m

  // DCM trajectory calculation
  // input: input_footstep_list - a list of footsteps to take not including the current stance configuration.
  //        initial_footstance  - a footstep object describing the stance leg. 
  // populates this object's footstep_list, rvrp_list, dcm_ini_list, dcm_eos_list
  void initialize_footsteps_rvrp(const std::vector<Footstep> & input_footstep_list, const Footstep & initial_footstance, bool clear_list=false);

  // input: input_footstep_list - a list of footsteps to take not including the current stance configuration.
  //        initial_footstance  - a footstep object describing the stance leg. 
  //        initial_rvrp        - an initial virtual repelant point (eg: average of the stance feet's rvrp). 
  // populates this object's footstep_list, rvrp_list, dcm_ini_list, dcm_eos_list
  void initialize_footsteps_rvrp(const std::vector<Footstep> & input_footstep_list, const Footstep & initial_footstance, const Eigen::Vector3d & initial_rvrp);

  // input: input_footstep_list - a list of footsteps to take not including the current stance configuration.
  //        left_footstance        - a footstep object describing the left stance feet
  //        right_footstance       - a footstep object describing the right stance feet
  // populates this object's footstep_list, rvrp_list, dcm_ini_list, dcm_eos_list. 
  void initialize_footsteps_rvrp(const std::vector<Footstep> & input_footstep_list, const Footstep & left_footstance, const Footstep & right_footstance);

  // input: input_footstep_list - a list of footsteps to take not including the current stance configuration.
  //        left_footstance        - a footstep object describing the left stance feet
  //        right_footstance       - a footstep object describing the right stance feet
  //        initial_com            - the initial location of the com 
  // populates this object's footstep_list, rvrp_list, dcm_ini_list, dcm_eos_list. 
  void initialize_footsteps_rvrp(const std::vector<Footstep> & input_footstep_list, const Footstep & left_footstance, const Footstep & right_footstance, const Eigen::Vector3d & initial_com);


   // Outputs the average r_vrp location given two footstances
  void get_average_rvrp(const Footstep & footstance_1, const Footstep & footstance_2, Eigen::Vector3d & average_rvrp);

    // computes all the dcm states. Computation properly populates the dcm_ini_list and dcm_eos_list
  void computeDCM_states();

  // Initialize trajectory clock
  void initialize_internal_clocks();

  // Given a delta_t compute the next desired DCM
  Eigen::Vector3d get_next_desired_DCM(const double & dt);

  // Given the current 
  Eigen::Vector3d get_desired_DCM(const int & step_index, const double & t);
  Eigen::Vector3d get_com_vel(const Eigen::Vector3d & current_com, const int & step_index, const double & t);


  double get_total_trajectory_time();
  void initialize_trajectory_discretization(const int & N_samples);

  void construct_trajectories(const std::vector<Footstep> & input_footstep_list, 
                              const Footstep & initial_left_footstance,
                              const Footstep & initial_right_footstance, 
                              const Eigen::Vector3d & initial_com,
                              const Eigen::Quaterniond initial_pelvis_ori);


  // trajectory objects
  Footstep mid_foot_;
  TrajSE3         traj_SE3_tmp;;
  TrajSE3         traj_SE3_left_foot;
  TrajSE3         traj_SE3_right_foot;
  TrajOrientation traj_ori_pelvis;
  TrajEuclidean   traj_pos_com;
  TrajEuclidean   traj_dcm_pos;

  std::vector<int> state_list;
  std::vector<int> bin_size_list;
  std::vector<int> stance_list;
  std::vector<Footstep> stance_location_list;
  std::vector<Footstep> swing_landing_location_list;

  void compute_trajectory_lists();
  void compute_pelvis_orientation_trajectory(const Eigen::Quaterniond & init_pelvis_ori,
                                             const Footstep & initial_left_footstance,
                                             const Footstep & initial_right_footstance);

  void compute_foot_trajectories(const Footstep & initial_left_footstance,
                                 const Footstep & initial_right_footstance);
  void compute_com_dcm_trajectory(const Eigen::Vector3d & initial_com);

  void setOrientationTrajectory(const int & starting_index, const int & N_bins, HermiteQuaternionCurve & curve, TrajOrientation & traj_ori);
  void setSwingFootTrajectory(const Footstep & init_location, const Footstep & landing_location, const int & starting_index, const int & N_bins, TrajSE3 & swing_foot);
  void setConstantSE3(const int & starting_index, const int & N_bins, TrajSE3 & traj, const Eigen::Vector3d & pos, const Eigen::Quaterniond & quat);  

private:
  // input: r_vrp_d_i - the desired virtual repelant point for the i-th step.
  //        t_step    - the time interval to use for backwards integration
  //        dcm_eos_i - the DCM state at the end of the i-th step. 
  // computes the step i's initial DCM state and the end-of-step i-1's dcm state. 
  // The computation is stored in the dcm_ini_list and dcm_eos_list. 
  Eigen::Vector3d computeDCM_ini_i(const Eigen::Vector3d & r_vrp_d_i, const double & t_step, const Eigen::Vector3d & dcm_eos_i);

  // Get the t_step for step i.
  double get_t_step(const int & step_i);

  double internal_timer;
  double internal_t_step;
  double internal_step_timer;
  int internal_step_i;

  int N_size;
  double internal_dt;




};

#endif