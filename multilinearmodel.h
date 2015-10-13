#ifndef MULTILINEARMODEL_H
#define MULTILINEARMODEL_H

#include "tensor.hpp"
#include "utils.hpp"

class MultilinearModel
{
public:
  MultilinearModel(){}
  explicit MultilinearModel(const string &filename);

  MultilinearModel project(const vector<int> &indices) const;

  void UpdateTM0(const Tensor1 &w);
  void UpdateTM1(const Tensor1 &w);
  void UpdateTMWithTM0(const Tensor1 &w);
  void UpdateTMWithTM1(const Tensor1 &w);
  void ApplyWeights(const Tensor1 &w0, const Tensor1 &w1);

  const Tensor1& GetTM() const {
    return tm;
  }

  const Tensor2& GetTM0() const { return tm0; }
  const Tensor2& GetTM1() const { return tm1; }
private:
  void UnfoldCoreTensor();

private:
  Tensor3 core;
  Tensor2 tu0, tu1;     // unfolded tensor in 0, 1 dimension

  Tensor2 tm0, tm1;  // tensor after mode product
  Tensor1 tm;        // tensor after 2 mode product
};

struct MultilinearModelPrior {
  VectorXd Wid_avg, Wexp_avg;
  VectorXd Wid0, Wexp0;       // identity and expression prior
  MatrixXd Uid, Uexp;
  MatrixXd sigma_Wid, sigma_Wexp;
  MatrixXd inv_sigma_Wid, inv_sigma_Wexp;
  double weight_Wid, weight_Wexp;

  void load(const string &filename_id, const string &filename_exp) {
    cout << "loading prior data ..." << endl;
    const string fnwid = filename_id;
    ifstream fwid(fnwid, ios::in | ios::binary);

    int ndims;
    fwid.read(reinterpret_cast<char*>(&ndims), sizeof(int));
    cout << "identity prior dim = " << ndims << endl;

    Wid_avg.resize(ndims);
    Wid0.resize(ndims);
    sigma_Wid.resize(ndims, ndims);

    fwid.read(reinterpret_cast<char*>(Wid_avg.data()), sizeof(double)*ndims);
    fwid.read(reinterpret_cast<char*>(Wid0.data()), sizeof(double)*ndims);
    fwid.read(reinterpret_cast<char*>(sigma_Wid.data()), sizeof(double)*ndims*ndims);
    inv_sigma_Wid = sigma_Wid.inverse();

    int m, n;
    fwid.read(reinterpret_cast<char*>(&m), sizeof(int));
    fwid.read(reinterpret_cast<char*>(&n), sizeof(int));
    cout << "Uid size: " << m << 'x' << n << endl;
    Uid.resize(m, n);
    fwid.read(reinterpret_cast<char*>(Uid.data()), sizeof(double)*m*n);

    fwid.close();

    message("identity prior loaded.");
    /*
    cout << "Wid_avg = " << Wid_avg << endl;
    cout << "Wid0 = " << Wid0 << endl;
    cout << "sigma_Wid = " << sigma_Wid << endl;
    cout << "Uid = " << Uid << endl;
    */

    message("processing identity prior.");
    inv_sigma_Wid = sigma_Wid.inverse();
    message("done");

    const string fnwexp = filename_exp;
    ifstream fwexp(fnwexp, ios::in | ios::binary);

    fwexp.read(reinterpret_cast<char*>(&ndims), sizeof(int));
    cout << "expression prior dim = " << ndims << endl;

    Wexp0.resize(ndims);
    Wexp_avg.resize(ndims);
    sigma_Wexp.resize(ndims, ndims);

    fwexp.read(reinterpret_cast<char*>(Wexp_avg.data()), sizeof(double)*ndims);
    fwexp.read(reinterpret_cast<char*>(Wexp0.data()), sizeof(double)*ndims);
    fwexp.read(reinterpret_cast<char*>(sigma_Wexp.data()), sizeof(double)*ndims*ndims);
    inv_sigma_Wexp = sigma_Wexp.inverse();

    fwexp.read(reinterpret_cast<char*>(&m), sizeof(int));
    fwexp.read(reinterpret_cast<char*>(&n), sizeof(int));
    cout << "Uexp size: " << m << 'x' << n << endl;
    Uexp.resize(m, n);
    fwexp.read(reinterpret_cast<char*>(Uexp.data()), sizeof(double)*m*n);

    fwexp.close();

    message("expression prior loaded.");
    /*
    cout << "Wexp_avg = " << Wexp_avg << endl;
    cout << "Wexp0 = " << Wexp0 << endl;
    cout << "sigma_Wexp = " << sigma_Wexp << endl;
    cout << "Uexp = " << Uexp << endl;
    */
    message("processing expression prior.");
    inv_sigma_Wexp = sigma_Wexp.inverse();
    message("done.");
  }
};

#endif // MULTILINEARMODEL_H
