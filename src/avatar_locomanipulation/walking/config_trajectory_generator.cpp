#include <avatar_locomanipulation/walking/config_trajectory_generator.hpp>

// Constructor
ConfigTrajectoryGenerator::ConfigTrajectoryGenerator(){	
}

ConfigTrajectoryGenerator::ConfigTrajectoryGenerator(std::shared_ptr<RobotModel> & robot_model_in){
	this->setRobotModel(robot_model_in);
	commonInitialization();
}

ConfigTrajectoryGenerator::ConfigTrajectoryGenerator(std::shared_ptr<RobotModel> & robot_model_in, const int & N_size_in){
	N_size = N_size_in;
	this->setRobotModel(robot_model_in);
	commonInitialization();
}

// Destructor
ConfigTrajectoryGenerator::~ConfigTrajectoryGenerator(){
	std::cout << "[ConfigTrajectoryGenerator] Destroyed" << std::endl;
}


void ConfigTrajectoryGenerator::commonInitialization(){
	initializeDiscretization(N_size);
	initializeTasks();
	createTaskStack();

	// initialize temporary variables 
	tmp_pelvis_ori.setIdentity();
	tmp_pelvis_pos.setZero();
	tmp_com_pos.setZero();

	tmp_right_foot.setRightSide();
	tmp_left_foot.setLeftSide();

	tmp_rhand_pos.setZero();
	tmp_rhand_ori.setIdentity();

	tmp_lhand_pos.setZero();
	tmp_lhand_ori.setIdentity();

	tmp_torso_posture = Eigen::VectorXd::Zero(torso_posture_task->task_dim);
	tmp_neck_posture = Eigen::VectorXd::Zero(neck_posture_task->task_dim);
	tmp_rarm_posture = Eigen::VectorXd::Zero(rarm_posture_task->task_dim);
	tmp_larm_posture = Eigen::VectorXd::Zero(larm_posture_task->task_dim);
}

void ConfigTrajectoryGenerator::setRobotModel(std::shared_ptr<RobotModel> & robot_model_in){
	robot_model = robot_model_in;
	starting_config_ik_module.setRobotModel(robot_model_in);
	ik_module.setRobotModel(robot_model_in);
	q_start = Eigen::VectorXd::Zero(robot_model->getDimQ());
}

void ConfigTrajectoryGenerator::initializeDiscretization(const int & N_size_in){
	N_size = N_size_in;

	// std::cout << "Discretization set to " << N_size << std::endl;

	// Set a dummy dt. 
	double dt_dummy = 1e-3;
	// Initialize rajectory sizes 
	traj_q_config.set_dim_N_dt(robot_model->getDimQ(), N_size, dt_dummy);
	traj_SE3_left_hand.set_N_dt(N_size_in, dt_dummy);
	traj_SE3_right_hand.set_N_dt(N_size_in, dt_dummy);
	wpg.initialize_trajectory_discretization(N_size);

}

void ConfigTrajectoryGenerator::setStartingConfig(const Eigen::VectorXd & q_start_in){
	q_start = q_start_in;
	traj_q_config.set_pos(0, q_start);
	starting_config_ik_module.setInitialConfig(q_start);
	ik_module.setInitialConfig(q_start);
}


void ConfigTrajectoryGenerator::initializeTasks(){
	pelvis_ori_task = std::shared_ptr<Task>(new Task3DOrientation(robot_model, "pelvis"));
	com_task = std::shared_ptr<Task>(new TaskCOM(robot_model));

	rfoot_task = std::shared_ptr<Task>(new Task6DPose(robot_model, "rightCOP_Frame"));
	lfoot_task = std::shared_ptr<Task>(new Task6DPose(robot_model, "leftCOP_Frame"));

	rhand_task = std::shared_ptr<Task>(new Task6DPose(robot_model, "rightPalm"));
	lhand_task = std::shared_ptr<Task>(new Task6DPose(robot_model, "leftPalm"));

    std::vector<std::string> torso_joint_names = {"torsoYaw", "torsoPitch", "torsoRoll"};
    std::vector<std::string> neck_joint_names = {"lowerNeckPitch", "neckYaw", "upperNeckPitch"};
    std::vector<std::string> left_arm_joint_names = {"leftShoulderPitch", "leftShoulderRoll", "leftShoulderYaw", "leftElbowPitch", "leftForearmYaw", "leftWristRoll", "leftWristPitch"};
    std::vector<std::string> right_arm_joint_names = {"rightShoulderPitch", "rightShoulderRoll", "rightShoulderYaw", "rightElbowPitch", "rightForearmYaw", "rightWristRoll", "rightWristPitch"};

    torso_posture_task = std::shared_ptr<Task>(new TaskJointConfig(robot_model, torso_joint_names));
    neck_posture_task = std::shared_ptr<Task>(new TaskJointConfig(robot_model, neck_joint_names));
    rarm_posture_task = std::shared_ptr<Task>(new TaskJointConfig(robot_model, right_arm_joint_names));
    larm_posture_task = std::shared_ptr<Task>(new TaskJointConfig(robot_model, left_arm_joint_names));
}


void ConfigTrajectoryGenerator::setUseRightHand(bool use_right_hand_in){
	use_right_hand = use_right_hand_in;	
}
void ConfigTrajectoryGenerator::setUseLeftHand(bool use_left_hand_in){	
	use_left_hand = use_left_hand_in;
}
void ConfigTrajectoryGenerator::setUseTorsoJointPosition(bool use_torso_joint_position_in){	
	use_torso_joint_position = use_torso_joint_position_in;
}

void ConfigTrajectoryGenerator::reinitializeTaskStack(){
	this->createTaskStack();
}

void ConfigTrajectoryGenerator::createTaskStack(){
	// Set the task stack for the starting config IK
	std::vector< std::shared_ptr<Task> > vec_task_stack_start_config = {pelvis_ori_task, com_task, rfoot_task, lfoot_task, 
																						 rarm_posture_task, larm_posture_task, 
																						 torso_posture_task, neck_posture_task};

	std::vector< std::shared_ptr<Task> > vec_task_stack = {pelvis_ori_task, com_task, lfoot_task, rfoot_task, neck_posture_task};

	// Check whether to use a right hand SE(3) task or a right arm joint position task
	if (use_right_hand){
		vec_task_stack.push_back(rhand_task);
	}else{
		vec_task_stack.push_back(rarm_posture_task);
	}

	// Check whether to use a left hand SE(3) task or a left arm joint position task
	if (use_left_hand){
		vec_task_stack.push_back(lhand_task);
	}else{
		vec_task_stack.push_back(larm_posture_task);
	}

	// Check whether to use a torso joint position task
	if (use_torso_joint_position){
		vec_task_stack.push_back(torso_posture_task);	
	}

	// Clear the task hierarchy in the IK module
	starting_config_ik_module.clearTaskHierarchy();
	ik_module.clearTaskHierarchy();

	// Stack the tasks. Use reset to deallocate old value
	task_stack_starting_config.reset(new TaskStack(robot_model, vec_task_stack_start_config));
	task_stack.reset(new TaskStack(robot_model, vec_task_stack));

	// Add the tasks to the task hierarchy in the ik module
	starting_config_ik_module.addTasktoHierarchy(task_stack_starting_config);
	ik_module.addTasktoHierarchy(task_stack);

	// Prepare the ik modules
	starting_config_ik_module.prepareNewIKDataStrcutures();
	ik_module.prepareNewIKDataStrcutures();

}

int ConfigTrajectoryGenerator::getDiscretizationSize(){
	return N_size;
}


void ConfigTrajectoryGenerator::setPostureTaskReference(std::shared_ptr<Task> & posture_task, const Eigen::VectorXd & q_config){
	Eigen::VectorXd q_ref;

	std::vector<std::string> posture_task_joint_names;
	posture_task_joint_names = std::static_pointer_cast<TaskJointConfig>(posture_task)->getJointNames();

	getSelectedPostureTaskReferences(posture_task_joint_names, q_config, q_ref);
	posture_task->setReference(q_ref);
}

void ConfigTrajectoryGenerator::getSelectedPostureTaskReferences(std::vector<std::string> & selected_names, const Eigen::VectorXd & q_config, Eigen::VectorXd & q_ref){
  Eigen::VectorXd q_des;
  q_des = Eigen::VectorXd::Zero(selected_names.size());

  // Use the initial configuration to find the reference vector for the posture task
  for(int i = 0; i < selected_names.size(); i++){
    // std::cout << selected_names[i] << std::endl;
    q_des[i] = q_config[robot_model->getJointIndex(selected_names[i])];
  }
  q_ref = q_des;	
}


void ConfigTrajectoryGenerator::computeInitialConfigForFlatGround(const Eigen::VectorXd & q_guess, Eigen::VectorXd & q_out){
	// Set the starting config
	setStartingConfig(q_guess);
	// Update robot kinematics
	robot_model->updateFullKinematics(q_guess);

	// get x,y,z of left and right feet. pelvis orientation and com position 
	robot_model->getFrameWorldPose("leftCOP_Frame", tmp_left_foot.position, tmp_left_foot.orientation);
	robot_model->getFrameWorldPose("rightCOP_Frame", tmp_right_foot.position, tmp_right_foot.orientation);	
	robot_model->getFrameWorldPose("pelvis", tmp_pelvis_pos, tmp_pelvis_ori);	
	tmp_com_pos = robot_model->x_com;

	// bring feet to z = 0.0;
	// bring com height to z = 1.0.
	tmp_left_foot.position[2] = 0.0;
	tmp_right_foot.position[2] = 0.0;
	tmp_com_pos[2] = 1.0;

	// Set Task References
	pelvis_ori_task->setReference(tmp_pelvis_ori);
	com_task->setReference(tmp_com_pos);
	rfoot_task->setReference(tmp_right_foot.position, tmp_right_foot.orientation);
	lfoot_task->setReference(tmp_left_foot.position, tmp_left_foot.orientation);

	setPostureTaskReference(torso_posture_task, q_start);
	setPostureTaskReference(neck_posture_task, q_start);
	setPostureTaskReference(rarm_posture_task, q_start);
	setPostureTaskReference(larm_posture_task, q_start);


	// Perform IK
    int solve_result;
    double total_error_norm;
    std::vector<double> task_error_norms;
    Eigen::VectorXd q_sol = Eigen::VectorXd::Zero(robot_model->getDimQdot());	

	starting_config_ik_module.solveIK(solve_result, task_error_norms, total_error_norm, q_sol);
	starting_config_ik_module.printSolutionResults();

    q_out = q_sol;
}

// Given an initial configuration and footstep data list input, compute the task space walking trajectory.
// Warning: If hand tasks are enabled, they need to have been set already.
void ConfigTrajectoryGenerator::computeConfigurationTrajectory(const Eigen::VectorXd & q_init, const std::vector<Footstep> & input_footstep_list){
	// Set the starting config
	setStartingConfig(q_init);
	// Update robot kinematics
	robot_model->updateFullKinematics(q_init);

	// Get the initial footstep stances
	// Get the initial CoM position
	// Get the initial pelvis orientation

	// Construct the task space trajectories.
	// wpg.construct_trajectories(input_footstep_list, initial_left_footstance, initial_right_footsance, initial_com, initial_pelvis_ori)


	// set joint position task reference.
	// for loop. set references. check for convergence.



}



// construct_trajectories(const std::vector<Footstep> & input_footstep_list, 
//                                                      const Footstep & initial_left_footstance,
//                                                      const Footstep & initial_right_footstance, 
//                                                      const Eigen::Vector3d & initial_com,
//                                                      const Eigen::Quaterniond initial_pelvis_ori)