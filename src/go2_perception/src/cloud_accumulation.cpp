#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "vector"
#include "cstring"

using namespace std::chrono_literals;

// 自定义节点类
class CloudAccumulator : public rclcpp::Node
{
public:
    CloudAccumulator() : Node("cloud_accumulator")
    {   
        RCLCPP_INFO(this->get_logger(), "Cloud accumulator node started.");
        // 配置qos，确保数据能够传输，需要确认slam-toolbox用的是什么qos
        rclcpp::QoS qos = rclcpp::SensorDataQoS();
        qos.reliable();

        // 创建订阅方， 订阅机器狗的 /utlidar/cloud_deskewed 话题
        sub_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
            "/utlidar/cloud_deskewed",
            rclcpp::SensorDataQoS(),
            std::bind(&CloudAccumulator::cloud_callback, this, std::placeholders::_1)
        );

        // 创建发布方，发布机器狗的 /utlidar/cloud_accumulated 话题
        pub_ = this->create_publisher<sensor_msgs::msg::PointCloud2>(
            "/utlidar/cloud_accumulated",
            qos
        );

        // 创建定时器，定时发布数据
        timer_ = this->create_wall_timer(
            25ms,
            std::bind(&CloudAccumulator::timer_callback, this)
        );
    }

private:
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr sub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;

    std::vector<sensor_msgs::msg::PointCloud2::ConstSharedPtr> clouds_; // 创建存储点云的容器(注意点云数据是只读的，不能被修改)
    sensor_msgs::msg::PointCloud2 accumulated_cloud_; // 创建累积点云数据

    const size_t max_clouds_ = 25;  // 最多存储25帧点云数据
    const float min_height_ = 0.2f; // 最小高度阈值
    const float max_height_ = 0.8f; // 最大高度阈值


    // 回调函数（注意参数使用的是ConstSharedPtr类型, 防止点云信息被修改）
    void cloud_callback(const sensor_msgs::msg::PointCloud2::ConstSharedPtr cloud_msg)
    {
        clouds_.push_back(cloud_msg);

        // 如果点云的数量超过max_clouds_，则删除最老的点云数据
        if(clouds_.size() > max_clouds_)
        {
            clouds_.erase(clouds_.begin());
        }

        // 如果没有点云数据，则返回
        if(clouds_.empty())
        {
            return;
        }

        // 合并所有点云
        auto merged_cloud = merged_clouds();

        // 高度过滤
        auto filtered_cloud = filter_cloud(*merged_cloud);

        // 更新积累的点云
        accumulated_cloud_ = filtered_cloud;
        accumulated_cloud_.header.frame_id = "odom";    // 设置坐标系
    }

    // 合并多个点云的函数
    std::shared_ptr<sensor_msgs::msg::PointCloud2> merged_clouds()
    {   
        // std::shared_ptr<sensor_msgs::msg::PointCloud2>
        auto merged_cloud = std::make_shared<sensor_msgs::msg::PointCloud2>();

        *merged_cloud = *clouds_[0]; // 用第一个点云初始化 “合并后点云”  // *merged_cloud = *clouds_.front();

        // 遍历剩余点云，逐个合并到基础点云中
        for (size_t i = 1; i < clouds_.size(); i++)
        {
            // 1.累加总点数：合并后的点云的点数 = 已有点数 + 当前点云的点数
            merged_cloud->width += clouds_[i]->width;

            // 2.累加每行字节长度：合并后的点云的字节长度 = 已有点云的字节长度 + 当前点云的字节长度
            merged_cloud->row_step += clouds_[i]->row_step;

            // 3.拼接原始数据：把当前点云的data数组，追加到点云的data末尾
            merged_cloud->data.insert(merged_cloud->data.end(),
                    clouds_[i]->data.begin(), 
                    clouds_[i]->data.end()
                );
        }
        return merged_cloud;
    }

    // 高度过滤
    sensor_msgs::msg::PointCloud2 filter_cloud(const sensor_msgs::msg::PointCloud2 &cloud)
    {   
        // sensor_msgs::msg::PointCloud2
        sensor_msgs::msg::PointCloud2 filtered_cloud;
        // 复制头部信息和属性
        filtered_cloud.header = cloud.header;
        filtered_cloud.height = 1;  // 无序点云的 height 为1
        filtered_cloud.fields = cloud.fields;   // 字段定义（x/y/z等字段的偏移量、类型，完全复用）
        filtered_cloud.is_bigendian = cloud.is_bigendian;
        filtered_cloud.point_step = cloud.point_step;
        filtered_cloud.row_step = 0;
        filtered_cloud.is_dense = false; // 是否包含无效点（false=允许有无效点，更通用）

        // 遍历所有的点， 过滤出高度适合的点
        for (size_t i = 0; i < cloud.width * cloud.height; i++)
        {
            // 提取高度
            float z;
            memcpy(&z, &cloud.data[i * cloud.point_step + cloud.fields[2].offset], sizeof(float));

            // 只保留高度在 min_height_ 到 max_height_ 区间的点
            if(z >= min_height_ && z <= max_height_)
            {
                // 复制整个点的数据到过滤后点云
                filtered_cloud.data.insert(filtered_cloud.data.end(),
                        &cloud.data[i * cloud.point_step],  // 当前点的 “字节块起始地址”
                        &cloud.data[(i + 1) * cloud.point_step] // 当前点的 “字节块结束地址”（下一个点的起始地址）
                    );
                filtered_cloud.row_step += cloud.point_step;
                filtered_cloud.width++;
            }
        } 
        return filtered_cloud;
    }

    void timer_callback()
    {
        if(!accumulated_cloud_.data.empty())
        {
            // accumulated_cloud_.header.stamp = this->now();
            accumulated_cloud_.header.stamp = this->get_clock()->now();
            pub_->publish(accumulated_cloud_);
        }
    }
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<CloudAccumulator>());
    rclcpp::shutdown();
    return 0;
}