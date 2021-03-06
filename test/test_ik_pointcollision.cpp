#include <iostream>
#include <avatar_locomanipulation/ik_module/ik_module.hpp>

#include <avatar_locomanipulation/tasks/task.hpp>
#include <avatar_locomanipulation/tasks/task_stack.hpp>
#include <avatar_locomanipulation/tasks/task_pointcollision.hpp>
#include <avatar_locomanipulation/tasks/task_4dcontact_normal.hpp>
#include <avatar_locomanipulation/tasks/task_6dpose.hpp>
#include <avatar_locomanipulation/tasks/task_6dpose_wrt_midfeet.hpp>
#include <avatar_locomanipulation/tasks/task_3dorientation.hpp>
#include <avatar_locomanipulation/tasks/task_joint_config.hpp>
#include <avatar_locomanipulation/tasks/task_stack.hpp>
#include <avatar_locomanipulation/tasks/task_com.hpp>
#include <avatar_locomanipulation/tasks/task_contact_normal.hpp>


// Import ROS and Rviz visualization
#include <ros/ros.h>
#include <avatar_locomanipulation/bridge/val_rviz_translator.hpp>


void visualize_robot(Eigen::VectorXd & q_start, Eigen::VectorXd & q_end);

void printVec(const std::string & vec_name, const::Eigen::VectorXd vec){
  std::cout << vec_name << ": ";
  for(int i = 0; i < vec.size(); i++){
    std::cout << vec[i] << ", ";
  } 
  std::cout << std::endl;
}


void initialize_config(Eigen::VectorXd & q_init){

	std::string filename = THIS_PACKAGE_PATH"models/valkyrie_simplified_collisions.urdf";
  std::string srdf_filename = THIS_PACKAGE_PATH"models/valkyrie_disable_collisions.srdf";
  std::string meshDir  = THIS_PACKAGE_PATH"../val_model/";

	// Initialize Valkyrie RobotModel
	std::shared_ptr<RobotModel> valkyrie(new RobotModel(filename, meshDir, srdf_filename) );

  filename = THIS_PACKAGE_PATH"models/test_cart.urdf";
  meshDir  = THIS_PACKAGE_PATH"models/cart/";

  // Initialize Cart RobotModel
  std::shared_ptr<RobotModel> cart(new RobotModel(filename, meshDir) );


  // Define the configuration of Val
  Eigen::VectorXd q_start;
  q_start = Eigen::VectorXd::Zero(valkyrie->getDimQ());

  double theta = 0;//M_PI/4.0;
  Eigen::AngleAxis<double> aa(theta, Eigen::Vector3d(0.0, 0.0, 1.0));
  Eigen::Quaternion<double> init_quat(1.0, 0.0, 0.0, 0.0); //Initialized to remember the w component comes first
  init_quat = aa;

  q_start[3] = init_quat.x(); q_start[4] = init_quat.y(); q_start[5] = init_quat.z(); q_start[6] = init_quat.w(); // Set up the quaternion in q

  q_start[2] = 1.0; // set z value to 1.0, this is the pelvis location

  q_start[valkyrie->getJointIndex("leftHipPitch")] = -0.3;
  q_start[valkyrie->getJointIndex("rightHipPitch")] = -0.3;
  q_start[valkyrie->getJointIndex("leftKneePitch")] = 0.6;
  q_start[valkyrie->getJointIndex("rightKneePitch")] = 0.6;
  q_start[valkyrie->getJointIndex("leftAnklePitch")] = -0.3;
  q_start[valkyrie->getJointIndex("rightAnklePitch")] = -0.3;

  q_start[valkyrie->getJointIndex("rightShoulderPitch")] = -0.2;
  q_start[valkyrie->getJointIndex("rightShoulderRoll")] = 1.1;
  q_start[valkyrie->getJointIndex("rightElbowPitch")] = 0.4;
  q_start[valkyrie->getJointIndex("rightForearmYaw")] = 1.5;

  q_start[valkyrie->getJointIndex("leftShoulderPitch")] = -0.2;
  q_start[valkyrie->getJointIndex("leftShoulderRoll")] = -1.1;
  q_start[valkyrie->getJointIndex("leftElbowPitch")] = -0.4;
  q_start[valkyrie->getJointIndex("leftForearmYaw")] = 1.5;

  q_init = q_start;

  
}

void testIK_module(){
  std::cout << "[IK Module Test]" << std::endl;
  Eigen::VectorXd q_init;
  initialize_config(q_init);

   // Create IK Module
  std::string urdf_filename = THIS_PACKAGE_PATH"models/valkyrie_simplified_collisions.urdf";
  std::string srdf_filename = THIS_PACKAGE_PATH"models/valkyrie_disable_collisions.srdf";
  std::string meshDir_  = THIS_PACKAGE_PATH"../val_model/"; 
  std::cout << "Initialize Valkyrie Model" << std::endl;
  std::shared_ptr<RobotModel> valkyrie(new RobotModel(urdf_filename, meshDir_, srdf_filename));
  
  IKModule ik_module(valkyrie);
  ik_module.setInitialConfig(q_init);  

  // Update Robot Kinematics
  ik_module.robot_model->enableUpdateGeomOnKinematicsUpdate(true);
  ik_module.robot_model->updateFullKinematics(q_init);

  // Eigen::Vector3d cur_pos;
  // Eigen::Quaternion<double> cur_ori;
  // ik_module.robot_model->getFrameWorldPose("pelvis", cur_pos, cur_ori);
  // std::cout << "pwlvis cur pos:\n" << cur_pos << std::endl;
  // ik_module.robot_model->getFrameWorldPose("rightElbowPitch", cur_pos, cur_ori);
  // std::cout << "left palm cur pos:\n" << cur_pos << std::endl;

  std::shared_ptr<CollisionEnvironment> collision(new CollisionEnvironment(ik_module.robot_model) );

  collision->set_safety_distance_normal(0.3);

  // Define list of points for the pointcollision task
  std::vector<Eigen::Vector3d> point_list;
  Eigen::Vector3d point;
  point << 0.275, 0.0, 1.;
  point_list.push_back(point);
  point[0] = 0.275; point[1] = 0.0; point[2] = 0.8; 
  point_list.push_back(point);
  point[0] = 0.275; point[1] = 0.0; point[2] = 0.6; 
  point_list.push_back(point);
  point[0] = 0.275; point[1] = 0.0; point[2] = 0.4; 
  point_list.push_back(point);
  point[0] = 0.275; point[1] = 0.0; point[2] = 0.2; 
  point_list.push_back(point);
  
  Eigen::Vector3d floor_normal(0,0,1);
  Eigen::Vector3d floor_center(0,0,0); 

  // Create Tasks
  std::shared_ptr<Task> pelvis_task(new Task6DPose(ik_module.robot_model, "pelvis"));
  std::shared_ptr<Task> lfoot_task(new TaskContactNormalTask(ik_module.robot_model, "leftCOP_Frame", floor_normal, floor_center));
  std::shared_ptr<Task> rfoot_task(new TaskContactNormalTask(ik_module.robot_model, "rightCOP_Frame", floor_normal, floor_center));
  std::shared_ptr<Task> pointlist_task(new TaskPointCollision("pointlist_task", ik_module.robot_model, collision, point_list));


  // Stack Tasks in order of priority
  std::shared_ptr<Task> task_stack_priority_1(new TaskStack(ik_module.robot_model, {pelvis_task, lfoot_task, rfoot_task}));
  std::shared_ptr<Task> task_stack_priority_2(new TaskStack(ik_module.robot_model, {pointlist_task}));

  pointlist_task->setTaskGain(75.0);

  // Set desired Pelvis configuration
  Eigen::Vector3d pelvis_des_pos;
  Eigen::Quaternion<double> pelvis_des_quat;  

   // Set desired Foot configuration
  Eigen::Quaternion<double> rfoot_des_quat;

  Eigen::Quaternion<double> lfoot_des_quat;

  // Pelvis should be 1m above the ground and the orientation must be identity
  pelvis_des_pos.setZero();
  pelvis_des_pos[2] = 1.05;
  pelvis_des_quat.setIdentity();

  // Foot should be flat on the ground
  rfoot_des_quat.setIdentity();

  lfoot_des_quat.setIdentity();

  // Set Tsak references
  pelvis_task->setReference(pelvis_des_pos, pelvis_des_quat);
  lfoot_task->setReference(lfoot_des_quat);
  rfoot_task->setReference(rfoot_des_quat);

  // Get Errors -----------------------------------------------------------------------------
  Eigen::VectorXd task_error;
  pointlist_task->getError(task_error);
  std::cout << "Point Avoidance Task Error = " << task_error.transpose() << std::endl;

  ik_module.addTasktoHierarchy(task_stack_priority_1);
  ik_module.addTasktoHierarchy(task_stack_priority_2);

  int solve_result;
  double error_norm;
  Eigen::VectorXd q_sol = Eigen::VectorXd::Zero(ik_module.robot_model->getDimQdot());

  double error_tol = 1e-6;
  ik_module.setErrorTol(error_tol);
 
  int N = 500;
  int mN = 400;
  ik_module.setMaxIters(N);
  ik_module.setMaxMinorIters(mN);
  ik_module.prepareNewIKDataStrcutures();
  ik_module.solveIK(solve_result, error_norm, q_sol);
  ik_module.printSolutionResults();

  Eigen::VectorXd q_config = Eigen::VectorXd::Zero(q_init.size());
  printVec("q_sol", q_sol);

  visualize_robot(q_init, q_sol);
}

void visualize_robot(Eigen::VectorXd & q_start, Eigen::VectorXd & q_end){
  // Initialize ROS node for publishing joint messages
  ros::NodeHandle n;
  ros::Rate loop_rate(20);

  // Initialize Rviz translator
  ValRvizTranslator rviz_translator;

  // Transform broadcaster
  tf::TransformBroadcaster      br_ik;
  tf::TransformBroadcaster      br_robot;
  // Joint State Publisher
  ros::Publisher robot_ik_joint_state_pub = n.advertise<sensor_msgs::JointState>("robot1/joint_states", 10);
  ros::Publisher robot_joint_state_pub = n.advertise<sensor_msgs::JointState>("robot2/joint_states", 10);

  // Initialize Transforms and Messages
  tf::Transform tf_world_pelvis_init;
  tf::Transform tf_world_pelvis_end;

  sensor_msgs::JointState joint_msg_init;
  sensor_msgs::JointState joint_msg_end;

  // Initialize Robot Model
  std::string urdf_filename = THIS_PACKAGE_PATH"models/valkyrie_simplified_collisions.urdf";
  std::string srdf_filename = THIS_PACKAGE_PATH"models/valkyrie_disable_collisions.srdf";
  std::string meshDir_  = THIS_PACKAGE_PATH"../val_model/"; 
  std::cout << "Initialize Valkyrie Model" << std::endl;
  RobotModel valkyrie(urdf_filename, meshDir_, srdf_filename);

  // Visualize q_start and q_end in RVIZ
  rviz_translator.populate_joint_state_msg(valkyrie.model, q_start, tf_world_pelvis_init, joint_msg_init);
  rviz_translator.populate_joint_state_msg(valkyrie.model, q_end, tf_world_pelvis_end, joint_msg_end);

  while (ros::ok()){
      br_robot.sendTransform(tf::StampedTransform(tf_world_pelvis_init, ros::Time::now(), "world",  "val_robot/pelvis"));
      robot_joint_state_pub.publish(joint_msg_init);

      br_ik.sendTransform(tf::StampedTransform(tf_world_pelvis_end, ros::Time::now(), "world", "val_ik_robot/pelvis"));
      robot_ik_joint_state_pub.publish(joint_msg_end);
    ros::spinOnce();
    loop_rate.sleep();
  }
}




int main(int argc, char ** argv){
  ros::init(argc, argv, "test_ik_module");
  testIK_module();
  return 0;
}