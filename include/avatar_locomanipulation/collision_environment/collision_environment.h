#ifndef COLLISION_ENVIRONMENT_H
#define COLLISION_ENVIRONMENT_H

#include <Configuration.h> // Package Path
#include <avatar_locomanipulation/enable_pinocchio_with_hpp_fcl.h> // Enable HPP FCL
// Robot Model
#include <avatar_locomanipulation/models/robot_model.hpp>
// Directed Vectors
#include <avatar_locomanipulation/collision_environment/directed_vectors.hpp>



class CollisionEnvironment
{
private:

  // The two robot models which we are adding to the environment
  std::shared_ptr<RobotModel> valkyrie, object;


  // for internally filling the vector of directed vectors  
  // Members: std::string to - name of joint vector points to
  //          std::string from - name of joint vector comes from
  //          Eigen::Vector3d direction - normalized direction of vector
  //          double magnitude - magnitude of the vector for use with potential
  DirectedVectors dvector;


  // builds a map of names for easy use to frame name for getting current world position
  std::map<std::string, std::string> make_map_to_frame_vector();
  std::map<std::string, std::string> make_map_to_frame_vector_subset();


  // builds a map of names for easy use to collision body name for getting nearest point
  std::map<std::string, std::string> make_map_to_body_vector();


  // appends the models internally
  // Input: robot config, object config
  std::shared_ptr<RobotModel> append_models(Eigen::VectorXd & q, Eigen::VectorXd & obj_config);


  // These functions will be called by build_self_directed_vectors
  // Input: subset of world positions
  void build_directed_vector_to_rhand(std::map<std::string, Eigen::Vector3d> world_positions);
  void build_directed_vector_to_lhand(std::map<std::string, Eigen::Vector3d> world_positions);
  void build_directed_vector_to_elbows(std::map<std::string, Eigen::Vector3d> world_positions);
  void build_directed_vector_to_knees(std::map<std::string, Eigen::Vector3d> world_positions);
  void build_directed_vector_to_head(std::map<std::string, Eigen::Vector3d> world_positions);
public:

  // The vector of directed vectors and related information
  std::vector<DirectedVectors> directed_vectors;
  

  // Constructor fills the local data for valkyrie, object RobotModels
  // Inputs: RobotModel valkyrie
  //         RobotModel environmental_object
  CollisionEnvironment(std::shared_ptr<RobotModel> & val, std::shared_ptr<RobotModel> & obj);


  ~CollisionEnvironment();


  // builds vectors from object to joints of interest using maps defined by find_world_positions and find_near_points
  // Input: robot config, object config
  void build_directed_vectors(Eigen::VectorXd & q, Eigen::VectorXd & obj_config);


  // builds vectors from joints to joints 
  // Input: robot config, object config
  void build_self_directed_vectors(Eigen::VectorXd & q, Eigen::VectorXd & obj_config);


  // fills the map from joint name to world position
  std::map<std::string, Eigen::Vector3d> find_world_positions();


  // fills the map from joint name to world position for our subset used in self collision checking
  std::map<std::string, Eigen::Vector3d> find_world_positions_subset();


  // fills map from object link name to nearest point on that body to pelvis
  // Input: appended model,
  //        object link name on which we are interested in near point
  std::map<std::string, Eigen::Vector3d> find_near_points(std::shared_ptr<RobotModel> & appended, std::string name);
	

  // computes collision and outputs any contacts
  void compute_collision(Eigen::VectorXd & q, Eigen::VectorXd & obj_config);
};




#endif
