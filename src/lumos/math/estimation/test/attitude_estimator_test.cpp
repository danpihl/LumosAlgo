#include <cmath>
#include <gtest/gtest.h>

#include "lumos/math/estimation/attitude_estimator.h"
#include "lumos/math/transformations/angles.h"
#include "lumos/math/transformations/euler_angles.h"
#include "lumos/math/transformations/quaternion.h"

namespace lumos
{

  namespace
  {

    constexpr double EPSILON = 1e-9;
    constexpr double PI = 3.14159265358979323846;
    constexpr double GRAVITY = 9.80665;

    // Magnetic field in the world frame: pointing north (x) and down
    const Vec3<double> kMagWorld(0.22, 0.0, -0.42);

    // Ideal accelerometer sample for a body at rest with the given attitude
    Vec3<double> accelAtRest(const Quaternion<double> &attitude)
    {
      return attitude.conjugate().rotate(Vec3<double>(0.0, 0.0, GRAVITY));
    }

    // Ideal magnetometer sample for the given attitude
    Vec3<double> magAt(const Quaternion<double> &attitude)
    {
      return attitude.conjugate().rotate(kMagWorld);
    }

    Quaternion<double> fromEuler(const double roll, const double pitch,
                                 const double yaw)
    {
      return EulerAngles<double>(roll, pitch, yaw).toQuaternion();
    }

    // Angle of the rotation between two attitudes
    double attitudeError(const Quaternion<double> &q0,
                         const Quaternion<double> &q1)
    {
      return (q0.conjugate() * q1).log().norm();
    }

    // Angle between the up directions of two attitudes, which ignores heading
    double tiltError(const Quaternion<double> &q0, const Quaternion<double> &q1)
    {
      const Vec3<double> up(0.0, 0.0, 1.0);
      const Vec3<double> up0 = q0.conjugate().rotate(up);
      const Vec3<double> up1 = q1.conjugate().rotate(up);
      return std::atan2(up0.crossProduct(up1).norm(), up0 * up1);
    }

    void expectVecNear(const Vec3<double> &v0, const Vec3<double> &v1,
                       const double tolerance = EPSILON)
    {
      EXPECT_NEAR(v0.x, v1.x, tolerance);
      EXPECT_NEAR(v0.y, v1.y, tolerance);
      EXPECT_NEAR(v0.z, v1.z, tolerance);
    }

  } // namespace

  TEST(AttitudeEstimatorTest, DefaultState)
  {
    const AttitudeEstimator<double> estimator;
    EXPECT_FALSE(estimator.isInitialized());
    EXPECT_FALSE(estimator.accelUsed());
    EXPECT_NEAR(attitudeError(estimator.attitude(), Quaternion<double>()), 0.0,
                EPSILON);
    expectVecNear(estimator.angularRate(), Vec3<double>(0.0, 0.0, 0.0));
    expectVecNear(estimator.gyroBias(), Vec3<double>(0.0, 0.0, 0.0));
  }

  TEST(AttitudeEstimatorTest, InitializeFromAccel)
  {
    AttitudeEstimator<double> estimator;

    // Level
    EXPECT_TRUE(estimator.initialize(Vec3<double>(0.0, 0.0, GRAVITY)));
    EXPECT_TRUE(estimator.isInitialized());
    EXPECT_NEAR(attitudeError(estimator.attitude(), Quaternion<double>()), 0.0,
                EPSILON);

    // Tilted: roll and pitch are recovered, yaw is zero
    for (const double roll : {-2.5, -0.4, 0.0, 0.7, 3.0})
    {
      for (const double pitch : {-1.2, -0.3, 0.0, 0.5, 1.4})
      {
        const Quaternion<double> truth = fromEuler(roll, pitch, 0.0);
        EXPECT_TRUE(estimator.initialize(accelAtRest(truth)));
        EXPECT_NEAR(attitudeError(estimator.attitude(), truth), 0.0, EPSILON);

        const EulerAngles<double> euler = estimator.eulerAngles();
        EXPECT_NEAR(euler.roll, roll, EPSILON);
        EXPECT_NEAR(euler.pitch, pitch, EPSILON);
        EXPECT_NEAR(euler.yaw, 0.0, EPSILON);
      }
    }

    // The heading of the true attitude is not observable from the accelerometer
    const Quaternion<double> with_yaw = fromEuler(0.3, -0.2, 1.1);
    EXPECT_TRUE(estimator.initialize(accelAtRest(with_yaw)));
    EXPECT_NEAR(tiltError(estimator.attitude(), with_yaw), 0.0, EPSILON);
    EXPECT_NEAR(estimator.eulerAngles().yaw, 0.0, EPSILON);

    // The magnitude of the sample does not matter
    EXPECT_TRUE(estimator.initialize(accelAtRest(with_yaw) * 0.1));
    EXPECT_NEAR(tiltError(estimator.attitude(), with_yaw), 0.0, EPSILON);
  }

  TEST(AttitudeEstimatorTest, InitializeRejectsZeroSamples)
  {
    AttitudeEstimator<double> estimator;
    EXPECT_FALSE(estimator.initialize(Vec3<double>(0.0, 0.0, 0.0)));
    EXPECT_FALSE(estimator.isInitialized());
    EXPECT_FALSE(estimator.initialize(Vec3<double>(0.0, 0.0, GRAVITY),
                                      Vec3<double>(0.0, 0.0, 0.0)));
    EXPECT_FALSE(estimator.isInitialized());
  }

  TEST(AttitudeEstimatorTest, InitializeFromAccelAndMag)
  {
    AttitudeEstimator<double> estimator;
    for (const double roll : {-0.6, 0.0, 0.9})
    {
      for (const double pitch : {-0.8, 0.0, 0.4})
      {
        for (const double yaw : {-3.0, -1.2, 0.0, 0.5, 2.7})
        {
          const Quaternion<double> truth = fromEuler(roll, pitch, yaw);
          EXPECT_TRUE(estimator.initialize(accelAtRest(truth), magAt(truth)));
          EXPECT_NEAR(attitudeError(estimator.attitude(), truth), 0.0, 1e-9);
        }
      }
    }
  }

  TEST(AttitudeEstimatorTest, FirstUpdateInitializes)
  {
    AttitudeEstimator<double> estimator;
    const Quaternion<double> truth = fromEuler(0.4, -0.3, 0.0);
    estimator.update(Vec3<double>(0.0, 0.0, 0.0), accelAtRest(truth), 0.01);
    EXPECT_TRUE(estimator.isInitialized());
    EXPECT_TRUE(estimator.accelUsed());
    EXPECT_NEAR(attitudeError(estimator.attitude(), truth), 0.0, EPSILON);

    AttitudeEstimator<double> estimator_mag;
    const Quaternion<double> truth_mag = fromEuler(0.4, -0.3, 2.0);
    estimator_mag.update(Vec3<double>(0.0, 0.0, 0.0), accelAtRest(truth_mag),
                         magAt(truth_mag), 0.01);
    EXPECT_NEAR(attitudeError(estimator_mag.attitude(), truth_mag), 0.0, 1e-9);
  }

  TEST(AttitudeEstimatorTest, StaysAtTruthWhenStationary)
  {
    AttitudeEstimator<double> estimator;
    const Quaternion<double> truth = fromEuler(0.4, -0.3, 0.0);
    for (int i = 0; i < 2000; ++i)
    {
      estimator.update(Vec3<double>(0.0, 0.0, 0.0), accelAtRest(truth), 0.005);
    }
    EXPECT_NEAR(attitudeError(estimator.attitude(), truth), 0.0, EPSILON);
    expectVecNear(estimator.angularRate(), Vec3<double>(0.0, 0.0, 0.0));
    expectVecNear(estimator.gyroBias(), Vec3<double>(0.0, 0.0, 0.0));
    EXPECT_NEAR(estimator.attitude().norm(), 1.0, EPSILON);
  }

  TEST(AttitudeEstimatorTest, ConvergesFromWrongAttitude)
  {
    AttitudeEstimatorConfig<double> config;
    config.kp_accel = 2.0;
    config.ki = 0.0;
    AttitudeEstimator<double> estimator(config);

    const Quaternion<double> truth = fromEuler(0.5, -0.4, 0.0);
    estimator.setAttitude(fromEuler(-0.6, 0.7, 0.0));
    const double initial_error = tiltError(estimator.attitude(), truth);
    EXPECT_GT(initial_error, 1.0);

    // The tilt error decreases monotonically
    double previous_error = initial_error;
    for (int i = 0; i < 2000; ++i)
    {
      estimator.update(Vec3<double>(0.0, 0.0, 0.0), accelAtRest(truth), 0.005);
      const double error = tiltError(estimator.attitude(), truth);
      EXPECT_LE(error, previous_error + 1e-12);
      previous_error = error;
    }
    // 10 seconds with a time constant of 0.5 seconds
    EXPECT_LT(tiltError(estimator.attitude(), truth), 1e-6);
  }

  TEST(AttitudeEstimatorTest, ConvergenceRateFollowsGain)
  {
    // For a small error the tilt error decays as exp(-kp * t)
    const double kp = 1.5;
    AttitudeEstimatorConfig<double> config;
    config.kp_accel = kp;
    config.ki = 0.0;
    AttitudeEstimator<double> estimator(config);

    const Quaternion<double> truth;
    estimator.setAttitude(fromEuler(0.02, 0.0, 0.0));

    const double dt = 0.001;
    const double duration = 1.0;
    for (int i = 0; i < static_cast<int>(duration / dt); ++i)
    {
      estimator.update(Vec3<double>(0.0, 0.0, 0.0), accelAtRest(truth), dt);
    }
    EXPECT_NEAR(tiltError(estimator.attitude(), truth),
                0.02 * std::exp(-kp * duration), 1e-5);
  }

  TEST(AttitudeEstimatorTest, IntegratesGyro)
  {
    AttitudeEstimator<double> estimator;
    const double dt = 0.001;

    // Roll at a constant rate, with a consistent accelerometer
    const double roll_rate = 0.8;
    Quaternion<double> truth;
    estimator.setAttitude(truth);
    for (int i = 0; i < 1000; ++i)
    {
      // The accelerometer sample is taken at the start of the interval that
      // the gyro sample covers
      estimator.update(Vec3<double>(roll_rate, 0.0, 0.0), accelAtRest(truth), dt);
      truth = truth * Quaternion<double>::exp(Vec3<double>(roll_rate * dt, 0.0, 0.0));
    }
    EXPECT_NEAR(estimator.eulerAngles().roll, roll_rate, 1e-6);
    EXPECT_NEAR(attitudeError(estimator.attitude(), truth), 0.0, 1e-6);
    expectVecNear(estimator.angularRate(), Vec3<double>(roll_rate, 0.0, 0.0));

    // Yaw at a constant rate: the accelerometer does not change
    estimator.reset();
    estimator.setAttitude(Quaternion<double>());
    const double yaw_rate = -0.5;
    for (int i = 0; i < 2000; ++i)
    {
      estimator.update(Vec3<double>(0.0, 0.0, yaw_rate),
                       Vec3<double>(0.0, 0.0, GRAVITY), dt);
    }
    EXPECT_NEAR(estimator.eulerAngles().yaw, yaw_rate * 2.0, 1e-9);
    EXPECT_NEAR(estimator.eulerAngles().roll, 0.0, 1e-9);
    EXPECT_NEAR(estimator.eulerAngles().pitch, 0.0, 1e-9);
  }

  TEST(AttitudeEstimatorTest, PredictIsGyroOnly)
  {
    AttitudeEstimator<double> estimator;
    estimator.setAttitude(Quaternion<double>());
    estimator.setGyroBias(Vec3<double>(0.01, 0.0, 0.0));

    for (int i = 0; i < 1000; ++i)
    {
      estimator.predict(Vec3<double>(0.51, 0.0, 0.0), 0.001);
    }
    EXPECT_FALSE(estimator.accelUsed());
    // The bias is subtracted and left unchanged
    EXPECT_NEAR(estimator.eulerAngles().roll, 0.5, 1e-9);
    expectVecNear(estimator.gyroBias(), Vec3<double>(0.01, 0.0, 0.0));
    expectVecNear(estimator.angularRate(), Vec3<double>(0.5, 0.0, 0.0));
  }

  TEST(AttitudeEstimatorTest, EstimatesGyroBias)
  {
    AttitudeEstimatorConfig<double> config;
    config.kp_accel = 2.0;
    config.ki = 1.0;
    AttitudeEstimator<double> estimator(config);

    // Stationary, with a constant bias on the two axes the accelerometer
    // observes. The z axis bias is about gravity when level and is not
    // observable without a magnetometer
    const Quaternion<double> truth;
    const Vec3<double> bias(0.03, -0.02, 0.0);
    for (int i = 0; i < 20000; ++i)
    {
      estimator.update(bias, accelAtRest(truth), 0.002);
    }

    expectVecNear(estimator.gyroBias(), bias, 1e-6);
    // The rate output is bias free
    expectVecNear(estimator.angularRate(), Vec3<double>(0.0, 0.0, 0.0), 1e-6);
    EXPECT_LT(tiltError(estimator.attitude(), truth), 1e-6);
  }

  TEST(AttitudeEstimatorTest, BiasEstimationCanBeTurnedOffAndLimited)
  {
    const Vec3<double> bias(0.03, -0.02, 0.0);

    AttitudeEstimatorConfig<double> config_off;
    config_off.ki = 0.0;
    AttitudeEstimator<double> estimator_off(config_off);
    for (int i = 0; i < 5000; ++i)
    {
      estimator_off.update(bias, Vec3<double>(0.0, 0.0, GRAVITY), 0.002);
    }
    expectVecNear(estimator_off.gyroBias(), Vec3<double>(0.0, 0.0, 0.0));
    // Without bias estimation the bias leaves a steady state tilt of bias / kp
    EXPECT_NEAR(tiltError(estimator_off.attitude(), Quaternion<double>()),
                bias.norm() / config_off.kp_accel, 1e-4);

    AttitudeEstimatorConfig<double> config_limited;
    config_limited.ki = 1.0;
    config_limited.max_gyro_bias = 0.01;
    AttitudeEstimator<double> estimator_limited(config_limited);
    for (int i = 0; i < 20000; ++i)
    {
      estimator_limited.update(bias, Vec3<double>(0.0, 0.0, GRAVITY), 0.002);
      EXPECT_LE(std::abs(estimator_limited.gyroBias().x), 0.01 + EPSILON);
      EXPECT_LE(std::abs(estimator_limited.gyroBias().y), 0.01 + EPSILON);
    }
    EXPECT_NEAR(estimator_limited.gyroBias().x, 0.01, EPSILON);
    EXPECT_NEAR(estimator_limited.gyroBias().y, -0.01, EPSILON);
  }

  TEST(AttitudeEstimatorTest, RejectsAccelDuringLinearAcceleration)
  {
    AttitudeEstimator<double> estimator;
    estimator.setAttitude(Quaternion<double>());

    // Level, but accelerating hard sideways: the sample magnitude is far from
    // gravity, so it is ignored and the attitude does not tilt
    const Vec3<double> accelerating(8.0, 0.0, GRAVITY);
    for (int i = 0; i < 1000; ++i)
    {
      estimator.update(Vec3<double>(0.0, 0.0, 0.0), accelerating, 0.005);
      EXPECT_FALSE(estimator.accelUsed());
    }
    EXPECT_NEAR(attitudeError(estimator.attitude(), Quaternion<double>()), 0.0,
                EPSILON);

    // With rejection turned off the same samples tilt the estimate
    AttitudeEstimatorConfig<double> config;
    config.accel_rejection = 0.0;
    AttitudeEstimator<double> no_rejection(config);
    no_rejection.setAttitude(Quaternion<double>());
    for (int i = 0; i < 1000; ++i)
    {
      no_rejection.update(Vec3<double>(0.0, 0.0, 0.0), accelerating, 0.005);
      EXPECT_TRUE(no_rejection.accelUsed());
    }
    EXPECT_GT(attitudeError(no_rejection.attitude(), Quaternion<double>()), 0.3);

    // A zero sample (free fall, or a sensor dropout) is never used
    no_rejection.update(Vec3<double>(0.0, 0.0, 0.0), Vec3<double>(0.0, 0.0, 0.0),
                        0.005);
    EXPECT_FALSE(no_rejection.accelUsed());
  }

  TEST(AttitudeEstimatorTest, GravityUnitIsConfigurable)
  {
    // Accelerometer samples in units of g
    AttitudeEstimatorConfig<double> config;
    config.gravity = 1.0;
    AttitudeEstimator<double> estimator(config);
    estimator.update(Vec3<double>(0.0, 0.0, 0.0), Vec3<double>(0.0, 0.0, 1.0),
                     0.01);
    EXPECT_TRUE(estimator.accelUsed());
    estimator.update(Vec3<double>(0.0, 0.0, 0.0), Vec3<double>(0.0, 0.0, 1.5),
                     0.01);
    EXPECT_FALSE(estimator.accelUsed());
  }

  TEST(AttitudeEstimatorTest, MagnetometerCorrectsHeading)
  {
    AttitudeEstimatorConfig<double> config;
    config.kp_mag = 2.0;
    config.ki = 0.0;
    AttitudeEstimator<double> estimator(config);

    const Quaternion<double> truth = fromEuler(0.2, -0.3, 1.0);
    // Correct tilt, wrong heading
    estimator.setAttitude(fromEuler(0.2, -0.3, -0.5));

    for (int i = 0; i < 4000; ++i)
    {
      estimator.update(Vec3<double>(0.0, 0.0, 0.0), accelAtRest(truth),
                       magAt(truth), 0.005);
    }
    EXPECT_LT(attitudeError(estimator.attitude(), truth), 1e-6);
    EXPECT_NEAR(estimator.eulerAngles().yaw, 1.0, 1e-6);
  }

  TEST(AttitudeEstimatorTest, HeadingConvergenceRateFollowsGain)
  {
    // For a small error the heading error decays as exp(-kp_mag * t), whatever
    // the inclination of the field
    const double kp = 1.5;
    const double dt = 0.001;
    const double duration = 1.0;
    for (const Vec3<double> &field : {Vec3<double>(0.5, 0.0, 0.0), Vec3<double>(0.22, 0.0, -0.42), Vec3<double>(0.05, 0.0, 0.6)})
    {
      AttitudeEstimatorConfig<double> config;
      config.kp_mag = kp;
      config.ki = 0.0;
      AttitudeEstimator<double> estimator(config);
      estimator.setAttitude(fromEuler(0.0, 0.0, 0.02));

      for (int i = 0; i < static_cast<int>(duration / dt); ++i)
      {
        estimator.update(Vec3<double>(0.0, 0.0, 0.0), Vec3<double>(0.0, 0.0, GRAVITY), field, dt);
      }
      EXPECT_NEAR(estimator.eulerAngles().yaw, 0.02 * std::exp(-kp * duration), 1e-5);
    }
  }

  TEST(AttitudeEstimatorTest, MagnetometerDoesNotDisturbTilt)
  {
    AttitudeEstimatorConfig<double> config;
    config.ki = 0.0;
    AttitudeEstimator<double> estimator(config);

    const Quaternion<double> truth = fromEuler(0.2, -0.3, 0.0);
    estimator.setAttitude(truth);

    // A badly disturbed field: wrong inclination and a large heading offset
    const Vec3<double> disturbed_world(0.1, 0.3, 0.6);
    const Vec3<double> disturbed = truth.conjugate().rotate(disturbed_world);
    for (int i = 0; i < 2000; ++i)
    {
      estimator.update(Vec3<double>(0.0, 0.0, 0.0), accelAtRest(truth), disturbed,
                       0.005);
      EXPECT_LT(tiltError(estimator.attitude(), truth), 1e-9);
    }
    // The heading did follow the disturbed field
    EXPECT_GT(std::abs(estimator.eulerAngles().yaw), 0.5);
  }

  TEST(AttitudeEstimatorTest, MagnetometerMakesYawBiasObservable)
  {
    AttitudeEstimatorConfig<double> config;
    config.kp_mag = 2.0;
    config.ki = 1.0;
    AttitudeEstimator<double> estimator(config);

    const Quaternion<double> truth;
    const Vec3<double> bias(0.0, 0.0, 0.04);
    for (int i = 0; i < 20000; ++i)
    {
      estimator.update(bias, accelAtRest(truth), magAt(truth), 0.002);
    }
    expectVecNear(estimator.gyroBias(), bias, 1e-6);
    EXPECT_LT(attitudeError(estimator.attitude(), truth), 1e-6);
  }

  TEST(AttitudeEstimatorTest, TracksMotionWithBiasAndWrongStart)
  {
    // A tumbling body with a biased gyro and an estimator that starts off by
    // 20 degrees converges to the truth and tracks it
    AttitudeEstimatorConfig<double> config;
    config.kp_accel = 2.0;
    config.kp_mag = 2.0;
    config.ki = 0.5;
    AttitudeEstimator<double> estimator(config);

    const double dt = 0.002;
    const Vec3<double> bias(0.02, -0.015, 0.01);
    Quaternion<double> truth = fromEuler(0.1, 0.2, -0.4);
    estimator.setAttitude(truth * Quaternion<double>::exp(Vec3<double>(0.2, -0.2, 0.15)));

    Vec3<double> rate(0.0, 0.0, 0.0);
    for (int i = 0; i < 40000; ++i)
    {
      const double t = i * dt;
      rate = Vec3<double>(0.4 * std::sin(0.7 * t), 0.3 * std::cos(0.5 * t),
                          0.2 * std::sin(0.3 * t + 1.0));
      estimator.update(rate + bias, accelAtRest(truth), magAt(truth), dt);
      truth = (truth * Quaternion<double>::exp(rate * dt)).normalized();
    }

    EXPECT_LT(attitudeError(estimator.attitude(), truth), 2e-3);
    expectVecNear(estimator.gyroBias(), bias, 2e-3);
    expectVecNear(estimator.angularRate(), rate, 2e-3);
    EXPECT_NEAR(estimator.attitude().norm(), 1.0, EPSILON);
  }

  TEST(AttitudeEstimatorTest, AngularRateWorld)
  {
    AttitudeEstimator<double> estimator;
    // Body rotated 90 degrees about z: the body x axis is the world y axis
    estimator.setAttitude(fromEuler(0.0, 0.0, PI / 2.0));
    estimator.predict(Vec3<double>(0.3, 0.0, 0.0), 1e-9);
    expectVecNear(estimator.angularRate(), Vec3<double>(0.3, 0.0, 0.0));
    expectVecNear(estimator.angularRateWorld(), Vec3<double>(0.0, 0.3, 0.0),
                  1e-8);
  }

  TEST(AttitudeEstimatorTest, EulerRatesWhenLevelEqualBodyRates)
  {
    AttitudeEstimator<double> estimator;
    estimator.setAttitude(Quaternion<double>());
    estimator.predict(Vec3<double>(0.1, -0.2, 0.3), 1e-9);
    expectVecNear(estimator.eulerRates(), Vec3<double>(0.1, -0.2, 0.3), 1e-8);
  }

  TEST(AttitudeEstimatorTest, EulerRatesMatchNumericalDerivative)
  {
    const double dt = 1e-6;
    const Vec3<double> rate(0.4, -0.3, 0.5);

    for (const double roll : {-1.0, 0.0, 0.6})
    {
      for (const double pitch : {-1.1, 0.0, 0.8})
      {
        for (const double yaw : {-2.0, 0.3})
        {
          AttitudeEstimator<double> estimator;
          estimator.setAttitude(fromEuler(roll, pitch, yaw));

          const EulerAngles<double> before = estimator.eulerAngles();
          estimator.predict(rate, dt);
          const EulerAngles<double> after = estimator.eulerAngles();
          const Vec3<double> euler_rates = estimator.eulerRates();

          EXPECT_NEAR(euler_rates.x, (after.roll - before.roll) / dt, 1e-4);
          EXPECT_NEAR(euler_rates.y, (after.pitch - before.pitch) / dt, 1e-4);
          EXPECT_NEAR(euler_rates.z, (after.yaw - before.yaw) / dt, 1e-4);
        }
      }
    }
  }

  TEST(AttitudeEstimatorTest, RateLowPassFilter)
  {
    const double cutoff = 5.0;
    const double dt = 0.0005;
    AttitudeEstimatorConfig<double> config;
    config.rate_cutoff_frequency = cutoff;
    AttitudeEstimator<double> estimator(config);
    estimator.setAttitude(Quaternion<double>());

    // The first sample initializes the filter
    estimator.predict(Vec3<double>(0.0, 0.0, 0.0), dt);
    expectVecNear(estimator.angularRate(), Vec3<double>(0.0, 0.0, 0.0));

    // Step response of a first order low-pass: 1 - exp(-t / tau)
    const double tau = 1.0 / (2.0 * PI * cutoff);
    const int steps = static_cast<int>(tau / dt);
    for (int i = 0; i < steps; ++i)
    {
      estimator.predict(Vec3<double>(1.0, 0.0, 0.0), dt);
    }
    EXPECT_NEAR(estimator.angularRate().x, 1.0 - std::exp(-steps * dt / tau),
                5e-3);

    for (int i = 0; i < 20 * steps; ++i)
    {
      estimator.predict(Vec3<double>(1.0, 0.0, 0.0), dt);
    }
    EXPECT_NEAR(estimator.angularRate().x, 1.0, 1e-6);

    // The attitude itself integrates the unfiltered rate
    EXPECT_NEAR(estimator.eulerAngles().roll, 21 * steps * dt, 1e-9);
  }

  TEST(AttitudeEstimatorTest, NonPositiveDtIsIgnored)
  {
    AttitudeEstimator<double> estimator;
    const Quaternion<double> start = fromEuler(0.1, 0.2, 0.3);
    estimator.setAttitude(start);

    const Vec3<double> gyro(1.0, 2.0, 3.0);
    const Vec3<double> accel(0.0, 0.0, GRAVITY);
    estimator.update(gyro, accel, 0.0);
    estimator.update(gyro, accel, -0.01);
    estimator.update(gyro, accel, accel, 0.0);
    estimator.predict(gyro, 0.0);
    estimator.predict(gyro, std::nan(""));

    EXPECT_NEAR(attitudeError(estimator.attitude(), start), 0.0, EPSILON);
    expectVecNear(estimator.angularRate(), Vec3<double>(0.0, 0.0, 0.0));
  }

  TEST(AttitudeEstimatorTest, ResetAndConfig)
  {
    AttitudeEstimatorConfig<double> config;
    config.kp_accel = 3.0;
    AttitudeEstimator<double> estimator(config);
    EXPECT_EQ(estimator.config().kp_accel, 3.0);

    estimator.update(Vec3<double>(0.1, 0.2, 0.3),
                     accelAtRest(fromEuler(0.5, 0.2, 0.0)), 0.01);
    estimator.setGyroBias(Vec3<double>(0.01, 0.02, 0.03));
    EXPECT_TRUE(estimator.isInitialized());

    estimator.reset();
    EXPECT_FALSE(estimator.isInitialized());
    EXPECT_NEAR(attitudeError(estimator.attitude(), Quaternion<double>()), 0.0,
                EPSILON);
    expectVecNear(estimator.gyroBias(), Vec3<double>(0.0, 0.0, 0.0));
    expectVecNear(estimator.angularRate(), Vec3<double>(0.0, 0.0, 0.0));
    // The configuration is kept
    EXPECT_EQ(estimator.config().kp_accel, 3.0);

    config.kp_accel = 0.5;
    estimator.setConfig(config);
    EXPECT_EQ(estimator.config().kp_accel, 0.5);
  }

  TEST(AttitudeEstimatorTest, SetAttitudeNormalizes)
  {
    AttitudeEstimator<double> estimator;
    estimator.setAttitude(Quaternion<double>(2.0, 0.0, 0.0, 0.0));
    EXPECT_NEAR(estimator.attitude().norm(), 1.0, EPSILON);
    EXPECT_TRUE(estimator.isInitialized());
  }

  TEST(AttitudeEstimatorTest, FloatInstantiation)
  {
    AttitudeEstimatorf estimator;
    const Quaternion<float> truth =
        EulerAngles<float>(0.3f, -0.2f, 0.0f).toQuaternion();
    const Vec3<float> accel =
        truth.conjugate().rotate(Vec3<float>(0.0f, 0.0f, 9.80665f));

    for (int i = 0; i < 2000; ++i)
    {
      estimator.update(Vec3<float>(0.0f, 0.0f, 0.0f), accel, 0.005f);
    }
    const EulerAngles<float> euler = estimator.eulerAngles();
    EXPECT_NEAR(euler.roll, 0.3f, 1e-4f);
    EXPECT_NEAR(euler.pitch, -0.2f, 1e-4f);
    EXPECT_NEAR(estimator.attitude().norm(), 1.0f, 1e-5f);
    EXPECT_NEAR(estimator.eulerRates().x, 0.0f, 1e-4f);
  }

} // namespace lumos
