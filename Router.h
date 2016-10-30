#ifndef ROUTER_H
#define ROUTER_H

#include <ostream>
#include <vector>

using namespace std;


const double MIN_XPOS = 0.0;
const double MIN_YPOS = 0.0;
const double MAX_XPOS = 10.0;
const double MAX_YPOS = 10.0;

const double COMMUNICATION_RANGE = 3.50;

class Router
   {
    public:
       // Constructors
       Router();
       Router( const Router& RHS );
       
       // Operator overload
       Router& operator=( const Router& RHS );
       friend ostream& operator<<( ostream& out, const Router& src );
       
       // Supplementary functions
       void placeRouter( int numRouter );
       void calcNeighbors( int sensorNum, vector<Router>& data );
       double calcDistance( double x1, double y1, double x2, double y2 );
       
       // Data members
       double xPos, yPos;
       double avgXpos;
       double avgYpos;
       double degree;
       
       int routerNum;
       
       vector<bool> neighbors;
       
    private:
    
   };

#endif
