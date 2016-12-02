#include <cstdlib>
#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include "Router.h"
#include "Router.cpp"

using namespace std;

const int NUM_ROUTERS = 10;
const int NUM_ITERATIONS = 100;

void calcAvgVector( vector<Router>& data );
bool isConnected( vector<Router>& data );
void outputToFile( vector<Router>& data );

int main()
   {
    // Initialization
    int index, index2, routerIndex;
    vector<Router> network( NUM_ROUTERS );

       // Loop until a connected graph is generated
       do
          {
           // Loop through all routers to generate positions
           for( index = 0; index < NUM_ROUTERS; index++ )
              {             
               // Generate position of current router
               network[ index ].placeRouter( index );
              }
           // end loop
           
           // Loop through all routers to check neighbors
           for( index = 0; index < NUM_ROUTERS; index++ )
              {
               // clear neighbors
               network[ index ].neighbors.clear();
               
               // Calculate neighbors of cuurent router
               network[ index ].calcNeighbors( index, network );        
               
               // Give each router a pointer to the overall network      
               network[ index ].setNetwork( &network );
              }
           // end loop
          }
       while( !isConnected( network ) );
       // end loop
       
       // calculate average vector
       calcAvgVector( network );
          
    // Loop specified number of times for simulation
    for( index = 0; index < NUM_ITERATIONS; index++ )
       {
        // Loop through all routers
        for( routerIndex = 0; routerIndex < network.size(); routerIndex++ )
           {
            network[ index ].stepSimulation();
           }
        // end inner loop
       }
    // end outer loop
    
// TESTING: Output all router information 
for( index = 0; index < network.size(); index++ )
   {
    cout << network[ index ] << endl;
   }
// TESTING: Output all router information 

    // Output measurements
    outputToFile( network );

    // Exit program
    return 0;
   }
   

void calcAvgVector( vector<Router>& data )
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
          
       
    // Set average for all routers, also set router num
    for( index = 0; index < data.size(); index++ )
       {
        data[ index ].avgXpos = sumX / (double) data.size();
        data[ index ].avgYpos = sumY / (double) data.size();
       }
   }
   
bool isConnected( vector<Router>& data )
   {
    // Initialize function/variables
    stack<Router> vertices;
    Router vertex;
    bool nodesVisited[ NUM_ROUTERS ];
    int index;
    bool isConnected = true;
    
        // initialize nodes visited array
        for( index = 0; index < NUM_ROUTERS; index++ )
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
           if( !( nodesVisited[ vertex.routerNum ] ) )
              {
               // label vertex as discovered
               nodesVisited[ vertex.routerNum ] = true;
               
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
       for( index = 0; index < NUM_ROUTERS; index++ )
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

void outputToFile( vector<Router>& data )
	{
	// Initialize variables
	int index, index2 = 0, index3;
    ofstream fout;
    
	// Open "measurements.txt"
	fout.open("measurements.csv");

	// Begin loading file with measurement data
	for( index = 0; index < data.size(); index++ )
		{
		
		}
	fout.close();
	}

  