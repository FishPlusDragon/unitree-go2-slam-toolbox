from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():

    # 获取slam_toolbox包路径
    get_slam_toolbox_pkg = get_package_share_directory("slam_toolbox")
    # 配置文件的路径
    slam_toolbox_config = os.path.join(
        get_package_share_directory("go2_slam"),
        "config",
        "mapper_params_online_async.yaml"
        )
    #包含slam_toolbox的launch文件
    slam_toolbox_launch = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            launch_file_path=os.path.join(
                get_slam_toolbox_pkg,
                "launch",
                "online_async_launch.py"
            )
        ),launch_arguments=[("slam_params_file", slam_toolbox_config), ("use_sim_time", "false")]       
    )

    return LaunchDescription([
        slam_toolbox_launch
    ])