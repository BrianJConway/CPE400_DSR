#include "Sensor.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <random>

using namespace std;

Sensor::Sensor()
   {
    // Initialize data members
    xPos = 0;
    yPos = 0;
    estimation = 0;
    variance = 0;
    degree = 0;
    weightConst = -1;
    avgXpos = 0;
    avgYpos = 0;
    sensorNum = -1;
   }
   
Sensor::Sensor( const Sensor& RHS )
   {
    // initialize function/variables
    int index;
    
    // Copy data of other sensor
    xPos = RHS.xPos;
    yPos = RHS.yPos;
    measurement = RHS.measurement;
    estimation = RHS.estimation;
    variance = RHS.variance;
    dist = RHS.dist;
    avgXpos = RHS.avgXpos; 
    avgYpos = RHS.avgYpos;  
    weightConst = RHS.weightConst;    
    degree = RHS.degree;
    sensorNum = RHS.sensorNum;
    
    for( index = 0; index < RHS.neighbors.size(); index++ )
       {
        neighbors.push_back( RHS.neighbors[ index ] );
       }
    for( index = 0; index < RHS.measurements.size(); index++ )
       {
        measurements.push_back( RHS.measurements[ index ] );       
       }
    for( index = 0; index < RHS.weights.size(); index++ )
       {
        weights.push_back( RHS.weights[ index ] );       
       }
   }
  
Sensor& Sensor::operator=( const Sensor& RHS )
   {
    // initialize function/variables
    int index;
    
    // Check if assigning to itself
    if( this != &RHS )
       {
        // Copy data of other sensor
        xPos = RHS.xPos;
        yPos = RHS.yPos;
        measurement = RHS.measurement;
        estimation = RHS.estimation;
        variance = RHS.variance;
        dist = RHS.dist;
        degree = RHS.degree;
        avgXpos = RHS.avgXpos; 
        avgYpos = RHS.avgYpos; 
        weightConst = RHS.weightConst;          
        sensorNum = RHS.sensorNum;
        
        neighbors.clear();
        measurements.clear();
        weights.clear();
        
        for( index = 0; index < RHS.neighbors.size(); index++ )
           {
            neighbors.push_back( RHS.neighbors[ index ] );
           }
        for( index = 0; index < RHS.measurements.size(); index++ )
           {
            measurements.push_back( RHS.measurements[ index ] );       
           }
        for( index = 0; index < RHS.weights.size(); index++ )
           {
            weights.push_back( RHS.weights[ index ] );       
           }
       }
    // return current object
    return *this;
   }

ostream& operator<<( ostream& out, const Sensor& src )
   {
    // initialization
    int index;
    double sum = 0;
    // Output sensor data
    cout << "Position: ( " << src.xPos << ", " << src.yPos << " )" << endl
         << "measurement: " << src.measurement << endl
         << "estimation: " << src.estimation << endl 
         << "variance: " << src.variance << endl 
         << "distance: " << src.dist << endl 
         << "avg. xPos: " << src.avgXpos << endl
         << "avg. yPos: " << src.avgYpos << endl
         << "degree: " << src.degree << endl
         << "weight constant: " << src.weightConst << endl; 
         
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
       // Output weights
       cout << "Weights: ";
       for( index = 0; index < src.weights.size(); index++ )
          {
           cout << src.weights[ index ] << " ";
           sum += src.weights[ index ];
          }
       cout << endl << "Sum weights: " << sum << endl;
         
    // Return ostream object
    return out;
   }
   
void Sensor::placeSensor( int numSensor )
   {
    // Initialize function/variables
    random_device rd;
    default_random_engine generator( rd() );
    uniform_real_distribution<double> dist1( MIN_XPOS, MAX_XPOS );
    uniform_real_distribution<double> dist2( MIN_YPOS, MAX_YPOS );
    
    // Set sensor num
    sensorNum = numSensor;
    
    // Generate position of sensor and distance from the target
    
       // Generate x coordinate
       do
          {
           xPos = dist1( generator );
          }
       while( xPos == TARGET_XPOS );
       
       // Generate y coordinate
       do
          {
           yPos = dist2( generator );
          }
       while( yPos == TARGET_YPOS );
       
       // Generate distance 
       dist = calcDistance( xPos, yPos, TARGET_XPOS, TARGET_YPOS );
   }
          
void Sensor::calcVariance()
   {
    // initialize function/variables
    double diffX = xPos - avgXpos;
    double diffY = yPos - avgYpos;
    
    // Calculate variance
    if( dist <= SENSOR_RANGE )
       {
        variance = sqrt( pow( diffX , 2 ) + pow( diffY, 2 ) );
        variance = pow( variance, 2 );
        variance = ( variance + CONSTANT_VALUE ) / pow( SENSOR_RANGE, 2 );
       }
    else
       {
        variance = 0;
       } 
   }
   
void Sensor::calcMeasurement()
   {
    // intitialize function/variables
    random_device rd;
    default_random_engine generator( rd() );
    normal_distribution<double> dist( 0, variance );
    
    // Calculate estimation with gaussian noise
    measurement = GROUND_TRUTH + dist( generator );
    estimation = measurement;
   }
    
void Sensor::calcNeighbors( int sensorNum, vector<Sensor>& data )
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
        if( index != sensorNum )
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
   
void Sensor::initializeWeights( int sensorNum, vector<Sensor>& data, const int design )
   {
    // initialize function/variables
    random_device rd;
    default_random_engine generator( rd() );
    int index;
    double maxValue;
    
    // Loop through all sensors
    for( index = 0; index < data.size(); index++ )
       {
        // Check if weighing self
        if( index == sensorNum )
           {
            weights.push_back( 1 );
           }
        // Otherwise, assume not self weight
        else
           {
            weights.push_back( 0 );
           }
       } 
    // end loop
   }
    

void Sensor::calcWeights( int sensorNum, vector<Sensor>& data, const int design )
   {
    // initialize function/variables
    int index, index2;
    double weight = 0;
    double sum = 0;

    // Check if max degree weight design 
    if( design == 1 )
       {
        // Loop through each sensor
        for( index = 0; index < data.size(); index++ )
           {
            // Reset weight to zero for case when i =/= j and i,j not neighbors 
            weight = 0;
            
            // Otherwise, check if nodes are neighbors
            if( neighbors[ index ] )
               {
                // Calculate weight
                weight = 1 / ( double ) data.size(); 
               }
               
            // update weight value
            weights[ index ] = weight;
           }
        // end loop
        
        // Calculate self weight
        weights[ sensorNum ] = 1 - ( degree / ( double ) data.size() );
       }
    // Otherwise, assume metropolis design 
    else
       {
        // Loop through each sensor
        for( index = 0; index < data.size(); index++ )
           {
            // Reset weight to zero for case when i =/= j and i,j not neighbors 
            weight = 0;

            // Otherwise, check if nodes are neighbors
            if( neighbors[ index ] )
               {
                // Calculate weight
                weight = ( 1.00 / ( 1 + max( degree, data[ index ].degree ) ) );
               }
               
            // update weight value
            weights[ index ] = weight;
           }
        // end loop
        
        // Calculate self weight
           
           // Loop through all sensors
           for( index = 0; index < weights.size(); index++ )
              {
               // Check if neighbor with current sensor
               if( neighbors[ index ] )
                  {
                   // Add weight of neighbor to sum
                   sum += weights[ index ];
                  }
              }
              
           // Set self weight
           weights[ sensorNum ] = 1 - sum;
       }
   }

double Sensor::calcDistance( double x1, double y1, double x2, double y2 )
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
   
void Sensor::calcNextMeasurement( int sensorNum, vector<Sensor>& data )
   {
    // initialize function/variables
    double sum = 0;
    int index;
    
    // Calculate sum
    for( index = 0; index < data.size(); index++ )
       {
        if( neighbors[ index ] )
           {
            sum += weights[ index ] * data[ index ].estimation;
           }
       }
       
    // Calculate estimation
    estimation = estimation * weights[ sensorNum ] + sum; 
   }
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
