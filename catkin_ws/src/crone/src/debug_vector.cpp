#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/CommandTOL.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/DebugValue.h>
#include <std_msgs/String.h>
#include <math.h>  
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;


mavros_msgs::State current_state;
void state_cb(const mavros_msgs::State::ConstPtr& msg){
    current_state = *msg;
}

std_msgs::String current_cow;
void cow_cb(const std_msgs::String::ConstPtr& msg){
    current_cow = *msg;
}


int main(int argc, char **argv)
{


    ros::init(argc, argv, "debug_vector");
    ros::NodeHandle nh;

    // Suscriptor del topico mavros/state 
    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>
            ("mavros/state", 10, state_cb);
    // Suscriptor del topico cows_found (rosserial arduino)
    ros::Subscriber rosserial_sub = nh.subscribe<std_msgs::String>
            ("chatter", 10, cow_cb);

    // Publica en el topico DEBUG_VECT        
    ros::Publisher debug_pub = nh.advertise<mavros_msgs::DebugValue>
            ("mavros/debug_value/send", 20);

    ros::Rate rate(20.0);

    // Espera a tener conexión con la Pixhawk
    // UNCOMMENT THIS!!!!!
    while(ros::ok() && !current_state.connected){
        ros::spinOnce();
        rate.sleep();
    }

    // Para almacenar informacion de la vaca encontrada 
    mavros_msgs::DebugValue debb;
    debb.header.stamp = ros::Time::now();
    debb.header.frame_id = ""; //ID de dron
    debb.name = "COW0";
    debb.type = 1;  // mensaje del tipo DEBUG_VECT
    debb.index = -1;
    debb.data = {0.0, 0.0, 0.0} ;

    string north, east;
    int cont_mal = 0;

    while(ros::ok()){

	    debb.header.stamp = ros::Time::now();
	    if (current_cow.data.substr(0,3) == "COW") {
		
		int error = 0;
		for (int i = 7; i <= 15; i++){
			if (!isdigit(current_cow.data[i]))
				error++;		
		}
		for (int i = 19; i <= 27; i++){
			if (!isdigit(current_cow.data[i]))
				error++;		
		}
		
		if (error == 0){
			try{
				debb.name = current_cow.data.substr(0,4);
				north = current_cow.data.substr(5,1);
				debb.data[0] = stof(current_cow.data.substr(7,2) + "." + current_cow.data.substr(9,7));
				if ( north == "S")
					debb.data[0] *= -1.0;
				east = current_cow.data.substr(17,1);
				debb.data[1] = stof(current_cow.data.substr(19,2) +  "." + current_cow.data.substr(21,7));
				if ( east == "W")
					debb.data[1] *= -1.0;
				
			}
			catch(exception a){
				cont_mal++;
				cout <<"Cadena mala: "<< cont_mal <<endl;
			}

		}
		


	    }
          
        debug_pub.publish(debb); // Publica el DEBUG_VECTOR
        ros::spinOnce();
        rate.sleep();
    }
    return 0;
}
