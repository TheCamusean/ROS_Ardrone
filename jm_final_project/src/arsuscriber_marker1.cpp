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

float toggle=0;
float sonarH;
float pitch2=10000;
float x_current,y_current,z_current,z_orient;
float velx,vely,velz,angz;
float giro=0;
float readyforland=0;
float landonetime=0;
float toggle42=0;
float x_old=0;
float y_old=0;
float desired_height;

float Kd=-0.03;
float Ki = 0.01;

float x_old2=1000;
float z_old2=1000;
float dx,dy,dz2,dx2;
float sumz , sumx;
bool auto_mode;


ros::Publisher vel_pub;

void autocallback(const std_msgs::Bool & msgauto)
{
	auto_mode=msgauto.data;

}
void tog(const std_msgs::Float64 & msg)
{
	if (toggle==0){toggle=1;}
	else{toggle=0;}
}

void sonarfunct(const ardrone_autonomy::Navdata & msg)
{
	sonarH = msg.altd;
	//ROS_INFO("sonar=%i",sonarH);
}

void girico(const std_msgs::Float64 & msg)
{
giro=msg.data;
ROS_INFO("giro=%f",giro);
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

	// derivative part
	dx= (x_current-x_old)/0.01;
	dy= (y_current-y_old)/0.01;
	x_old =x_current;
	y_old =y_current;

	//Controller
	if(y_current>0){velx=-0.1*y_current+Kd*dy-0.01;}
	else if(y_current<0){velx=-0.1*y_current+Kd*dy+0.01;}
	else{velx=0;}
	//ROS_INFO("VELX = %f, y =%f , dy=%f ,Kd=%f",velx,y_current,dy,Kd);

	if(x_current>0){vely=-0.1*x_current+Kd*dx-0.01;}
	else if(x_current<0){vely=-0.1*x_current+Kd*dx+0.01;}
	else{vely=0;}

	if(z_current>1.1){velz=-0.1*z_current*z_current;}
	else if(z_current<1.3){velz=0.1*z_current*z_current;}
	else{velz=0;}

	if(yaw>0){angz=-0.8*yaw;}
	else if(yaw<0){angz=-0.8*yaw;}
	else{angz=0;}

	//Saturation
	if(velx>0.06){velx= 0.06;}
	else if(velx<-0.06){velx=-0.06;}

	if(vely>0.06){vely=0.06;}
	else if(vely<-0.06){vely=-0.06;}

	//display & send the message
	geometry_msgs::Twist velmsg;
	velmsg.linear.x=velx;
	velmsg.linear.y=vely;
	//velmsg.linear.z=velz;
	velmsg.angular.z=angz;

	vel_pub.publish(velmsg);
	pitch2=1000;
	desired_height=sonarH;


	ROS_INFO("STABILIZING:vx=%f,vy=%f,vz=%f,angz=%f,position:x=%f,y=%f,z=%f,yaw=%f , dx=%f,dy=%f",velx,vely,velz,angz,x_current, y_current,z_current,yaw,dx,dy);

}


else if(toggle==0 && giro==2)
	//Front cam, going to the marker

	{
	//derivative
	dx= (x_current-x_old)/0.01;
	dy= (y_current-y_old)/0.01;
	x_old =x_current;
	y_old =y_current;

	//integrative

	sumx = sumx +x_current*0.01;
	sumz = sumz +(z_current-1.4)*0.01;


	//controller
	if(x_current>0){vely=-0.1*x_current+Kd*dx-0.01 +Ki*sumx;}
	else if(x_current<0){vely=-0.1*x_current+Kd*dx-0.01;}
	else{vely=0;}

	

	if(z_current<1.4){velx=-0.1*z_current;}
	else if(z_current>1.4){velx=0.1*z_current;}
	else{velx=0;}

	if(pitch>0){angz=-0.6*pitch;}
	else if(pitch<0){angz=-0.6*pitch;}
	else{angz=0;}

	if (y_current>0){velz = -0.1*z_current;}
	else if(y_current<0){velz = 0.1*z_current;}
	else{velz=0;}

	velz = -0.01*(sonarH-desired_height);


	//Saturation
	if(velx>0.03){velx= 0.03;}
	else if(velx<-0.03){velx=-0.03;}


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


	ROS_INFO("Towards marker 2:vx=%f,vy=%f,vz=%f,angz=%f,position:x=%f,y=%f,z=%f,pitch=%f , dx=%f,dy=%f",velx,vely,velz,angz,x_current, y_current,z_current,pitch,dx,dy);}


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
	else if(z_current<1.3){velz=0.1*z_current*z_current;}
	else{velz=0;}

	if(yaw>1.57+0.2){angz=-0.2;}
	else if(yaw<1.57-0.2){angz=0.2;}
	else{angz=0;}

	if(velx>0.06){velx= 0.06;}
	else if(velx<-0.06){velx=-0.06;}

	if(vely>0.06){vely=0.06;}
	else if(vely<-0.06){vely=-0.06;}





	geometry_msgs::Twist velmsg;
	velmsg.linear.x=velx;
	velmsg.linear.y=vely;
	//velmsg.linear.z=velz;
	velmsg.angular.z=angz;
	vel_pub.publish(velmsg);


	ROS_INFO("yaw=%f",yaw);
	//Spin completed constraints
	if(yaw<1.57+0.2 && yaw>1.57-0.2)
		 {giro = 2;

		  toggle42=1;
		ROS_INFO("Out of Turning proccess Change toggle giro=%f",giro);
		  }


    }



  }// del auto mode

}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "arsuscriber_marker1");
  ros::NodeHandle n;

  ros::Rate loop_rate(100);

  // sonar_height
  ros::Subscriber  SON= n.subscribe("/ardrone/navdata",1,sonarfunct);

  // velocidades y ar_pose
  ros::Subscriber sub = n.subscribe("ar_pose_marker", 1, arCallback);
  vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 1);

  // el toggle
  ros::Subscriber subs_tog = n.subscribe("/toggledetect", 1, tog);

  //el giro
  ros::Subscriber gir = n.subscribe("giraco", 1, girico);
  ros::Publisher giro_pub = n.advertise<std_msgs::Float64>("/giraco", 1);

  //land to the main
ros::Publisher readyland_pub = n.advertise<std_msgs::Empty>("/ready_land", 1);

//auto
 ros::Subscriber  auto_sus= n.subscribe("automatic",1,autocallback);
//toggle Request
ros::Publisher toggle_request=n.advertise<std_msgs::Empty>("/toggle_request",1);


  while(ros::ok()){
	if(toggle42==1){
		toggle42=0;
		std_msgs::Empty req_msg;
		toggle_request.publish(req_msg);
		ROS_INFO("Toggle REquest");}


	 ros::spinOnce();
    	loop_rate.sleep();
   }	//while
  }//int main
