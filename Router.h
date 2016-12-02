#ifndef ROUTER_H
#define ROUTER_H

#include <ostream>
#include <vector>
#include <string>
#include <queue>

using namespace std;

enum packetType
   {
    PTYPE_REQUEST
    PTYPE_REPLY
    PTYPE_DATA
   }

const double MIN_XPOS = 0.0;
const double MIN_YPOS = 0.0;
const double MAX_XPOS = 10.0;
const double MAX_YPOS = 10.0;

const double COMMUNICATION_RANGE = 3.50;

struct Packet
   {
    packetType type;
    string srcAddress;
    string destAddress;
    
    vector<string> Route;
   };

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
       void setNetwork( vector<Router>* n );
       
       // Routing simulation funcions
       void stepSimulation();
       
       // Data members
       double xPos, yPos;
       double avgXpos;
       double avgYpos;
       double degree;
       
       int routerNum;
       string address;
       
       vector<bool> neighbors;
       queue<Packet> buffer;
       
    private:
       void processNextPacket();
       void sendPacket( Packet data, string destAddress );
       void getPacket( Packet data );      
       
       vector<Router> network;
    
   };

#endif