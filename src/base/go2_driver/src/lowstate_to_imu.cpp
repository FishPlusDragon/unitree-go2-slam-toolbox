#include "rclcpp/rclcpp.hpp"
#include "unitree_go/msg/low_state.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include "tf2/LinearMath/Quaternion.h"
#include "geometry_msgs/msg/transform_stamped.hpp"

class LowStateToImuNode : public rclcpp::Node
{
public:
    LowStateToImuNode() : Node("lowstate_to_imu_node")
    {
        // 1. 订阅 LowState 消息
        lowstate_sub_ = this->create_subscription<unitree_go::msg::LowState>(
            "/lf/lowstate", 10, 
            std::bind(&LowStateToImuNode::lowstate_callback, this, std::placeholders::_1));
        
        // 2. 发布标准 Imu 话题
        imu_pub_ = this->create_publisher<sensor_msgs::msg::Imu>("/imu", 10);
        
        // 3. 创建 TF 发布器
        tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        
        RCLCPP_INFO(this->get_logger(), "IMU 转换节点已启动");
    }

private:
    // 低层状态信息订阅
    rclcpp::Subscription<unitree_go::msg::LowState>::SharedPtr lowstate_sub_;
    // IMU 信息发布
    rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr imu_pub_;
    // TF 广播器
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;

    // 从低层状态信息中提取IMU信息
    void lowstate_callback(const unitree_go::msg::LowState::SharedPtr lowstate_msg)
    {
        // 创建 IMU 消息
        auto imu_msg = std::make_unique<sensor_msgs::msg::Imu>();
        
        imu_msg->header.stamp = this->now();
        imu_msg->header.frame_id = "imu"; 

        // 四元数转换（Unitree 格式 [w,x,y,z] → ROS [x,y,z,w]）
        const auto& quat = lowstate_msg->imu_state.quaternion;
        imu_msg->orientation.x = quat[1];  
        imu_msg->orientation.y = quat[2];  
        imu_msg->orientation.z = quat[3];  
        imu_msg->orientation.w = quat[0];  

        // 角速度（单位：rad/s）
        imu_msg->angular_velocity.x = lowstate_msg->imu_state.gyroscope[0];
        imu_msg->angular_velocity.y = lowstate_msg->imu_state.gyroscope[1];
        imu_msg->angular_velocity.z = lowstate_msg->imu_state.gyroscope[2];
        
        // 加速度
        imu_msg->linear_acceleration.x = lowstate_msg->imu_state.accelerometer[0];
        imu_msg->linear_acceleration.y = lowstate_msg->imu_state.accelerometer[1];
        imu_msg->linear_acceleration.z = lowstate_msg->imu_state.accelerometer[2];

        // 协方差矩阵
        // 角速度协方差
        imu_msg->angular_velocity_covariance[0] = 0.0002;  // xx
        imu_msg->angular_velocity_covariance[4] = 0.0002;  // yy
        imu_msg->angular_velocity_covariance[8] = 0.0002;  // zz
        
        // 线性加速度协方差
        imu_msg->linear_acceleration_covariance[0] = 0.02;  // xx
        imu_msg->linear_acceleration_covariance[4] = 0.02;  // yy
        imu_msg->linear_acceleration_covariance[8] = 0.02;  // zz
        
        // 方向协方差（-1表示未知）
        imu_msg->orientation_covariance[0] = -1.0;

        // 发布 IMU 消息
        imu_pub_->publish(std::move(imu_msg));

        // 发布 TF 变换
        publish_tf_transform();
    }

    // 发布 TF 变换
    void publish_tf_transform()
    {
        geometry_msgs::msg::TransformStamped tf;
        
        tf.header.stamp = this->now();
        tf.header.frame_id = "base_link";    
        tf.child_frame_id = "imu";          
        
        // IMU相对于base_link的精确位置（从urdf文件中获取）
        tf.transform.translation.x = -0.02557;  // 前向偏移2.557cm
        tf.transform.translation.y = 0.0;
        tf.transform.translation.z = 0.04232;   // 向上偏移4.232cm
        
        // 直接设置四元数
        tf.transform.rotation.x = 0.0;
        tf.transform.rotation.y = 0.0;
        tf.transform.rotation.z = 0.0;
        tf.transform.rotation.w = 1.0;  // 单位四元数，无旋转
        
        // 发送TF变换
        tf_broadcaster_->sendTransform(tf);
    }
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<LowStateToImuNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}