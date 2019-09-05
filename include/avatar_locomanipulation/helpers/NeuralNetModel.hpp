#pragma once

#include <Eigen/Dense>
#include <avatar_locomanipulation/helpers/IOUtilities.hpp>
#include <iostream>
#include <vector>

enum ActivationFunction { None = 0, Tanh = 1, ReLU = 2, LeakyReLU = 3, Sigmoid = 4};

class Layer {
   public:
    Layer(Eigen::MatrixXd weight, Eigen::MatrixXd bias,
          ActivationFunction act_fn);
    virtual ~Layer();
    Eigen::MatrixXd GetOutput(const Eigen::MatrixXd& input);
    int GetNumInput() { return num_input_; }
    int GetNumOutput() { return num_output_; }
    Eigen::MatrixXd GetWeight() { return weight_; }
    Eigen::MatrixXd GetBias() { return bias_; }
    ActivationFunction GetActivationFunction() { return act_fn_; }

   private:
    Eigen::MatrixXd weight_;
    Eigen::MatrixXd bias_;
    int num_input_;
    int num_output_;
    ActivationFunction act_fn_;
};

class NeuralNetModel {
   public:
    NeuralNetModel(std::vector<Layer> layers);
    NeuralNetModel(std::vector<Layer> layers, Eigen::VectorXd logstd);
    NeuralNetModel(const myYAML::Node& node, bool b_stochastic);

    virtual ~NeuralNetModel();

    std::vector<Layer> GetLayers() { return layers_; }
    Eigen::MatrixXd GetOutput(const Eigen::MatrixXd& input);
    Eigen::MatrixXd GetOutput(const Eigen::MatrixXd& input, int idx);
    void GetOutput(const Eigen::MatrixXd& _input,
                   const Eigen::VectorXd& _lb,
                   const Eigen::VectorXd& _ub,
                   Eigen::MatrixXd& _output,
                   Eigen::MatrixXd& _mean,
                   Eigen::VectorXd& _neglogp);

    int GetNumInput() { return num_input_; }
    int GetNumOutput() { return num_output_; }
    void PrintInfo();

    Eigen::MatrixXd CropMatrix(Eigen::MatrixXd value, Eigen::MatrixXd min,
                               Eigen::MatrixXd max, std::string source);

   private:
    void Initialize_(std::vector<Layer> layers, bool b_stoch = false,
                     Eigen::VectorXd logstd = Eigen::VectorXd::Zero(1));
    int num_input_;
    int num_output_;
    int num_layer_;
    std::vector<Layer> layers_;
    bool b_stochastic_;
    Eigen::VectorXd logstd_;
    Eigen::VectorXd std_;
};
