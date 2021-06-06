
#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/Trajectory.h>
#include <cmath>
#include <iostream>

using namespace std;


geometry_msgs::PoseStamped current_pose;

void pose_cb(const geometry_msgs::PoseStamped::ConstPtr& msg){

  current_pose = *msg;
}



int main(int argc, char** argv){
  ros::init(argc, argv, "traj");

    
  ros::NodeHandle node;
  ros::Subscriber pose_sub = node.subscribe<geometry_msgs::PoseStamped>
            ("mavros/local_position/pose", 10, pose_cb);
  ros::Publisher traj_pub = node.advertise<mavros_msgs::Trajectory>
            ("/mavros/trajectory/generated", 10);

  ros::Rate rate(20.0);

  mavros_msgs::Trajectory trajectory;
  
  trajectory.header.seq = 0;
  trajectory.header.stamp = ros::Time::now();
  trajectory.header.frame_id = "local_origin";

  //trajectory.type = nan("");
  trajectory.point_1.header = trajectory.header;
  trajectory.point_1.type_mask = 4088;
  trajectory.point_1.position.x = 0.0;
  trajectory.point_1.position.y = 0.0;	
  trajectory.point_1.position.z = 0.0;	
  trajectory.point_valid[0] = 1;
  trajectory.command[0] = 17;
  trajectory.command[1] = 65535;
  trajectory.command[2] = 65535;
  trajectory.command[3] = 65535;
  trajectory.command[4] = 65535;



    while(ros::ok()){

	trajectory.header.seq++;
	trajectory.point_1.position.x = current_pose.pose.position.x;
  	trajectory.point_1.position.y = current_pose.pose.position.y;	
  	trajectory.point_1.position.z = current_pose.pose.position.z + 1.0;	
	traj_pub.publish(trajectory);

        ros::spinOnce();
        rate.sleep();
    }
  return 0;
};
