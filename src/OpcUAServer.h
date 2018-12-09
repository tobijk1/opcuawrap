/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. 
 *
 * Copyright (C) 2018 Tobias Klausmann 
 * <tobias.johannes.klausmann@mni.thm.de>
 */

#include <open62541/open62541.h>
#include <string>

#ifndef SRC_OPCUASERVER_H_
#define SRC_OPCUASERVER_H_

enum OpcServerRole {
   RoleServer = 0,
   RoleClientServer,
};


namespace n_opcua {

class OpcUAServer {
private:
   bool running;
   uint16_t port;
   UA_Server *server;
   UA_ServerConfig *config;
   std::string name;
   std::string locale;
   OpcServerRole role;
public:
   /**
    * @brief OpcUAServer Default constructor for a new OpcUAServer object
    * @param sport The server port - defaults to 4840
    */
   OpcUAServer(uint16_t sport = 4840);
   /**
    * @brief ~OpcUAServer destructor for the Server object, if the server is
    * running, call terminate() first
    */
   virtual ~OpcUAServer();
   /**
    * @brief Start the server execution
    */
   void run();
   /**
    * @brief Terminate the server  execution
    */
   void terminate();

   /**
    * @brief Set server name
    * @param sname the servers name
    * @param locale the language the server name represents - defaults to
    * "en-US"
    */
   void setName(std::string sname, std::string locale = "en-US");

   UA_Server *getServer() {
      return server;
   }
   /**
    * @brief Get the server role
    * @return the role assigned
    */
   OpcServerRole getRole();
   /**
    * @brief Set the server role, this has to be done before run() is called
    * @param value
    */
   void setRole(const OpcServerRole value);
};

} /* namespace n_opcua */

#endif /* SRC_OPCUASERVER_H_ */
