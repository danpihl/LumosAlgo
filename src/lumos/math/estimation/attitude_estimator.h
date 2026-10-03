#ifndef LUMOS_MATH_ESTIMATION_ATTITUDE_ESTIMATOR_H_
#define LUMOS_MATH_ESTIMATION_ATTITUDE_ESTIMATOR_H_

#include <cmath>
#include <limits>

#include "lumos/math/estimation/class_def/attitude_estimator.h"
#include "lumos/math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "lumos/math/lin_alg/vector_low_dim/vec3.h"
#include "lumos/math/transformations/angles.h"
#include "lumos/math/transformations/euler_angles.h"
#include "lumos/math/transformations/quaternion.h"

namespace lumos
{

  template <typename T>
  AttitudeEstimator<T>::AttitudeEstimator()
      : config_(), attitude_(), gyro_bias_(0, 0, 0), angular_rate_(0, 0, 0),
        is_initialized_(false), rate_is_initialized_(false), accel_used_(false)
  {
  }

  template <typename T>
  AttitudeEstimator<T>::AttitudeEstimator(
      const AttitudeEstimatorConfig<T> &config)
      : config_(config), attitude_(), gyro_bias_(0, 0, 0), angular_rate_(0, 0, 0),
        is_initialized_(false), rate_is_initialized_(false), accel_used_(false)
  {
  }

  template <typename T>
  void AttitudeEstimator<T>::reset()
  {
    attitude_ = Quaternion<T>();
    gyro_bias_ = Vec3<T>(0, 0, 0);
    angular_rate_ = Vec3<T>(0, 0, 0);
    is_initialized_ = false;
    rate_is_initialized_ = false;
    accel_used_ = false;
  }

  template <typename T>
  bool AttitudeEstimator<T>::initialize(const Vec3<T> &accel)
  {
    const T accel_norm = accel.norm();
    if (!(accel_norm > std::numeric_limits<T>::epsilon()))
    {
      return false;
    }

    // At rest the accelerometer measures the world up direction in the body
    // frame, which is the third row of the rotation matrix
    const T roll = std::atan2(accel.y, accel.z);
    const T pitch =
        std::atan2(-accel.x, std::sqrt(accel.y * accel.y + accel.z * accel.z));

    attitude_ = EulerAngles<T>(roll, pitch, T(0)).toQuaternion();
    is_initialized_ = true;
    return true;
  }

  template <typename T>
  bool AttitudeEstimator<T>::initialize(const Vec3<T> &accel,
                                        const Vec3<T> &mag)
  {
    if (!(mag.norm() > std::numeric_limits<T>::epsilon()))
    {
      return false;
    }
    if (!initialize(accel))
    {
      return false;
    }

    // Rotate the magnetometer sample into the levelled frame (yaw still zero).
    // There the horizontal field points to north, which is at an angle of -yaw
    const Vec3<T> mag_level = attitude_.rotate(mag);
    const T horizontal_sq = mag_level.x * mag_level.x + mag_level.y * mag_level.y;
    if (!(horizontal_sq > std::numeric_limits<T>::epsilon()))
    {
      // The field is vertical, so it carries no heading information
      return true;
    }
    const T yaw = std::atan2(-mag_level.y, mag_level.x);

    const EulerAngles<T> euler = EulerAngles<T>::fromQuaternion(attitude_);
    attitude_ = EulerAngles<T>(euler.roll, euler.pitch, yaw).toQuaternion();
    return true;
  }

  template <typename T>
  void AttitudeEstimator<T>::setAttitude(const Quaternion<T> &attitude)
  {
    attitude_ = attitude.normalized();
    is_initialized_ = true;
  }

  template <typename T>
  void AttitudeEstimator<T>::setGyroBias(const Vec3<T> &gyro_bias)
  {
    gyro_bias_ = gyro_bias;
  }

  template <typename T>
  void AttitudeEstimator<T>::setConfig(const AttitudeEstimatorConfig<T> &config)
  {
    config_ = config;
  }

  template <typename T>
  const AttitudeEstimatorConfig<T> &AttitudeEstimator<T>::config() const
  {
    return config_;
  }

  // Rotation error between the measured and the estimated up direction, as a
  // small rotation vector in the body frame. Returns false if the sample is
  // rejected
  template <typename T>
  bool AttitudeEstimator<T>::accelError(const Vec3<T> &accel,
                                        Vec3<T> &error) const
  {
    const T accel_norm = accel.norm();
    if (!(accel_norm > std::numeric_limits<T>::epsilon()))
    {
      return false;
    }
    if (config_.accel_rejection > T(0) &&
        std::abs(accel_norm - config_.gravity) >
            config_.accel_rejection * config_.gravity)
    {
      return false;
    }

    const Vec3<T> up_measured = accel / accel_norm;
    const Vec3<T> up_estimated =
        attitude_.conjugate().rotate(Vec3<T>(T(0), T(0), T(1)));
    error = up_measured.crossProduct(up_estimated);
    return true;
  }

  // Heading error from the magnetometer, as a small rotation vector in the body
  // frame. It is a rotation about the estimated up direction only, so that the
  // magnetometer does not disturb roll and pitch, and it does not depend on
  // the inclination of the field
  template <typename T>
  bool AttitudeEstimator<T>::magError(const Vec3<T> &mag, Vec3<T> &error) const
  {
    const T mag_norm = mag.norm();
    if (!(mag_norm > std::numeric_limits<T>::epsilon()))
    {
      return false;
    }

    // In the world frame the horizontal part of the field should point to
    // north (world x). The sine of its angle from north is the heading error
    const Vec3<T> mag_world = attitude_.rotate(mag / mag_norm);
    const T horizontal =
        std::sqrt(mag_world.x * mag_world.x + mag_world.y * mag_world.y);
    // A field that is close to vertical carries no usable heading
    if (!(horizontal > T(1e-3)))
    {
      return false;
    }
    const Vec3<T> up_estimated =
        attitude_.conjugate().rotate(Vec3<T>(T(0), T(0), T(1)));

    error = (-mag_world.y / horizontal) * up_estimated;
    return true;
  }

  // error is the sum of the correction terms, already scaled by their
  // proportional gains
  template <typename T>
  void AttitudeEstimator<T>::step(const Vec3<T> &gyro, const Vec3<T> &error,
                                  T dt)
  {
    // The integral of the error is the negative of the gyro bias
    if (config_.ki > T(0))
    {
      gyro_bias_ = gyro_bias_ - (config_.ki * dt) * error;
      if (config_.max_gyro_bias > T(0))
      {
        const T limit = config_.max_gyro_bias;
        gyro_bias_.x = std::fmin(std::fmax(gyro_bias_.x, -limit), limit);
        gyro_bias_.y = std::fmin(std::fmax(gyro_bias_.y, -limit), limit);
        gyro_bias_.z = std::fmin(std::fmax(gyro_bias_.z, -limit), limit);
      }
    }

    const Vec3<T> rate = gyro - gyro_bias_;

    attitude_ = (attitude_ * Quaternion<T>::exp((rate + error) * dt)).normalized();

    if (config_.rate_cutoff_frequency > T(0) && rate_is_initialized_)
    {
      const T time_constant =
          T(1) / (T(2) * pi<T>() * config_.rate_cutoff_frequency);
      const T alpha = dt / (time_constant + dt);
      angular_rate_ = angular_rate_ + alpha * (rate - angular_rate_);
    }
    else
    {
      angular_rate_ = rate;
    }
    rate_is_initialized_ = true;
  }

  template <typename T>
  void AttitudeEstimator<T>::update(const Vec3<T> &gyro, const Vec3<T> &accel,
                                    T dt)
  {
    if (!(dt > T(0)))
    {
      return;
    }
    if (!is_initialized_)
    {
      initialize(accel);
    }

    Vec3<T> error(0, 0, 0);
    Vec3<T> accel_error(0, 0, 0);
    accel_used_ = accelError(accel, accel_error);
    if (accel_used_)
    {
      error = config_.kp_accel * accel_error;
    }

    step(gyro, error, dt);
  }

  template <typename T>
  void AttitudeEstimator<T>::update(const Vec3<T> &gyro, const Vec3<T> &accel,
                                    const Vec3<T> &mag, T dt)
  {
    if (!(dt > T(0)))
    {
      return;
    }
    if (!is_initialized_)
    {
      if (!initialize(accel, mag))
      {
        initialize(accel);
      }
    }

    Vec3<T> error(0, 0, 0);
    Vec3<T> accel_error(0, 0, 0);
    accel_used_ = accelError(accel, accel_error);
    if (accel_used_)
    {
      error = config_.kp_accel * accel_error;
    }
    Vec3<T> mag_error(0, 0, 0);
    if (magError(mag, mag_error))
    {
      error = error + config_.kp_mag * mag_error;
    }

    step(gyro, error, dt);
  }

  template <typename T>
  void AttitudeEstimator<T>::predict(const Vec3<T> &gyro, T dt)
  {
    if (!(dt > T(0)))
    {
      return;
    }
    accel_used_ = false;
    // Without a correction term the bias estimate is left unchanged
    step(gyro, Vec3<T>(0, 0, 0), dt);
  }

  template <typename T>
  const Quaternion<T> &AttitudeEstimator<T>::attitude() const
  {
    return attitude_;
  }

  template <typename T>
  FixedSizeMatrix<T, 3, 3> AttitudeEstimator<T>::rotationMatrix() const
  {
    return attitude_.toRotationMatrix();
  }

  template <typename T>
  EulerAngles<T> AttitudeEstimator<T>::eulerAngles() const
  {
    return EulerAngles<T>::fromQuaternion(attitude_);
  }

  template <typename T>
  const Vec3<T> &AttitudeEstimator<T>::angularRate() const
  {
    return angular_rate_;
  }

  template <typename T>
  Vec3<T> AttitudeEstimator<T>::angularRateWorld() const
  {
    return attitude_.rotate(angular_rate_);
  }

  template <typename T>
  Vec3<T> AttitudeEstimator<T>::eulerRates() const
  {
    const EulerAngles<T> euler = eulerAngles();
    const T sr = std::sin(euler.roll);
    const T cr = std::cos(euler.roll);
    const T cp = std::cos(euler.pitch);
    const T tp = std::tan(euler.pitch);

    // Rate about the levelled vertical and lateral axes
    const T vertical = angular_rate_.y * sr + angular_rate_.z * cr;
    const T lateral = angular_rate_.y * cr - angular_rate_.z * sr;

    return Vec3<T>(angular_rate_.x + vertical * tp, lateral, vertical / cp);
  }

  template <typename T>
  const Vec3<T> &AttitudeEstimator<T>::gyroBias() const
  {
    return gyro_bias_;
  }

  template <typename T>
  bool AttitudeEstimator<T>::isInitialized() const
  {
    return is_initialized_;
  }

  template <typename T>
  bool AttitudeEstimator<T>::accelUsed() const
  {
    return accel_used_;
  }

} // namespace lumos

#endif // LUMOS_MATH_ESTIMATION_ATTITUDE_ESTIMATOR_H_
