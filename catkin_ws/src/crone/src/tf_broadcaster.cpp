
// Publishes tf transformations map -> drone frame -> camera frame



#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <std_msgs/Float64.h>



geometry_msgs::PoseStamped current_pose;

void pose_cb(const geometry_msgs::PoseStamped::ConstPtr& msg){
  current_pose = *msg;
}

std_msgs::Float64 current_alt;

void alt_cb(const std_msgs::Float64::ConstPtr& msg){
  current_alt = *msg;
}

int main(int argc, char** argv){
  ros::init(argc, argv, "tf_broadcaster");

    
  ros::NodeHandle node;
  ros::Subscriber pose_sub = node.subscribe<geometry_msgs::PoseStamped>
            ("mavros/local_position/pose", 10, pose_cb);
  ros::Subscriber alt_sub = node.subscribe<std_msgs::Float64>
            ("mavros/global_position/rel_alt", 10, alt_cb); // Relative altitude is the z component of the drone position
  ros::Rate rate(10.0);

  //Initializing variables
  current_pose.pose.position.x = 0.0;
  current_pose.pose.position.y = 0.0;
  current_pose.pose.position.z = 0.0;
  current_pose.pose.orientation.x = 0.0;
  current_pose.pose.orientation.y = 0.0;
  current_pose.pose.orientation.z = 0.0;
  current_pose.pose.orientation.w = 1.0;
  current_alt.data = 0.0;

  static tf::TransformBroadcaster br1;
  tf::Transform transform1;


  static tf::TransformBroadcaster br;
  tf::Transform transform;
  transform.setOrigin( tf::Vector3(0.15, 0.0, 0.0) );
  // q: x, y, z, w
  // roll (X), pitch (Y), yaw (Z), from parent to child in order: yaw -> pitch -> roll
  tf::Quaternion q;
  //q.setRPY(1.5708, 0.0, -1.5708);
  q.setRPY(-1.5708, 0.0, -1.5708);
  transform.setRotation(q);

  ROS_INFO("Broadcasting tf transforms...");
  while(ros::ok()){

  transform1.setOrigin( tf::Vector3(current_pose.pose.position.x, current_pose.pose.position.y, current_pose.pose.position.z) );
  // transform1.setOrigin( tf::Vector3(current_pose.pose.position.x, current_pose.pose.position.y, current_alt.data) );
  // q: x, y, z, w
  // roll (X), pitch (Y), yaw (Z), from parent to child in order: yaw -> pitch -> roll
  transform1.setRotation(tf::Quaternion(current_pose.pose.orientation.x, current_pose.pose.orientation.y, current_pose.pose.orientation.z, current_pose.pose.orientation.w) );
  br1.sendTransform(tf::StampedTransform(transform1, ros::Time::now(), "map", "base_link"));

  br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "base_link", "camera_link"));
  //FOR REALSENSE D435:
  br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "base_link", "camera_depth_optical_frame"));
  br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "base_link", "camera_color_optical_frame"));

  ros::spinOnce();
  rate.sleep();
}
  return 0;
};
