#include <avatar_locomanipulation/cubic_interpolation_module/cubic_interpolation_six_dim_vec.hpp>


CubicInterpolationSixDimVec::CubicInterpolationSixDimVec(){

}


CubicInterpolationSixDimVec::CubicInterpolationSixDimVec(const std::string & filename_input){
	filename = filename_input;

	// Initializa Param Handler
	ParamHandler param_handler;
	// Load the yaml file
	param_handler.load_yaml_file(filename);
	// Get number of waypoitns
	param_handler.getInteger("num_waypoints", N);

	for(int i=1; i<(N-2); ++i){
		temp = std::shared_ptr<CubicInterpolationSixDim>(new CubicInterpolationSixDim(i, filename) );
		six_dim_vec.push_back(temp);
	}

	std::cout << "[CubicInterpolationSixDimVec] Created" << std::endl;
}


CubicInterpolationSixDimVec::~CubicInterpolationSixDimVec(){

}


void CubicInterpolationSixDimVec::evaluate(const double & s_global){
	// Clamp 0.0 <= s <= 1.0
	s_ = clamp(s_global);

	for(int i=0; i<(N-2); ++i){
		// Compute smax and smin for the current interpolator. 
		// Assume that the path length between every waypoints are roughly equal.			
		smax = static_cast<double>(i+3) / static_cast<double>(N-1);
		smin = static_cast<double>(i) / static_cast<double>(N);
		// Ensure we use global s to select the proper CubicInterpolationSixDim to use
		if( smin <= s_ && s_ < smax){
			// Use the global s to obtain the local s as used by each OneDim
			s_local = ( 1.0 / (smax - smin)) * (s_ - smin);
			// Feed evaluate the local s value
			six_dim_vec[i]->evaluate(s_local);
			temp = six_dim_vec[i];
			break;
		}
	} 
	if (s_ >= 1.0){
		six_dim_vec[N-3]->evaluate(s_);	
		temp = six_dim_vec[N-3];	
	}

	convertToQuat();

}

void CubicInterpolationSixDimVec::getPose(const double & s_global, Eigen::Vector3d & position_out, Eigen::Quaterniond & orientation_out){
	evaluate(s_global);
	position_out = pos_out;
	orientation_out = quat_out;
}


double CubicInterpolationSixDimVec::clamp(const double & s_in){
    if (s_in < 0.0){
        return 0.0;
    }
    else if(s_in > 1.0){
        return 1.0;
    }else{
        return s_in;
    }

}


void CubicInterpolationSixDimVec::convertToQuat(){
	pos_out[0] = temp->output[0];
	pos_out[1] = temp->output[1];
	pos_out[2] = temp->output[2];
	
	aa[0] = temp->output[3];
	aa[1] = temp->output[4];
	aa[2] = temp->output[5];


	angle = atan2(sin(aa.norm()), cos(aa.norm()));
	axis = aa.normalized();

	quat_out = Eigen::AngleAxisd(angle, axis);

}