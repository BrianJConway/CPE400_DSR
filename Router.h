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

// Differentiates between data packets, router requests, and route replies
enum packetType
   {
    PTYPE_REQUEST,
    PTYPE_REPLY,
    PTYPE_DATA
   };

// Min and max possible router locations
const double MIN_XPOS = 0.0;
const double MIN_YPOS = 0.0;
const double MAX_XPOS = 10.0;
const double MAX_YPOS = 10.0;

// Range of the router
const double COMMUNICATION_RANGE = 3.50;

// Determines how likely a host is to generate a packet, higher is less likely
const int PACKET_RARITY = 100;

// Describes a route from one router address to another
struct Route
   {
    // Number of hops
    int length;
    
    // Addresses along the route
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

// Generates unique router addresses
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

// Generates unique packet ID numbers
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
       void calcNeighbors( int routerNum, vector<Router>& data );
       double calcDistance( double x1, double y1, double x2, double y2 );
       void setNetwork( vector<Router>* n );
       
       // Routing simulation funcions
       void stepSimulation();
       
       
       
       // Location
       double xPos, yPos;
       double avgXpos;
       double avgYpos;
       
       // Number of neighbors
       double degree;
       
       // Router index
       int routerNum;
       
       // IP address
       string address;
       
       // Flags that indicate which routers are neighbors of current router
       vector<bool> neighbors;

       // Reputations of router's neighbors
       vector<int> reputations;       
       
       // Gene sequencce of router
       vector<char> geneSeq;

       // Vector of packet ID's so we don't process the same route requests
       vector<int> seenPackets;
       
       // Packets that are put aside temporarily until a route is routed
       vector<Packet> waitingPackets;
       
       // Packets waiting to be processed
       queue<Packet> buffer;
       
       // Routes obtained by processing route requests and replies
       set<Route> routes;
       
    private:
       // Process a single packet ( route request, route reply, data )
       void processPacket( Packet data );
       
       // Send packet to a specific neighbor based on their IP address
       void sendPacket( Packet data, string destAddress );
       
       // Receive a packet and determine if it should be added to your buffer
       void getPacket( Packet data );      
       
       // Extract all useful routes from the packet
       void processRoutes( Packet data );
       
       // Send packet to all neighbors
       void broadcastPacket( Packet data );
       
       // Check if router has a route to the router at the specified IP address
       bool hasRoute( string address );
       
       // Update reputations of all neighbors and eliminate faulty neighbors
       void checkReputations();
       
       // Get rid of all routes containing the router at the specified index
       void deleteRoutes( int routerIndex );
       
       
       
       
       
       // Get the shortest route to the router at the IP address
       vector<string> getRoute( string address );
       
       // All hosts connected to the router
       vector<Host> hosts;
       
       // Pointer to the network as a whole
       vector<Router>* network;
       
       // Generators for IP addresses and packet ID's
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
