#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot

	   /* simple_arm::GoToPosition srv;
    srv.request.joint_1 = 1.57;
    srv.request.joint_2 = 1.57;*/

    ball_chaser::DriveToTarget srv;

    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

	if (!client.call(srv))
        ROS_ERROR("Failed to call service drive_bot srv");


}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    
    
    

	int find_white = 0;
    int ball_position_x = 0;
    int ball_position_y = 0;
	// position of white ball
    int image_size = img.data.size();
	//ref image size

	// Then, identify if this pixel falls in the left, mid, or right side of the image

    for(int i = 0; i+2<image_size; i+=3){
	int red_color = img.data[i];
	int green_color = img.data[i+1];
	int blue_color = img.data[i+2];
	//set 'color'_channel to img.data.size() value

	if(red_color == 255 && green_color == 255 && blue_color == 255)
	{
		int x_pos =  (i % (img.width * 3))/3;



		//int y_pos =  (i % (img.height * 3))/3;
		ball_position_x += x_pos;
		//ball_pos_y += y_pos;
		find_white += 1;
 	}
    }
    // Request a stop when there's no white ball seen by the camera
    //Ball chase properties
    if(find_white == 0)
    {
	drive_robot(0.0, 0.0); 
    }
	//ball not in sight
    else

	// Depending on the white ball position, call the drive_bot function and pass velocities to it
    {
	int ball_pos = ball_position_x / find_white;
        if (ball_pos < img.width / 3)
        {
      		drive_robot(0.0, 0.5);
    	}


    	else if (ball_pos > img.width * 2 / 3)
    	{
      		drive_robot(0.0, -0.5);
    	}
    	else
    	{
      		drive_robot(0.5, 0.0);
    	}
	//given values ref* drive_bot    
	}





}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
