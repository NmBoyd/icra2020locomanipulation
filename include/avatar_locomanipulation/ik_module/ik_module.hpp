#ifndef ALP_IK_MODULE_H
#define ALP_IK_MODULE_H

#include <avatar_locomanipulation/models/valkyrie_model.hpp>
#include <avatar_locomanipulation/tasks/task.hpp>
#include <avatar_locomanipulation/helpers/pseudo_inverse.hpp>
#include <iostream>

#define IK_OPTIMAL_SOL 1	// SUCCESS ||f(x)|| <= error_tol
#define IK_SUBOPTIMAL_SOL 2 // SUCCESS ||grad(f(x))|| <= grad_tol
#define IK_MAX_ITERATIONS_HIT 3 // FAILURE iter >= MAX_ITERS 
#define IK_MIN_STEP_HIT 4   // FAILURE k_step <= k_step_min


class IKModule{
public:
	IKModule();
	IKModule(std::shared_ptr<ValkyrieModel> robot_model_in);

	~IKModule();

	std::shared_ptr<ValkyrieModel> robot_model;

	void setInitialConfig(const Eigen::VectorXd & q_init);

	bool solveIK(int & solve_result, double & error_norm, Eigen::VectorXd & q_sol, bool inertia_weighted=false);

	// This adds a lower priority task to the hierarchy. 
	void addTasktoHierarchy(std::shared_ptr<Task> & task_input);
	void clearTaskHierarchy();

	void prepareIKDataStrcutures();

	void updateTaskJacobians();
	void computePseudoInverses(bool inertia_weighted=false);
	void computeTaskErrors();
	void compute_dq();

	void printTaskErrorsHeader();
	void printTaskErrors();

	Eigen::VectorXd dq_tot;

private:
	unsigned int svdOptions = Eigen::ComputeThinU | Eigen::ComputeThinV;

	Eigen::VectorXd q_start;
	Eigen::VectorXd q_current;
	Eigen::VectorXd q_step;


	// Task hierarchy list
	// A hierarchy of tasks in order of priority
	std::vector< std::shared_ptr<Task> > task_hierarchy;

	Eigen::MatrixXd I_ ; // Identity Matrix
	Eigen::MatrixXd Ntmp_ ; // Temporary Null Space Matrix

	std::vector<Eigen::MatrixXd> J_; // Task Jacobian
	std::vector<Eigen::MatrixXd> N_; // Task Nullspace
	std::vector<Eigen::MatrixXd> JN_; // Projected Task Jacobian
	std::vector<Eigen::MatrixXd> JNpinv_; // Projected Task Jacobian Pseudo Inverse
	std::vector<Eigen::VectorXd> dx_; // Task Errors
	std::vector<double> dx_norms_; // Task Error Norms


	std::vector< Eigen::JacobiSVD<Eigen::MatrixXd> > svd_list_; // List of SVD for pseudoinverses

	// IK parameters
	double singular_values_threshold = 1e-4;

	double max_iters = 100; // maximum IK iters
	double k_step = 1.0; // starting step
	double beta = 0.8; // Backtracking Line Search Parameter
	double alpha = 0.5; // Backtracking Line Search Parameter

	double error_tol = 1e-4; // Task tolerance for success
	double grad_tol = 1e-6; // Gradient descent tolerance for suboptimality

	double k_min_step = 1e-20; // Minimum step (do we need this?)

	double total_error_norm = 0.0;

	double f_q = 0.0;
	double f_q_m_dq = 0.0;




};

#endif