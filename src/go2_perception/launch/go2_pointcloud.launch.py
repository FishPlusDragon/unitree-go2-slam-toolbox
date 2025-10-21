from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(
            package='go2_perception', executable='cloud_accumulation',
            remappings=[
                ('/utlidar/cloud_accumulated', '/trans_cloud')
                ],
            name='cloud_accumulation_node'
        ),
        Node(
            package='go2_perception', executable='pointcloud_to_laserscan_node',
            remappings=[
                ('cloud_in', '/trans_cloud'), 
                ('scan', '/scan')
                ],
            parameters=[{
                'target_frame': 'base_footprint',
                'transform_tolerance': 0.05,
                # 高度按需调整
                'min_height': 0.1,
                'max_height': 0.5,

                'angle_min': -3.14,  
                'angle_max': 3.14,  
                'angle_increment': 0.0087,
                'scan_time': 0.1,
                'range_min': 0.00,          
                'range_max': 10.0,
                'use_inf': True,
                'inf_epsilon': 1.0

            }],
            name='pointcloud_to_laserscan_node'
        )
    ])
