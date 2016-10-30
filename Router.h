#ifndef SENSOR_H
#define SENSOR_H

#include <ostream>
#include <vector>

using namespace std;


const double MIN_XPOS = 0.0;
const double MIN_YPOS = 0.0;
const double MAX_XPOS = 4.0;
const double MAX_YPOS = 4.0;

const double TARGET_XPOS = 2.0;
const double TARGET_YPOS = 2.0;

const double GROUND_TRUTH = 50;

const double SENSOR_RANGE = 3.50;
const double COMMUNICATION_RANGE = 3.50;

const double CONSTANT_VALUE = 0.001;

class Sensor
   {
    public:
       // Constructors
       Sensor();
       Sensor( const Sensor& RHS );
       
       // Operator overload
       Sensor& operator=( const Sensor& RHS );
       friend ostream& operator<<( ostream& out, const Sensor& src );
       
       // Supplementary functions
       void placeSensor( int numSensor );
       void calcMeasurement();
       void calcVariance();
       void calcNeighbors( int sensorNum, vector<Sensor>& data );
       void initializeWeights( int sensorNum, vector<Sensor>& data, const int design );
       void calcWeights( int sensorNum, vector<Sensor>& data, const int design );
       double calcDistance( double x1, double y1, double x2, double y2 );
       void calcNextMeasurement( int sensorNum, vector<Sensor>& data );
       
       // Data members
       double xPos, yPos;
       double measurement;
       double estimation;
       double variance;
       double dist;
       double avgXpos;
       double avgYpos;
       double degree;
       double weightConst;
       
       int sensorNum;
       
       vector<bool> neighbors;
       vector<double> measurements;
       vector<double> weights;
       
    private:
    
   };

#endif
