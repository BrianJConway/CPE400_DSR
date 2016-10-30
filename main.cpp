#include <cstdlib>
#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include "Router.h"
#include "Router.cpp"

using namespace std;

const int NUM_SENSORS = 10;
const int NUM_ITERATIONS = 100;
const int WEIGHT_DESIGN = 2;

void calcAvgVector( vector<Sensor>& data );
bool isConnected( vector<Sensor>& data );
void outputToFile( vector<Sensor>& data );

int main()
   {
    // Initialization
    int index, index2, sensorIndex;
    vector<Sensor> sensors( NUM_SENSORS );

       // Loop until a connected graph is generated
       do
          {
           // Loop through all sensors to generate positions
           for( index = 0; index < NUM_SENSORS; index++ )
              {             
               // Generate position of current sensor
               sensors[ index ].placeSensor( index );
              }
           // end loop
           
           // Loop through all sensors to check neighbors
           for( index = 0; index < NUM_SENSORS; index++ )
              {
               // clear neighbors
               sensors[ index ].neighbors.clear();
               
               // Calculate neighbors of cuurent sensor
               sensors[ index ].calcNeighbors( index, sensors );
              }
           // end loop
          }
       while( !isConnected( sensors ) );
       // end loop
       
       // calculate average vector
       calcAvgVector( sensors );
       
       // Calculate variance, measurements, and initialize weights 
       for( index = 0; index < sensors.size(); index++ )
          {
           sensors[ index ].calcVariance();           
           sensors[ index ].calcMeasurement();
           sensors[ index ].initializeWeights( index, sensors, WEIGHT_DESIGN );
          }
          
    // Loop specified number of times for consensus filter
    for( index = 0; index < NUM_ITERATIONS; index++ )
       {
        // Recalculate weights for all sensors
        for( index2 = 0; index2 < sensors.size(); index2++ )
           {
            sensors[ index2 ].calcWeights( index2, sensors, WEIGHT_DESIGN );
           }
  
        // Loop through all sensors
        for( sensorIndex = 0; sensorIndex < sensors.size(); sensorIndex++ )
           {
            // Record current measurement
            sensors[ sensorIndex ].measurements.push_back(
                                            sensors[ sensorIndex ].estimation);
                                                 
            // Update measurement
            sensors[ sensorIndex ].calcNextMeasurement( sensorIndex, sensors );
           }
        // end inner loop
       }
    // end outer loop
    
// TESTING: Output all sensor information 
for( index = 0; index < sensors.size(); index++ )
   {
    cout << sensors[ index ] << endl;
   }
// TESTING: Output all sensor information 

    // Output measurements
    outputToFile( sensors );

    // Exit program
    return 0;
   }
   

void calcAvgVector( vector<Sensor>& data )
   {
    // initialize function/variables
    int index;
    double sumX = 0;
    double sumY = 0;
    double avgX, avgY;
    
    // Calculate average position
       
       // Calculate sum of all x and y positions
       for( index = 0; index < data.size(); index++ )
          {
           sumX += data[ index ].xPos;
           sumY += data[ index ].yPos;
          }
          
       
    // Set average for all sensors, also set sensor num
    for( index = 0; index < data.size(); index++ )
       {
        data[ index ].avgXpos = sumX / (double) data.size();
        data[ index ].avgYpos = sumY / (double) data.size();
       }
   }
   
bool isConnected( vector<Sensor>& data )
   {
    // Initialize function/variables
    stack<Sensor> vertices;
    Sensor vertex;
    bool nodesVisited[ NUM_SENSORS ];
    int index;
    bool isConnected = true;
    
        // initialize nodes visited array
        for( index = 0; index < NUM_SENSORS; index++ )
           {
            nodesVisited[ index ] = false;
           }

    // Do a depth-first traversal of the graph
    
       // Push first vertex onto the stack
       vertices.push( data[ 0 ] );
       
       // Loop while stack is not empty
       while( !vertices.empty() )
          {
           // Pop current vertex
           vertex = vertices.top();
           vertices.pop();
           
           // check if vertex was not visited
           if( !( nodesVisited[ vertex.sensorNum ] ) )
              {
               // label vertex as discovered
               nodesVisited[ vertex.sensorNum ] = true;
               
               // Loop through all neighbors 
               for( index = 0; index < vertex.neighbors.size(); index++ )
                  {
                   // Check if current vertex is a neighbor
                   if( vertex.neighbors[ index ] )
                      {
                       // Push neighbor onto stack
                       vertices.push( data[ index ] );
                      }
                  }
               // end loop
              }
          }
       // end loop
    
    // Check if all nodes were visited

       // loop through nodes visited array
       for( index = 0; index < NUM_SENSORS; index++ )
          {
           // Check if node was not visited
           if( nodesVisited[ index ] == false )
              {
               // Set return value to false
               isConnected = false;
              }
          }
       // end loop
       
    // return whether graph is connected
    return isConnected;
   }

void outputToFile( vector<Sensor>& data )
	{
	// Initialize variables
	int index, index2 = 0, index3;
    ofstream fout;
    
	// Open "measurements.txt"
	fout.open("measurements.csv");

	// Begin loading file with measurement data
	for( index = 0; index < data.size(); index++ )
		{
		// Prime the loop to avoid extra comma at the end
		fout << data[ index ].measurements[ index2 ];
		for( index3 = 1; index3 < NUM_ITERATIONS; index3++ )
			{
			fout  << ", " << data[ index ].measurements[ index3 ];
			}

		fout << endl;
		}
	fout.close();
	}

  
