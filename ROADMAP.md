# LumosAlgo Roadmap

## Vision

LumosAlgo is intended to be the de facto, go-to standard library for robotics
and embedded systems: what the C++ standard library is for general
programming, LumosAlgo should be for robotics and embedded work. It is a
library you can adopt to iterate quickly and bring up functionality fast,
without first having to write or integrate the math, estimation, control and
signal processing underneath.

## Design principles

- **Portable.** The library builds and behaves the same across platforms, from
  microcontrollers to desktop and server targets, and across compilers.
  Nothing in the core may depend on a specific OS, architecture or toolchain.
- **Portable baseline, optional acceleration.** Every algorithm has a plain
  C++ implementation that works everywhere. Some algorithms may additionally
  have SIMD (or other platform-specific) variants, selected through
  compile-time flags. An accelerated variant must give the same results as the
  baseline, within floating point tolerance, and the library must be fully
  usable with all acceleration turned off.
- **Embedded-ready.** Code meant for embedded targets works on the fixed-size
  types (`FixedSizeMatrix`, `Vec2`/`Vec3`/`Vec4`), with no heap allocation and
  no exceptions.
- **Wide type coverage.** Algorithms are templated on the scalar type and work
  for every type that makes sense for them (`float` and `double` throughout,
  and integer or fixed-point types where applicable), without being tied to
  one precision.
- **Interoperable, consistent types.** The library's types work together:
  any algorithm that takes or returns a vector, matrix, rotation or pose uses
  the library's own types for it, and conversions exist between related types
  (fixed and dynamic size, the rotation representations, different scalar
  types). Naming, member access, type aliases and conventions are the same
  across modules, so that knowing one module carries over to the others.
- **Quick to adopt.** Sensible defaults, consistent naming and interfaces
  across modules, and minimal setup, so that a new project gets working
  functionality with little code.
- **Broad and coherent.** The library aims to cover the common needs of the
  field in one place, with modules that build on each other rather than a
  collection of unrelated parts.

## Roadmap

Candidate algorithms and functions, grouped by area. The sections are ordered
roughly by dependency: later sections build on earlier ones.

Legend: `[x]` implemented, `[ ]` not yet implemented.

## Next up

1. **Linear algebra foundations (section 1), first pass.** This is the main
   blocker: Kalman filters, LQR, MPC, least-squares fitting, inverse
   kinematics and the nonlinear optimisers all need a solver or a
   decomposition, and none exists yet.
2. **Kalman filter (section 3) and PID (section 4).** Short follow-ups once
   the first pass of section 1 is in place.
3. **Platform support (section 12).** Mainly removing the OS dependency that
   `logging.h` brings into `FixedSizeMatrix`. Move this ahead of step 2 if a
   microcontroller build is needed soon.

## 0. Type consistency and interoperability

Work that brings the existing code in line with the "wide type coverage" and
"interoperable, consistent types" principles.

- [x] Fixed-size types return fixed-size results:
      `AxisAngle::toRotationMatrix()` and `Vec3::toCrossProductMatrix()` now
      return `FixedSizeMatrix<T, 3, 3>`, like `Quaternion` and `EulerAngles`
- [x] Conversions between `FixedSizeMatrix` and the dynamic `Matrix`, and
      between `Vec2`/`Vec3`/`Vec4` and the dynamic `Vector`
      (`lin_alg/conversions.h`)
- [x] Consistent member naming: `Quaternion` members are `w`, `x`, `y`, `z`,
      like the other types (previously `w_`, `x_`, `y_`, `z_`)
- [x] Arithmetic operators for `FixedSizeMatrix`, matching the dynamic
      `Matrix`: `+`, `-`, scalar `*` and `/`, element-wise `^` and `/`, unary
      `-`, plus `==` and `!=`
- [x] `FixedSizeMatrix::transposed()` returns the swapped dimensions (it was
      wrong for non-square matrices)
- [x] Complete set of rotation conversions between quaternion, rotation
      matrix, Euler angles (all six rotation orders) and axis-angle
- [x] Filters accept and return the library's `Vector<T>` as well as
      `std::vector<T>`
- [x] One alias style (`using`), each alias defined once, and the previously
      commented out aliases in `pre_defs.h` enabled
- [x] `Quaternion` is a plain template like the other types (no explicit
      instantiation for `float` and `double` only)
- [x] Converting constructors between scalar types for `Quaternion`,
      `AxisAngle`, `EulerAngles`, `SE2`, `SE3` and the twists
- [ ] `FixedSizeVector` is declared outside the `lumos` namespace, and
      overlaps with `Vec2`/`Vec3`/`Vec4` and `FixedSizeMatrix<T, N, 1>`
- [ ] Filters and curves store their data in `std::vector`/`std::deque`
      internally; fixed-size variants are needed for heap-free use
- [ ] `math.h` does not include the geometry, image and structures modules
      (the includes are commented out)
- [ ] `FixedSizeMatrix` products with the dynamic `Vector` (commented out in
      `matrix_fixed.h`; convert with `lin_alg/conversions.h` for now)
- [ ] Integer scalar types: several functions use floating point literals or
      `std::sqrt` and are untested for integer `T`

## 1. Linear algebra foundations

All on `FixedSizeMatrix`, with no heap allocation.

First pass, in this order:

- [ ] Closed-form 2x2, 3x3 and 4x4 inverse and determinant
- [ ] Cholesky (LLT/LDLT) with a solve, for symmetric positive definite
      matrices such as covariances and cost matrices
- [ ] LU with partial pivoting, with solve, inverse and determinant for
      general square matrices of any fixed size
- [ ] QR (Householder) with a least-squares solve

Later:

- [ ] SVD and pseudo-inverse
- [ ] Symmetric eigenvalue decomposition
- [ ] Matrix exponential (needed for zero-order-hold discretisation)

## 2. Transformations

- [x] Rotation matrix conversions: quaternion to and from rotation matrix
      (`transformations/quaternion.h`)
- [x] SE(3) pose: compose, invert, transform a point, homogeneous 4x4 matrix
      (`transformations/se3.h`)
- [x] SE(2) pose: compose, invert, transform a point, homogeneous 3x3 matrix
      (`transformations/se2.h`)
- [x] Lie group operations: hat/vee, exp/log maps for SO(3), SE(3) and SE(2),
      SO(3) left Jacobian and its inverse (`transformations/so3.h`)
- [x] Quaternion: axis-angle construction, rotation of a vector, inverse,
      exp/log, slerp (`transformations/quaternion.h`)
- [x] Pose interpolation for SE(3) and SE(2)
- [x] Angle utilities: wrapping, shortest angular distance, angle
      interpolation, degree/radian conversion (`transformations/angles.h`)
- [x] Rotation matrix and quaternion to Euler angles, for every
      `RotationOrder` (`transformations/euler_angles.h`)
- [x] Fixed-size `AxisAngle`, with conversions to and from quaternion and
      rotation matrix (`transformations/axis_angle.h`)
- [ ] Adjoint of SE(3), and the SE(3) left Jacobian
- [ ] Re-orthonormalisation of a drifted rotation matrix

## 3. State estimation

- [ ] Kalman filters: linear, extended, unscented, error-state
- [ ] Attitude filters: complementary, Madgwick, Mahony
- [ ] Simple estimators: alpha-beta filter, recursive least squares
- [ ] Sensor fusion helpers: IMU preintegration, covariance propagation

## 4. Control

- [ ] PID with anti-windup, derivative filtering and feed-forward
- [ ] LQR with a discrete Riccati solver, state-space discretisation
- [ ] Lead/lag compensators, rate limiters, deadband, saturation
- [ ] Trajectory tracking: pure pursuit, Stanley

## 5. Signal processing (extends `filters/`)

- [x] FIR and IIR filter classes with basic first and second order designs,
      including notch (`filters/fir_filter.h`, `filters/iir_filter.h`)
- [x] IIR design: Butterworth low-pass/high-pass, Chebyshev type I low-pass
- [x] IIR analysis: stability check, poles and zeros
- [ ] IIR design: band-pass and band-stop Butterworth/Chebyshev, Chebyshev
      high-pass
- [ ] Biquad cascade (second order sections) filter class, which is
      numerically better than the direct form for orders above about 6
- [ ] FIR design with Hamming/Hann/Blackman windows (currently rectangular)
- [ ] Fixed-size, heap-free filter variants for embedded targets
- [ ] Streaming filters: moving average, median, exponential smoothing
- [ ] Spectral: FFT, Goertzel, window functions
- [ ] Numerical differentiation (Savitzky-Golay) and integration (trapezoid,
      RK4)

## 6. Trajectories and planning (extends `curves/`)

- [ ] Profiles: trapezoidal and S-curve velocity, cubic and minimum-jerk
      polynomials
- [ ] Splines: cubic and Catmull-Rom, with arc-length parameterisation
- [ ] Kinematics: forward and inverse, Jacobians, differential-drive and
      Ackermann models

### Path and motion planning

Environment representation and collision checking:

- [ ] Planning spaces: 2D and 3D occupancy grids, costmaps with obstacle
      inflation, graphs and roadmaps, configuration spaces for manipulators
- [ ] Collision checking: point, circle and polygon footprints against a grid,
      swept-path checks, signed distance fields
- [ ] A common planner interface (start, goal, validity check, cost), so that
      planners are interchangeable

Graph and grid search:

- [ ] Dijkstra, A*, weighted A*, bidirectional search
- [ ] Any-angle search: Theta*, lazy Theta*
- [ ] Jump point search for uniform grids
- [ ] Incremental replanning: D* Lite, LPA*
- [ ] Anytime search: ARA*
- [ ] Heuristics: Euclidean, octile, precomputed and non-holonomic heuristics
- [ ] A heap-free priority queue with a fixed capacity

Sampling-based planning:

- [ ] RRT, RRT-Connect, RRT*
- [ ] Informed RRT*, BIT*
- [ ] PRM and lazy PRM
- [ ] Sampling: uniform, Gaussian, goal-biased, and low-discrepancy (Halton,
      Sobol) sequences

Kinodynamic and non-holonomic planning:

- [ ] Dubins and Reeds-Shepp paths
- [ ] Hybrid A*
- [ ] State lattice planning with motion primitives
- [ ] Clothoids and continuous-curvature paths

Local planning and obstacle avoidance:

- [ ] Dynamic window approach
- [ ] Timed elastic band
- [ ] Artificial potential fields
- [ ] Vector field histogram
- [ ] Velocity obstacles and ORCA for moving obstacles and multiple robots

Optimisation-based planning (builds on section 8):

- [ ] CHOMP and STOMP
- [ ] Minimum-snap and minimum-jerk trajectories through waypoints
- [ ] Trajectory optimisation through convex corridors

Path post-processing:

- [ ] Shortcutting and path simplification (Douglas-Peucker)
- [ ] Smoothing: spline fitting, gradient-based smoothing
- [ ] Time parameterisation under velocity, acceleration and jerk limits
      (TOPP-RA)
- [ ] Path utilities: arc length, curvature, resampling, closest point and
      projection onto a path

Coverage, exploration and task-level planning:

- [ ] Coverage planning: boustrophedon decomposition, spiral coverage
- [ ] Frontier-based exploration
- [ ] Waypoint ordering: travelling salesman heuristics
- [ ] Multi-robot path finding: conflict-based search, prioritised planning

Manipulator planning:

- [ ] Joint-space and Cartesian-space interpolation
- [ ] Inverse kinematics: damped least squares, Jacobian transpose, null-space
      projection for redundant arms
- [ ] Joint limit and self-collision handling

## 7. Geometry and perception (extends `geometry/`)

- [ ] Primitives: AABB/OBB, sphere, ray, polygon, with intersection and
      distance queries
- [ ] Algorithms: convex hull, point-in-polygon, KD-tree or grid nearest
      neighbour
- [ ] Fitting: line, plane and circle by least squares, RANSAC
- [ ] Registration: ICP, Kabsch/Umeyama alignment
- [ ] Camera: pinhole projection and distortion models

## 8. Optimisation

- [ ] Gauss-Newton and Levenberg-Marquardt
- [ ] Gradient descent, 1D root finding (Newton, bisection, Brent)
- [ ] Small QP solver (for MPC)

## 9. Embedded utilities

- [ ] Containers: ring buffer, fixed-capacity vector, static queue (no heap)
- [ ] Numerics: fixed-point (Q-format) type, fast trig/atan2/inverse-sqrt,
      lookup tables with interpolation
- [ ] Integrity: CRC, checksums, COBS framing
- [ ] Statistics: running mean and variance, online min/max

## 10. Time and scheduling

- [ ] Timestamp and duration types, independent of the platform clock
- [ ] Rate keepers and periodic loop timing
- [ ] Timeouts and watchdog helpers
- [ ] Timestamped ring buffers with interpolation between samples

## 11. Units and safety

- [ ] Compile-time unit types (length, angle, time, velocity and so on)
- [ ] Bounded and saturating arithmetic, range-checked types
- [ ] Fault and health state machines

## 12. Platform support

- [ ] Hardware abstraction for time and logging, so that the core has no OS
      dependency (`logging.h` currently needs `<sys/time.h>`, `<thread>` and
      `<mutex>`)
- [ ] Build configuration flags for optional features and acceleration
- [ ] SIMD backends, selected at compile time, with the plain C++
      implementation as the reference
- [ ] Bare-metal profile: no heap, no exceptions, no RTTI
- [ ] Portable constants in place of the non-standard `M_PI`
- [ ] Continuous integration across compilers and targets

## 13. Probability and statistics

- [ ] Gaussian types with mean and covariance
- [ ] Mahalanobis distance and chi-square gating
- [ ] Random number generation that is reproducible across platforms
- [ ] Sampling from common distributions

## 14. Sensor models and calibration

- [ ] IMU bias and scale models
- [ ] Magnetometer hard-iron and soft-iron calibration
- [ ] Camera intrinsic and extrinsic calibration
- [ ] Hand-eye calibration
- [ ] Allan variance

## 15. Navigation and geodesy

- [ ] WGS84 to and from ECEF
- [ ] Local frames: ENU and NED conversions
- [ ] Great-circle distance and bearing
- [ ] Magnetic declination
- [ ] Dead reckoning

## 16. Mapping and localisation

- [ ] Occupancy grids and distance transforms
- [ ] Scan matching
- [ ] Particle filter localisation
- [ ] Pose-graph optimisation

## 17. Dynamics and simulation

- [ ] Rigid-body dynamics and inertia tensors
- [ ] ODE integrators
- [ ] Simple vehicle and motor models for testing controllers

## 18. Communication and serialisation

- [ ] Fixed-layout message packing and endianness helpers
- [ ] Framing: COBS, SLIP
- [ ] Checksums
- [ ] A small publish/subscribe bus

## 19. Image and point-cloud processing (extends `image/`)

- [ ] Images: convolution, gradients, feature detection
- [ ] Point clouds: voxel downsampling, normal estimation
