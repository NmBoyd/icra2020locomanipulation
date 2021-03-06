#ifndef ALM_LOCOMANIPULATION_A_STAR_PLANNER_H
#define ALM_LOCOMANIPULATION_A_STAR_PLANNER_H

// ROS related files
#include <ros/ros.h>
#include <avatar_locomanipulation/BinaryClassifierQuery.h>

// CPP version of the neural network
#include <avatar_locomanipulation/helpers/NeuralNetModel.hpp>
#include <avatar_locomanipulation/helpers/IOUtilities.hpp>

// Lib for loading environment variables
#include <stdlib.h>
// Parameter Loader and Saver
#include <avatar_locomanipulation/helpers/yaml_data_saver.hpp>
#include <avatar_locomanipulation/helpers/param_handler.hpp>
#include <iostream>
#include <fstream>

#include <avatar_locomanipulation/planners/a_star_planner.hpp>
#include <avatar_locomanipulation/walking/config_trajectory_generator.hpp>
#include <avatar_locomanipulation/data_types/manipulation_function.hpp>
#include <avatar_locomanipulation/data_types/footstep.hpp>

#define CONTACT_TRANSITION_DATA_LEFT_FOOT_STANCE 0
#define CONTACT_TRANSITION_DATA_RIGHT_FOOT_STANCE 1

#define CONTACT_TRANSITION_DATA_LEFT_HAND 0
#define CONTACT_TRANSITION_DATA_RIGHT_HAND 1
#define CONTACT_TRANSITION_DATA_BOTH_HANDS 2

#define EDGE_TYPE_LOCOMANIPULATION 0
#define EDGE_TYPE_MANIPULATION 1

namespace planner{
    class LMVertex: public Node{
    public:
        LMVertex(); // Constructor
        virtual ~LMVertex(); // Destructor

        // Problem specific:
        LMVertex(double s_in, Eigen::VectorXd & q_init_in); // Constructor
        LMVertex(double s_in, Eigen::VectorXd & q_init_in, Footstep & left_foot_in, Footstep & right_foot_in); // Constructor
        LMVertex(double s_in, Footstep & left_foot_in, Footstep & right_foot_in); // Constructor
        LMVertex(double s_in); // Constructor

        double s = 0.0;
        Eigen::VectorXd q_init;
        bool take_a_step = false;

        Footstep left_foot;
        Footstep right_foot;
        Footstep mid_foot;
    
        void setRobotConfig(const Eigen::VectorXd & q_input);
        void common_initialization();

        double getAngle(const Eigen::Quaterniond & quat_in);
        Eigen::AngleAxisd tmp_aa;


    };


    class LocomanipulationPlanner: public A_starPlanner{
    public:
        LocomanipulationPlanner();
        virtual ~LocomanipulationPlanner();

        virtual void setStartNode(const shared_ptr<Node> begin_input);
        virtual void setGoalNode(const shared_ptr<Node> goal_input);

        virtual double gScore(const shared_ptr<Node> current, const shared_ptr<Node> neighbor);
        virtual double heuristicCost(const shared_ptr<Node> neighbor,const shared_ptr<Node> goal);
        virtual bool goalReached(shared_ptr<Node> current_node, shared_ptr<Node> goal);

        virtual bool constructPath();

        virtual std::vector< shared_ptr<Node> > getNeighbors(shared_ptr<Node> & current);

        bool reconstructConfigurationTrajectory();
        bool reconstructConfigurationTrajectoryv2();
        void printPath();

        std::shared_ptr<LMVertex> current_;        
        std::shared_ptr<LMVertex> goal_;
        std::shared_ptr<LMVertex> begin_;        
        std::shared_ptr<LMVertex> neighbor_;        
        std::shared_ptr<LMVertex> opt_node_;

        // Locomanipulation specific member variables and functions.
        void generateDiscretization();

        void initializeLocomanipulationVariables(std::shared_ptr<RobotModel> robot_model_in, std::shared_ptr<ManipulationFunction> f_s_in, std::shared_ptr<ConfigTrajectoryGenerator> ctg_in);
        
        void setClassifierClient(ros::ServiceClient & classifier_client_in);
        bool print_classifier_results = false;
        bool use_classifier = false;
        bool classifier_store_mistakes = false;
        bool classifier_store_mistakes_during_reconstruction = false;

        bool trust_classifier = false;

        // if true, the reconstruction step will be performed regardless of the approach in order to save the yaml file output.
        bool store_output = false;

        // Lazy evaluation speeds up solutions as we only prune out bad neighbors according to the classifier
        bool classifier_lazy_evaluate = true; 

        void setNeuralNetwork(std::shared_ptr<NeuralNetModel> nn_model_in, const Eigen::VectorXd & nn_mean_in, const Eigen::VectorXd & nn_std_dev_in);

        std::shared_ptr<RobotModel> robot_model;
        std::shared_ptr<ManipulationFunction> f_s;
        std::shared_ptr<ConfigTrajectoryGenerator> ctg;
        std::shared_ptr<NeuralNetModel> nn_model;
        Eigen::VectorXd nn_mean;
        Eigen::VectorXd nn_std;
        bool enable_cpp_nn = false;

        std::vector<Footstep> input_footstep_list;

        // Body Path heuristic. Feet positions w.r.t hand pose
        Eigen::Vector3d lf_pos_wrt_hand;
        Eigen::Vector3d rf_pos_wrt_hand;

        Eigen::Quaterniond lf_ori_wrt_hand;
        Eigen::Quaterniond rf_ori_wrt_hand;

        Eigen::Vector3d lf_guess_pos;
        Eigen::Quaterniond lf_guess_ori;

        Eigen::Vector3d rf_guess_pos;
        Eigen::Quaterniond rf_guess_ori;

        // Set the planner origin
        Eigen::Vector3d planner_origin_pos;
        Eigen::Quaterniond planner_origin_ori;      
        Eigen::Matrix3d R_planner_origin;
        Eigen::Matrix3d R_planner_origin_transpose;         

        // Discretized values
        double delta_s_max = 0.08;
        std::vector<double> delta_s_vals;
        std::vector<double> dx_vals;
        std::vector<double> dy_vals;
        std::vector<double> dtheta_vals;

        // lattice discretization to be used. with orientation aligned with the starting origin frame but 
        // the translation origin is aligned with the stance frame
        double dx = 0.05; // dx translation discretization
        double dy = 0.05; // dy translation discretization
        double dtheta = 10.0*M_PI/180.0; //10.0*M_PI/180.0; // 10 degrees of discretization

        double max_lattice_translation = 0.4; //0.3;//0.4;
        double max_lattice_theta = M_PI*7.0/8.0;
        double min_lattice_theta = -M_PI*7.0/8.0;

        double max_lattice_radius = 2.0; // meters from the origin
        double max_foot_to_hand_radius = 1.5; // meters from the landiing foot

        // swing foot kinematic limits w.r.t the stance frame
        double max_reach = 0.4;
        double min_reach = -0.3;

        double max_width = 0.4;
        double min_width = 0.2;

        double max_theta = 0.6; // radians
        double min_theta = -0.15; 


        // Get Neighbors Member Variables 
        double delta_s = 0.0;
        bool first_node_evaluated = false;
        std::vector < std::shared_ptr<Node> > neighbors;
        std::shared_ptr<LMVertex> parent_;
        std::shared_ptr<LMVertex> neighbor_change;

        //goal reached member variables
        double goal_s = 1.0;
        double goal_tol = 0.01;

        // planner parameters
        double w_heuristic = 2000; //10; 
        double w_distance = 100; //20; 
        double w_s = 100.0;     
        double w_step = 10;
        double w_transition_distance = 0.0; //10.0;

        double w_feasibility = 10.0; //0.0;//1e2;
        double feasibility_threshold= 0.5;

        int N_s = 5; // number of discretizations to make for the s variable when checking with the neural network
        int N_size_per_edge = 60;

        // robot_config temp 
        Eigen::VectorXd q_tmp;

        // Temporary variables
        Eigen::Vector3d tmp_pos;
        Eigen::Quaterniond tmp_ori;
        Eigen::Vector3d foot_frame_landing_foot_pos;
        Eigen::Quaterniond foot_frame_landing_foot_ori;


        // Full path configuration trajectory
        std::vector< shared_ptr<Node> > forward_order_optimal_path; 
        std::vector<double> s_traj; // Trajectory of s(i)
        TrajEuclidean   path_traj_q_config;     // Trajectory of configurations q


        // Data holder for the entire trajectory
        double dt_out;
        std::vector<double> time_vec;
        std::vector<Eigen::VectorXd> com_pos_traj;

        std::vector<Eigen::VectorXd> left_foot_pos_traj;
        std::vector<Eigen::VectorXd> right_foot_pos_traj;
        std::vector<Eigen::VectorXd> left_hand_pos_traj;
        std::vector<Eigen::VectorXd> right_hand_pos_traj;

        std::vector<Eigen::VectorXd> pelvis_ori_traj;
        std::vector<Eigen::VectorXd> left_foot_ori_traj;
        std::vector<Eigen::VectorXd> right_foot_ori_traj;
        std::vector<Eigen::VectorXd> left_hand_ori_traj;
        std::vector<Eigen::VectorXd> right_hand_ori_traj;

        std::vector<Eigen::VectorXd> q_vec_traj;
        std::vector<Footstep> footstep_list_trajectory;

        std::vector<Eigen::VectorXd> foot_landing_pos_traj;
        std::vector<Eigen::VectorXd> foot_landing_ori_traj;

        Clock::time_point start_edge_transition_evaluation; 
        Clock::time_point end_edge_transition_evaluation; 
        double edge_feasibility_evaluation_time = 0;

        int num_edge_evaluations = 0;       
        double mean_feasibility_evaluation_time;
        double std_feasibility_evaluation_time;
        std::vector<double> edge_eval_times;

        TrajEuclidean   tmp_traj_q_config;     // Trajectory of configurations q
        std::map<std::string, TrajEuclidean> edge_to_trajectory;

    private:
        // Converts the input position and orientation 
        // from the world frame to the planner frame
        void convertWorldToPlannerOrigin(const Eigen::Vector3d & pos_in, const Eigen::Quaterniond & ori_in,
                                    Eigen::Vector3d & pos_out, Eigen::Quaterniond & ori_out);
        // Converts the input position and orientation 
        // from the planner frame to the world frame
        void convertPlannerToWorldOrigin(const Eigen::Vector3d & pos_in, const Eigen::Quaterniond & ori_in,
                                    Eigen::Vector3d & pos_out, Eigen::Quaterniond & ori_out);

        bool withinKinematicBounds(Footstep & stance_foot, Eigen::Vector3d & landing_pos, Eigen::Quaterniond & landing_ori);

        double getAngle(const Eigen::Quaterniond & quat_in);
        Eigen::AngleAxisd tmp_aa;

        // Creates the footstep neighbors
        void generateNonFootstepNeighbors();
        void generateFootstepNeighbors(int footstep_side);

        // Ensures that is is within 0.0 and goal_s
        double clamp_s_variable(const double s_in);

        // Edge identification between the current node and its parent
        bool edgeHasStepTaken(shared_ptr<LMVertex> from_node, shared_ptr<LMVertex> to_node, int footstep_side);
        bool edgeHasSVarMoved(shared_ptr<LMVertex> from_node, shared_ptr<LMVertex> to_node);

        // Computes the feasibility between two nodes
        double getFeasibility(const shared_ptr<LMVertex> & from_node,const shared_ptr<LMVertex> & to_node);


        // Sets the NN variables for the stance foot, swing foot, and pelvis
        void setStanceSwingPelvisNNVariables(const shared_ptr<LMVertex> & from_node, const shared_ptr<LMVertex> & to_node, int robot_side);


        // Sets the stance foot for the neural network input 
        void setStanceFoot(const shared_ptr<LMVertex> & from_node, const int robot_side);
        // Sets the swing foot for the neural network input
        void setSwingFoot(const shared_ptr<LMVertex> & from_node, const shared_ptr<LMVertex> & to_node, const int robot_side);

        // Sets the hand poses for the neural network input based on the manipulation function and an input s
        // Assumes that nn_manipulation_type has already been set
        // Assumes that the stance frame has already been set
        void setHandPoses(double s_value);

         // Convert world to stance frame defined by feasibility_stance_foot_pos and feasibility_stance_foot_ori
        void convertWorldToStanceOrigin(const Eigen::Vector3d & pos_in, const Eigen::Quaterniond & ori_in,
                                                                       Eigen::Vector3d & pos_out, Eigen::Quaterniond & ori_out);

        // Assumes NN variables have been set except for the hand pose.
        // Queries the NN for feasibility along the s trajectory
        void computeHandTrajectoryFeasibility(const shared_ptr<LMVertex> & from_node, const shared_ptr<LMVertex> & to_node);

        // Feasibility function temporary variables 
        Eigen::Vector3d feasibility_stance_foot_pos;
        Eigen::Quaterniond feasibility_stance_foot_ori;
        Eigen::Matrix3d R_stance_origin;
        Eigen::Matrix3d R_stance_origin_transpose;      

        double nn_starting_pelvis_height = 0.0;

        int nn_stance_origin;
        int nn_manipulation_type;

        Eigen::Vector3d    nn_swing_foot_start_pos;
        Eigen::Quaterniond nn_swing_foot_start_ori;
        Eigen::Vector3d    nn_pelvis_pos;
        Eigen::Quaterniond nn_pelvis_ori;
        Eigen::Vector3d    nn_landing_foot_pos;
        Eigen::Quaterniond nn_landing_foot_ori;
        Eigen::Vector3d    nn_right_hand_start_pos;
        Eigen::Quaterniond nn_right_hand_start_ori;
        Eigen::Vector3d    nn_left_hand_start_pos;
        Eigen::Quaterniond nn_left_hand_start_ori;

        double nn_feasibility_score = 0.0;
        double nn_prediction_score = 0.0;
        double nn_delta_s = 0.0;

        // Define client to neural network
        ros::ServiceClient classifier_client;
    
        // Prepare classifier service request
        avatar_locomanipulation::BinaryClassifierQuery classifier_srv;

        //        
        double getClassifierResult();

        // Helpers for setting up classifier input
        double prediction_result = 0.0;
        int classifier_input_dim = 32;
        Eigen::Vector3d tmp_ori_vec3;

        // CPP Neural network helper functions:
        // Convert quat to vec
        Eigen::Vector3d quatToVec(const Eigen::Quaterniond & ori);
        void normalizeInputCalculate(const Eigen::VectorXd & x_in, const Eigen::VectorXd & data_mean, const Eigen::VectorXd & data_std_dev, Eigen::VectorXd & x_normalized);
        // ----

        // CPP Neural Network variables
        Eigen::VectorXd cpp_nn_rawDatum;
        Eigen::VectorXd cpp_nn_normalized_datum;
        int cpp_nn_input_size;
        Eigen::MatrixXd cpp_nn_data_input;
        Eigen::MatrixXd cpp_nn_pred;


        void addToXVector(const Eigen::Vector3d & pos, const Eigen::Quaterniond & ori, std::vector<double> & x);
        void populateXVector(std::vector<double> & x, 
            const double & stance_origin_in, const double & manipulation_type_in,
            const Eigen::Vector3d & swing_foot_start_pos_in, const Eigen::Quaterniond & swing_foot_start_ori_in,  
            const Eigen::Vector3d & pelvis_pos_in, const Eigen::Quaterniond & pelvis_ori_in,  
            const Eigen::Vector3d & landing_foot_pos_in, const Eigen::Quaterniond & landing_foot_ori_in,  
            const Eigen::Vector3d & right_hand_start_pos_in, const Eigen::Quaterniond & right_hand_start_ori_in,  
            const Eigen::Vector3d & left_hand_start_pos_in, const Eigen::Quaterniond & left_hand_start_ori_in);

        // Store the data
        std::string str_stance_origin;
        std::string str_manipulation_type;
        void storeTransitionDatawithTaskSpaceInfo(const shared_ptr<LMVertex> & start_node_traj, bool result);

        void appendPosString(const Eigen::Vector3d & pos, std::string & str_in_out);
        void appendOriString(const Eigen::Quaterniond & ori, std::string & str_in_out);
        std::size_t getDataHash();


        // Store trajectories
        void setSaveFileName(std::string save_filename_in);
        std::string save_filename = "sample_trajectory.yaml";

        void storeTrajectories();

        void clearStoredTrajectories();
        void appendToStoredTrajectories();

        void edgeFeasibilityTimeStart();
        void edgeFeasibilityTimeStop();

        // Get Edge Key
        std::string getEdgeKey(shared_ptr<LMVertex> from_node, shared_ptr<LMVertex> to_node);


    };



}


#endif