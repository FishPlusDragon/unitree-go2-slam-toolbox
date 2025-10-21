from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.actions import IncludeLaunchDescription, ExecuteProcess, RegisterEventHandler
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
from launch.event_handlers import OnProcessStart
from launch.substitutions import LaunchConfiguration, Command
from launch.conditions import IfCondition
import os
import launch

def generate_launch_description():
    get_nav2_pkg = get_package_share_directory("go2_navigation2")
    get_bringup_pkg = get_package_share_directory("nav2_bringup")
    go2_description_pkg = get_package_share_directory("go2_description")
    go2_core_pkg = get_package_share_directory("go2_core")

    use_sim_time = launch.substitutions.LaunchConfiguration('use_sim_time', default='false')
    # map_yaml_path = launch.substitutions.LaunchConfiguration(
    #     'map', default=os.path.join(get_nav2_pkg, 'maps', '01map.yaml'))
    map_yaml_path = launch.substitutions.LaunchConfiguration(
        'map', default=os.path.join('4floor_first_map.yaml'))
    nav2_param_path = launch.substitutions.LaunchConfiguration(
        'params_file', default=os.path.join(get_nav2_pkg, 'config', 'nav2_params.yaml'))
    rviz_config_dir = os.path.join(get_bringup_pkg, 'rviz', 'nav2_default_view.rviz')

    # 包含nav2的launch文件
    nav2_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(get_bringup_pkg, "launch", "navigation_launch.py")),
        launch_arguments=[("params_file", nav2_param_path), ("use_sim_time", use_sim_time), ("map", map_yaml_path)]
    )

    # --- map_server ---
    map_server = Node(
        package='nav2_map_server',
        executable='map_server',
        name='map_server',
        output='screen',
        parameters=[{
            'yaml_filename': map_yaml_path,
            'use_sim_time': use_sim_time,
            'topic_name': 'map'
        }]
    )

    # --- AMCL ---
    amcl = Node(
        package='nav2_amcl',
        executable='amcl',
        name='amcl',
        output='screen',
        parameters=[nav2_param_path, {'use_sim_time': use_sim_time}]
    )

    # --- lifecycle_manager ---
    lifecycle_manager = Node(
        package='nav2_lifecycle_manager',
        executable='lifecycle_manager',
        name='lifecycle_manager_navigation',
        output='screen',
        parameters=[{
            'use_sim_time': use_sim_time,
            'autostart': True,
            'node_names': ['map_server', 'amcl']
        }]
    )

    # 里程计融合imu
    go2_robot_localization = IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                os.path.join(go2_core_pkg, "launch", "go2_robot_localization.launch.py")
            )
        )
    
    # --- RViz2 ---
    rviz2 = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', rviz_config_dir],
        parameters=[{'use_sim_time': use_sim_time}],
        output='screen'
    )

    # --- 驱动 ---
    go2_driver = Node(
        package="go2_driver",
        executable="driver"
    )

    # 包含scan话题
    cloud_launch = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            launch_file_path=os.path.join(
                get_package_share_directory("go2_perception"),
                "launch",
                "go2_pointcloud.launch.py",
            )
        )
    )

        # 包含模型可视化
    go2_display_launch = IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                os.path.join(go2_description_pkg, "launch", "display.launch.py")
            ),
            launch_arguments=[("use_joint_state_publisher", "false")] 
            # condition=IfCondition(LaunchConfiguration('use_display'))
        )

    return LaunchDescription([
        go2_driver,
        map_server,
        amcl,
        lifecycle_manager,
        nav2_launch,
        # use_imu_tf_arg,
        # imu_tf,
        go2_robot_localization,
        rviz2,
        cloud_launch,
        go2_display_launch
    ])