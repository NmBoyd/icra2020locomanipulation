#include <avatar_locomanipulation/collision_environment/collision_environment.h>

int main(int argc, char ** argv){
  std::string filename = THIS_PACKAGE_PATH"models/valkyrie_simplified_collisions.urdf";
  std::string srdf_filename = THIS_PACKAGE_PATH"models/valkyrie_disable_collisions.srdf";
  std::string meshDir  = THIS_PACKAGE_PATH"../val_model/";

	// Initialize Valkyrie RobotModel
	std::shared_ptr<RobotModel> valkyrie(new RobotModel(filename, meshDir, srdf_filename) );

  filename = THIS_PACKAGE_PATH"models/test_cart.urdf";
  meshDir  = THIS_PACKAGE_PATH"models/cart/";

  // Initialize Cart RobotModel
  std::shared_ptr<RobotModel> cart(new RobotModel(filename, meshDir) );
	
	std::cout << "------ Valkyrie Model ------ " << std::endl;
  std::cout << valkyrie->model;	
  // std::cout << "------ Valkyrie GeometryModel ------ " << std::endl;
  // std::cout << valkyrie->geomModel; 
  std::cout << "------ Cart Model ------ " << std::endl;
  std::cout << cart->model;	
  std::cout << "------ Cart GeometryModel ------ " << std::endl;
  std::cout << cart->geomModel;

  std::cout << "valkyrie->getDimQ(): " << valkyrie->getDimQ() << std::endl;

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

  // Define the configuration of the cart
  Eigen::VectorXd cart_config;
  cart_config = Eigen::VectorXd::Zero(cart->getDimQ());
  cart_config[0] = 0.0;  cart_config[1] = 0.0;  cart_config[2] = -0.2;
  double theta1 = 0;//M_PI/4.0;	
  Eigen::AngleAxis<double> bb(theta1, Eigen::Vector3d(0.0, 0.0, 1.0)); // yaw pi/4 to the left	
  Eigen::Quaternion<double> quat_init; quat_init =  bb;
  cart_config[3] = quat_init.x();// 0.0;	
  cart_config[4] = quat_init.y(); //0.0;
  cart_config[5] = quat_init.z(); //sin(theta/2.0);
  cart_config[6] = quat_init.w(); //cos(theta/2.0);

  valkyrie->q_current = q_start;
  cart->q_current = cart_config;

  valkyrie->updateFullKinematics(q_start);
  valkyrie->updateGeometry(q_start);

  std::shared_ptr<CollisionEnvironment> collision(new CollisionEnvironment(valkyrie, cart) );

  collision->directed_vectors.clear();
  // collision->build_directed_vector_to_rhand();
  // 
  std::string frame = "rightPalm";
  collision->build_object_directed_vectors(frame);
  for(int o=0; o<collision->directed_vectors.size(); ++o){
    std::cout << "collision->directed_vectors[o].from: " << collision->directed_vectors[o].from << std::endl;
    std::cout << "collision->directed_vectors[o].to: " << collision->directed_vectors[o].to << std::endl;
    std::cout << "collision->directed_vectors[o].magnitude: " << collision->directed_vectors[o].magnitude << std::endl;
    std::cout << "collision->directed_vectors[o].direction: \n" << collision->directed_vectors[o].direction << std::endl;
  }
  // collision->self_collision_dx();

  // collision->directed_vectors.clear();
  // collision->build_directed_vector_to_lhand();
  // collision->self_collision_dx();
  // for(int o=0; o<collision->directed_vectors.size(); ++o){
  //   std::cout << "collision->directed_vectors[o].from: " << collision->directed_vectors[o].from << std::endl;
  //   std::cout << "collision->directed_vectors[o].to: " << collision->directed_vectors[o].to << std::endl;
  //   std::cout << "collision->directed_vectors[o].magnitude: " << collision->directed_vectors[o].magnitude << std::endl;
  //   std::cout << "collision->directed_vectors[o].direction: \n" << collision->directed_vectors[o].direction << std::endl;
  // }

  // collision->directed_vectors.clear();
  // collision->build_directed_vector_to_head();
  // collision->self_collision_dx();
  // for(int o=0; o<collision->directed_vectors.size(); ++o){
  //   std::cout << "collision->directed_vectors[o].from: " << collision->directed_vectors[o].from << std::endl;
  //   std::cout << "collision->directed_vectors[o].to: " << collision->directed_vectors[o].to << std::endl;
  //   std::cout << "collision->directed_vectors[o].magnitude: " << collision->directed_vectors[o].magnitude << std::endl;
  //   std::cout << "collision->directed_vectors[o].direction: \n" << collision->directed_vectors[o].direction << std::endl;
  // }

  // collision->directed_vectors.clear();
  // collision->build_directed_vector_to_rknee();
  // collision->self_collision_dx();
  // for(int o=0; o<collision->directed_vectors.size(); ++o){
  //   std::cout << "collision->directed_vectors[o].from: " << collision->directed_vectors[o].from << std::endl;
  //   std::cout << "collision->directed_vectors[o].to: " << collision->directed_vectors[o].to << std::endl;
  //   std::cout << "collision->directed_vectors[o].magnitude: " << collision->directed_vectors[o].magnitude << std::endl;
  //   std::cout << "collision->directed_vectors[o].direction: \n" << collision->directed_vectors[o].direction << std::endl;
  // }

  // collision->directed_vectors.clear();
  // collision->build_directed_vector_to_lknee();
  // collision->self_collision_dx();
  // for(int o=0; o<collision->directed_vectors.size(); ++o){
  //   std::cout << "collision->directed_vectors[o].from: " << collision->directed_vectors[o].from << std::endl;
  //   std::cout << "collision->directed_vectors[o].to: " << collision->directed_vectors[o].to << std::endl;
  //   std::cout << "collision->directed_vectors[o].magnitude: " << collision->directed_vectors[o].magnitude << std::endl;
  //   std::cout << "collision->directed_vectors[o].direction: \n" << collision->directed_vectors[o].direction << std::endl;
  // }

  // collision->directed_vectors.clear();
  // collision->build_directed_vector_to_relbow();
  // collision->self_collision_dx();
  // for(int o=0; o<collision->directed_vectors.size(); ++o){
  //   std::cout << "collision->directed_vectors[o].from: " << collision->directed_vectors[o].from << std::endl;
  //   std::cout << "collision->directed_vectors[o].to: " << collision->directed_vectors[o].to << std::endl;
  //   std::cout << "collision->directed_vectors[o].magnitude: " << collision->directed_vectors[o].magnitude << std::endl;
  //   std::cout << "collision->directed_vectors[o].direction: \n" << collision->directed_vectors[o].direction << std::endl;
  // }

  // collision->directed_vectors.clear();
  // collision->build_directed_vector_to_lelbow();
  // collision->self_collision_dx();
  // for(int o=0; o<collision->directed_vectors.size(); ++o){
  //   std::cout << "collision->directed_vectors[o].from: " << collision->directed_vectors[o].from << std::endl;
  //   std::cout << "collision->directed_vectors[o].to: " << collision->directed_vectors[o].to << std::endl;
  //   std::cout << "collision->directed_vectors[o].magnitude: " << collision->directed_vectors[o].magnitude << std::endl;
  //   std::cout << "collision->directed_vectors[o].direction: \n" << collision->directed_vectors[o].direction << std::endl;
  // }

  

}