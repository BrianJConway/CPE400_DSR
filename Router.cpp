#include "Router.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <random>

using namespace std;

void swap( string& one, string& other );

// Host Constructor
Host::Host( Router* r, vector<Router>* n )
   {
    router = r;
    network = n;
   }

void Host::stepSimulation()
   {
    // Initialization
    random_device rd;
    default_random_engine generator( rd() );
    
    // Generates integers between 1 and PACKET_RARITY constant
    uniform_int_distribution<int> dist1( 1, PACKET_RARITY );
    
    // Generates random integer for the index of a router
    uniform_int_distribution<int> dist2( 0, network->size() - 1 );
    
    // Get random chance to send packet and get a destination router address
    int packetChance = dist1( generator );
    int destRouter = dist2( generator );
    
    // Ensure packet's destination isn't the host's router 
    while( destRouter == router->routerNum )
       {
        destRouter = dist2( generator );
       }
       
    // Get the address of the destination router based on the router's index in the network
    string destAddress = network->at( destRouter ).address;
    
    // If randomly generated number based on rarity constant was 1, send the packet to the router
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
    reputations.reserve(10);
    
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
         << "initial degree: " << src.degree << endl
         << "number of routes: " << src.routes.size() << endl
         << "number of hosts connected to router: " << src.hosts.size() << endl;
         
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
       
       // Output final gene sequence
       cout << "Final gene sequence: ";
       for( index = 0; index < src.geneSeq.size(); index++ )
          {
           cout << src.geneSeq[ index ];
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
    
    // Set router num
    routerNum = numRouter;
    
    // Generate position of router
    
       // Generate x coordinate
       xPos = dist1( generator );
       
       // Generate y coordinate
       yPos = dist2( generator );
   }
          

void Router::calcNeighbors( int routerNum, vector<Router>& data )
   {
    // initialize function/variables
    int index;
    int reputation;
    bool isNeighbor;
    double distance;
    degree = 0;
    
    // Loop through all routers
    for( index = 0; index < data.size(); index++ )
       {
        // Reset flag
        isNeighbor = false;
        
        // Check if not comparing to self
        if( index != routerNum )
           {
            // Calculate distance to other router
            distance = calcDistance( xPos, yPos, data[ index ].xPos, data[ index].yPos );
                                                           
            // Check if router is within communication range
            if( distance <= COMMUNICATION_RANGE )
               {
                // Set neighbor flag
                isNeighbor = true;
		reputations[index] = 100;
                degree++;
               }
	    else
		{
		reputations[index] = 0;
		}
           }    
           
        // Push neighbor flag to neighbor vector
        neighbors.push_back( isNeighbor );
	//reputations.push_back( reputation );
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
        // Host may or may not send router a packet to a random other router
        hosts[ index ].stepSimulation();
       }
    // end loop
        
    cout << " Buffer Size: " << buffer.size() << endl;

	// Add check to update reputations of all neighbors in the network
	checkReputations();
       
    // Check if there is a packet in the buffer
    if( !buffer.empty() )
       {
        // Process next packet in buffer
        Packet data = buffer.front();
        buffer.pop();
        
        // Check if data packet without a route to destination
        if( data.type == PTYPE_DATA && !( hasRoute( data.destAddress ) ) && data.destAddress != address )
           {
            // Store in waiting packet buffer since we don't have a route yet
            waitingPackets.push_back( data );
            
            // Add and process a route request to the packet's destination router to obtain a route
            Packet request( pidGen.getNextID(), PTYPE_REQUEST, address, data.destAddress );
            seenPackets.push_back( request.packetID );
            request.route.path.push_back( address );
            processPacket( request );
           }
        // Otherwise, assume packet is either a data packet with a route, a route reply, or a route request
        else
           {
            // Check if packet is a data packet from the current router
            if( data.type == PTYPE_DATA && data.srcAddress == address )
               {
                // Attach route to data packet
                data.route.path = getRoute( data.destAddress );
               }           
               
            // Process the packet
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
    hosts.clear();
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
    
cout << "ROUTER " << address << " Processing Packet ID " << data.packetID << endl;

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
	   cout << "ROUTER " << address << " RECEIVED RREQ FROM " << data.srcAddress << endl << endl;
           
	   // Generate route reply
           data.type = PTYPE_REPLY;
           swap( data.srcAddress, data.destAddress );
           reverse( data.route.path.begin(), data.route.path.end() );       
          
	   // Show the path the packet is traveling
	   cout << "Sending packet back to source along the path:" << endl;
           for( int index = 0; index < data.route.path.size(); index++ )
           	{
           	cout << data.route.path[ index ] << "  ";
           	}
           cout << endl << endl;
        
            // Send route reply to next router in route
            vector<string>::iterator it = find( data.route.path.begin(), data.route.path.end(), address );
            
	    // Add P for reply to gene sequence
	    geneSeq.push_back( 'P' );


            sendPacket( data, *( next( it ) ) );
           }
        // Otherwise, check if packet was returning route reply
        else if( data.type == PTYPE_REPLY )
           {      
	// TODO: DEBUG
	cout << "ROUTER " << address << " RECEIVED RREP FROM " << data.srcAddress << endl;

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
        
	// Add R for request to gene sequence
	geneSeq.push_back( 'R' );

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

	   // Initialization of random generator
	   random_device rd;
	   default_random_engine generator( rd() );

	   // Generates integers between 1 and 5
	   uniform_int_distribution<int> chance1( 1, 5 );	
	   int dataPacketChance = chance1( generator );

	   // If the chance is rolled, the data packet is lost
	   if( dataPacketChance == 1 )
		{
		cout << endl << "This packet was lost, Q added" << endl;
		geneSeq.push_back( 'Q' );
		}
	   // Otherwise add a D for data to represent it was sent successfully
	   else
		{
		geneSeq.push_back( 'D' );
		// Send to next router in route
		vector<string>::iterator it = find( data.route.path.begin(), data.route.path.end(), address );      
		sendPacket( data, *( next( it ) ) );
		}
           }
           
	// If the data was a reply, push a P onto the gene sequence
	if( data.type == PTYPE_REPLY )
		{
		geneSeq.push_back( 'P' );
		// Send to next router in route
		vector<string>::iterator it = find( data.route.path.begin(), data.route.path.end(), address );      
		sendPacket( data, *( next( it ) ) );
		}
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
    
cout << "ROUTER " << address << " START BROADCASTING" <<  endl;
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
   
  
void Router::checkReputations()
	{

	// Initialize variables
	int Qcounter;

	// Loop through all routers in the network to adjust the reputation between nodes
	for( int index = 0; index < network->size(); index++ )
		{
		Qcounter = 0;

		if( neighbors[ index ] )
			{
			for( int i = 0; i < network->at( index ).geneSeq.size(); i++ )
				{
				if( network->at( index ).geneSeq[i] == 'Q' )
					{
					Qcounter++;
					}
				}

			// Adjust reputation only if there is actually a gene sequence to check
			if( network->at( index ).geneSeq.size() != 0 )
				{
				reputations[index] = 100 - ((Qcounter*100)/network->at( index ).geneSeq.size()); 
				}
		
			cout << "Current rep to neighbor " << index << ": " << reputations[index] << endl;

			// If reputation falls too low, set respective neighbor flags to false and delete routes with bad node in them
			if( reputations[index] < 75 )
				{
				cout << endl << endl << "REPUTATION DANGEROUSLY LOW, NODE WILL BE TERMINATED...." << endl << endl << endl;
				neighbors[index] = false;
				network->at( index ).neighbors[ routerNum ] = false;
				deleteRoutes( index );
				}		
			}
		}	
	}
   
void Router::deleteRoutes( int routerIndex )
	{
	bool deleted = true;

	string address = network->at( routerIndex ).address;

	while( deleted )
		{
		deleted = false;

		for( set<Route>::iterator it = routes.begin(); it != routes.end(); it++ )
			{
			if( find( it->path.begin(), it->path.end(), address ) != it->path.end() )
				{
				routes.erase( it );
				deleted = true;
				break;
				}
			}
		}
	}







  
   
   
   
   
   
   
