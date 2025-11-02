#include "rclcpp/rclcpp.hpp"
// 发布里程计消息的头文件
#include "nav_msgs/msg/odometry.hpp"
#include "unitree_go/msg/sport_mode_state.hpp"
#include <geometry_msgs/msg/pose_stamped.hpp>
// 发布坐标变换的头文件
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2_ros/transform_broadcaster.h"
// 发布关节状态信息的头文件
#include "sensor_msgs/msg/joint_state.hpp"
#include "unitree_go/msg/low_state.hpp"

using namespace std::placeholders;

// 自定义节点类
class Driver : public rclcpp::Node
{
public:
    Driver() : Node("driver"), body_height_(0.30) 
    {
      RCLCPP_INFO(this->get_logger(), "Driver节点创建, 用于发布里程计消息，坐标变换和关节状态信息");

      // 声明参数
      // this->declare_parameter("publish_tf", true);

      // 获取参数
      // publish_tf = this->get_parameter("publish_tf").as_bool();

      //坐标变换广播器
      tf_bro_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);

      //运动状态订阅
      sub_ = this->create_subscription<unitree_go::msg::SportModeState>("/lf/sportmodestate", 10, std::bind(&Driver::state_cb, this, _1));


      odom_pub_ = this->create_publisher<nav_msgs::msg::Odometry>("odom", 10);
      robot_pose_sub_ = this->create_subscription<geometry_msgs::msg::PoseStamped>("/utlidar/robot_pose", 10, std::bind(&Driver::pose_callback, this, std::placeholders::_1));


      
      //关节状态发布
      joint_state_pub_ = this->create_publisher<sensor_msgs::msg::JointState>("joint_states", 10);
      low_state_sub_ = this->create_subscription<unitree_go::msg::LowState>("/lf/lowstate", 10, std::bind(&Driver::low_state_cb, this, std::placeholders::_1));
    }

private:
    // 发布关节状态信息
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr joint_state_pub_;
    // 订阅低层状态
    rclcpp::Subscription<unitree_go::msg::LowState>::SharedPtr low_state_sub_;
    // 广播坐标变换
    std::unique_ptr<tf2_ros::TransformBroadcaster> tf_bro_;
    // 订阅go2的状态
    rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr robot_pose_sub_;
    //发布里程计
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_;
    // 创建订阅者，订阅机器狗高层运动状态
    rclcpp::Subscription<unitree_go::msg::SportModeState>::SharedPtr sub_;
    
    // bool publish_tf, odom_published_;
    double body_height_;

    void state_cb(const unitree_go::msg::SportModeState::SharedPtr state_msg)
    {
        body_height_ = state_msg->body_height + 0.057 - 0.046825;   // 0.057: base_link在趴着的情况下的高度， -0.046825：雷达比base_link低这么多
    }

    //订阅低层信息获取关节状态， 组织消息并发布
    void low_state_cb(const unitree_go::msg::LowState::SharedPtr low_state)
    { 

      sensor_msgs::msg::JointState joint_state;

      //组织数据
      joint_state.header.stamp = this->now();          
      joint_state.name = {
        "FL_hip_joint", "FL_thigh_joint", "FL_calf_joint", 
        "FR_hip_joint", "FR_thigh_joint", "FR_calf_joint", 
        "RL_hip_joint", "RL_thigh_joint", "RL_calf_joint", 
        "RR_hip_joint", "RR_thigh_joint", "RR_calf_joint" 
      };

      //遍历低层状态信息中的关节数据
      for(size_t i = 0; i < 12; i++)
      {
        auto motor = low_state->motor_state[i];

        joint_state.position.push_back(motor.q);
      }

      joint_state_pub_->publish(joint_state);
    }

     void pose_callback(const geometry_msgs::msg::PoseStamped::SharedPtr msg) // 这里拿的是robot_pose， 是机器狗雷达的位置，所以需要进行一些换算变成base_footprint的位置
    {   
        rclcpp::Time now = this->now();
        
        geometry_msgs::msg::TransformStamped transform;
        transform.header.stamp = now;  
        transform.header.frame_id = "odom";
        transform.child_frame_id = "base_footprint";  
        transform.transform.translation.x = msg->pose.position.x - 0.28945;  
        transform.transform.translation.y = msg->pose.position.y;   
        transform.transform.translation.z = msg->pose.position.z - body_height_;  
        transform.transform.rotation.x = msg->pose.orientation.x;  
        transform.transform.rotation.y = msg->pose.orientation.y;  
        transform.transform.rotation.z = msg->pose.orientation.z;  
        transform.transform.rotation.w = msg->pose.orientation.w;  
        tf_bro_->sendTransform(transform);  

        nav_msgs::msg::Odometry odom;    
        odom.header.stamp = now;    
        odom.header.frame_id = "odom";    
        odom.child_frame_id = "base_footprint";    
        odom.pose.pose.position.x = transform.transform.translation.x;    
        odom.pose.pose.position.y = msg->pose.position.y;     
        odom.pose.pose.position.z = transform.transform.translation.z;    
        odom.pose.pose.orientation.x = msg->pose.orientation.x;    
        odom.pose.pose.orientation.y = msg->pose.orientation.y;    
        odom.pose.pose.orientation.z = msg->pose.orientation.z;    
        odom.pose.pose.orientation.w = msg->pose.orientation.w;    
        odom_pub_->publish(odom);   
    }
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Driver>());
    rclcpp::shutdown();
    return 0;
}




