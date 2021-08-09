#ifndef _FEATURE_EXTRACTION_H_
#define _FEATURE_EXTRACTION_H_


//roslaunch race_feature_extraction test_pdbnodelet_node.launch
//Emulate race_object_tracking by : "rosrun race_feature_extraction test_feature_extraction"

/* _________________________________
   |                                 |
   |           INCLUDES              |
   |_________________________________| */

//system includes
#include <stdio.h>
#include <stdlib.h>

//ROS includes
#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <std_msgs/String.h>
#include <nodelet/nodelet.h>
#include <pluginlib/class_list_macros.h>   
#include <pcl/filters/uniform_sampling.h>
#include <pcl/visualization/cloud_viewer.h>

//package includes
#include <feature_extraction/spin_image.h>
#include <race_perception_msgs/perception_msgs.h>
#include <race_perception_msgs/CompleteRTOV.h>

//Eigen includes
#include <Eigen/Core>

//PCL includes
#include <pcl/io/pcd_io.h>
#include <pcl/features/normal_3d.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/filters/voxel_grid.h>
#include <CGAL/Plane_3.h>
#include <pcl/filters/uniform_sampling.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_types.h>
#include <pcl/common/common_headers.h>

////new added
#include <pcl/tracking/kld_adaptive_particle_filter_omp.h>
#include <pcl/tracking/distance_coherence.h>
#include <pcl/tracking/hsv_color_coherence.h>
#include <pcl/tracking/approx_nearest_pair_point_cloud_coherence.h>
#include <pcl/tracking/nearest_pair_point_cloud_coherence.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/common/centroid.h>
#include <pcl/common/pca.h>
#include <pcl/filters/passthrough.h>
#include <pcl/filters/project_inliers.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/common/transforms.h>
#include <pcl/filters/conditional_removal.h>
#include <pcl/io/pcd_io.h>
//new includes from preprocessing
//ROS includes
#include <ros/ros.h>
//#include "pcl_ros/impl/transforms.hpp"
#include <pcl/conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl/filters/conditional_removal.h>
#include <pcl/sample_consensus/ransac.h>
#include <pcl/sample_consensus/sac_model_plane.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/ModelCoefficients.h>
#include <pcl/io/pcd_io.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/kdtree/kdtree.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/segmentation/extract_clusters.h>
#include <pcl/filters/project_inliers.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/surface/convex_hull.h>
#include <pcl/segmentation/extract_polygonal_prism_data.h>
//#include <pcl/common/impl/transforms.hpp>
#include <pcl/common/transforms.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>
#include <tf_conversions/tf_eigen.h>
//#include "/opt/ros/fuerte/stacks/geometry/tf_conversions/include/tf_conversions/tf_eigen.h"
#include <visualization_msgs/Marker.h>
#include <pcl/segmentation/conditional_euclidean_clustering.h>

#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>
#include <race_perception_utils/print.h>
#include <pcl_conversions/pcl_conversions.h>


//raceua includes
#include <race_perception_db/perception_db.h>
#include <race_perception_db/perception_db_serializer.h>
#include <race_perception_db/MsgTest.h>
#include <race_perception_utils/cycle.h>
#include <race_perception_utils/print.h>


#include <pcl/kdtree/impl/kdtree_flann.hpp>



/* _________________________________
   |                                 |
   |        Class definition         |
   |_________________________________| */

namespace ros {class Publisher;}

/* _________________________________
   |                                 |
   |            NameSpace            |
   |_________________________________| */

using namespace std;
using namespace pcl;
using namespace ros;
using namespace race_perception_db;
using namespace race_perception_msgs;
using namespace race_perception_utils;
using namespace tf;
/* _________________________________
  |                                 |
  |         Global constant         |
  |_________________________________| */

// #define spin_image_width 8
// #define spin_image_support_lenght 0.2
// #define subsample_spinimages 10

 /* _________________________________
  |                                 |
  |        Global Parameters       |
 |_________________________________| */

    //spin images parameters
//     int    spin_image_width = 8 ;
//     double spin_image_support_lenght = 0.1;
//     int    subsample_spinimages = 10;
//     double uniform_sampling_size = 0.02;

    //TPAMI
    int    spin_image_width = 4 ;
    double spin_image_support_lenght = 0.09;
    int    subsample_spinimages = 10;
    double uniform_sampling_size = 0.03;


/* _________________________________
   |                                 |
   |        GLOBAL VARIABLES         |
   |_________________________________| */
            
//typedef pcl::PointXYZRGB PointT;
typedef PointCloud<PointT> PointCloudT;
typedef boost::shared_ptr<PointCloudT> PointCloudPtrT;

 boost::shared_ptr<tf::TransformListener> _p_transform_listener;


PointCloudPtrT cloud_reference;
PointCloudPtrT initial_cloud_ref;

boost::shared_ptr<TransformBroadcaster> _br; //a transform broadcaster




namespace race_feature_extraction
{
    template <class PointT>
        class FeatureExtraction: public nodelet::Nodelet 
    {
        public:
            //Type defs

//   DefaultPointRepresentation ()
//   {
//     nr_dimensions_ = 153;
//   }

                 
            //local variables
            string _name;
            bool _verb;
            ros::NodeHandle* _p_nh; // The pointer to the node handle
            ros::NodeHandle _nh; // The node handle
            ros::NodeHandle _priv_nh; // The node handle
            ros::NodeHandle* _p_priv_nh; // The node handle
            bool _flg_is_nodelet; //a flag to check if this code is running as a node or a nodelet
            boost::shared_ptr<ros::Subscriber> _p_pcin_subscriber;
            boost::shared_ptr<ros::Subscriber> _p_pcin_subscriber2;

            //boost::shared_ptr<ros::Publisher> _p_pcin_publisher;
            boost::shared_ptr<ros::Publisher> _p_crtov_publisher;
	    boost::shared_ptr<ros::Publisher> _p_projected_object_point_cloud_to_table_publisher;
	    boost::shared_ptr<ros::Publisher> _p_projected_object_point_cloud_to_table_publisher2;

            Publisher marker_publisher;
            Publisher neat_marker_publisher;

            std::string _id_name;
            PerceptionDB* _pdb; //initialize the class
            boost::shared_ptr<CycleDebug> cd;

	    tf::StampedTransform stf;

            /* _________________________________
               |                                 |
               |           PARAMETERS			|
               |_________________________________| */

            //double _param1;
            //string _param2;

            /* _________________________________
               |                                 |
               |           CONSTRUCTORS          |
               |_________________________________| */
    
	    
            FeatureExtraction(){_flg_is_nodelet = true;};
	    

       

            FeatureExtraction(ros::NodeHandle* n)
            {
                _flg_is_nodelet = false; 
                _p_nh = n; //if this is a node set both the nodehandle and private node handle to n
                _p_priv_nh = n; 
                onInit();
            };

            /**
             * @brief Destructor
             */
            ~FeatureExtraction()
            {
                PrettyPrint pp(_name);
                pp.info(std::ostringstream().flush() << _name.c_str() << ": Destructor called");
                pp.info(std::ostringstream().flush() << _name.c_str() << ": Finished destructor");
                pp.printCallback();

            };

	    /* _________________________________
               |                                 |
               |           CLASS METHODS         |
               |_________________________________| */

            void onInit(void)
            {
                //create a node handle in internal nh_ variable, and point p_nh_
                //to it. Only done if we are using a nodelet.
                if (_flg_is_nodelet == true)
                {
                    _nh = getNodeHandle(); 
                    _p_nh = &_nh;
                    _priv_nh = getPrivateNodeHandle(); 
                    _p_priv_nh = &_priv_nh;
                }

                /////////////////////////////////
                /* ______________________________
                   |                             |
                   |  working area for grasping  |
                   |_____________________________| */
		
		//init listener
		_p_transform_listener = (boost::shared_ptr<tf::TransformListener>) new tf::TransformListener;
		ros::Duration(0.5).sleep();
		ROS_INFO(" a tf lisener has been created" )  ;	
		//init broadcaster
		_br = (boost::shared_ptr<TransformBroadcaster>) new TransformBroadcaster;
		ROS_INFO(" a tf broadcaster has been created" )  ;	

                _id_name = "_ObjID_";
                //Initialize tf stuff

                //initialize parameters
                _name = _p_priv_nh->getNamespace();

                PrettyPrint pp(_name);

                _p_priv_nh->param<bool>("verbose", _verb , false);

		//read spin images parameters
		_p_priv_nh->param<int>("/perception/spin_image_width", spin_image_width, spin_image_width);
		_p_priv_nh->param<double>("/perception/spin_image_support_lenght", spin_image_support_lenght, spin_image_support_lenght);
		_p_priv_nh->param<int>("/perception/subsample_spinimages", subsample_spinimages, subsample_spinimages);
		_p_priv_nh->param<double>("/perception/uniform_sampling_size", uniform_sampling_size, uniform_sampling_size);

                //create a cycle debug
                cd = (boost::shared_ptr<CycleDebug>) new CycleDebug(_p_nh, _name);

                //initialize the subscriber
                //Mike: topic name is computed by getting the namespace path and then adding
                // /tracker/tracked_object_point_cloud
                unsigned found = _name.find_last_of("/\\");
                std::string pcin_topic = _name.substr(0,found) + "/tracker/tracked_object_point_cloud";
                _p_pcin_subscriber = (boost::shared_ptr<ros::Subscriber>) new ros::Subscriber;
                *_p_pcin_subscriber = _p_nh->subscribe (pcin_topic, 1, &FeatureExtraction::callback, this);

                //initialize the Publisher
                //_p_pcin_publisher = (boost::shared_ptr<ros::Publisher>) new ros::Publisher;
                //*_p_pcin_publisher = _p_nh->advertise<race_perception_msgs::RTOV> ("spin_images_tracked_object_view", 100);
		
		//initialize the publishers
		neat_marker_publisher = _p_nh->advertise<visualization_msgs::MarkerArray>("/perception/feature_extraction/neat_markers", 100);
			
		_p_crtov_publisher = (boost::shared_ptr<ros::Publisher>) new ros::Publisher;
                *_p_crtov_publisher = _p_priv_nh->advertise<race_perception_msgs::CompleteRTOV> ("spin_images_tracked_object_view", 100);

		_p_projected_object_point_cloud_to_table_publisher = (boost::shared_ptr<ros::Publisher>) new ros::Publisher;
		*_p_projected_object_point_cloud_to_table_publisher = _p_nh->advertise<sensor_msgs::PointCloud2>("/projected_object_point_clouds", 100);
	
		_p_projected_object_point_cloud_to_table_publisher2 = (boost::shared_ptr<ros::Publisher>) new ros::Publisher;
		*_p_projected_object_point_cloud_to_table_publisher2 = _p_nh->advertise<sensor_msgs::PointCloud2>("/projected_object_point_clouds_after_transform", 100);
	
		
                //initialize the database
                _pdb = race_perception_db::PerceptionDB::getPerceptionDB(_p_priv_nh, _flg_is_nodelet); //initialize the database class

                //Output initialization information
                pp.printInitialization();				

            };
	    

            /**
             * @brief This is Miguel's version of the callback.
             * @param msg
             */
            void callback(const race_perception_msgs::PCTOV::ConstPtr& msg)
            {
                cd->tic();//cycle debug tic
                PrettyPrint pp(_name);

		//read spin images parameters
		_p_priv_nh->param<int>("/perception/spin_image_width", spin_image_width, spin_image_width);
		_p_priv_nh->param<double>("/perception/spin_image_support_lenght", spin_image_support_lenght, spin_image_support_lenght);
		_p_priv_nh->param<int>("/perception/subsample_spinimages", subsample_spinimages, subsample_spinimages);
      		_p_priv_nh->param<double>("/perception/uniform_sampling_size", uniform_sampling_size, uniform_sampling_size);

		
		pp.info(std::ostringstream().flush()<<"\t\t[-] spin_image_width :"<< spin_image_width);
		pp.info(std::ostringstream().flush()<<"\t\t[-] spin_image_support_lenght :"<< spin_image_support_lenght);
		pp.info(std::ostringstream().flush()<<"\t\t[-] subsample_spinimages :"<< subsample_spinimages);
		pp.info(std::ostringstream().flush()<<"\t\t[-] uniform_sampling_size :"<< uniform_sampling_size);

		ros::Time beginProc = ros::Time::now(); //start tic


		/* ________________________________________________
                   |                                                 |
                   |  Compute the Spin-Images for given point cloud  |
                   |_________________________________________________| */
		
		
                boost::shared_ptr<PointCloud<PointT> > target_pc (new PointCloud<PointT>); //Declare a boost share ptr to the pointCloud
                pcl::fromROSMsg(msg->point_cloud,*target_pc ); //Convert the pointcloud msg to a pcl point cloud


		
		pp.info(std::ostringstream().flush() << "The size of converted point cloud  = " << target_pc->points.size() );


		//Declare a boost share ptr to the spin image msg
                boost::shared_ptr< vector <SITOV> > msg_out;
                msg_out = (boost::shared_ptr< vector <SITOV> >) new (vector <SITOV>);
		pp.info(std::ostringstream().flush() << "Given point cloud has " << target_pc->points.size() << " points.");
      
// 		//downsampling voxel grid approach 
// 		boost::shared_ptr<PointCloud<PointT> > cloud_filtered (new PointCloud<PointT>);
// 		pcl::VoxelGrid<PointT > voxelized_point_cloud;	
// 		voxelized_point_cloud.setInputCloud (target_pc);
// 		voxelized_point_cloud.setLeafSize (uniform_sampling_size, uniform_sampling_size, uniform_sampling_size);
// 		voxelized_point_cloud.filter (*cloud_filtered);
// 		find nearest point to each voxel centroid
// 		pcl::PointCloud<int> uniform_sampling_indices;
// 		for (int i =0; i < cloud_filtered->points.size() ;i++)
// 		{
// 		    int nearest_point_index = 0;
// 		    double minimum_distance = 1000;
// 		    for (int j=0; j < target_pc->points.size(); j++)
// 		    {		
// 			double distance = sqrt(	pow((cloud_filtered->points[i].x - target_pc->points[j].x) , 2) +
// 						pow((cloud_filtered->points[i].y - target_pc->points[j].y) , 2) +
// 						pow((cloud_filtered->points[i].z - target_pc->points[j].z), 2));
// 			if (distance < minimum_distance)
// 			{
// 				nearest_point_index = j;
// 				minimum_distance = distance;
// 			}
// 		    }
// 		    uniform_sampling_indices.push_back(nearest_point_index);
// 		
// 		}
// 		for (int i=0; i<uniform_sampling_indices.size(); i++)
// 		{
// 		    ROS_INFO("uniform sampling at[%i] = %i",i, uniform_sampling_indices.points.at(i));
// 		}
// 		boost::shared_ptr<PointCloud<PointT> > model_keypoints (new PointCloud<PointT>);
// 		pcl::copyPointCloud (*target_pc, uniform_sampling_indices.points, *model_keypoints);
		
		//visualization minimum and maximum points for a given point cloud;
// 		PointT minimum_pt;
// 		PointT maximum_pt;
// 		getMinMax3D(*target_pc, minimum_pt, maximum_pt); // min max for bounding box
// 
// 				
	
			
		
		
		boost::shared_ptr<PointCloud<PointT> > uniform_keypoints (new PointCloud<PointT>);
		boost::shared_ptr<pcl::PointCloud<int> >uniform_sampling_indices (new PointCloud<int>);
		keypoint_selection( target_pc, 
				    uniform_sampling_size,
				    uniform_keypoints,
				    uniform_sampling_indices);
		
		if (_verb)
		{
		    ROS_INFO ("uniform_sampling_size = %f", uniform_sampling_size);
		    ROS_INFO ("number of keypoints = %i", uniform_keypoints->points.size());
		}
// 		 pcl::search::KdTree<pcl::PointXYZRGBA>::Ptr kdtree (new pcl::search::KdTree<pcl::PointXYZRGBA>);
// 				 
// // 		pcl::search::KdTree<PointT>::Ptr kdtree (new pcl::search::KdTree<PointT>);
// 		NormalEstimation<pcl::PointXYZRGBA, Normal> normal_estimation;
// 		normal_estimation.setInputCloud (uniform_keypoints);
// 		normal_estimation.setSearchMethod (kdtree);
// 		normal_estimation.setRadiusSearch ( 0.05);
// 		PointCloud<Normal>::Ptr Keypoints_with_normal (new PointCloud< Normal>);
// 		normal_estimation.compute (*Keypoints_with_normal);
 		
		
		
 		//visualization point cloud
// 		pcl::visualization::PCLVisualizer viewer1 ("keypoints selection");
// 		viewer1.addPointCloud (target_pc, "original");
// 		pcl::visualization::PointCloudColorHandlerCustom<PointT> Model_keypoints_color_handler2 (uniform_keypoints, 0,0, 255);
// 		viewer1.addPointCloud (uniform_keypoints, Model_keypoints_color_handler2, "keypoints");
// 		viewer1.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, "keypoints");
// 		viewer1.setBackgroundColor (255, 255, 255);
// 		while (!viewer1.wasStopped ())
// 		{ viewer1.spinOnce (100);}
// 		
// 		//visualization surface normal estimation for the given point cloud
// 		pcl::visualization::PCLVisualizer viewer2 ("keypoints selection");
// 		viewer2.addPointCloud (target_pc, "original");
// 		viewer2.addPointCloud (uniform_keypoints, Model_keypoints_color_handler2, "keypoints");
// 		viewer2.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, "keypoints");
// 		viewer2.setBackgroundColor (255, 255, 255);
// 		viewer2.addPointCloudNormals<pcl::PointXYZRGBA, pcl::Normal> (uniform_keypoints, Keypoints_with_normal, 1, 0.05, "normals");
// 		viewer2.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255,0, 0, "normals"); 
// 		while (!viewer2.wasStopped ())
// 		{ viewer2.spinOnce (100);}
		
		//select keypoints
// 		int numer_of_keypoints =subsample_spinimages;
// 	    	size_t subsample_step = (uniform_sampling_indices.size())/numer_of_keypoints; //to compute the subsampling step;
// 		pcl::PointCloud<int> new_sample;
// 		new_sample=uniform_sampling_indices;
// 		new_sample.clear();
// 		int j=0;
// 		ROS_INFO("size of selected point=%i",uniform_sampling_indices.size() );
// 		ROS_INFO("numer_of_keypoints=%i", numer_of_keypoints);
// 		ROS_INFO("subsample_step=%i", subsample_step);
// 		for (size_t i =subsample_step ; (i <= numer_of_keypoints*subsample_step)and (i<uniform_sampling_indices.size()); i += subsample_step) // 
// 		{
// 		  ROS_INFO("sampled_indices.at(%i)=%i",i, uniform_sampling_indices.at(i));
// 		  new_sample.points.push_back(uniform_sampling_indices.at(i)); 
// 		  ROS_INFO("new_indices.at(%i)=%i",i, new_sample.at(j));
// 		  j++;
// 		}
// 		ROS_INFO("size of new_sample =%i", new_sample.points.size());
// 		boost::shared_ptr<PointCloud<PointT> > model_keypoints2 (new PointCloud<PointT>);
// 		pcl::copyPointCloud (*target_pc, new_sample.points, *model_keypoints2);
// 		pcl::visualization::PointCloudColorHandlerCustom<PointT> Model_keypoints_color_handler3 (model_keypoints2, 0,0, 255);
// 		viewer2.addPointCloud (model_keypoints2, Model_keypoints_color_handler3, "keypoints2");
// 		viewer2.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, "keypoints2");
// 		viewer2.setBackgroundColor (255, 255, 255);
// 		while (!viewer2.wasStopped ())
// 		{ viewer2.spinOnce (100);}
		
		
		
		
		
// 		//  Downsample Clouds to Extract keypoints ( uniformSampling )	
// 		ROS_INFO("uniform_sampling_size = %f", uniform_sampling_size);
// // 		boost::shared_ptr<PointCloud<PointT> > model_keypoints (new PointCloud<PointT>);
// 		pcl::PointCloud<int> sampled_indices;
// 		pcl::UniformSampling<PointT> uniform_sampling;// 
// 		uniform_sampling.setInputCloud (target_pc);
// 		uniform_sampling.setRadiusSearch (uniform_sampling_size);
// 		uniform_sampling.compute (sampled_indices);
// 		pcl::copyPointCloud (*target_pc, sampled_indices.points, *model_keypoints);
// 		std::cout << "Model total points: " << target_pc->size () << "; Selected Keypoints: " << model_keypoints->size () << std::endl;
// 		//visualization
// 		pcl::visualization::PCLVisualizer viewer2 ("keypoints selection");
// 		viewer2.addPointCloud (target_pc, "original");
// 		pcl::visualization::PointCloudColorHandlerCustom<PointT> Model_keypoints_color_handler2 (model_keypoints, 255,0, 0);
// 		viewer2.addPointCloud (model_keypoints, Model_keypoints_color_handler2, "keypoints");
// 		viewer2.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 15, "keypoints");
// 			
// 		//select keypoints
// 		int numer_of_keypoints =subsample_spinimages;
// 	    	size_t subsample_step = (sampled_indices.size())/numer_of_keypoints; //to compute the subsampling step;
// 		pcl::PointCloud<int> new_sample;
// 		new_sample=sampled_indices;
// 		new_sample.clear();
// 		int j=0;
// 		ROS_INFO("size of selected point=%i",sampled_indices.size() );
// 		ROS_INFO("numer_of_keypoints=%i", numer_of_keypoints);
// 		ROS_INFO("subsample_step=%i", subsample_step);
// 		for (size_t i =subsample_step ; (i <= numer_of_keypoints*subsample_step)and (i<sampled_indices.size()); i += subsample_step) // 
// 		{
// 		  ROS_INFO("i=%i", i);
// 		  ROS_INFO("sampled_indices.at(i)=%i", sampled_indices.at(i));
// 		  new_sample.points.push_back(sampled_indices.at(i)); 
// 		  ROS_INFO("new_indices.at(i)=%i", new_sample.at(j));
// 		  j++;
// 		}
// 		ROS_INFO("size of new_sample =%i", new_sample.points.size());
// 		boost::shared_ptr<PointCloud<PointT> > model_keypoints2 (new PointCloud<PointT>);
// 		pcl::copyPointCloud (*target_pc, new_sample.points, *model_keypoints2);
// 		pcl::visualization::PointCloudColorHandlerCustom<PointT> Model_keypoints_color_handler3 (model_keypoints2, 0,0, 255);
// 		viewer2.addPointCloud (model_keypoints2, Model_keypoints_color_handler3, "keypoints2");
// 		viewer2.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, "keypoints2");
// 		viewer2.setBackgroundColor (255, 255, 255);
// 		while (!viewer2.wasStopped ())
// 		{ viewer2.spinOnce (100);}
// 		
                    //////////////////////////////////////
                    //
                    //sampling approach
                    //
                    //////////////////////////////////////
                    
//                 if (!estimateSpinImages( target_pc, 
//                             0.01 /*downsampling_voxel_size*/, 
//                             0.05 /*normal_estimation_radius*/,
//                             spin_image_width /*spin_image_width*/,
//                             0.0 /*spin_image_cos_angle*/,
//                             1 /*spin_image_minimum_neighbor_density*/,
//                             spin_image_support_lenght /*spin_image_support_lenght*/,
//                             msg_out,
//                             subsample_spinimages /*subsample spinimages*/
//                             ))
//                 {
//                     pp.error(std::ostringstream().flush() << "Could not compute spin images");
//                     pp.printCallback();
//                     return;
// 
//                 }
                    //////////////////////////////////////
                    //
                    //voxel approach
                    //
                    //////////////////////////////////////
                if (!estimateSpinImages2(target_pc, 
                            0.01 /*downsampling_voxel_size*/, 
                            0.05 /*normal_estimation_radius*/,
                            spin_image_width /*spin_image_width*/,
                            0.0 /*spin_image_cos_angle*/,
                            1 /*spin_image_minimum_neighbor_density*/,
                            spin_image_support_lenght/*spin_image_support_lenght*/,
                            msg_out,
			     uniform_sampling_indices
                            ))		    
                {
                    pp.error(std::ostringstream().flush() << "Could not compute spin images");
			if (_verb)       
			{
			pp.printCallback();
			}
                    return;
                }

                
		pp.info(std::ostringstream().flush() << "Computed " << msg_out->size() << " spin images for given point cloud. ");
		
// 		visulazeObjectViewSpinImageMatlab (*msg_out, "/home/hamidreza/VisualizedSpinImages.txt");
		
                //get toc
                ros::Duration duration = ros::Time::now() - beginProc;
                double duration_sec = duration.toSec();
                pp.info(std::ostringstream().flush() << "Compute Spin-images for given point cloud took " << duration_sec << " secs");

                /* _____________________________________________
                   |                                            |
                   |  Write features to DB based on TID and VID |
                   |____________________________________________| */

                beginProc = ros::Time::now();

                //Declare SITOV (Spin Images of Tracked Object View)
                SITOV _sitov;

                //Declare RTOV (Representation of Tracked Object View)
                RTOV _rtov;
                _rtov.track_id = msg->track_id;
                _rtov.view_id = msg->view_id;

		pp.info(std::ostringstream().flush() << "Track_id = " << msg->track_id << "\tView_id = " << msg->view_id );
		
		
                //declare the RTOV complete variable
		race_perception_msgs::CompleteRTOV _crtov;
		_crtov.track_id = msg->track_id;
		_crtov.view_id = msg->view_id;
		_crtov.ground_truth_name = msg->ground_truth_name.c_str();
		_crtov.pose_stamped = msg -> pose_stamped;
		_crtov.dimensions = msg -> dimensions;
		pp.info(std::ostringstream().flush() << "object_pose.x = %f " << msg -> pose_stamped.pose.position.x);
		pp.info(std::ostringstream().flush() << "object_pose.y = %f " << msg -> pose_stamped.pose.position.y);
		pp.info(std::ostringstream().flush() << "object_pose.z = %f " << msg -> pose_stamped.pose.position.z);
                //Add the Spin Images in msg_out to sitovs to put in the DB
                for (size_t i = 0; i < msg_out->size(); i++)
                {
                    _sitov = msg_out->at(i); //copy spin images
                    _sitov.track_id = msg->track_id; //copy track_id
                    _sitov.view_id = msg->view_id; //copy view_id
                    _sitov.spin_img_id = i; //copy spin image id

                    //Addd sitov to completertov sitov list
                    _crtov.sitov.push_back(_sitov);

                    if (msg->is_key_view) //add sitovs to the DB only if this is a key view
                    {
                        //Serialize to add to DB
                        uint32_t serial_size = ros::serialization::serializationLength(_sitov);
                        boost::shared_array<uint8_t> buffer(new uint8_t[serial_size]);
                        PerceptionDBSerializer<boost::shared_array<uint8_t>, SITOV>::serialize(buffer, _sitov, serial_size);	
                        leveldb::Slice s((char*)buffer.get(), serial_size);
                        std::string key = _pdb->makeSIKey(key::SI, msg->track_id, msg->view_id, i );

                        //Put slice to the DB
                        _pdb->put(key, s); 

                        //Add to the list of SITOV keys for this RTOV
                        _rtov.sitov_keys.push_back(key);
                        buffer.reset();
                    }

                }


                pp.info(std::ostringstream().flush() << "Is keyview: " << (int)msg->is_key_view);
                //Add RTOV to the DB (only if this is a key view)
                if (msg->is_key_view)                 
                {
                    uint32_t serial_size = ros::serialization::serializationLength(_rtov);
                    boost::shared_array<uint8_t> buffer(new uint8_t[serial_size]);
                    PerceptionDBSerializer<boost::shared_array<uint8_t>, RTOV>::serialize(buffer, _rtov, serial_size);	
                    leveldb::Slice s((char*)buffer.get(), serial_size);
                    std::string key = _pdb->makeKey(key::RV, msg->track_id, msg->view_id);

                    //Put slice to the db
                    _pdb->put(key, s);
                    buffer.reset();
                }

                //publish a RTOV msg for recognition
                //_p_pcin_publisher->publish (_rtov);

                //Publish the CompleteRTOV to recognition
                _p_crtov_publisher->publish (_crtov);

                //Toc
                duration = (ros::Time::now() - beginProc);
                duration_sec = duration.toSec();
                pp.info(std::ostringstream().flush() << "Write the features to DB took " << duration_sec << " secs");

                //print the callback report
		//cout<< "verb ="<< _verb;
		//if (_verb)
		//{
		    pp.printCallback();
		//}
                //dealocate pointers
                target_pc.reset();
                msg_out.reset();

                cd->run();//cycle debug run
            }

            //void callback(const race_perception_msgs::PCTOV::ConstPtr& msg)
            //{
            //PrettyPrint pp;
            ////cd->run();//cycle debug run

            //[> ________________________________________________
            //|                                                 |
            //|  Compute the Spin-Images for given point cloud |
            //|_______________________________________________| */
            //ros::Time beginProc = ros::Time::now();

            ////Declare a boost share ptr to the pointCloud
            //boost::shared_ptr<PointCloud<PointT> > target_pc (new PointCloud<PointT>);
            ////Call the library function for testing
            //pcl::fromROSMsg(msg->point_cloud,*target_pc );

            ////Declare a boost share ptr to the spin image msg
            //boost::shared_ptr< vector <SITOV> > msg_out;
            //msg_out = (boost::shared_ptr< vector <SITOV> >) new (vector <SITOV>);

            //if (!estimateSpinImages( target_pc, 
            //0.01 [>downsampling_voxel_size<], 
            //0.05 [>normal_estimation_radius<],
            //8 [>spin_image_width<],
            //0.0 [>spin_image_cos_angle<],
            //1 [>spin_image_minimum_neighbor_density<],
            //0.2 [>spin_image_support_lenght<],
            //msg_out,
            //30
            //))
            //{
            //pp.error(std::ostringstream().flush() << "Could not compute spin image");
            //}

            //pp.info(std::ostringstream().flush() << msg_out->size()<< " Spin-images computed for given point cloud");

            //ros::Duration duration = ros::Time::now() - beginProc;
            //double duration_sec = duration.toSec();
            //pp.info(std::ostringstream().flush() << "Compute Spin-images for given point cloud took " << duration_sec << " secs");

            //[> _____________________________________________
            //|                                            |
            //|  Wirte feature to DB based on TID and VID |
            //|__________________________________________| */

            //beginProc = ros::Time::now();

            ////Declare SITOV
            //SITOV _sitov;

            ////Declare RTOV
            //RTOV _rtov;
            //_rtov.track_id = msg->track_id;
            //_rtov.view_id = msg->view_id;
            //// 				if (msg->is_key_view)
            //// 				{
            //// 				    _rtov.is_key_view = true;
            //// 				}


            //for (size_t i = 0; i < msg_out->size(); i++)
            //{

            //_sitov = msg_out->at(i);
            //_sitov.track_id = msg->track_id;
            //_sitov.view_id = msg->view_id;
            //_sitov.spin_img_id = i;

            //uint32_t serial_size = ros::serialization::serializationLength(_sitov);
            //boost::shared_array<uint8_t> buffer(new uint8_t[serial_size]);
            //PerceptionDBSerializer<boost::shared_array<uint8_t>, SITOV>::serialize(buffer, _sitov, serial_size);	
            //leveldb::Slice s((char*)buffer.get(), serial_size);
            //std::string key = _pdb->makeSIKey(key::SI, msg->track_id, msg->view_id, i );

            ////Put slice to the db
            //_pdb->put(key, s); 
            ////create a list of key of spinimage
            //_rtov.sitov_keys.push_back(key);

            //}

            //uint32_t serial_size = ros::serialization::serializationLength(_rtov);

            //boost::shared_array<uint8_t> buffer(new uint8_t[serial_size]);
            //PerceptionDBSerializer<boost::shared_array<uint8_t>, RTOV>::serialize(buffer, _rtov, serial_size);	

            //leveldb::Slice s((char*)buffer.get(), serial_size);

            //std::string key = _pdb->makeKey(key::RV, msg->track_id, msg->view_id);

            ////Put slice to the db
            //_pdb->put(key, s);

            ////publish a RTOV msg for recognition
            //_p_pcin_publisher->publish (_rtov);

            //duration = (ros::Time::now() - beginProc);
            //duration_sec = duration.toSec();
            //pp.info(std::ostringstream().flush() << "Write the features to DB took " << duration_sec << " secs");
            ////ROS_INFO("FeatureExtraction-Publish a _rtov msg");
            //pp.printCallback();
            //}

            /* _________________________________
               |                                 |
               |           ACCESSORS             |
               |_________________________________| */
	    
	    
int set_neat_visualization_marker_array_for_manipulation(string object_frame_id, unsigned int TID )
{

  //STEP 1: need to get the position of the object so we can draw
  //text nearby
//   tf::StampedTransform stf; //the transform
//   //std::string tracker_frame_id =  msg->header.frame_id;
//   std::string tracker_frame_id = "/perception/pipeline" + boost::lexical_cast<std::string>(msg->track_id) + "/tracker";

  ROS_INFO ("set_neat_visualization_marker_array_for_manipulation: object_frame_id = %s", object_frame_id.c_str());
  
  visualization_msgs::MarkerArray marker_array; 
  geometry_msgs::Point p;
  double duration = 0;
  bool locked = true;
  duration=5;
  bool finish= true;
	
  /* _________________________________
      |                                 |
      |           DRAW XYZ AXES         |
      |_________________________________| */
  if (1)
  {	
	  visualization_msgs::Marker marker;
	  double axis_dimension = 0.9;
	  marker.header.frame_id = object_frame_id.c_str();
	  marker.header.stamp = ros::Time();

	  //marker.frame_locked = locked;
	  marker.type = visualization_msgs::Marker::LINE_STRIP;
	//  if (finish)
	//	  marker.action = visualization_msgs::Marker::DELETE;
	 // else
		  marker.action = visualization_msgs::Marker::ADD;

	  marker.scale.x = 0.05; marker.scale.y = 0.5; marker.scale.z = 4;
	  marker.scale.x = 05; marker.scale.y = 0.5; marker.scale.z = 4; 
	  marker.lifetime = Duration(duration);

	  
	  //X axis
	  marker.ns = "axes_x";
	  marker.id = TID;
	  marker.color.r = 1.0; marker.color.g = 0.0;	marker.color.b = 0.0; marker.color.a = 1.0; //red color
	  marker.points.erase(marker.points.begin(), marker.points.end());
	  p.x = 0; p.y = 0; p.z = 0; marker.points.push_back(p);
	  p.x = 1 * axis_dimension; p.y = 0; p.z = 0; marker.points.push_back(p);
	  marker_array.markers.push_back(marker);

	  //Y axis
	  marker.ns = "axes_y";
	  marker.id = TID;
	  marker.color.r = 0.0; marker.color.g = 1.0;	marker.color.b = 0.0; marker.color.a = 1.0; //green color
	  marker.points.erase(marker.points.begin(), marker.points.end());
	  p.x = 0; p.y = 0; p.z = 0; marker.points.push_back(p);
	  p.x = 0; p.y = 1 * axis_dimension; p.z = 0; marker.points.push_back(p);
	  marker_array.markers.push_back(marker);

	  //Z axis
	  marker.ns = "axes_z";
	  marker.id = TID;
	  marker.color.r = 0.0; marker.color.g = 0.0;	marker.color.b = 1.0; marker.color.a = 1.0; //blue color
	  marker.points.erase(marker.points.begin(), marker.points.end());
	  p.x = 0; p.y = 0; p.z = 0; marker.points.push_back(p);
	  p.x = 0; p.y = 0; p.z = 1 * axis_dimension; marker.points.push_back(p);
	  marker_array.markers.push_back(marker);
  }


  
  
  
  /* _________________________________
	   |                                 |
	   |         DRAW TEXT INFO          |
	   |_________________________________| */
// 	if (1)
// 	{
// 		visualization_msgs::Marker marker;
// 		marker.header.frame_id = _tracker_frame_id;
// 		//marker.header.frame_id = _fixed_frame_id;
// 		marker.frame_locked = locked;
// 		marker.header.stamp = ros::Time();
// 		marker.ns = "information";
// 		marker.id = _track_id;
// 		marker.type = visualization_msgs::Marker::TEXT_VIEW_FACING;
// 		marker.lifetime = Duration(duration);
// 		marker.action = visualization_msgs::Marker::ADD;
// 		//if (finish)
// 		//marker.action = visualization_msgs::Marker::DELETE;
// 		//else
// 
// 		//marker.pose = _tracked_object_msg.bounding_box.pose_stamped.pose;
// 
// 		//marker.pose.position.x = 0 + marker.pose.position.x*0.9 + 0;
// 		//marker.pose.position.y = 0 + marker.pose.position.y*0.9 + 0.1;
// 		marker.pose.position.z = _tracked_object_msg.bounding_box.dimensions.z/2+0.1;
// 		marker.scale.z = 0.02; 
// 		//marker.color.r  = 1; marker.color.g  = 1; marker.color.b  = 1; marker.color.a = 1;
// 
// 		if (finish)
// 		{
// 			marker.color.r  = 0.5; marker.color.g  = 0; marker.color.b  = 0; marker.color.a = 1;
// 		}
// 		else
// 		{
// 			marker.color.r  = 0; marker.color.g  = 0; marker.color.b  = 0; marker.color.a = 1;
// 		}
// 
// 		marker.lifetime = Duration(duration);
// 
// 		char tmp_str[255]; 
// 		//sprintf(tmp_str,"\nfr=%0.1f ad=%0.2f vel=%0.2f t=%0.1f did=%d", _fit_ratio, _accumulated_distance, _velocity, _time_since_velocity_computation, _demonstrator_id);
// 		sprintf(tmp_str,"%0.1f", _fit_ratio);
// 
// 		marker.text = "TID" + boost::lexical_cast<std::string>(_track_id) + " V" + boost::lexical_cast<std::string>(view_count) + "(" + tmp_str + ")";
// 
// 		//marker.text.append(tmp_str);
// 		//char tmp_str1[255]; 
// 		//sprintf(tmp_str1,"\nr=%0.1f p=%0.1f y=%0.1f",diff_roll, diff_pitch, diff_yaw);
// 		//marker.text.append(tmp_str1);
// 
// 		marker.text.append("\n");
// 
// 		if (_state_is_moving)
// 		{
// 			marker.text.append("[M");
// 		}
// 		else
// 		{
// 			marker.text.append("[S");
// 		}
// 
// 		//marker.text.append("\n");
// 		if ((ros::Time::now() - _point_cloud_sent_tic).toSec() < 1.0 && (ros::Time::now() - _key_view_tic).toSec() < 1.0) 
// 			marker.text.append(",C,K]");
// 		else if ((ros::Time::now() - _point_cloud_sent_tic).toSec() < 1.0) 
// 			marker.text.append(",C,_]");
// 		else
// 			marker.text.append(",_,_]");
// 
// 		if (finish)	marker.text.append("\n(TRACKING LOST)");
// 
// 
// 		marker_array.markers.push_back(marker);
// 	}

// 	/* _________________________________
// 	   |                                 |
// 	   |             DRAW WIREFRAME      |
// 	   |_________________________________| */
// 	if (1)
// 	{
// 		visualization_msgs::Marker marker;
// 		marker.header.frame_id = _tracker_frame_id;
// 		marker.header.stamp = ros::Time();
// 
// 		marker.ns = "wireframe";
// 		marker.id = _track_id;
// 		marker.frame_locked = locked;
// 		marker.type = visualization_msgs::Marker::LINE_LIST;
// 		//if (finish)
// 		//marker.action = visualization_msgs::Marker::DELETE;
// 		//else
// 		marker.action = visualization_msgs::Marker::ADD;
// 		marker.lifetime = Duration(duration);
// 
// 		//marker.pose = _tracked_object_msg.bounding_box.pose_stamped.pose;
// 
// 		marker.scale.x = 0.005; 
// 		double x = _tracked_object_msg.bounding_box.dimensions.x/2; 
// 		double y = _tracked_object_msg.bounding_box.dimensions.y/2; 
// 		double z = _tracked_object_msg.bounding_box.dimensions.z/2; 
// 
// 		_color.a = 0.5;
// 		marker.color = _color;
// 		marker.color.r = 0.5;
// 		marker.color.g = 0.5;
// 		marker.color.b = 0.5;
// 		//marker
// 		if (finish)
// 		{
// 			marker.color.r = 0.1;
// 			marker.color.g = 0.1;
// 			marker.color.b = 0.1;
// 		}
// 
// 		p.x =  x; p.y =  y; p.z =  z; marker.points.push_back(p);
// 		p.x = -x; p.y =  y; p.z =  z; marker.points.push_back(p);
// 		p.x =  x; p.y =  y; p.z = -z; marker.points.push_back(p);
// 		p.x = -x; p.y =  y; p.z = -z; marker.points.push_back(p);
// 		p.x =  x; p.y =  y; p.z = -z; marker.points.push_back(p);
// 		p.x =  x; p.y =  y; p.z =  z; marker.points.push_back(p);
// 		p.x = -x; p.y =  y; p.z = -z; marker.points.push_back(p);
// 		p.x = -x; p.y =  y; p.z =  z; marker.points.push_back(p);
// 
// 		p.x =  x; p.y =  -y; p.z =  z; marker.points.push_back(p);
// 		p.x = -x; p.y =  -y; p.z =  z; marker.points.push_back(p);
// 		p.x =  x; p.y =  -y; p.z = -z; marker.points.push_back(p);
// 		p.x = -x; p.y =  -y; p.z = -z; marker.points.push_back(p);
// 		p.x =  x; p.y =  -y; p.z = -z; marker.points.push_back(p);
// 		p.x =  x; p.y =  -y; p.z =  z; marker.points.push_back(p);
// 		p.x = -x; p.y =  -y; p.z = -z; marker.points.push_back(p);
// 		p.x = -x; p.y =  -y; p.z =  z; marker.points.push_back(p);
// 
// 		p.x =  x; p.y =  y; p.z =  z; marker.points.push_back(p);
// 		p.x =  x; p.y = -y; p.z =  z; marker.points.push_back(p);
// 		p.x =  x; p.y =  y; p.z = -z; marker.points.push_back(p);
// 		p.x =  x; p.y = -y; p.z = -z; marker.points.push_back(p);
// 
// 		p.x = -x; p.y =  y; p.z =  z; marker.points.push_back(p);
// 		p.x = -x; p.y = -y; p.z =  z; marker.points.push_back(p);
// 		p.x = -x; p.y =  y; p.z = -z; marker.points.push_back(p);
// 		p.x = -x; p.y = -y; p.z = -z; marker.points.push_back(p);
// 
// 		marker_array.markers.push_back(marker);
// 	}
// 
// 
// 	/* _________________________________
// 	   |                                 |
// 	   |             DRAW BBOX           |
// 	   |_________________________________| */
// 	if (1)
// 	{
// 		visualization_msgs::Marker marker;
// 		marker.header.frame_id = _tracker_frame_id;
// 		marker.header.stamp = ros::Time();
// 
// 		marker.ns = "boundingbox";
// 		marker.id = _track_id;
// 		marker.type = visualization_msgs::Marker::CUBE;
// 		marker.frame_locked = locked;
// 		//if (finish)
// 		//marker.action = visualization_msgs::Marker::DELETE;
// 		//else
// 		marker.action = visualization_msgs::Marker::ADD;
// 		marker.lifetime = Duration(duration);
// 
// 		//marker.pose = _tracked_object_msg.bounding_box.pose_stamped.pose;
// 
// 		marker.scale.x = _tracked_object_msg.bounding_box.dimensions.x; 
// 		marker.scale.y = _tracked_object_msg.bounding_box.dimensions.y; 
// 		marker.scale.z = _tracked_object_msg.bounding_box.dimensions.z; 
// 
// 		_color.a = 0.1;
// 		marker.color = _color;
// 		if (finish)
// 		{
// 			marker.color.r = 0.9;
// 			marker.color.g = 0.0;
// 			marker.color.b = 0.0;
// 		}
// 
// 		marker_array.markers.push_back(marker);
// 	}

	neat_marker_publisher.publish(marker_array);
	return 1;


    }
            void rot_mat(int arg1);


    };

    
    
    class FeatureExtractionNodelet: public FeatureExtraction<pcl::PointXYZRGBA>{};
   // PLUGINLIB_DECLARE_CLASS(race_feature_extraction, FeatureExtractionNodelet, race_feature_extraction::FeatureExtractionNodelet, nodelet::Nodelet);
    PLUGINLIB_EXPORT_CLASS(race_feature_extraction::FeatureExtractionNodelet, nodelet::Nodelet);
}//end feature_extraction namespace
#endif


