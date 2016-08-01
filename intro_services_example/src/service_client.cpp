#include "ros/ros.h"
#include "intro_services_example/AddTwoInts.h"
#include <cstdlib>

 int main(int argc, char **argv)
{

ros::init(argc, argv, "service_client");
if (argc != 3)
{
   ROS_INFO("usage: service_client X Y");
   return 1;
}




ros::NodeHandle n;

ros::ServiceClient client = n.serviceClient<intro_services_example::AddTwoInts>("add_two_ints");

  intro_services_example::AddTwoInts srv;
  srv.request.a = atoll(argv[1]);
  srv.request.b = atoll(argv[2]);


  if (client.call(srv)) {
    ROS_INFO("Sum: %ld", (long int)srv.response.sum);
  }
  else {
    ROS_ERROR("Failed to call service add_two_ints");
    return 1;
  }
  return 0;
}

