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
  // for internally filling the vector of directed vectors  
  // Members: std::string to - name of joint vector points to
  //          std::string from - name of joint vector comes from
  //          Eigen::Vector3d direction - normalized direction of vector
  //          double magnitude - magnitude of the vector for use with potential
  DirectedVectors dvector;

  // a map from collision body names to frame names
  std::map<std::string, std::string> collision_to_frame;

  // The tolerance distance for avoiding self collisions
  double safety_dist = 0.05;


  // appends the models internally
  // Input: robot config, object config 
  // Used in distance computation, which does not require the appended config
  std::shared_ptr<RobotModel> append_models(Eigen::VectorXd & q, Eigen::VectorXd & obj_config);


  // builds a map from our common names like "rhand" to frame names like "rightPalm"
  std::map<std::string, std::string> make_map_to_frame_names_subset();


  // Input: empty map string to string
  // fills the map with key being the collision body names and value being the corresponding frame name
  void map_collision_names_to_frame_names();


  // Builds a directed vector using world positions for the case when two bodies are in collision
  // called internally from build_directed_vectors_name
  // Input: name of two collision bodies
  void get_dvector_collision_links(const std::string & from_name, const std::string & to_name);

public:

  // The two robot models which we are adding to the environment
  std::shared_ptr<RobotModel> valkyrie, object;

  // The vector of directed vectors and related information
  std::vector<DirectedVectors>  self_directed_vectors;
  

  // Constructor fills the local data for valkyrie, object RobotModels
  // Inputs: RobotModel valkyrie
  //         RobotModel environmental_object
  CollisionEnvironment(std::shared_ptr<RobotModel> & val, std::shared_ptr<RobotModel> & obj);

  // Constructor for only valkyrie model
  CollisionEnvironment(std::shared_ptr<RobotModel> & val);


  ~CollisionEnvironment();


  // Inputs: - List of collision object names, with the first being used as the "to" object
  //         - (Empty) map from names of "from" collision links to the nearest point on those objects
  //         - (Empty) map from names of "from"collision links to nearest points on the "to" objects
  void find_self_near_points(std::vector<std::string> & list, std::map<std::string, Eigen::Vector3d> & from_near_points, std::map<std::string, Eigen::Vector3d> & to_near_points);

  
  // Fills struct DirectedVector self_directed_vectors with the relevant vectors
  //  to the link indicated in the fnc name
  void build_directed_vector_to_rhand();
  void build_directed_vector_to_lhand();
  void build_directed_vector_to_head();
  void build_directed_vector_to_lknee();
  void build_directed_vector_to_rknee();
  void build_directed_vector_to_relbow();
  void build_directed_vector_to_lelbow();


  // computes collision and outputs any contacts
  void compute_collision(Eigen::VectorXd & q, Eigen::VectorXd & obj_config);


  // gives us a command for dx to move away from self collision
  std::vector<Eigen::Vector3d> self_collision_dx();
  

  // Sets the safety distance between robot links
  void set_safety_distance(double & safety_dist_in);
};




#endif
