#include "Router.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <random>

using namespace std;

Router::Router()
   {
    // Initialize data members
    xPos = 0;
    yPos = 0;
    degree = 0;
    avgXpos = 0;
    avgYpos = 0;
    routerNum = -1;
    
    // TODO: Statically retrieve router address
    
   }
   
Router::Router( const Router& RHS )
   {
    // initialize function/variables
    int index;
    
    // Copy data of other sensor
    xPos = RHS.xPos;
    yPos = RHS.yPos;
    avgXpos = RHS.avgXpos; 
    avgYpos = RHS.avgYpos;  
    degree = RHS.degree;
    routerNum = RHS.routerNum;
    
    for( index = 0; index < RHS.neighbors.size(); index++ )
       {
        neighbors.push_back( RHS.neighbors[ index ] );
       }
   }
  
Router& Router::operator=( const Router& RHS )
   {
    // initialize function/variables
    int index;
    
    // Check if assigning to itself
    if( this != &RHS )
       {
        // Copy data of other sensor
        xPos = RHS.xPos;
        yPos = RHS.yPos;
        degree = RHS.degree;
        avgXpos = RHS.avgXpos; 
        avgYpos = RHS.avgYpos; 
        routerNum = RHS.routerNum;
        
        neighbors.clear();
        
        for( index = 0; index < RHS.neighbors.size(); index++ )
           {
            neighbors.push_back( RHS.neighbors[ index ] );
           }
       }
    // return current object
    return *this;
   }

ostream& operator<<( ostream& out, const Router& src )
   {
    // initialization
    int index;
    double sum = 0;
    
    // Output sensor data
    cout << "Position: ( " << src.xPos << ", " << src.yPos << " )" << endl
         << "avg. xPos: " << src.avgXpos << endl
         << "avg. yPos: " << src.avgYpos << endl
         << "degree: " << src.degree << endl; 
         
       // Output which sensors are neighbors
       cout << "Neighbors: ";
       for( index = 0; index < src.neighbors.size(); index++ )
          {
           if( src.neighbors[ index ] == true )
              {
               cout << index << " ";
              }
          }
          
       cout << endl;
         
    // Return ostream object
    return out;
   }
   
void Router::placeRouter( int numRouter )
   {
    // Initialize function/variables
    random_device rd;
    default_random_engine generator( rd() );
    uniform_real_distribution<double> dist1( MIN_XPOS, MAX_XPOS );
    uniform_real_distribution<double> dist2( MIN_YPOS, MAX_YPOS );
    
    // Set sensor num
    routerNum = numRouter;
    
    // Generate position of sensor and distance from the target
    
       // Generate x coordinate
       xPos = dist1( generator );
       
       // Generate y coordinate
       yPos = dist2( generator );
   }
          

void Router::calcNeighbors( int routerNum, vector<Router>& data )
   {
    // initialize function/variables
    int index;
    bool isNeighbor;
    double distance;
    degree = 0;
    
    // Loop through all sensors
    for( index = 0; index < data.size(); index++ )
       {
        // Reset flag
        isNeighbor = false;
        
        // Check if not comparing to self
        if( index != routerNum )
           {
            // Calculate distance to other sensor
            distance = calcDistance( xPos, yPos, data[ index ].xPos, data[ index].yPos );
                                                           
            // Check if sensor is within communication range
            if( distance <= COMMUNICATION_RANGE )
               {
                // Set neighbor flag
                isNeighbor = true;
                degree++;
               }
           }    
           
        // Push neighbor flag to neighbor vector
        neighbors.push_back( isNeighbor );
       }
    // end loop
   }

double Router::calcDistance( double x1, double y1, double x2, double y2 )
   {
    // initialize function/variables
    double x = pow( x2 - x1, 2 );
    double y = pow( y2 - y1, 2 );
    double dist;
    
    // Calculate euclidean distance
    dist = sqrt( x + y );
    
    // return result
    return dist;
   }
   
void Router::stepSimulation()
   {
    if( !buffer.empty() )
       {
        processNextPacket();
       }
   }
   
void Router::setNetwork( vector<Router>* n )
   {
    network = n;
   }

void processNextPacket()
   {
   
   }
   
void sendPacket( Packet data, string destAddress )
   {
   
   }

void getPacket( Packet data )
  {
   
  }   
   
   
   
   
   
   
   
   
   
   
   
