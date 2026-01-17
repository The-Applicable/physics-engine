#pragma once
#include "Vector3.h"

class Matrix3 {
 public:
  float data[9];

  Matrix3() { setIdentity(); }

  void setIdentity() {
    data[0] = 1;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 1;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    data[8] = 1;
  }

  Vector3 operator*(const Vector3& v) const {
    return Vector3(data[0] * v.x + data[1] * v.y + data[2] * v.z,
                   data[3] * v.x + data[4] * v.y + data[5] * v.z,
                   data[6] * v.x + data[7] * v.y + data[8] * v.z);
  }

  Matrix3 operator*(const Matrix3& m) const {
    Matrix3 result;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        result.data[i * 3 + j] = data[i * 3 + 0] * m.data[0 * 3 + j] +
                                 data[i * 3 + 1] * m.data[1 * 3 + j] +
                                 data[i * 3 + 2] * m.data[2 * 3 + j];
      }
    }
    return result;
  }

  Matrix3 transpose() const {
    Matrix3 result;
    result.data[0] = data[0];
    result.data[1] = data[3];
    result.data[2] = data[6];
    result.data[3] = data[1];
    result.data[4] = data[4];
    result.data[5] = data[7];
    result.data[6] = data[2];
    result.data[7] = data[5];
    result.data[8] = data[8];
    return result;
  }
};