cmake_minimum_required(VERSION 3.8)
project(go2_driver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(unitree_go REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)




add_executable(driver src/driver.cpp)
add_executable(footprint_to_link src/footprint_to_link.cpp)
add_executable(lowstate_to_imu src/lowstate_to_imu.cpp)

target_include_directories(driver PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(driver PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
ament_target_dependencies(
  driver
  "rclcpp"
  "unitree_go"
  "nav_msgs"
  "tf2"
  "tf2_ros"
  "geometry_msgs"
  "sensor_msgs"
)

ament_target_dependencies(
  footprint_to_link
  "rclcpp"
  "unitree_go"
  "nav_msgs"
  "tf2"
  "tf2_ros"
  "geometry_msgs"
  "sensor_msgs"
)

ament_target_dependencies(
  lowstate_to_imu
  "rclcpp"
  "unitree_go"
  "nav_msgs"
  "tf2"
  "tf2_ros"
  "geometry_msgs"
  "sensor_msgs"
)

install(TARGETS 
  driver
  footprint_to_link
  lowstate_to_imu
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch params rviz DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
