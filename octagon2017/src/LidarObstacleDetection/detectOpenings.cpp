#include "ros/ros.h"
#include <vector>
#include <math.h>
  
using namespace std;

//Global Variables
ros::Publisher obstaclePub;

static double maxRadius = 10.0;
static double MM2M  = 0.001;
static int M2MM = 1000;
static double nonSeparationThresh = 200;
static int highThresh = 50;
static int lowThresh = 4;
static int forgiveCount = 3;
static int linkedCount;
static int sumOfPoints;
static int obsSizeNum;
static bool isAlreadyLinking = false;
static bool isThereAnObstacle = false;
static double sumOfHeadings = 0.0;
static double obstacleStopThreshold;
static double movingObstacleSize = 0.45;
static double movingObstacleThresh = 0.07;
static vector<geometry_msgs::Point32> lidarData;
vector<yeti_snowplow::lidar_point> lmsData;
vector<yeti_snowplow::obstacle> obstacles;




void localizationCallback(const yeti_snowplow::robot_position::ConstPtr& position) 

{
    /* This fires every time a new robot position is published */
    //Not used for now

    return;
}

void scanCallback(const sensor_msgs::PointCloud::ConstPtr& scannedData)
{
    /* This fires every time a new obstacle scan is published */
    //location contains an array of points, which contains an x and a y relative to the robot
    lidarData = scannedData->points;
    convertPointCloudToClass();
    findObstacles();
    
}


// input: robot position relative to field
// input: array of lidar scan data
// output: array of obstacles (x,y,r(size), moving(boolean))

int main(int argc, char **argv)
{
	ros::init(argc, argv, "obstacle_detection");

	ros::NodeHandle n;

    ros::Subscriber localizationSub = n.subscribe("robot_location", 1, localizationCallback);

    ros::Subscriber scanSub = n.subscribe("laser_scan_point_cloud", 1, scanCallback);

    ros::Publisher obstaclePub;//ROS obstacle publisher

    obstaclePub = n.advertise<yeti_snowplow::obstacles>("obstacles", 100);

    while(ros::ok())
    {
        //Publish all obstacles
        yeti_snowplow::obstacles msg;
        msg.obstacles = obstacles;
        obstaclePub.publish(msg);
    }
	ros::spin();
	
	return 0;
}

