from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from ament_index_python.packages import get_package_share_directory
from launch.conditions import IfCondition
import os

def generate_launch_description():
    # 获取包路径
    get_packages = get_package_share_directory('go2_core')
    config_file = os.path.join(get_packages, 'config', 'go2_ekf_params.yaml')

    # EKF 节点
    go2_ekf = Node(
        package="robot_localization",
        executable="ekf_node",
        name="ekf_filter_node",
        output="screen",
        parameters=[config_file]
    )

    return LaunchDescription([
        go2_ekf,
    ])