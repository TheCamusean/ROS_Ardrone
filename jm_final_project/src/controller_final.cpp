#include <ros/ros.h>
#include <math.h>
#include "ardrone_autonomy/Navdata.h"
#include "ar_pose/ARMarker.h"
#include "std_msgs/String.h"
#include "std_msgs/Empty.h"
#include "std_msgs/Float64.h"
#include "geometry_msgs/Twist.h"
#include <tf/transform_listener.h>
#include "sensor_msgs/Range.h"
#include "std_msgs/Bool.h"
#include "jm_final_project/giro_toggle.h"

float toggle=0;

float x_current,y_current,z_current,z_orient;
float velx,vely,velz,angz;

float giro=0;
float readyforland=0;
float landonetime=0;

float x_old=0;
float y_old=0;
float desired_height;

float Kd=-0.03;

float dx,dy;

bool auto_mode;

ros::Publisher vel_pub;

void autocallback(const std_msgs::Bool & msgauto)
{
	auto_mode=msgauto.data;

}
void inputCallback(const jm_final_project::giro_toggle & msg)
{
	giro=msg.giro;
	toggle=msg.toggle;
}





void arCallback(const ar_pose::ARMarker & msg)
{
     if(auto_mode==0)
 {
	x_current=msg.pose.pose.position.x;
	y_current=msg.pose.pose.position.y;
	z_current=msg.pose.pose.position.z;

	// Convert quaternion to RPY
	tf::Quaternion qt;
	double roll,pitch,yaw;
	tf::quaternionMsgToTF(msg.pose.pose.orientation,qt);
	tf::Matrix3x3(qt).getRPY(roll,pitch,yaw);


 if(toggle==1 && giro==0)
 //Bottom cam, stabilizing according to the marker
	{

	// derivative part(0.01 is our sampling time)
	dx= (x_current-x_old)/0.01;
	dy= (y_current-y_old)/0.01;
	x_old =x_current;
	y_old =y_current;

	//Controller (Consideramos if para meter el DeadZone(0.01))
	if(y_current>0){velx=-0.1*y_current+Kd*dy-0.01;}
	else if(y_current<0){velx=-0.1*y_current+Kd*dy+0.01;}
	
	
	if(x_current>0){vely=-0.1*x_current+Kd*dx-0.01;}
	else if(x_current<0){vely=-0.1*x_current+Kd*dx+0.01;}
	

	if(z_current>1.1){velz=-0.1*z_current*z_current;}
	else if(z_current<1.3){velz=0.1*z_current*z_current;}
	

	if(yaw>0){angz=-0.8*yaw;}
	else if(yaw<0){angz=-0.8*yaw;}
	

	//Saturation
	if(velx>0.06){velx= 0.06;}
	else if(velx<-0.06){velx=-0.06;}

	if(vely>0.06){vely=0.06;}
	else if(vely<-0.06){vely=-0.06;}

	//display & send the message
	geometry_msgs::Twist velmsg;
	velmsg.linear.x=velx;
	velmsg.linear.y=vely;
	velmsg.linear.z=velz;
	velmsg.angular.z=angz;

	vel_pub.publish(velmsg);
	


	//ROS_INFO("STABILIZING:vx=%f,vy=%f,vz=%f,angz=%f,position:x=%f,y=%f,z=%f,yaw=%f , dx=%f,dy=%f",velx,vely,velz,angz,x_current, y_current,z_current,yaw,dx,dy);

	}


else if(toggle==0 && giro==2)
	//Front cam, going to the marker
	// FOR EXPERIMENTATION
	{
	//derivative
	dx= (x_current-x_old)/0.01;
	dy= (y_current-y_old)/0.01;
	x_old =x_current;
	y_old =y_current;


	//controller
	if(x_current>0){vely=-0.1*x_current+Kd*dx-0.01;}
	else if(x_current<0){vely=-0.1*x_current+Kd*dx-0.01;}
	

	if(y_current>0){velz=-0.05*y_current;}
	else if(y_current<0){velz=-0.05*y_current;}
	

	if(z_current<1.1){velx=-0.1*z_current;}
	else if(z_current>1.1){velx=0.1*z_current;}
	

	if(pitch>0){angz=-0.5*pitch;}
	else if(pitch<0){angz=-0.5*pitch;}
	


	// For the simulation this must be commented in order to do it faster
	//Saturation

	if(velx>0.1){velx= 0.1;}
	else if(velx<-0.1){velx=-0.1;}


	if(vely>0.08){vely=0.08;}
	else if(vely<-0.08){vely=-0.08;}

	if (velz>0.03){velz=0.03;}
	else if(velz<-0.03){velz=-0.03;}

	geometry_msgs::Twist velmsg;
	velmsg.linear.x=velx;
	velmsg.linear.y=vely;
	velmsg.linear.z=velz;
	velmsg.angular.z=angz;

	vel_pub.publish(velmsg);


	//ROS_INFO("Towards marker 2:vx=%f,vy=%f,vz=%f,angz=%f,position:x=%f,y=%f,z=%f,pitch=%f , dx=%f,dy=%f",velx,vely,velz,angz,x_current, y_current,z_current,pitch,dx,dy);

}


else if(toggle==1 && giro==1)
	//Bottom cam, stabilizing while spining
	{
	//derivative
	dx= (x_current-x_old)/0.01;
	dy= (y_current-y_old)/0.01;
	x_old =x_current;
	y_old =y_current;

	//controller
	if(y_current>0){velx=-0.1*y_current+Kd*dy-0.01;}
	else if(y_current<0){velx=-0.1*y_current+Kd*dy+0.01;}
	
	
	if(x_current>0){vely=-0.1*x_current+Kd*dx-0.01;}
	else if(x_current<0){vely=-0.1*x_current+Kd*dx+0.01;}

	
	if(z_current>1.1){velz=-0.1*z_current*z_current;}
	else if(z_current<1.1){velz=0.1*z_current*z_current;}
	
	// GIRO
	if(yaw>1.57+0.2){angz=-0.2;}
	else if(yaw<1.57-0.2){angz=0.2;}
	
	
	//Saturation
	if(velx>0.06){velx= 0.06;}
	else if(velx<-0.06){velx=-0.06;}

	if(vely>0.06){vely=0.06;}
	else if(vely<-0.06){vely=-0.06;}

	ROS_INFO("spinning");
	geometry_msgs::Twist velmsg;
	velmsg.linear.x=velx;
	velmsg.linear.y=vely;
	velmsg.linear.z=velz;
	velmsg.angular.z=angz;
	vel_pub.publish(velmsg);

    }



  }// del auto mode

}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "controller_final");
  ros::NodeHandle n;

  ros::Rate loop_rate(100);


  // velocidades y ar_pose
  	ros::Subscriber sub = n.subscribe("ar_pose_marker", 1, arCallback);
  	vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 1);

  // toggle & giro msg
	ros::Subscriber input_sub=n.subscribe("/giro_toggle",1,inputCallback);

  //auto
 	ros::Subscriber  auto_sus= n.subscribe("automatic",1,autocallback);



  while(ros::ok()){
	

	 ros::spinOnce();
    	loop_rate.sleep();
   }	//while
  }//int main
