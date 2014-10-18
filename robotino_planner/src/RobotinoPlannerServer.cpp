/*
 * RobotinoPlannerServer.cpp
 *
 *  Created on: 12.10.2014
 *      Author: adrianohrl@unifei.edu.br
 */

#include "RobotinoPlannerServer.h"

RobotinoPlannerServer::RobotinoPlannerServer():
	server_(nh_, "planner", boost::bind(&RobotinoPlannerServer::execute, this, _1), false)
{
	go_srv_ = nh_.serviceClient<robotino_leds::GoFromTo>("go_from_to");
	move_srv_ = nh_.advertiseService("move_to", &RobotinoPlannerServer::moveTo, this);
	stop_srv_ = nh_.serviceClient<robotino_leds::StopTransportation>("stop_transportation");
	transport_srv_ = nh_.serviceClient<robotino_leds::TransportProduct>("transport_product");
	has_arrived_sub_ = nh_.subscribe("has_arrived", 1, &RobotinoPlannerServer::hasArrived, this);

	// Module A variables:	
	has_arrived_ = false;
	num_lists_ = 2;
	num_orders_ = 3;
	list_ = 0;
	order_ = 0;

	// Module B variables:
	
}

RobotinoPlannerServer::~RobotinoPlannerServer()
{
	go_srv_.shutdown();
	move_srv_.shutdown();
	stop_srv_.shutdown();
	transport_srv_.shutdown();
	has_arrived_sub_.shutdown();
}

void RobotinoPlannerServer::spin()
{
	ros::Rate loop_rate(5);
	ROS_INFO("Robotino Planner Server up and running");
	while(nh_.ok())
	{
		ros::spinOnce();
		loop_rate.sleep();
	}
}

void RobotinoPlannerServer::execute(const robotino_planner::PlannerGoalConstPtr& goal)
{
	ros::Rate loop_rate(10);
	if(!acceptNewGoal(goal))
	{
		ROS_WARN("Goal not accepted");
		return;
	}
	while(nh_.ok())
	{
		if(server_.isPreemptRequested())
		{
			if(server_.isNewGoalAvailable())
			{
				if(!acceptNewGoal(server_.acceptNewGoal()))
				{
					return;
				}
			}
			else
			{
				ROS_INFO("Cancel request");
				server_.setPreempted();
				return;
			}
		}
		if (module_ == MODULE_A || module_ == MODULE_B)
		{
			controlLoop();
		}
		ros::spinOnce();
		loop_rate.sleep();
	}
	server_.setAborted(robotino_planner::PlannerResult(), "Aborting on the goal because the node has been killed.");
}

void RobotinoPlannerServer::controlLoop()
{
	if (module_ == MODULE_A)
	{
		
		if (list_ < num_lists_) // for loop structure
		{
			
			if (order_ < num_orders_) // for loop structure
			{
				
				order_++;
			}
			list_++;
		}
	}
	else if (module_ == MODULE_B)
	{
		
	}
}

bool RobotinoPlannerServer::acceptNewGoal(const robotino_planner::PlannerGoalConstPtr& goal)
{
	switch (goal->module)
	{
		case 0:
			module_ = MODULE_A;
			num_lists_ = 2;
			num_orders_ = 3;			
			list_ = 0;
			order_ = 0;
			break;
		case 1:
			module_ = MODULE_B;
			break;
		default:
			ROS_ERROR("Invalid Modulo: %d", goal->module);
	}
}

void RobotinoPlannerServer::hasArrived(const std_msgs::BoolConstPtr& msg)
{
	has_arrived_ = msg->data;
}

bool RobotinoPlannerServer::moveTo(robotino_planner::MoveTo::Request &req, robotino_planner::MoveTo::Response &res)
{
	has_arrived_ = false;
	//////
	return true;
}

int RobotinoPlannerServer::getProductCode(Product product)
{
	int product_code = 0;
	switch (product)
	{
		case NONE:
			product_code = 0;
			break;
		case TV:
			product_code = 1;
			break;
		case DVD:
			product_code = 2;
			break;
		case CELULAR:
			product_code = 3;
			break;
		case TABLET:
			product_code = 4;
			break;
		case NOTEBOOK:
			product_code = 5;
	}
	return product_code;
}

int RobotinoPlannerServer::getPlaceCode(Place place)
{
	int place_code = 0;
	switch (place)
	{
		case ORIGIN:
			place_code = 0;
			break;
		case SETOR_DE_CONTROLE:
			place_code = 1;
			break;
		case EXAMES:
			place_code = 2;
			break;
		case CENTRO_CIRURGICO:
			place_code = 3;
			break;
		case SETOR_DE_RECUPERACAO:
			place_code = 4;
			break;
		case SETOR_DE_SAIDA:
			place_code = 5;
	}
	return place_code;
}