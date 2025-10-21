from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, Command
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
import os
from launch.conditions import IfCondition

def generate_launch_description():

    go2_desc_pkg = get_package_share_directory("go2_description")
    go2_driver_pkg = get_package_share_directory("go2_driver")

    #为 rviz2 启动添加开关
    use_rviz = DeclareLaunchArgument(
        name="use_rviz",
        default_value="false"      # 调试时使用，运行slam时关闭，在主程序中打开对应的rviz2
    )

    return LaunchDescription([
        use_rviz,
        # 机器人模型可视化
        IncludeLaunchDescription(
            launch_description_source = PythonLaunchDescriptionSource(
                launch_file_path=os.path.join(go2_desc_pkg, "launch", "display.launch.py")
            ),
            launch_arguments=[("use_joint_state_publisher", "false")]   #关节状态有driver节点发布，不需要使用默认
        ),
        # 包含 rviz2
        Node(
            package="rviz2",
            executable="rviz2",
            arguments=["-d", os.path.join(go2_driver_pkg, "rviz", "display.rviz")],
            condition=IfCondition(LaunchConfiguration("use_rviz"))
        ),
    
        # 速度消息桥接
        Node(
            package="go2_twist_bridge",
            executable="twist_bridge"
        ),

        # 添加base_link到base_footprint的动态坐标转换
        Node(
            package="go2_driver",
            executable="footprint_to_link"
        ),


        # 里程计消息发布、广播里程计坐标、发布关节状态信息
        Node(
            package="go2_driver",
            executable="driver",
            parameters=[os.path.join(go2_driver_pkg, "params", "driver.yaml")]
        ),

        # imu
        Node(
            package="go2_driver",
            executable="lowstate_to_imu"
        )
    ])


