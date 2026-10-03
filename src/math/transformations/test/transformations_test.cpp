#include <cmath>
#include <gtest/gtest.h>

#include "math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "math/lin_alg/vector_low_dim/vec2.h"
#include "math/lin_alg/vector_low_dim/vec3.h"
#include "math/transformations/angles.h"
#include "math/transformations/axis_angle.h"
#include "math/transformations/euler_angles.h"
#include "math/transformations/quaternion.h"
#include "math/transformations/se2.h"
#include "math/transformations/se3.h"
#include "math/transformations/so3.h"

namespace lumos {

// Constants for testing
constexpr double EPSILON = 1e-9;
constexpr double PI = 3.14159265358979323846;

template <uint16_t R, uint16_t C>
void expectMatrixNear(const FixedSizeMatrix<double, R, C> &m0,
                      const FixedSizeMatrix<double, R, C> &m1,
                      const double tolerance = EPSILON) {
  for (size_t r = 0; r < R; r++) {
    for (size_t c = 0; c < C; c++) {
      EXPECT_NEAR(m0(r, c), m1(r, c), tolerance) << "at (" << r << ", " << c
                                                 << ")";
    }
  }
}

void expectVecNear(const Vec3<double> &v0, const Vec3<double> &v1,
                   const double tolerance = EPSILON) {
  EXPECT_NEAR(v0.x, v1.x, tolerance);
  EXPECT_NEAR(v0.y, v1.y, tolerance);
  EXPECT_NEAR(v0.z, v1.z, tolerance);
}

void expectVecNear(const Vec2<double> &v0, const Vec2<double> &v1,
                   const double tolerance = EPSILON) {
  EXPECT_NEAR(v0.x, v1.x, tolerance);
  EXPECT_NEAR(v0.y, v1.y, tolerance);
}

// q and -q represent the same rotation
void expectSameRotation(const Quaternion<double> &q0,
                        const Quaternion<double> &q1,
                        const double tolerance = EPSILON) {
  EXPECT_NEAR(std::abs(q0.dot(q1)), 1.0, tolerance);
}

// Rotation vectors covering small, regular and close to pi angles
std::vector<Vec3<double>> testRotationVectors() {
  const Vec3<double> axis = Vec3<double>(1.0, -2.0, 0.5).normalized();
  std::vector<Vec3<double>> rotation_vectors = {
      Vec3<double>(0.0, 0.0, 0.0),  Vec3<double>(0.3, 0.0, 0.0),
      Vec3<double>(0.0, -1.2, 0.0), Vec3<double>(0.0, 0.0, 2.5),
      Vec3<double>(0.4, -0.7, 1.1), Vec3<double>(PI / 2.0, 0.0, 0.0)};
  for (const double angle : {1e-12, 1e-8, 1e-5, 1e-3, 0.1, 0.19999, 0.20001,
                             1.0, 3.0, PI - 1e-3, PI - 1e-6, PI - 1e-9}) {
    rotation_vectors.push_back(angle * axis);
  }
  return rotation_vectors;
}

// ----------------------------- Angles -----------------------------

TEST(AnglesTest, DegRadConversion) {
  EXPECT_NEAR(degToRad(180.0), PI, EPSILON);
  EXPECT_NEAR(degToRad(-90.0), -PI / 2.0, EPSILON);
  EXPECT_NEAR(radToDeg(PI / 4.0), 45.0, EPSILON);
  EXPECT_NEAR(radToDeg(degToRad(123.4)), 123.4, EPSILON);
  EXPECT_NEAR(degToRad(180.0f), static_cast<float>(PI), 1e-6f);
}

TEST(AnglesTest, WrapTo2Pi) {
  EXPECT_NEAR(wrapTo2Pi(0.0), 0.0, EPSILON);
  EXPECT_NEAR(wrapTo2Pi(1.0), 1.0, EPSILON);
  EXPECT_NEAR(wrapTo2Pi(-1.0), 2.0 * PI - 1.0, EPSILON);
  EXPECT_NEAR(wrapTo2Pi(2.0 * PI + 0.5), 0.5, EPSILON);
  EXPECT_NEAR(wrapTo2Pi(-6.0 * PI + 0.5), 0.5, EPSILON);
  EXPECT_NEAR(wrapTo2Pi(2.0 * PI), 0.0, EPSILON);

  for (double angle = -50.0; angle < 50.0; angle += 0.37) {
    const double wrapped = wrapTo2Pi(angle);
    EXPECT_GE(wrapped, 0.0);
    EXPECT_LT(wrapped, 2.0 * PI);
    EXPECT_NEAR(std::sin(wrapped), std::sin(angle), EPSILON);
    EXPECT_NEAR(std::cos(wrapped), std::cos(angle), EPSILON);
  }

  // A tiny negative angle must not round up to exactly 2*pi
  EXPECT_LT(wrapTo2Pi(-1e-20), 2.0 * PI);
  EXPECT_LT(wrapTo2Pi(-1e-10f), 2.0f * pi<float>());
}

TEST(AnglesTest, WrapToPi) {
  EXPECT_NEAR(wrapToPi(0.0), 0.0, EPSILON);
  EXPECT_NEAR(wrapToPi(1.0), 1.0, EPSILON);
  EXPECT_NEAR(wrapToPi(-1.0), -1.0, EPSILON);
  EXPECT_NEAR(wrapToPi(PI + 0.5), -PI + 0.5, EPSILON);
  EXPECT_NEAR(wrapToPi(-PI - 0.5), PI - 0.5, EPSILON);
  EXPECT_NEAR(wrapToPi(4.0 * PI + 0.25), 0.25, EPSILON);
  // The interval is [-pi, pi)
  EXPECT_NEAR(wrapToPi(PI), -PI, EPSILON);
  EXPECT_NEAR(wrapToPi(-PI), -PI, EPSILON);

  for (double angle = -50.0; angle < 50.0; angle += 0.37) {
    const double wrapped = wrapToPi(angle);
    EXPECT_GE(wrapped, -PI);
    EXPECT_LT(wrapped, PI);
    EXPECT_NEAR(std::sin(wrapped), std::sin(angle), EPSILON);
    EXPECT_NEAR(std::cos(wrapped), std::cos(angle), EPSILON);
  }
}

TEST(AnglesTest, ShortestAngularDistance) {
  EXPECT_NEAR(shortestAngularDistance(0.0, 1.0), 1.0, EPSILON);
  EXPECT_NEAR(shortestAngularDistance(1.0, 0.0), -1.0, EPSILON);
  // Crossing the +-pi boundary
  EXPECT_NEAR(shortestAngularDistance(PI - 0.1, -PI + 0.1), 0.2, EPSILON);
  EXPECT_NEAR(shortestAngularDistance(-PI + 0.1, PI - 0.1), -0.2, EPSILON);
  // Unwrapped inputs
  EXPECT_NEAR(shortestAngularDistance(0.1, 6.0 * PI + 0.3), 0.2, EPSILON);
}

TEST(AnglesTest, InterpolateAngle) {
  EXPECT_NEAR(interpolateAngle(0.0, 1.0, 0.0), 0.0, EPSILON);
  EXPECT_NEAR(interpolateAngle(0.0, 1.0, 1.0), 1.0, EPSILON);
  EXPECT_NEAR(interpolateAngle(0.0, 1.0, 0.25), 0.25, EPSILON);
  // Goes the short way across the +-pi boundary
  EXPECT_NEAR(interpolateAngle(PI - 0.1, -PI + 0.3, 0.5), -PI + 0.1, EPSILON);
  EXPECT_NEAR(interpolateAngle(PI - 0.3, -PI + 0.1, 0.5), PI - 0.1, EPSILON);
}

// --------------------------- Quaternion ---------------------------

TEST(QuaternionTest, DefaultIsIdentity) {
  const Quaternion<double> q;
  EXPECT_EQ(q.w, 1.0);
  EXPECT_EQ(q.x, 0.0);
  EXPECT_EQ(q.y, 0.0);
  EXPECT_EQ(q.z, 0.0);
  expectMatrixNear(q.toRotationMatrix(), unitFixedSizeMatrix<double, 3, 3>());
}

TEST(QuaternionTest, NormAndNormalize) {
  Quaternion<double> q(1.0, 2.0, 3.0, 4.0);
  EXPECT_NEAR(q.squaredNorm(), 30.0, EPSILON);
  EXPECT_NEAR(q.norm(), std::sqrt(30.0), EPSILON);

  const Quaternion<double> qn = q.normalized();
  EXPECT_NEAR(qn.norm(), 1.0, EPSILON);
  // normalized() leaves the original untouched
  EXPECT_NEAR(q.norm(), std::sqrt(30.0), EPSILON);

  q.normalize();
  EXPECT_NEAR(q.norm(), 1.0, EPSILON);
  EXPECT_NEAR(q.w, qn.w, EPSILON);
  EXPECT_NEAR(q.z, qn.z, EPSILON);
}

TEST(QuaternionTest, Inverse) {
  // Non unit quaternion: inverse differs from the conjugate
  const Quaternion<double> q(1.0, 2.0, 3.0, 4.0);
  const Quaternion<double> p = q * q.inverse();
  EXPECT_NEAR(p.w, 1.0, EPSILON);
  EXPECT_NEAR(p.x, 0.0, EPSILON);
  EXPECT_NEAR(p.y, 0.0, EPSILON);
  EXPECT_NEAR(p.z, 0.0, EPSILON);

  const Quaternion<double> qn = q.normalized();
  const Quaternion<double> qn_inv = qn.inverse();
  const Quaternion<double> qn_conj = qn.conjugate();
  EXPECT_NEAR(qn_inv.w, qn_conj.w, EPSILON);
  EXPECT_NEAR(qn_inv.x, qn_conj.x, EPSILON);
  EXPECT_NEAR(qn_inv.y, qn_conj.y, EPSILON);
  EXPECT_NEAR(qn_inv.z, qn_conj.z, EPSILON);
}

TEST(QuaternionTest, FromAxisAngleMatchesElementaryRotations) {
  const double angle = 0.7;
  expectMatrixNear(
      Quaternion<double>::fromAxisAngle(Vec3<double>(1.0, 0.0, 0.0), angle)
          .toRotationMatrix(),
      fixedRotationMatrixX(angle));
  expectMatrixNear(
      Quaternion<double>::fromAxisAngle(Vec3<double>(0.0, 1.0, 0.0), angle)
          .toRotationMatrix(),
      fixedRotationMatrixY(angle));
  expectMatrixNear(
      Quaternion<double>::fromAxisAngle(Vec3<double>(0.0, 0.0, 1.0), angle)
          .toRotationMatrix(),
      fixedRotationMatrixZ(angle));

  // Axis is normalized internally
  const Quaternion<double> q =
      Quaternion<double>::fromAxisAngle(Vec3<double>(0.0, 0.0, 5.0), angle);
  EXPECT_NEAR(q.norm(), 1.0, EPSILON);
  expectMatrixNear(q.toRotationMatrix(), fixedRotationMatrixZ(angle));

  // Zero axis gives identity
  const Quaternion<double> q_zero =
      Quaternion<double>::fromAxisAngle(Vec3<double>(0.0, 0.0, 0.0), angle);
  EXPECT_EQ(q_zero.w, 1.0);
  EXPECT_EQ(q_zero.x, 0.0);
}

TEST(QuaternionTest, RotateVector) {
  const Quaternion<double> q = Quaternion<double>::fromAxisAngle(
      Vec3<double>(0.0, 0.0, 1.0), PI / 2.0);
  expectVecNear(q.rotate(Vec3<double>(1.0, 0.0, 0.0)),
                Vec3<double>(0.0, 1.0, 0.0));
  expectVecNear(q.rotate(Vec3<double>(0.0, 1.0, 0.0)),
                Vec3<double>(-1.0, 0.0, 0.0));
  expectVecNear(q.rotate(Vec3<double>(0.0, 0.0, 1.0)),
                Vec3<double>(0.0, 0.0, 1.0));

  // Agrees with the rotation matrix
  const Vec3<double> v(0.3, -1.7, 2.2);
  for (const Vec3<double> &phi : testRotationVectors()) {
    const Quaternion<double> qi = Quaternion<double>::exp(phi);
    expectVecNear(qi.rotate(v), qi.toRotationMatrix() * v);
  }
}

TEST(QuaternionTest, ProductMatchesRotationMatrixProduct) {
  const Quaternion<double> q0 =
      Quaternion<double>::exp(Vec3<double>(0.4, -0.7, 1.1));
  const Quaternion<double> q1 =
      Quaternion<double>::exp(Vec3<double>(-1.3, 0.2, 0.6));
  expectMatrixNear((q0 * q1).toRotationMatrix(),
                   q0.toRotationMatrix() * q1.toRotationMatrix());
}

TEST(QuaternionTest, RotationMatrixRoundTrip) {
  for (const Vec3<double> &phi : testRotationVectors()) {
    const Quaternion<double> q = Quaternion<double>::exp(phi);
    const FixedSizeMatrix<double, 3, 3> r = q.toRotationMatrix();
    const Quaternion<double> q_back = Quaternion<double>::fromRotationMatrix(r);
    EXPECT_NEAR(q_back.norm(), 1.0, EPSILON);
    expectSameRotation(q, q_back);
    expectMatrixNear(q_back.toRotationMatrix(), r);
  }
}

TEST(QuaternionTest, FromRotationMatrixAllBranches) {
  // 180 degree rotations about each axis have trace -1 and exercise the
  // three branches that do not pivot on w
  const std::vector<Vec3<double>> axes = {Vec3<double>(1.0, 0.0, 0.0),
                                          Vec3<double>(0.0, 1.0, 0.0),
                                          Vec3<double>(0.0, 0.0, 1.0)};
  for (const Vec3<double> &axis : axes) {
    const Quaternion<double> q = Quaternion<double>::fromAxisAngle(axis, PI);
    const Quaternion<double> q_back =
        Quaternion<double>::fromRotationMatrix(q.toRotationMatrix());
    expectSameRotation(q, q_back);
  }
}

TEST(QuaternionTest, ExpLogRoundTrip) {
  for (const Vec3<double> &phi : testRotationVectors()) {
    const Quaternion<double> q = Quaternion<double>::exp(phi);
    EXPECT_NEAR(q.norm(), 1.0, EPSILON);
    expectVecNear(q.log(), phi);
  }
}

TEST(QuaternionTest, ExpMatchesAxisAngle) {
  const Vec3<double> axis = Vec3<double>(1.0, -2.0, 0.5).normalized();
  const double angle = 1.3;
  const Quaternion<double> q0 = Quaternion<double>::exp(angle * axis);
  const Quaternion<double> q1 = Quaternion<double>::fromAxisAngle(axis, angle);
  EXPECT_NEAR(q0.w, q1.w, EPSILON);
  EXPECT_NEAR(q0.x, q1.x, EPSILON);
  EXPECT_NEAR(q0.y, q1.y, EPSILON);
  EXPECT_NEAR(q0.z, q1.z, EPSILON);
}

TEST(QuaternionTest, LogOfNegatedQuaternionIsSameRotation) {
  const Vec3<double> phi(0.4, -0.7, 1.1);
  const Quaternion<double> q = Quaternion<double>::exp(phi);
  expectVecNear((q * -1.0).log(), phi);
}

TEST(QuaternionTest, Slerp) {
  const Vec3<double> axis(0.0, 0.0, 1.0);
  const Quaternion<double> q0 = Quaternion<double>::fromAxisAngle(axis, 0.2);
  const Quaternion<double> q1 = Quaternion<double>::fromAxisAngle(axis, 1.4);

  expectSameRotation(slerp(q0, q1, 0.0), q0);
  expectSameRotation(slerp(q0, q1, 1.0), q1);
  // Constant angular velocity about the common axis
  for (const double t : {0.1, 0.25, 0.5, 0.9}) {
    const Quaternion<double> q = slerp(q0, q1, t);
    EXPECT_NEAR(q.norm(), 1.0, EPSILON);
    expectVecNear(q.log(), (0.2 + t * 1.2) * axis);
  }
}

TEST(QuaternionTest, SlerpTakesShortestPath) {
  const Vec3<double> axis(0.0, 0.0, 1.0);
  const Quaternion<double> q0 = Quaternion<double>::fromAxisAngle(axis, 0.2);
  // Same rotation as an angle of 1.4, but on the other hemisphere
  const Quaternion<double> q1 =
      Quaternion<double>::fromAxisAngle(axis, 1.4) * -1.0;
  expectVecNear(slerp(q0, q1, 0.5).log(), 0.8 * axis);
}

TEST(QuaternionTest, SlerpNearlyParallel) {
  const Vec3<double> axis(0.0, 1.0, 0.0);
  const Quaternion<double> q0 = Quaternion<double>::fromAxisAngle(axis, 0.5);
  const Quaternion<double> q1 =
      Quaternion<double>::fromAxisAngle(axis, 0.5 + 1e-4);
  const Quaternion<double> q = slerp(q0, q1, 0.5);
  EXPECT_NEAR(q.norm(), 1.0, EPSILON);
  expectVecNear(q.log(), (0.5 + 0.5e-4) * axis);

  // Identical inputs
  expectSameRotation(slerp(q0, q0, 0.3), q0);
}

TEST(QuaternionTest, FloatInstantiation) {
  const Quaternion<float> q =
      Quaternion<float>::exp(Vec3<float>(0.4f, -0.7f, 1.1f));
  const Vec3<float> phi = q.log();
  EXPECT_NEAR(phi.x, 0.4f, 1e-5f);
  EXPECT_NEAR(phi.y, -0.7f, 1e-5f);
  EXPECT_NEAR(phi.z, 1.1f, 1e-5f);
}

// ------------------------------ SO(3) ------------------------------

TEST(SO3Test, HatVee) {
  const Vec3<double> a(0.3, -1.7, 2.2);
  const Vec3<double> b(-0.9, 0.4, 1.5);

  expectVecNear(hat(a) * b, a.crossProduct(b));
  expectVecNear(vee(hat(a)), a);

  // Skew symmetric
  const FixedSizeMatrix<double, 3, 3> k = hat(a);
  const FixedSizeMatrix<double, 3, 3> kt = k.transposed();
  for (size_t r = 0; r < 3; r++) {
    for (size_t c = 0; c < 3; c++) {
      EXPECT_NEAR(k(r, c), -kt(r, c), EPSILON);
    }
  }
}

TEST(SO3Test, ExpMatchesElementaryRotations) {
  const double angle = -0.9;
  expectMatrixNear(expSO3(Vec3<double>(angle, 0.0, 0.0)),
                   fixedRotationMatrixX(angle));
  expectMatrixNear(expSO3(Vec3<double>(0.0, angle, 0.0)),
                   fixedRotationMatrixY(angle));
  expectMatrixNear(expSO3(Vec3<double>(0.0, 0.0, angle)),
                   fixedRotationMatrixZ(angle));
  expectMatrixNear(expSO3(Vec3<double>(0.0, 0.0, 0.0)),
                   unitFixedSizeMatrix<double, 3, 3>());
}

TEST(SO3Test, ExpIsRotationMatrix) {
  for (const Vec3<double> &phi : testRotationVectors()) {
    const FixedSizeMatrix<double, 3, 3> r = expSO3(phi);
    expectMatrixNear(r * r.transposed(), unitFixedSizeMatrix<double, 3, 3>());
    // Matches the quaternion exponential
    expectMatrixNear(r, Quaternion<double>::exp(phi).toRotationMatrix());
    // The rotation axis is left unchanged
    expectVecNear(r * phi, phi);
  }
}

TEST(SO3Test, ExpLogRoundTrip) {
  for (const Vec3<double> &phi : testRotationVectors()) {
    // Close to pi the log is ill conditioned in the matrix elements
    const double tolerance = (phi.norm() > 3.1) ? 1e-6 : EPSILON;
    expectVecNear(logSO3(expSO3(phi)), phi, tolerance);
  }
}

TEST(SO3Test, LogOfHalfTurn) {
  const std::vector<Vec3<double>> axes = {
      Vec3<double>(1.0, 0.0, 0.0), Vec3<double>(0.0, 1.0, 0.0),
      Vec3<double>(0.0, 0.0, 1.0), Vec3<double>(1.0, 2.0, 3.0).normalized()};
  for (const Vec3<double> &axis : axes) {
    const FixedSizeMatrix<double, 3, 3> r = expSO3(PI * axis);
    const Vec3<double> phi = logSO3(r);
    EXPECT_NEAR(phi.norm(), PI, 1e-7);
    // The axis sign is ambiguous at exactly pi, compare the rotations
    expectMatrixNear(expSO3(phi), r, 1e-7);
  }
}

TEST(SO3Test, LeftJacobianInverse) {
  for (const Vec3<double> &phi : testRotationVectors()) {
    expectMatrixNear(leftJacobianSO3(phi) * leftJacobianInverseSO3(phi),
                     unitFixedSizeMatrix<double, 3, 3>());
  }
  expectMatrixNear(leftJacobianSO3(Vec3<double>(0.0, 0.0, 0.0)),
                   unitFixedSizeMatrix<double, 3, 3>());
}

TEST(SO3Test, LeftJacobianIsDerivativeOfExp) {
  // exp(phi + d) ~= exp(J_l(phi) * d) * exp(phi) for small d
  const Vec3<double> d(1e-6, -2e-6, 1.5e-6);
  for (const Vec3<double> &phi : testRotationVectors()) {
    if (phi.norm() > 3.1) {
      continue;
    }
    const FixedSizeMatrix<double, 3, 3> lhs = expSO3(phi + d);
    const FixedSizeMatrix<double, 3, 3> rhs =
        expSO3(leftJacobianSO3(phi) * d) * expSO3(phi);
    expectMatrixNear(lhs, rhs, 1e-10);
  }
}

TEST(SO3Test, CoefficientsAreContinuousAcrossSeriesThreshold) {
  // The series and the closed forms must agree where they are switched
  const double below = internal::so3SmallAngle<double>() - 1e-9;
  const double above = internal::so3SmallAngle<double>() + 1e-9;
  EXPECT_NEAR(internal::sinc(below), internal::sinc(above), 1e-9);
  EXPECT_NEAR(internal::xMinusSinOverCube(below),
              internal::xMinusSinOverCube(above), 1e-9);
  EXPECT_NEAR(internal::leftJacobianInverseCoefficient(below),
              internal::leftJacobianInverseCoefficient(above), 1e-9);

  // Limits at zero
  EXPECT_NEAR(internal::sinc(0.0), 1.0, EPSILON);
  EXPECT_NEAR(internal::oneMinusCosOverSquare(0.0), 0.5, EPSILON);
  EXPECT_NEAR(internal::xMinusSinOverCube(0.0), 1.0 / 6.0, EPSILON);
  EXPECT_NEAR(internal::leftJacobianInverseCoefficient(0.0), 1.0 / 12.0,
              EPSILON);
}

// ------------------------------ SE(3) ------------------------------

class SE3Test : public ::testing::Test {
protected:
  void SetUp() override {
    pose_a = SE3<double>(expSO3(Vec3<double>(0.4, -0.7, 1.1)),
                         Vec3<double>(1.0, 2.0, 3.0));
    pose_b = SE3<double>(expSO3(Vec3<double>(-1.3, 0.2, 0.6)),
                         Vec3<double>(-0.5, 0.25, 4.0));
    point = Vec3<double>(0.3, -1.7, 2.2);
  }

  SE3<double> pose_a, pose_b;
  Vec3<double> point;
};

void expectPoseNear(const SE3<double> &p0, const SE3<double> &p1,
                    const double tolerance = EPSILON) {
  expectMatrixNear(p0.rotation, p1.rotation, tolerance);
  expectVecNear(p0.translation, p1.translation, tolerance);
}

TEST_F(SE3Test, DefaultIsIdentity) {
  const SE3<double> identity;
  expectMatrixNear(identity.rotation, unitFixedSizeMatrix<double, 3, 3>());
  expectVecNear(identity.translation, Vec3<double>(0.0, 0.0, 0.0));
  expectVecNear(identity * point, point);
  expectPoseNear(identity * pose_a, pose_a);
  expectPoseNear(pose_a * identity, pose_a);
}

TEST_F(SE3Test, TransformPoint) {
  // 90 degrees about z, then translate
  const SE3<double> pose(fixedRotationMatrixZ(PI / 2.0),
                         Vec3<double>(1.0, 2.0, 3.0));
  expectVecNear(pose * Vec3<double>(1.0, 0.0, 0.0),
                Vec3<double>(1.0, 3.0, 3.0));
  expectVecNear(pose.transformPoint(Vec3<double>(1.0, 0.0, 0.0)),
                Vec3<double>(1.0, 3.0, 3.0));
  // Directions are not translated
  expectVecNear(pose.rotateVector(Vec3<double>(1.0, 0.0, 0.0)),
                Vec3<double>(0.0, 1.0, 0.0));
}

TEST_F(SE3Test, Inverse) {
  expectPoseNear(pose_a * pose_a.inverse(), SE3<double>());
  expectPoseNear(pose_a.inverse() * pose_a, SE3<double>());
  expectVecNear(pose_a.inverse() * (pose_a * point), point);
}

TEST_F(SE3Test, Composition) {
  expectVecNear((pose_a * pose_b) * point, pose_a * (pose_b * point));
  // Associative
  const SE3<double> pose_c(expSO3(Vec3<double>(0.1, 0.2, -0.3)),
                           Vec3<double>(7.0, -8.0, 9.0));
  expectPoseNear((pose_a * pose_b) * pose_c, pose_a * (pose_b * pose_c));
  // (a * b)^-1 == b^-1 * a^-1
  expectPoseNear((pose_a * pose_b).inverse(),
                 pose_b.inverse() * pose_a.inverse());
}

TEST_F(SE3Test, HomogeneousMatrix) {
  const FixedSizeMatrix<double, 4, 4> m = pose_a.toHomogeneousMatrix();
  for (size_t r = 0; r < 3; r++) {
    for (size_t c = 0; c < 3; c++) {
      EXPECT_EQ(m(r, c), pose_a.rotation(r, c));
    }
  }
  EXPECT_EQ(m(0, 3), 1.0);
  EXPECT_EQ(m(1, 3), 2.0);
  EXPECT_EQ(m(2, 3), 3.0);
  EXPECT_EQ(m(3, 0), 0.0);
  EXPECT_EQ(m(3, 1), 0.0);
  EXPECT_EQ(m(3, 2), 0.0);
  EXPECT_EQ(m(3, 3), 1.0);

  // Round trip
  expectPoseNear(SE3<double>(m), pose_a);

  // Composition is the matrix product
  expectMatrixNear((pose_a * pose_b).toHomogeneousMatrix(),
                   m * pose_b.toHomogeneousMatrix());
}

TEST_F(SE3Test, QuaternionConstruction) {
  const Quaternion<double> q =
      Quaternion<double>::exp(Vec3<double>(0.4, -0.7, 1.1));
  const SE3<double> pose(q, Vec3<double>(1.0, 2.0, 3.0));
  expectPoseNear(pose, pose_a);
  expectSameRotation(pose.toQuaternion(), q);
}

TEST_F(SE3Test, ExpOfPureTranslationAndPureRotation) {
  const SE3<double> translation_only = SE3<double>::exp(
      Twist3<double>(Vec3<double>(1.0, 2.0, 3.0), Vec3<double>(0.0, 0.0, 0.0)));
  expectPoseNear(translation_only,
                 SE3<double>(unitFixedSizeMatrix<double, 3, 3>(),
                             Vec3<double>(1.0, 2.0, 3.0)));

  const Vec3<double> phi(0.4, -0.7, 1.1);
  const SE3<double> rotation_only =
      SE3<double>::exp(Twist3<double>(Vec3<double>(0.0, 0.0, 0.0), phi));
  expectPoseNear(rotation_only,
                 SE3<double>(expSO3(phi), Vec3<double>(0.0, 0.0, 0.0)));
}

TEST_F(SE3Test, ExpOfScrewMotion) {
  // Rotating about z while moving along x traces a circular arc. A quarter
  // turn with unit forward speed per radian ends at (1, 1, 0), plus the
  // travel along the axis
  const SE3<double> pose = SE3<double>::exp(Twist3<double>(
      Vec3<double>(PI / 2.0, 0.0, 0.5), Vec3<double>(0.0, 0.0, PI / 2.0)));
  expectMatrixNear(pose.rotation, fixedRotationMatrixZ(PI / 2.0));
  expectVecNear(pose.translation, Vec3<double>(1.0, 1.0, 0.5));
}

TEST_F(SE3Test, ExpLogRoundTrip) {
  const Vec3<double> linear(1.0, -2.0, 0.5);
  for (const Vec3<double> &phi : testRotationVectors()) {
    const double tolerance = (phi.norm() > 3.1) ? 1e-5 : EPSILON;
    const Twist3<double> twist = SE3<double>::exp(Twist3<double>(linear, phi)).log();
    expectVecNear(twist.linear, linear, tolerance);
    expectVecNear(twist.angular, phi, tolerance);
  }

  expectPoseNear(SE3<double>::exp(pose_a.log()), pose_a);
  expectPoseNear(SE3<double>::exp(pose_b.log()), pose_b);
}

TEST_F(SE3Test, LogOfIdentityIsZero) {
  const Twist3<double> twist = SE3<double>().log();
  expectVecNear(twist.linear, Vec3<double>(0.0, 0.0, 0.0));
  expectVecNear(twist.angular, Vec3<double>(0.0, 0.0, 0.0));

  const Twist3<double> default_twist;
  expectPoseNear(SE3<double>::exp(default_twist), SE3<double>());
}

TEST_F(SE3Test, ExpOfScaledTwistComposes) {
  // exp(s * xi) * exp(t * xi) == exp((s + t) * xi)
  const Twist3<double> twist = pose_a.log();
  const SE3<double> p0 = SE3<double>::exp(
      Twist3<double>(0.3 * twist.linear, 0.3 * twist.angular));
  const SE3<double> p1 = SE3<double>::exp(
      Twist3<double>(0.7 * twist.linear, 0.7 * twist.angular));
  expectPoseNear(p0 * p1, pose_a);
}

TEST_F(SE3Test, Interpolate) {
  expectPoseNear(interpolate(pose_a, pose_b, 0.0), pose_a);
  expectPoseNear(interpolate(pose_a, pose_b, 1.0), pose_b);

  // The midpoint is the same from both ends
  expectPoseNear(interpolate(pose_a, pose_b, 0.5),
                 interpolate(pose_b, pose_a, 0.5));

  // Two equal steps of the half way relative motion reach the end
  const SE3<double> mid = interpolate(pose_a, pose_b, 0.5);
  const SE3<double> step = pose_a.inverse() * mid;
  expectPoseNear(pose_a * step * step, pose_b);

  // Pure translation interpolates linearly
  const SE3<double> t0(unitFixedSizeMatrix<double, 3, 3>(),
                       Vec3<double>(0.0, 0.0, 0.0));
  const SE3<double> t1(unitFixedSizeMatrix<double, 3, 3>(),
                       Vec3<double>(4.0, -2.0, 8.0));
  expectVecNear(interpolate(t0, t1, 0.25).translation,
                Vec3<double>(1.0, -0.5, 2.0));
}

TEST_F(SE3Test, FloatInstantiation) {
  const SE3<float> pose(expSO3(Vec3<float>(0.4f, -0.7f, 1.1f)),
                        Vec3<float>(1.0f, 2.0f, 3.0f));
  const SE3<float> identity = pose * pose.inverse();
  EXPECT_NEAR(identity.translation.x, 0.0f, 1e-5f);
  EXPECT_NEAR(identity.rotation(0, 0), 1.0f, 1e-5f);

  const SE3<float> round_trip = SE3<float>::exp(pose.log());
  EXPECT_NEAR(round_trip.translation.x, 1.0f, 1e-5f);
  EXPECT_NEAR(round_trip.translation.z, 3.0f, 1e-5f);
  EXPECT_NEAR(round_trip.rotation(0, 1), pose.rotation(0, 1), 1e-5f);
}

// ------------------------------ SE(2) ------------------------------

class SE2Test : public ::testing::Test {
protected:
  void SetUp() override {
    pose_a = SE2<double>(0.7, Vec2<double>(1.0, 2.0));
    pose_b = SE2<double>(-2.4, Vec2<double>(-0.5, 4.0));
    point = Vec2<double>(0.3, -1.7);
  }

  SE2<double> pose_a, pose_b;
  Vec2<double> point;
};

void expectPoseNear(const SE2<double> &p0, const SE2<double> &p1,
                    const double tolerance = EPSILON) {
  EXPECT_NEAR(shortestAngularDistance(p0.theta, p1.theta), 0.0, tolerance);
  expectVecNear(p0.translation, p1.translation, tolerance);
}

TEST_F(SE2Test, DefaultIsIdentity) {
  const SE2<double> identity;
  EXPECT_EQ(identity.theta, 0.0);
  expectVecNear(identity.translation, Vec2<double>(0.0, 0.0));
  expectVecNear(identity * point, point);
  expectPoseNear(identity * pose_a, pose_a);
  expectPoseNear(pose_a * identity, pose_a);
}

TEST_F(SE2Test, ThetaIsWrapped) {
  EXPECT_NEAR(SE2<double>(2.0 * PI + 0.3, 0.0, 0.0).theta, 0.3, EPSILON);
  EXPECT_NEAR(SE2<double>(PI + 0.3, Vec2<double>(0.0, 0.0)).theta, -PI + 0.3,
              EPSILON);
  // Composition wraps as well
  const SE2<double> pose = SE2<double>(3.0, 0.0, 0.0) * SE2<double>(3.0, 0.0, 0.0);
  EXPECT_NEAR(pose.theta, 6.0 - 2.0 * PI, EPSILON);
}

TEST_F(SE2Test, TransformPoint) {
  const SE2<double> pose(PI / 2.0, 1.0, 2.0);
  expectVecNear(pose * Vec2<double>(1.0, 0.0), Vec2<double>(1.0, 3.0));
  expectVecNear(pose.transformPoint(Vec2<double>(1.0, 0.0)),
                Vec2<double>(1.0, 3.0));
  expectVecNear(pose.rotateVector(Vec2<double>(1.0, 0.0)),
                Vec2<double>(0.0, 1.0));
}

TEST_F(SE2Test, Inverse) {
  expectPoseNear(pose_a * pose_a.inverse(), SE2<double>());
  expectPoseNear(pose_a.inverse() * pose_a, SE2<double>());
  expectVecNear(pose_a.inverse() * (pose_a * point), point);
}

TEST_F(SE2Test, Composition) {
  expectVecNear((pose_a * pose_b) * point, pose_a * (pose_b * point));
  const SE2<double> pose_c(0.2, 7.0, -8.0);
  expectPoseNear((pose_a * pose_b) * pose_c, pose_a * (pose_b * pose_c));
  expectPoseNear((pose_a * pose_b).inverse(),
                 pose_b.inverse() * pose_a.inverse());
}

TEST_F(SE2Test, HomogeneousMatrix) {
  const FixedSizeMatrix<double, 3, 3> m = pose_a.toHomogeneousMatrix();
  EXPECT_NEAR(m(0, 0), std::cos(0.7), EPSILON);
  EXPECT_NEAR(m(0, 1), -std::sin(0.7), EPSILON);
  EXPECT_NEAR(m(1, 0), std::sin(0.7), EPSILON);
  EXPECT_NEAR(m(1, 1), std::cos(0.7), EPSILON);
  EXPECT_EQ(m(0, 2), 1.0);
  EXPECT_EQ(m(1, 2), 2.0);
  EXPECT_EQ(m(2, 0), 0.0);
  EXPECT_EQ(m(2, 1), 0.0);
  EXPECT_EQ(m(2, 2), 1.0);

  expectPoseNear(SE2<double>(m), pose_a);
  expectMatrixNear((pose_a * pose_b).toHomogeneousMatrix(),
                   m * pose_b.toHomogeneousMatrix());
  expectMatrixNear(pose_a.rotationMatrix(), fixedRotationMatrix2D(0.7));
}

TEST_F(SE2Test, ExpOfStraightLineAndArc) {
  // No rotation: straight line
  expectPoseNear(SE2<double>::exp(Twist2<double>(Vec2<double>(2.0, -1.0), 0.0)),
                 SE2<double>(0.0, 2.0, -1.0));

  // Driving forward along x while turning left a quarter turn, on a circle
  // of radius 1
  expectPoseNear(
      SE2<double>::exp(Twist2<double>(Vec2<double>(PI / 2.0, 0.0), PI / 2.0)),
      SE2<double>(PI / 2.0, 1.0, 1.0));
}

TEST_F(SE2Test, ExpLogRoundTrip) {
  const Vec2<double> linear(1.0, -2.0);
  for (const double w : {-3.1, -1.0, -1e-3, -1e-9, 0.0, 1e-12, 1e-6, 0.1,
                         0.19999, 0.20001, 1.0, 2.5, 3.1}) {
    const Twist2<double> twist =
        SE2<double>::exp(Twist2<double>(linear, w)).log();
    expectVecNear(twist.linear, linear);
    EXPECT_NEAR(twist.angular, w, EPSILON);
  }

  expectPoseNear(SE2<double>::exp(pose_a.log()), pose_a);
  expectPoseNear(SE2<double>::exp(pose_b.log()), pose_b);

  const Twist2<double> zero = SE2<double>().log();
  expectVecNear(zero.linear, Vec2<double>(0.0, 0.0));
  EXPECT_EQ(zero.angular, 0.0);
  expectPoseNear(SE2<double>::exp(Twist2<double>()), SE2<double>());
}

TEST_F(SE2Test, MatchesSE3InPlane) {
  // A planar motion embedded in 3D gives the same result as SE(3)
  const Twist2<double> twist2(Vec2<double>(1.5, -0.4), 0.9);
  const Twist3<double> twist3(Vec3<double>(1.5, -0.4, 0.0),
                              Vec3<double>(0.0, 0.0, 0.9));
  const SE2<double> pose2 = SE2<double>::exp(twist2);
  const SE3<double> pose3 = SE3<double>::exp(twist3);

  EXPECT_NEAR(pose2.translation.x, pose3.translation.x, EPSILON);
  EXPECT_NEAR(pose2.translation.y, pose3.translation.y, EPSILON);
  EXPECT_NEAR(pose3.translation.z, 0.0, EPSILON);
  expectMatrixNear(pose3.rotation, fixedRotationMatrixZ(pose2.theta));
}

TEST_F(SE2Test, Interpolate) {
  expectPoseNear(interpolate(pose_a, pose_b, 0.0), pose_a);
  expectPoseNear(interpolate(pose_a, pose_b, 1.0), pose_b);
  expectPoseNear(interpolate(pose_a, pose_b, 0.5),
                 interpolate(pose_b, pose_a, 0.5));

  const SE2<double> mid = interpolate(pose_a, pose_b, 0.5);
  const SE2<double> step = pose_a.inverse() * mid;
  expectPoseNear(pose_a * step * step, pose_b);

  // Pure translation interpolates linearly
  const SE2<double> t0(0.0, 0.0, 0.0);
  const SE2<double> t1(0.0, 4.0, -2.0);
  expectPoseNear(interpolate(t0, t1, 0.25), SE2<double>(0.0, 1.0, -0.5));
}

TEST_F(SE2Test, FloatInstantiation) {
  const SE2<float> pose(0.7f, 1.0f, 2.0f);
  const SE2<float> round_trip = SE2<float>::exp(pose.log());
  EXPECT_NEAR(round_trip.theta, 0.7f, 1e-5f);
  EXPECT_NEAR(round_trip.translation.x, 1.0f, 1e-5f);
  EXPECT_NEAR(round_trip.translation.y, 2.0f, 1e-5f);
}

// --------------------------- Axis angle ---------------------------

TEST(AxisAngleTest, DefaultIsZeroRotation) {
  const AxisAngle<double> axis_angle;
  EXPECT_EQ(axis_angle.phi, 0.0);
  expectMatrixNear(axis_angle.toRotationMatrix(),
                   unitFixedSizeMatrix<double, 3, 3>());
}

TEST(AxisAngleTest, ToRotationMatrixIsFixedSizeAndMatchesElementary) {
  const double angle = 0.7;
  const FixedSizeMatrix<double, 3, 3> rx =
      AxisAngle<double>(angle, 1.0, 0.0, 0.0).toRotationMatrix();
  expectMatrixNear(rx, fixedRotationMatrixX(angle));
  expectMatrixNear(AxisAngle<double>(angle, 0.0, 1.0, 0.0).toRotationMatrix(),
                   fixedRotationMatrixY(angle));
  expectMatrixNear(AxisAngle<double>(angle, 0.0, 0.0, 1.0).toRotationMatrix(),
                   fixedRotationMatrixZ(angle));
  // The axis does not need to be normalized
  expectMatrixNear(AxisAngle<double>(angle, 0.0, 0.0, 4.0).toRotationMatrix(),
                   fixedRotationMatrixZ(angle));
}

TEST(AxisAngleTest, RotationVectorRoundTrip) {
  const Vec3<double> phi(0.4, -0.7, 1.1);
  const AxisAngle<double> axis_angle(phi);
  EXPECT_NEAR(axis_angle.phi, phi.norm(), EPSILON);
  expectVecNear(axis_angle.toRotationVector(), phi);
  expectMatrixNear(axis_angle.toRotationMatrix(), expSO3(phi));
}

TEST(AxisAngleTest, QuaternionAndRotationMatrixRoundTrip) {
  for (const Vec3<double> &phi : testRotationVectors()) {
    const double tolerance = (phi.norm() > 3.1) ? 1e-6 : EPSILON;
    const AxisAngle<double> axis_angle(phi);

    const Quaternion<double> q = axis_angle.toQuaternion();
    expectSameRotation(q, Quaternion<double>::exp(phi));
    expectVecNear(AxisAngle<double>::fromQuaternion(q).toRotationVector(), phi,
                  tolerance);

    const FixedSizeMatrix<double, 3, 3> r = axis_angle.toRotationMatrix();
    expectVecNear(AxisAngle<double>::fromRotationMatrix(r).toRotationVector(),
                  phi, tolerance);
    expectVecNear(rotationMatrixToAxisAngle(r).toRotationVector(), phi,
                  tolerance);
  }
}

TEST(AxisAngleTest, FromIdentityHasUnitAxis) {
  const AxisAngle<double> axis_angle = AxisAngle<double>::fromRotationMatrix(
      unitFixedSizeMatrix<double, 3, 3>());
  EXPECT_NEAR(axis_angle.phi, 0.0, EPSILON);
  EXPECT_NEAR(
      Vec3<double>(axis_angle.x, axis_angle.y, axis_angle.z).norm(), 1.0,
      EPSILON);
}

// --------------------------- Euler angles ---------------------------

const RotationOrder kAllRotationOrders[] = {
    RotationOrder::XYZ, RotationOrder::XZY, RotationOrder::YXZ,
    RotationOrder::YZX, RotationOrder::ZXY, RotationOrder::ZYX};

// Angle about the axis that is applied second for the given order, which is
// the one limited to [-pi/2, pi/2]
double middleAngle(const EulerAngles<double> &e, const RotationOrder order) {
  switch (order) {
  case RotationOrder::XYZ:
  case RotationOrder::ZYX:
    return e.pitch;
  case RotationOrder::XZY:
  case RotationOrder::YZX:
    return e.yaw;
  default:
    return e.roll;
  }
}

TEST(EulerAnglesTest, DefaultOrderIsXYZ) {
  const EulerAngles<double> e(0.3, -0.5, 1.2);
  expectMatrixNear(e.toRotationMatrix(),
                   e.toRotationMatrix(RotationOrder::XYZ));
  expectMatrixNear(e.toRotationMatrix(),
                   fixedRotationMatrixZ(1.2) * fixedRotationMatrixY(-0.5) *
                       fixedRotationMatrixX(0.3));
}

TEST(EulerAnglesTest, IsCopyAssignable) {
  EulerAngles<double> e0(0.3, -0.5, 1.2);
  EulerAngles<double> e1;
  e1 = e0;
  e1.roll = 1.0;
  EXPECT_EQ(e1.pitch, -0.5);
  // The rotation matrix follows the current angles
  expectMatrixNear(e1.rollMatrix(), fixedRotationMatrixX(1.0));
}

TEST(EulerAnglesTest, FromRotationMatrixRoundTripAllOrders) {
  for (const RotationOrder order : kAllRotationOrders) {
    for (const double a : {-2.8, -1.0, 0.0, 0.4, 3.0}) {
      for (const double b : {-1.5, -0.6, 0.0, 0.9, 1.5}) {
        for (const double c : {-3.0, -0.2, 0.0, 1.3, 2.9}) {
          // b is assigned to the middle angle, which must be within +-pi/2
          EulerAngles<double> e;
          switch (order) {
          case RotationOrder::XYZ:
          case RotationOrder::ZYX:
            e = EulerAngles<double>(a, b, c);
            break;
          case RotationOrder::XZY:
          case RotationOrder::YZX:
            e = EulerAngles<double>(a, c, b);
            break;
          default:
            e = EulerAngles<double>(b, a, c);
            break;
          }
          ASSERT_EQ(middleAngle(e, order), b);

          const FixedSizeMatrix<double, 3, 3> r = e.toRotationMatrix(order);
          const EulerAngles<double> e_back =
              EulerAngles<double>::fromRotationMatrix(r, order);

          EXPECT_NEAR(e_back.roll, e.roll, EPSILON);
          EXPECT_NEAR(e_back.pitch, e.pitch, EPSILON);
          EXPECT_NEAR(e_back.yaw, e.yaw, EPSILON);
        }
      }
    }
  }
}

TEST(EulerAnglesTest, FromRotationMatrixReproducesAnyRotation) {
  // For arbitrary rotations the angles are not unique, but they must
  // reproduce the rotation
  for (const RotationOrder order : kAllRotationOrders) {
    for (const Vec3<double> &phi : testRotationVectors()) {
      const FixedSizeMatrix<double, 3, 3> r = expSO3(phi);
      const EulerAngles<double> e =
          EulerAngles<double>::fromRotationMatrix(r, order);
      expectMatrixNear(e.toRotationMatrix(order), r);
      EXPECT_LE(std::abs(middleAngle(e, order)), PI / 2.0 + EPSILON);
    }
  }
}

TEST(EulerAnglesTest, GimbalLock) {
  for (const RotationOrder order : kAllRotationOrders) {
    for (const double sign : {-1.0, 1.0}) {
      EulerAngles<double> e;
      switch (order) {
      case RotationOrder::XYZ:
      case RotationOrder::ZYX:
        e = EulerAngles<double>(0.4, sign * PI / 2.0, -1.1);
        break;
      case RotationOrder::XZY:
      case RotationOrder::YZX:
        e = EulerAngles<double>(0.4, -1.1, sign * PI / 2.0);
        break;
      default:
        e = EulerAngles<double>(sign * PI / 2.0, 0.4, -1.1);
        break;
      }
      const FixedSizeMatrix<double, 3, 3> r = e.toRotationMatrix(order);
      const EulerAngles<double> e_back =
          EulerAngles<double>::fromRotationMatrix(r, order);

      // The individual outer angles are not recoverable, the rotation is
      expectMatrixNear(e_back.toRotationMatrix(order), r, 1e-7);
      EXPECT_NEAR(std::abs(middleAngle(e_back, order)), PI / 2.0, 1e-7);
      EXPECT_FALSE(std::isnan(e_back.roll));
      EXPECT_FALSE(std::isnan(e_back.pitch));
      EXPECT_FALSE(std::isnan(e_back.yaw));
    }
  }
}

TEST(EulerAnglesTest, QuaternionRoundTrip) {
  const EulerAngles<double> e(0.3, -0.5, 1.2);

  const Quaternion<double> q = e.toQuaternion();
  expectMatrixNear(q.toRotationMatrix(), e.toRotationMatrix());
  const EulerAngles<double> e_back = EulerAngles<double>::fromQuaternion(q);
  EXPECT_NEAR(e_back.roll, 0.3, EPSILON);
  EXPECT_NEAR(e_back.pitch, -0.5, EPSILON);
  EXPECT_NEAR(e_back.yaw, 1.2, EPSILON);

  for (const RotationOrder order : kAllRotationOrders) {
    const Quaternion<double> qo = e.toQuaternion(order);
    expectMatrixNear(qo.toRotationMatrix(), e.toRotationMatrix(order));
    const EulerAngles<double> eo =
        EulerAngles<double>::fromQuaternion(qo, order);
    EXPECT_NEAR(eo.roll, 0.3, EPSILON);
    EXPECT_NEAR(eo.pitch, -0.5, EPSILON);
    EXPECT_NEAR(eo.yaw, 1.2, EPSILON);
  }
}

TEST(EulerAnglesTest, YawOnlyMatchesSE2) {
  // A planar rotation is the same in every representation
  const double yaw = 0.8;
  const EulerAngles<double> e(0.0, 0.0, yaw);
  expectMatrixNear(e.toRotationMatrix(), fixedRotationMatrixZ(yaw));
  expectMatrixNear(
      e.toRotationMatrix(),
      AxisAngle<double>(yaw, 0.0, 0.0, 1.0).toRotationMatrix());
  EXPECT_NEAR(SE2<double>(yaw, 0.0, 0.0).theta,
              EulerAngles<double>::fromRotationMatrix(e.toRotationMatrix()).yaw,
              EPSILON);
}

// All four rotation representations convert to each other consistently
TEST(RotationConversionTest, AllRepresentationsAgree) {
  for (const Vec3<double> &phi : testRotationVectors()) {
    if (phi.norm() > 3.1) {
      continue;
    }
    const Quaternion<double> q = Quaternion<double>::exp(phi);
    const FixedSizeMatrix<double, 3, 3> r = q.toRotationMatrix();
    const AxisAngle<double> axis_angle = AxisAngle<double>::fromQuaternion(q);
    const EulerAngles<double> euler = EulerAngles<double>::fromQuaternion(q);

    expectMatrixNear(axis_angle.toRotationMatrix(), r);
    expectMatrixNear(euler.toRotationMatrix(), r);
    expectSameRotation(axis_angle.toQuaternion(), q);
    expectSameRotation(euler.toQuaternion(), q);
    expectSameRotation(Quaternion<double>::fromRotationMatrix(r), q);
  }
}

// ---------------------- Scalar type conversions ----------------------

TEST(ScalarConversionTest, QuaternionMembersAndConversion) {
  const Quaternion<double> qd(0.5, -0.5, 0.5, 0.5);
  // Members are named like those of the vector types
  EXPECT_EQ(qd.w, 0.5);
  EXPECT_EQ(qd.x, -0.5);
  EXPECT_EQ(qd.y, 0.5);
  EXPECT_EQ(qd.z, 0.5);

  const Quaternion<float> qf = qd;
  EXPECT_FLOAT_EQ(qf.w, 0.5f);
  EXPECT_FLOAT_EQ(qf.x, -0.5f);
  const Quaternion<double> qd_back = qf;
  EXPECT_DOUBLE_EQ(qd_back.z, 0.5);
}

TEST(ScalarConversionTest, RotationTypes) {
  const AxisAngle<float> axis_angle_f = AxisAngle<double>(0.5, 0.0, 0.0, 1.0);
  EXPECT_FLOAT_EQ(axis_angle_f.phi, 0.5f);
  EXPECT_FLOAT_EQ(axis_angle_f.z, 1.0f);

  const EulerAngles<float> euler_f = EulerAngles<double>(0.25, 0.5, 0.75);
  EXPECT_FLOAT_EQ(euler_f.roll, 0.25f);
  EXPECT_FLOAT_EQ(euler_f.yaw, 0.75f);
}

TEST(ScalarConversionTest, Poses) {
  const SE3<double> pose_d(fixedRotationMatrixZ(0.5),
                           Vec3<double>(1.0, 2.0, 3.0));
  const SE3<float> pose_f = pose_d;
  EXPECT_FLOAT_EQ(pose_f.translation.y, 2.0f);
  EXPECT_NEAR(pose_f.rotation(0, 0), std::cos(0.5), 1e-6);

  const Twist3<float> twist_f =
      Twist3<double>(Vec3<double>(1.0, 2.0, 3.0), Vec3<double>(4.0, 5.0, 6.0));
  EXPECT_FLOAT_EQ(twist_f.angular.z, 6.0f);

  const SE2<float> pose2_f = SE2<double>(0.5, 1.0, 2.0);
  EXPECT_FLOAT_EQ(pose2_f.theta, 0.5f);
  EXPECT_FLOAT_EQ(pose2_f.translation.x, 1.0f);

  const Twist2<float> twist2_f = Twist2<double>(Vec2<double>(1.0, 2.0), 3.0);
  EXPECT_FLOAT_EQ(twist2_f.angular, 3.0f);
}

} // namespace lumos
