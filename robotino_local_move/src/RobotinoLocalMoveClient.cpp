/*
 * RobotinoLocalMoveClient.cpp
 *
 *  Created on: 14.12.2011
 *      Author: indorewala@servicerobotics.eu
 */

#include "RobotinoLocalMoveClient.h"

RobotinoLocalMoveClient::RobotinoLocalMoveClient():
	client_("local_move", false),
	max_time_(1.0)
{
}

RobotinoLocalMoveClient::~RobotinoLocalMoveClient()
{
}

bool RobotinoLocalMoveClient::checkServer()
{
	for (int i = 0; i < 5; ++i)
	{
		ros::spinOnce();
		if (client_.waitForServer(ros::Duration(1.0)))
		{
			ROS_INFO("Connected to the local move server");
			return true;
		}
		else
			ROS_INFO("Waiting for local move server");
	}

	ROS_ERROR("local move server not running");
	return false;
}

int RobotinoLocalMoveClient::spin()
{
	ros::Rate loop_rate(5);
	ros::Time start_time = ros::Time::now();

	while (nh_.ok())
	{
		if (client_.getState() == actionlib::SimpleClientGoalState::RECALLED || client_.getState() == actionlib::SimpleClientGoalState::PREEMPTED)
			return 0; // Aborted
		else if (client_.waitForResult(ros::Duration(1.0)))
		{
			ROS_INFO("Local move succeeded");
			return 1; // Succeeded
		}
		else
			ROS_INFO("Local move is being executed");

		if ((ros::Time::now() - start_time).toSec() > max_time_)
		{
			ROS_INFO("Aborting Local Move");
			client_.cancelAllGoals();
			return 2; // Timeout
		}

		ros::spinOnce();
		loop_rate.sleep();
	}
}

void RobotinoLocalMoveClient::setMaxTime(const float& time)
{
	max_time_ = time;
}

void RobotinoLocalMoveClient::sendGoal(const robotino_local_move::LocalMoveGoal& goal)
{
	client_.sendGoal(goal);
	ROS_INFO("Goal sent");
}

void RobotinoLocalMoveClient::cancelGoal()
{
	ROS_INFO("Aborting Local move");
	client_.cancelAllGoals();
}
