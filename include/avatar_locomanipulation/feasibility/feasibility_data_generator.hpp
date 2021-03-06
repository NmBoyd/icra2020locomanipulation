#ifndef ALM_FEASIBILITY_DATA_GENERATOR_H
#define ALM_FEASIBILITY_DATA_GENERATOR_H

#include <math.h>
#include <time.h>
#include <random>

#include <stdlib.h>

#include <Configuration.h>
#include <avatar_locomanipulation/models/robot_model.hpp>
#include <avatar_locomanipulation/ik_module/ik_module.hpp>
#include <avatar_locomanipulation/walking/walking_pattern_generator.hpp>
#include <avatar_locomanipulation/walking/config_trajectory_generator.hpp>

#include <avatar_locomanipulation/data_types/footstep.hpp>
#include <avatar_locomanipulation/data_types/trajectory_SE3.hpp>

#include <avatar_locomanipulation/tasks/task_6dpose.hpp>
#include <avatar_locomanipulation/tasks/task_3dorientation.hpp>
#include <avatar_locomanipulation/tasks/task_com.hpp>
#include <avatar_locomanipulation/tasks/task_joint_config.hpp>
#include <avatar_locomanipulation/tasks/task_stack.hpp>

#include <avatar_locomanipulation/helpers/orientation_utils.hpp>
#include <avatar_locomanipulation/helpers/convex_hull.hpp>

// Parameter Loader and Saver
#include <avatar_locomanipulation/helpers/yaml_data_saver.hpp>
#include <avatar_locomanipulation/helpers/param_handler.hpp>
#include <fstream>

#define CASE_MANIPULATION_LEFT_HAND 0
#define CASE_MANIPULATION_RIGHT_HAND 1
#define CASE_MANIPULATION_BOTH_HANDS 2

#define CASE_STANCE_LEFT_FOOT 0 // Left foot stance right foot swing 
#define CASE_STANCE_RIGHT_FOOT 1 // Right foot stance left foot swing
#define CASE_STANCE_DOUBLE_SUPPORT 2 // No transition

class FeasibilityDataGenerator{
public:
	FeasibilityDataGenerator();
	~FeasibilityDataGenerator();

    void setRobotModel(std::shared_ptr<RobotModel> & robot_model_in);

    // Initialize the seed number to be used:
    void initializeSeed(unsigned int seed_number);
    // Initialize the starting IK tasks
    void initializeStartingIKTasks();
    // Initialize configuration limits
    void initializeConfigurationLimits();

    // Randomly generate a number from a specified interval
	double generateRandMinMax(const double min, const double max);
	// Generate random starting configuration 
    bool randomizeStartingConfiguration();
    // Sets the starting IK configuration
	void setStartingIKConfig(const Eigen::VectorXd & q_ik_start_in);
	// Randomize the foot landing configuration
	void randomizeFootLandingConfiguration();

	// load yaml file which sets all the parameters
	void loadParamFile(const std::string filepath);

	// sets up the tasks and parameters for the config trajectory generation module.
	void initializeConfigTrajectoryGenerationModule();
	// Randomly generate a contact transition data
	// -- assumes initializeConfigTrajectoryGenerationModule() has already been called.
	bool generateContactTransitionData(bool store_data=false);

	// runs in a loop until generateContactTransiitionData() returns true for N = num_data_to_generate times.
	bool generateNDataTransitions(int num_data_to_generate, bool store_data=false);

	// set whether to generate only positive examples
	void setGenerateOnlyPositiveExamples(bool generate_only_positive_examples_in);
	bool generate_only_positive_examples = false;

	// Emit YAML Files
	int initial_config_counter = 0;
	void storeInitialConfiguration();

	int raw_positive_transition_data_counter = 0;
	void storePositiveTransitionData();

	int positive_transition_data_counter = 0;	
	int negative_transition_data_counter = 0;
	void storeTransitionDatawithTaskSpaceInfo(bool result);

    // Parameter Handler
    ParamHandler param_handler;

    // Public Member Variables
	std::shared_ptr<RobotModel> robot_model;
	std::shared_ptr<IKModule> ik_start_config_module;
	std::shared_ptr<ConfigTrajectoryGenerator> ctg;

	std::vector<std::string> upper_body_joint_names;
	std::shared_ptr<Task> upper_body_config_task;

	std::shared_ptr<Task> left_foot_task;
	std::shared_ptr<Task> right_foot_task;
	std::shared_ptr<Task> pelvis_task;

	std::vector< std::shared_ptr<Task> > vec_task_stack;
	std::shared_ptr<Task> ik_task_stack;

	// Task References
	Eigen::VectorXd joint_pos;
	
	Footstep landing_footstep;
	Footstep left_footstep;
	Footstep right_footstep;

	Eigen::Vector3d pelvis_pos;
	Eigen::Quaterniond pelvis_ori;	

    Eigen::Vector3d starting_rhand_pos;
    Eigen::Quaterniond starting_rhand_ori;

    Eigen::Vector3d starting_lhand_pos;
    Eigen::Quaterniond starting_lhand_ori;

	// std::shared_ptr<Task> pelvis_ori;
	// std::shared_ptr<Task> com_task;

	void common_initialization();

	// data generation parameters
	double max_reach = 0.4; // maximum forward step
	double min_reach = -0.2; // maximum backward step
	double max_width = 0.4; // maximum side step
	double min_width = 0.2; // minimum side step
	double max_theta = 0.6; // maximum foot angle w.r.t stance
	double min_theta = -0.15; // minimum foot angle w.r.t stance

	double convex_hull_percentage = 0.9; // Percentage of convex hull to use for randomization of pelvis location
	double pelvis_height_min = 0.9;
	double pelvis_height_max = 1.05;

	double com_height_min = 0.9;
	double com_height_max = 1.0;

	double walking_com_height = 0.95;
	double walking_double_support_time = 0.45; // AKA transfer time
	double walking_single_support_time = 1.0; // AKA swing time	
	double walking_settling_percentage = 0.999;
	double walking_swing_height = 0.1;

	std::string manipulation_type = ""; // "left_hand" "right_hand" "both_hands" 
	std::string stance_foot = ""; // "left_foot" / "right_foot"

	int N_resolution = 60; // Resolution discretization to use for solving the transition trajectory
	int loaded_seed_number = 1;

	std::string parent_folder_path = "Data/"; // folder file path to use when generating the data 

	void printDataGenerationParameters();

	// robot configuration data types
	Eigen::VectorXd q_min;
	Eigen::VectorXd q_max;	
	Eigen::VectorXd q_ik_start;
	Eigen::VectorXd q_start;	

	// Trajectory configuration
	TrajEuclidean   traj_q_config;

	// data gen case
	int data_gen_manipulation_case;
	int data_gen_stance_case;


	// temp variables
	Eigen::Vector3d stance_foot_pos;
	Eigen::Quaterniond stance_foot_ori;

	Eigen::Vector3d swing_foot_pos;
	Eigen::Quaterniond swing_foot_ori;
	double swing_foot_theta_angle;

	Eigen::Vector3d landing_foot_pos;
	Eigen::Quaterniond landing_foot_ori;
	double landing_foot_theta_angle;


	Eigen::VectorXd q_rand;


	void getFeetVertexList();
	void getRandomPelvisLocation(Eigen::Vector3d & pelvis_out);
	std::vector<Eigen::Vector3d> foot_contact_list_3d;
	std::vector<math_utils::Point> foot_contact_list_2d;
	std::vector<math_utils::Point> contact_hull_vertices;
	std::mt19937 generator;


};

#endif