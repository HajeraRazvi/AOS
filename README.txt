README
========================================================================================================================

Authentication Server: (auth_server.c) 
---------------------------------------------------------------------------------

To Run:  - Compilation command "gcc auth_server.c -o s".
                 -To execute "./s 8888" . Please provide port 8888 as it is given in user file as default.

Functionality: To Register the new user and to facilitate login for existing user. Authentication verification is done during login phase.

*****************************************************************************************************************************************************************

User Node: (user.c)
-----------------------------------------------------------------------------------

To Run: - Compilation command "gcc user.c -o u". 
                - To execute "./u auth_server_IP self_IP local_port". Three arguments are provided while running the user executable. 
		(1) IP address of Authentication Server. 
		(2) IP Address of Current User. 
		(3) Port number of Current User.

		- User node searches the peer information in its file first(for example: user abc- searches in abc.txt), if not found asks super node.
		- The peer information will be displayed either in super node , or on other super node screen.(if its not found in registerd SuperNode).
		- Once the user info is found(by connected SN or other SN) Enter the peer information manually in user screen to get connected to other peer.
		- Once the user is found it is added to friend list of that particular user.
		- Both user should give the respective friend info , to chat with each other.
		(For example: abc should provide xyz(as friend to chat) and also xyz should provide abc as friend to establish peer to peer communication) 
Usage example-    ./u 127.0.0.1 127.0.0.1 5656

Functionality: To get user input for Registration and Login. To establish peer to peer communication with known user.
To communicate with Super Nodes to fetch the information of unknown and registered user. 

******************************************************************************************************************************************************************

Super Node: (super_node.c)
------------------------------------------------------------------------------------

To Run:  - Compilation command "gcc super_node.c -o sup -lpthread".
                 -To execute "./sup port_no", Port number of current Super Node.
                 - SuperNode will display the user details that is requested by user node.
		 And User node has to manually provide this information on its screen to commumicate with its peer.
                 - If the user info is not present within the registered SuperNode it connects to other SuperNode to fetch details.
		   The input of other supernode (i.e IP and Port Num) should be entered manually in the super node screen.
Usage example-    ./sup 127.0.0.1 7777

Functionality: SuperNode will register the current user and will provide user info , if requested by registered user.
It establishes communication between supernodes to fetch the registered user node information across the supernodes. 

====================================================================================================================================================================




