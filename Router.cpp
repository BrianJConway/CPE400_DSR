#include "Router.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <random>

using namespace std;

void swap( string& one, string& other );

Host::Host( Router* r, vector<Router>* n )
   {
    router = r;
    network = n;
   }

void Host::stepSimulation()
   {
    // Generate hosts
    random_device rd;
    default_random_engine generator( rd() );
    uniform_int_distribution<int> dist1( 1, PACKET_RARITY );
    uniform_int_distribution<int> dist2( 0, network->size() - 1 );
    
    // Get random chance to send packet and get a destination router address
    int packetChance = dist1( generator );
    int destRouter = dist2( generator );
    
    while( destRouter == router->routerNum )
       {
        destRouter = dist2( generator );
       }
       
    string destAddress = network->at( destRouter ).address;
    
    if( packetChance == 1 )
       {
        // Generate packet and send to router
        Packet dataPacket( pidGen.getNextID(), PTYPE_DATA, router->address, destAddress );
        router->buffer.push( dataPacket );
       }
   }

Router::Router()
   {
    // Initialize data members
    xPos = 0;
    yPos = 0;
    degree = 0;
    avgXpos = 0;
    avgYpos = 0;
    routerNum = -1;
    
    address = ipGen.getNextAddress();
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
    address = RHS.address;
       
    seenPackets = RHS.seenPackets;
    waitingPackets = RHS.waitingPackets;
    buffer = RHS.buffer;
    routes = RHS.routes;

    network = RHS.network;
    ipGen = RHS.ipGen;
    pidGen = RHS.pidGen;
    
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
        address = RHS.address;
           
        seenPackets = RHS.seenPackets;
        waitingPackets = RHS.waitingPackets;
        buffer = RHS.buffer;
        routes = RHS.routes;

        network = RHS.network;
        ipGen = RHS.ipGen;
        pidGen = RHS.pidGen;
            
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
    
    // Loop through all hosts
    for( int index = 0; index < hosts.size(); index++ )
       {
        hosts[ index ].stepSimulation();
       }
    // end loop
        
    cout << " buffer size: " << buffer.size();
       
    if( !buffer.empty() )
       {
        cout << " BUFFER NOT EMPTY" << endl;
        
        // Process next packet in buffer
        Packet data = buffer.front();
        buffer.pop();
        
        // Check if data packet without a route to destination
        if( data.type == PTYPE_DATA && !( hasRoute( data.destAddress ) ) && data.destAddress != address )
           {
            // Store in waiting packet buffer
            waitingPackets.push_back( data );
            
            // Add route request to the destination router
            Packet request( pidGen.getNextID(), PTYPE_REQUEST, address, data.destAddress );
            seenPackets.push_back( request.packetID );
            request.route.path.push_back( address );
            processPacket( request );
           }
        else
           {
            if( data.type == PTYPE_DATA && data.srcAddress == address )
               {
                data.route.path = getRoute( data.destAddress );
               }           
               
            processPacket( data );
           }

       } 
   }
  
  
vector<string> Router::getRoute( string address )
   {
    string currentDest;
    
    // Loop through known routes
    for( set<Route>::iterator it = routes.begin(); it != routes.end(); it++ )
       {
        vector<string> route = it->path;
        
        // Get last address in current route's path
        currentDest = it->path.back();
        
        // Check if route ends with destination address
        if( currentDest == address )
           {
            return route;
           }
       }
    //end loop
   }
   
void Router::setNetwork( vector<Router>* n )
   {
    // Set network
    network = n;

    // Generate hosts
    random_device rd;
    default_random_engine generator( rd() );
    uniform_int_distribution<int> dist1( 1, 3 );
    int numHosts = dist1( generator );
    
    Host host( this, network );
    
    for( int index = 0; index < numHosts; index++ )
       {
        hosts.push_back( host );
       }
   }

void Router::processPacket( Packet data )
   {
    // initialize function/variables
    int index;
    
cout << "ROUTER " << address << "Processing Packet ID " << data.packetID << endl;

    // Process routes from packet if route request or route reply
    if( data.type != PTYPE_DATA )
       {       
        processRoutes( data );
       }
   
    // Check if we are destination
    if( data.destAddress == address )
       {
        // Check if packet was route request
        if( data.type == PTYPE_REQUEST )
           {
// TODO: DEBUG
cout << endl << "ROUTER " << address << "RECEIVED RREQ FROM " << data.srcAddress << endl;
            // Generate route reply
            data.type = PTYPE_REPLY;
            swap( data.srcAddress, data.destAddress );
            reverse( data.route.path.begin(), data.route.path.end() );       
          
        for( int index = 0; index < data.route.path.size(); index++ )
           {
            cout << data.route.path[ index ] << "  ";
           }
        cout << endl;
        
            // Send route reply to next router in route
            vector<string>::iterator it = find( data.route.path.begin(), data.route.path.end(), address );
            
            sendPacket( data, *( next( it ) ) );
           }
        // Otherwise, check if packet was returning route reply
        else if( data.type == PTYPE_REPLY )
           {      
// TODO: DEBUG
cout << "ROUTER " << address << "RECEIVED RREP FROM " << data.srcAddress << endl;

            // Check if there is a corresponding waiting packet
            for( vector<Packet>::iterator it2 = waitingPackets.begin(); it2 != waitingPackets.end(); it2++ )
               {
                cout << "loopin through waiting packets" << endl;
                
                // Check if destination of waiting packet matches src of route reply
                if( (*it2).destAddress == data.srcAddress )
                   {
                    cout << "found waiting packet that matches route" << endl;
                    reverse( data.route.path.begin(), data.route.path.end() );  
                         
                    for( int index = 0; index < data.route.path.size(); index++ )
                       {
                        it2->route.path.push_back( data.route.path[ index ] );
                       }
        
                    processPacket( *it2 );
                    waitingPackets.erase( it2 );
                    break;
                   }
               } 
            cout << "DONE PROCESSING ROUTE REPLY" << endl;
           }
           
        // Otherwise, assume it's a data packet
        else
           {
            cout << "ROUTER " << address << " RECEIVED DATA PACKET FROM " << data.srcAddress << endl;
           }
       }
    // Otherwise, check if route request
    else if( data.type == PTYPE_REQUEST )
       {        
// TODO: DEBUG
cout << "ROUTER " << address << " BROADCASTING RREQ DEST " << data.destAddress << endl;
        
        // Send to all neighbors
        broadcastPacket( data );
       }    
    // Otherwise, assume packet was returning route reply or data
    else
       {
        if( data.type == PTYPE_DATA )
           {
            cout << "PROCESSING DATA PACKET, ROUTE: " << endl;
            for( int index = 0; index < data.route.path.size(); index++ )
           {
            cout << data.route.path[ index ] << "  ";
           }
        cout << endl;
           } 
           
        // Send to next router in route
        vector<string>::iterator it = find( data.route.path.begin(), data.route.path.end(), address );      
        sendPacket( data, *( next( it ) ) );

       }
   }
   
void Router::processRoutes( Packet data )
   {
    // initialize function/variables
    int index = 0;
    
    // Get route from packet
    Route route = data.route;
    
    // Reverse route
    reverse( route.path.begin(), route.path.end() );
    
    // Loop through all valid sub-routes
    while( route.path.size() > 1 )
       {
        // Add current route to set 
        routes.insert( route );
        
        // Get next sub-route
        route.path.pop_back();
        route.length--;
       }
if( address == "192.168.5.0" )
   {
    cout << "DONE PROCESSING ROUTES ROUTER 5" << endl;
   }
   
   }
   
void Router::sendPacket( Packet data, string destAddress )
   {
    // Initialize function/variables
    int index;
    
    // Loop through all routers
    for( index = 0; index < network->size(); index++ )
       {
        // Check if current router matches destination address
        if( network->at( index ).address == destAddress )
           {
cout << "ROUTER " << address << " SENDING PACKET TO " << destAddress << endl;
            // Send packet to router
            network->at( index ).getPacket( data );
           }
       }
    // end loop
   }
   
void Router::broadcastPacket( Packet data )
   {
    int index;
    
cout << "ROUTER " << address << "START BROADCASTING" <<  endl;
    for( index = 0; index < network->size(); index++ )
       {
        if( neighbors[ index ] )
           {
cout << "ROUTER " << address << " BROADCASTING PACKET TO " << network->at(index).address <<  endl;
            network->at( index ).getPacket( data );
           }
       }
   }

void Router::getPacket( Packet data )
  {
    // Check if not discarding the packet
    if( find( seenPackets.begin(), seenPackets.end(), data.packetID ) == seenPackets.end() || data.type == PTYPE_REPLY || data.type == PTYPE_DATA ) // maybe also check if packet is a route reply
       {
        cout << "ROUTER " << address << " RECEIVED PACKET" << endl;
        
        // Add self to packet route if route request
        if( data.type == PTYPE_REQUEST )
           {           
            data.route.path.push_back( address );
            data.route.length++;
           }
        
        // Add packet to buffer
        buffer.push( data );

        // Add to seen packets
        seenPackets.push_back( data.packetID );
       }
  }   

bool Router::hasRoute( string address )
   {
    // initialize function/variables
    int index;
    bool result = false;
    string currentDest;
    
    // Loop through known routes
    for( set<Route>::iterator it = routes.begin(); it != routes.end(); it++ )
       {
        // Get last address in current route's path
        currentDest = it->path.back();
        
        // Check if route ends with destination address
        if( currentDest == address )
           {
            result = true;
           }
       }
    //end loop
    
    return result;
   }   

void swap( string& one, string& other )
   {
    string temp = one;
    one = other;
    other = temp;
   }
   
   
   
   
   
   
   
   
   
   
