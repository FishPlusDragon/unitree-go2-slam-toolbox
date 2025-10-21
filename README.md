# Unitree Go2 入门项目  

本项目适合零基础接触 Unitree Go2 的同学，耗时约2个月，难度适中，可帮助快速掌握机器狗 ROS2 开发基础。  


## 🛠️ 硬件准备  
1. Unitree Go2 EDU 版机器狗  
2. 安装 ROS2 Humble 的 PC 主机  
3. 5-10米网线（用于机器狗与PC连接）  


## ✅ 已实现功能  
- Rviz2 中机器狗模型可视化  
- 点云累积及 `PointCloud2_to_LaserScan` 消息转换  
- 支持 ROS2 官方键盘控制节点  
- 基于 IMU 融合里程计（odom）数据  
- 集成 slam-toolbox 实现建图功能  


## 🚧 待完成功能  
- 自主导航功能（核心待开发）  


## 🔧 开发环境配置  
推荐参考以下教程完成基础环境搭建：  
- 赵虚左老师课程：[ROS2 入门到实战](https://www.bilibili.com/video/BV1vv5YzBEQH?spm_id_from=333.788.videopod.episodes&vd_source=4bd0448ccc277efab1a6915315abd6b9&p=5)（更适合新手）  
- 宇树官方文档：[Go2 开发者快速入门](https://support.unitree.com/home/zh/developer/Quick_start)  


## 📦 依赖安装  
1. 安装机器人定位融合包  
   ```bash
   sudo apt update && sudo apt install ros-humble-robot-localization
   ```  

2. 安装建图工具  
   ```bash
   sudo apt update && sudo apt install ros-humble-slam-toolbox
   ```  

3. 其他依赖：编译时若提示缺少包，可根据报错信息用 `sudo apt install ros-humble-<缺失包名>` 安装  


## 🚀 快速启动步骤  

1. **创建并进入工作空间**  
   ```bash
   mkdir -p go2_ws_toolbox/src && cd go2_ws_toolbox/src
   ```  

2. **克隆仓库**  
   ```bash
   git clone git@github.com:FishPlusDragon/unitree-go2-slam-toolbox.git
   ```  

3. **编译工作空间**  
   ```bash
   cd .. && colcon build
   ```  

4. **启动功能**  
   - 加载环境变量  
     ```bash
     source install/setup.bash
     ```  
   - 启动 SLAM 建图（包含可视化）  
     ```bash
     ros2 launch go2_core go2_start.launch.py
     ```  
   - 新终端启动键盘控制（控制机器狗移动建图）  
     ```bash
     ros2 run teleop_twist_keyboard teleop_twist_keyboard
     ```  


> ⚠️ 注意：建图时建议将移动速度调至 0.3m/s 左右，步态选择“经典模式”以保证稳定性。


## 注意：
- 由于作者的学业压力，本项目不再维护，但如果有bug的以及有大佬调好了导航，请在issue中交流，让我学习学习

## 鸣谢：
- 感谢所有对我学习路上有帮助的人
