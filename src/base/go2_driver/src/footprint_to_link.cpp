#include "rclcpp/rclcpp.hpp"
#include "unitree_go/msg/sport_mode_state.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include "tf2/LinearMath/Quaternion.h"
#include "geometry_msgs/msg/transform_stamped.hpp"

using namespace std::placeholders;
using namespace std::chrono_literals;

// 自定义节点类
class TFDynamicBroadcaster : public rclcpp::Node
{
public:
    TFDynamicBroadcaster() : Node("tf_dynamic_broadcaster")
    {   
        sub_ = this->create_subscription<unitree_go::msg::SportModeState>("/lf/sportmodestate", 10, std::bind(&TFDynamicBroadcaster::state_cb, this, _1));
        timer_ = this->create_wall_timer(100ms, std::bind(&TFDynamicBroadcaster::timer_cb, this));

        tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);

        RCLCPP_INFO(this->get_logger(), "启动动态tf变换节点");
    }

private:
    // 创建订阅者，订阅机器狗高层运动状态
    rclcpp::Subscription<unitree_go::msg::SportModeState>::SharedPtr sub_;
    // 创建定时器，以一定频率发布base_footprint到base_link的tf变换
    rclcpp::TimerBase::SharedPtr timer_;
    // tf广播器
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;

    // 机器狗身体高度
    double body_height_;

    // 订阅者回调函数
    void state_cb(const unitree_go::msg::SportModeState::SharedPtr state_msg)
    {
        // 创建对象
        body_height_ = state_msg->body_height + 0.057;          // 0.057从urdf文件中得知
    }

    // 定时器回调函数
    void timer_cb()
    {
        // 创建对象
        geometry_msgs::msg::TransformStamped transform_;
        // 组织消息
        transform_.header.stamp = this->get_clock()->now();
        transform_.header.frame_id = "base_footprint";
        transform_.child_frame_id = "base_link";

        // base_footprint到base_link的平移向量,只有z轴有值
        transform_.transform.translation.x = 0.0;
        transform_.transform.translation.y = 0.0;
        transform_.transform.translation.z = body_height_;

        // base_footprint到base_link的旋转向量,单位向量，没有值
        tf2::Quaternion qtn;
        qtn.setRPY(0.0, 0.0, 0.0);
        transform_.transform.rotation.x = qtn.x();
        transform_.transform.rotation.y = qtn.y();
        transform_.transform.rotation.z = qtn.z();
        transform_.transform.rotation.w = qtn.w();

        tf_broadcaster_->sendTransform(transform_);
    }
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TFDynamicBroadcaster>());
    rclcpp::shutdown();
    return 0;
}

