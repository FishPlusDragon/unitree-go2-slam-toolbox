from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, Command
from ament_index_python.packages import get_package_share_directory
from launch_ros.parameter_descriptions import ParameterValue
import os
from launch.conditions import IfCondition

def generate_launch_description():
    #获取功能包路径
    go2_description_pkg = get_package_share_directory("go2_description")
    use_joint_state_publisher = DeclareLaunchArgument(
        name="use_joint_state_publisher",
        default_value="false"
    )
    model = DeclareLaunchArgument(
        name="urdf_path",
        default_value=os.path.join(go2_description_pkg, "urdf", "go2_description.urdf")
    )

    robot_desc = ParameterValue(Command(["xacro ", LaunchConfiguration("urdf_path")]))

    # 加载机器人的urdf文件
    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        parameters=[{"robot_description":robot_desc}]
    )
    # 发布关节状态
    joint_state_publisher = Node(
        package="joint_state_publisher",
        executable="joint_state_publisher",
        condition=IfCondition(LaunchConfiguration("use_joint_state_publisher")))

    return LaunchDescription([
        model,
        use_joint_state_publisher,
        robot_state_publisher,
        joint_state_publisher])
