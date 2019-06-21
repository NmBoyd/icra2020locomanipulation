#include <avatar_locomanipulation/helpers/hermite_curve_vec.hpp>


// Constructor
HermiteCurveVec::HermiteCurveVec(){}

HermiteCurveVec::HermiteCurveVec(const Eigen::VectorXd & start_pos, const Eigen::VectorXd & start_vel, 
			   const Eigen::VectorXd & end_pos, const Eigen::VectorXd & end_vel):  p1(start_pos), v1(start_vel), p2(end_pos), v2(end_vel){
	// Create N hermite curves with the specified boundary conditions
	for(int i = 0; i < start_pos.size(); i++){
		curves.push_back(HermiteCurve(start_pos[i], start_vel[i], end_pos[i], end_vel[i]));
	}
	output = Eigen::VectorXd::Zero(start_pos.size());
}

// Destructor
HermiteCurveVec::~HermiteCurveVec(){}

// Evaluation functions
Eigen::VectorXd HermiteCurveVec::evaluate(const double & s_in){
	for(int i = 0; i < p1.size(); i++){
		output[i] = curves[i].evaluate(s_in);
	}
	return output;
}

Eigen::VectorXd HermiteCurveVec::evaluateFirstDerivative(const double & s_in){
	for(int i = 0; i < p1.size(); i++){
		output[i] = curves[i].evaluateFirstDerivative(s_in);
	}
	return output;
}

Eigen::VectorXd HermiteCurveVec::evaluateSecondDerivative(const double & s_in){
	for(int i = 0; i < p1.size(); i++){
		output[i] = curves[i].evaluateSecondDerivative(s_in);
	}
	return output;
}
