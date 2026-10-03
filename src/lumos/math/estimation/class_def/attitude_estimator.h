#ifndef LUMOS_MATH_ESTIMATION_CLASS_DEF_ATTITUDE_ESTIMATOR_H_
#define LUMOS_MATH_ESTIMATION_CLASS_DEF_ATTITUDE_ESTIMATOR_H_

#include "lumos/math/lin_alg/matrix_fixed/class_def/matrix_fixed.h"
#include "lumos/math/lin_alg/vector_low_dim/class_def/vec3.h"
#include "lumos/math/misc/forward_decl.h"
#include "lumos/math/transformations/class_def/euler_angles.h"
#include "lumos/math/transformations/class_def/quaternion.h"

namespace lumos
{

  template <typename T>
  struct AttitudeEstimatorConfig
  {
    // Proportional gain of the accelerometer correction [1/s]. The attitude
    // follows the accelerometer with a time constant of about 1 / kp_accel
    T kp_accel = T(1.0);
    // Proportional gain of the magnetometer (heading) correction [1/s]
    T kp_mag = T(1.0);
    // Integral gain, which drives the gyro bias estimate [1/s^2]. Zero turns
    // bias estimation off
    T ki = T(0.05);
    // Magnitude of gravity, in the same unit as the accelerometer samples
    T gravity = T(9.80665);
    // The accelerometer is only used while its magnitude is within this
    // fraction of gravity, so that it is ignored during linear acceleration.
    // Zero or negative uses every nonzero sample
    T accel_rejection = T(0.2);
    // Each component of the gyro bias estimate is limited to this magnitude
    // [rad/s]. Zero or negative means no limit
    T max_gyro_bias = T(0);
    // Cutoff frequency of the first order low-pass filter on the angular rate
    // output [Hz]. Zero or negative means no filtering
    T rate_cutoff_frequency = T(0);
  };

  // Attitude and attitude rate estimator for an IMU, with an optional
  // magnetometer. It is an explicit complementary filter on the rotation group
  // (Mahony et al.): the gyro is integrated, and the accelerometer and
  // magnetometer slowly correct the drift and estimate the gyro bias.
  //
  // Conventions:
  //  - The attitude rotates vectors from the body frame to the world frame:
  //    v_world = attitude().rotate(v_body)
  //  - The world z axis points up. An accelerometer at rest measures
  //    (0, 0, +gravity) when the body frame is aligned with the world frame
  //  - With a magnetometer the world x axis points to magnetic north, so the
  //    world frame is north-west-up. Without one the heading is not observable:
  //    it starts at zero and drifts with the gyro
  //  - Gyro samples are in rad/s, in the body frame
  //
  // Uses no heap memory and does not throw
  template <typename T>
  class AttitudeEstimator
  {
  public:
    AttitudeEstimator();
    explicit AttitudeEstimator(const AttitudeEstimatorConfig<T> &config);

    // Clears the attitude, bias and rate. The next update initializes the
    // attitude from its accelerometer (and magnetometer) sample
    void reset();

    // Sets roll and pitch from the accelerometer, and yaw to zero or to the
    // magnetic heading. Returns false if a sample is too small to use
    bool initialize(const Vec3<T> &accel);
    bool initialize(const Vec3<T> &accel, const Vec3<T> &mag);

    void setAttitude(const Quaternion<T> &attitude);
    void setGyroBias(const Vec3<T> &gyro_bias);
    void setConfig(const AttitudeEstimatorConfig<T> &config);
    const AttitudeEstimatorConfig<T> &config() const;

    // Advances the estimate by dt seconds. Calls with dt <= 0 are ignored
    void update(const Vec3<T> &gyro, const Vec3<T> &accel, T dt);
    void update(const Vec3<T> &gyro, const Vec3<T> &accel, const Vec3<T> &mag,
                T dt);
    // Gyro only, for when no accelerometer sample is available
    void predict(const Vec3<T> &gyro, T dt);

    // Attitude
    const Quaternion<T> &attitude() const;
    FixedSizeMatrix<T, 3, 3> rotationMatrix() const;
    // Roll, pitch and yaw with RotationOrder::XYZ
    EulerAngles<T> eulerAngles() const;

    // Attitude rate
    // Bias corrected (and optionally low-pass filtered) angular rate in the
    // body frame [rad/s]
    const Vec3<T> &angularRate() const;
    // The same angular rate expressed in the world frame
    Vec3<T> angularRateWorld() const;
    // Time derivatives of roll, pitch and yaw, as (x, y, z). Not defined at a
    // pitch of +-90 degrees, where the yaw and roll rates grow without bound
    Vec3<T> eulerRates() const;

    const Vec3<T> &gyroBias() const;
    bool isInitialized() const;
    // Whether the last update used its accelerometer sample
    bool accelUsed() const;

  private:
    void step(const Vec3<T> &gyro, const Vec3<T> &error, T dt);
    bool accelError(const Vec3<T> &accel, Vec3<T> &error) const;
    bool magError(const Vec3<T> &mag, Vec3<T> &error) const;

    AttitudeEstimatorConfig<T> config_;
    Quaternion<T> attitude_;
    Vec3<T> gyro_bias_;
    Vec3<T> angular_rate_;
    bool is_initialized_;
    bool rate_is_initialized_;
    bool accel_used_;
  };

  using AttitudeEstimatord = AttitudeEstimator<double>;
  using AttitudeEstimatorf = AttitudeEstimator<float>;

} // namespace lumos

#endif // LUMOS_MATH_ESTIMATION_CLASS_DEF_ATTITUDE_ESTIMATOR_H_
