#ifndef ROUTER_H
#define ROUTER_H

#include <ostream>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <algorithm>

using namespace std;

class Host;

enum packetType
   {
    PTYPE_REQUEST,
    PTYPE_REPLY,
    PTYPE_DATA
   };

const double MIN_XPOS = 0.0;
const double MIN_YPOS = 0.0;
const double MAX_XPOS = 10.0;
const double MAX_YPOS = 10.0;

const double COMMUNICATION_RANGE = 3.50;

const int PACKET_RARITY = 1000;

struct Route
   {
    int length;
    vector<string> path;
    
    // Used for sorting routes by length
    bool operator<(Route other) const
       {
        return length < other.length;
       }  

    Route& operator=(const Route& other)
       {
        length = other.length;
        
        path.clear();
        
        for( int index = 0; index < path.size(); index++ )
           {
            path.push_back( other.path[ index ] );
           }
       }  
   };

struct Packet
   {
    int packetID;
    packetType type;
    string srcAddress;
    string destAddress;
    
    Route route;

    Packet(int p, packetType t, string s, string d): packetID(p), type(t), srcAddress(s), destAddress(d) {}
   };

class addressGenerator
   {
    public:
       static int count;

       string getNextAddress()
          {  
           string result = "192.168." + to_string( count ) + ".0";
            
           count++;
            
           return result; 
          }
   };

class packetIDGenerator
   {
    public:
       static int count;

       int getNextID()
          {
           count++;
           return count;
          }
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

       vector<int> reputations;       
       vector<char> geneSeq;

       vector<bool> neighbors;
       vector<int> seenPackets;
       vector<Packet> waitingPackets;
       queue<Packet> buffer;
       set<Route> routes;
       
    private:
       void processPacket( Packet data );
       void sendPacket( Packet data, string destAddress );
       void getPacket( Packet data );      
       void processRoutes( Packet data );
       void broadcastPacket( Packet data );
       bool hasRoute( string address );
       void checkReputations();
       void deleteRoutes( int routerIndex );
       vector<string> getRoute( string address );
       vector<Host> hosts;
       
       vector<Router>* network;
       addressGenerator ipGen;
       packetIDGenerator pidGen;
   };

class Host
   {
    public:
       Host( Router* r, vector<Router>* n );
       void stepSimulation();
       
    private:   
       Router* router; 
       vector<Router>* network;
       packetIDGenerator pidGen;
   };

#endif
