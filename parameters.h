#ifndef MULTILINEARRECONSTRUCTION_PARAMETERS_H
#define MULTILINEARRECONSTRUCTION_PARAMETERS_H

#include "glm/glm.hpp"
#include "mathutils.hpp"

struct CameraParameters {
  CameraParameters() {}
  CameraParameters(double fovy, double far, int image_width, int image_height)
    : fovy(fovy), far(far) {
    focal_length = 1.0 / tan(0.5 * fovy);
    image_plane_center = glm::vec2(image_width * 0.5, image_height * 0.5);
    image_size = glm::vec2(image_width, image_height);
  }

  static CameraParameters DefaultParameters(int image_width,
                                            int image_height) {
    const double fovy = deg2rad(15.0);
    const double far = 1000.0;
    return CameraParameters(fovy, far, image_width, image_height);
  }

  friend istream& operator>>(istream& is, CameraParameters& params);
  friend ostream& operator<<(ostream& os, const CameraParameters& params);

  double fovy;
  double far;
  double focal_length;
  glm::dvec2 image_plane_center;
  glm::dvec2 image_size;
};

inline istream& operator>>(istream& is, CameraParameters& params) {
  is >> params.fovy >> params.far >> params.focal_length
     >> params.image_plane_center.x >> params.image_plane_center.y
     >> params.image_size.x >> params.image_size.y;
  return is;
}

inline ostream& operator<<(ostream& os, const CameraParameters& params) {
  os << params.fovy << "\n";
  os << params.far << "\n";
  os << params.focal_length << "\n";
  os << params.image_plane_center.x << ' ' << params.image_plane_center.y << "\n";
  os << params.image_size.x << ' ' << params.image_size.y;
  return os;
}

struct ModelParameters {
  static ModelParameters DefaultParameters(const MatrixXd& Uid,
                                           const MatrixXd& Uexp) {
    ModelParameters model_params;

    // Make a neutral face
    model_params.Wexp_FACS.resize(ModelParameters::nFACSDim);
    model_params.Wexp_FACS(0) = 1.0;
    for (int i = 1; i < ModelParameters::nFACSDim; ++i)
      model_params.Wexp_FACS(i) = 0.0;
    model_params.Wexp = model_params.Wexp_FACS.transpose() * Uexp;

    // Use average identity
    model_params.Wid = Uid.colwise().mean();

    model_params.R = Vector3d(1e-3, 1e-3, 1e-3);
    model_params.T = Vector3d(0, 0, -1.0);

    model_params.vindices = VectorXi::Zero(ModelParameters::nVertices);

    return model_params;
  }

  friend istream& operator>>(istream& is, ModelParameters& params);
  friend ostream& operator<<(ostream& os, const ModelParameters& params);

  static const int nFACSDim = 47;
  static const int nVertices = 73;
  VectorXd Wid;               // identity weights
  VectorXd Wexp, Wexp_FACS;   // expression weights
  Vector3d R;                 // rotation
  Vector3d T;                 // translation
  VectorXi vindices;          // vertex indices
};

namespace {
  template <typename T>
  void write_vector(ostream& os, const Matrix<T, Dynamic, 1>& v) {
    os << v.rows() << ' ';
    for(int i=0;i<v.rows();++i) {
      os << v(i) << ' ';
    }
    os << "\n";
  }
  template <typename T>
  void read_vector(istream& is, Matrix<T, Dynamic, 1>& v) {
    int nrows;
    is >> nrows;
    v.resize(nrows, 1);
    for(int i=0;i<nrows;++i) is >> v(i);
  }
}

inline istream& operator>>(istream& is, ModelParameters& params) {
  read_vector(is, params.Wid);
  read_vector(is, params.Wexp);
  read_vector(is, params.Wexp_FACS);
  is >> params.R(0) >> params.R(1) >> params.R(2);
  is >> params.T(0) >> params.T(1) >> params.T(2);
  read_vector(is, params.vindices);
  return is;
}

inline ostream& operator<<(ostream& os, const ModelParameters& params) {
  write_vector(os, params.Wid);
  write_vector(os, params.Wexp);
  write_vector(os, params.Wexp_FACS);
  os << params.R(0) << ' ' << params.R(1) << ' ' << params.R(2) << "\n";
  os << params.T(0) << ' ' << params.T(1) << ' ' << params.T(2) << "\n";
  write_vector(os, params.vindices);
  return os;
}

struct ReconstructionStats {
  double max_error, min_error, avg_error;

  friend istream& operator>>(istream& is, ReconstructionStats& stats);
  friend ostream& operator<<(ostream& os, const ReconstructionStats& stats);
};

inline istream& operator>>(istream& is, ReconstructionStats& stats) {
  is >> stats.avg_error >> stats.min_error >> stats.max_error;
  return is;
}

inline ostream& operator<<(ostream& os, const ReconstructionStats& stats) {
  os << stats.avg_error << ' ' << stats.min_error << ' ' << stats.max_error;
  return os;
}


struct ReconstructionResult {
  CameraParameters params_cam;
  ModelParameters params_model;
  ReconstructionStats stats;
};

template <typename Constraint>
struct ReconstructionParameters {
  int imageWidth, imageHeight;
  vector<Constraint> cons;
};

struct OptimizationParameters {
  OptimizationParameters() : errorThreshold(1e-6), errorDiffThreshold(1e-6),
                             w_prior_id(100.0), w_prior_exp(100.0),
                             d_w_prior_id(10.0), d_w_prior_exp(10.0),
                             max_iters(3), num_initializations(1) {}

  static OptimizationParameters Defaults() {
    return OptimizationParameters();
  }

  double errorThreshold;
  double errorDiffThreshold;

  double w_prior_id, w_prior_exp, d_w_prior_id, d_w_prior_exp;
  int max_iters;
  int num_initializations;
  double perturbation_range;
};



#endif //MULTILINEARRECONSTRUCTION_PARAMETERS_H
