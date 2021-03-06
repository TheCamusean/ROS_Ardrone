#include "ros/ros.h"
#include "std_msgs/Empty.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "land_publisher_node");
  ros::NodeHandle n;
  ros::Rate loop_rate(10);

  ros::Publisher land_pub = n.advertise<std_msgs::Empty>("ardrone/land", 1000);

  ROS_INFO("-- Starting simple publisher node");

  while (ros::ok())
  {
    std_msgs::Empty msg;
		land_pub.publish(msg);

    ros::spinOnce();
    loop_rate.sleep();
  }

  return 0;
}
