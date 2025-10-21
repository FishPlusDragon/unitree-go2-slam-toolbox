# Unitree go2入门项目

本项目适合于没有用过unitree go2的同学花费2个月左右做的难度适中的入门项目


## :bulb: 硬件设施
1. unitree go2 EDU版一台
2. 安装了ros2 humble的pc主机一台
3. 一根5到10米的网线
   

## :memo: 项目已完成的功能
1. rviz2中的模型可视化
2. 点云累积以及PointCloud2_to_Lascan消息
3. 可以使用ros2官方的键盘控制
4. imu融合odom
5. slam-toolbox建图
### :warning:项目未完成的工作
导航！！！！！！！


## :wrench: 开发环境配置
详见赵虚左老师的课程：https://www.bilibili.com/video/BV1vv5YzBEQH?spm_id_from=333.788.videopod.episodes&vd_source=4bd0448ccc277efab1a6915315abd6b9&p=5 <br>
或宇树官方：https://support.unitree.com/home/zh/developer/Quick_start <br>
建议看赵虚左老师的，这里不再赘述。


## :package: 安装环境
1. 安装robot_localization
```
sudo apt update

sudo apt install ros-humble-robot-localization
```

2. 安装slam-toolbox
```
sudo apt update

sudo apt install ros-humble-slam-toolbox
```

3. 还有什么缺的到时看编译的报错吧


## :rocket: 快速启动
