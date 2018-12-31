/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. 
 *
 * Copyright (C) 2018 Tobias Klausmann 
 * <tobias.johannes.klausmann@mni.thm.de>
 */

#include <open62541/ua_server.h>
#include <open62541/ua_server_config.h>
#include <open62541/ua_config_default.h>

#include <open62541/ua_client.h>

#include <string>
#include <set>

#ifndef SRC_OPCUASERVER_H_
#define SRC_OPCUASERVER_H_

enum OpcServerRole {
   RoleServer = 0,
   RoleClientServer,
   RoleDiscoveryServer,
};


namespace n_opcua {

class OpcUAServer {
private:
   bool running;
   uint16_t port;
   std::string name;
   std::string locale;
   std::string description;
   OpcServerRole role;
   UA_Server *server;
   UA_ServerConfig *config;
   UA_ByteString *cert;

   /* URI */
   std::string _uri;

   /* Capabilities */
   std::set<std::string> caps;

   /* LDS Registry */
   std::string _ldsServerURI;
   UA_Client *ldsRegisterClient;

   /**
    * @brief Push the capabilites to the actual config (helper)
    */
   void populateCapabilites();

   /**
    * @brief Remove the capabilites from the server (helper)
    */
   void removeCapabilites();
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
    * @brief Terminate the server execution
    */
   void terminate();

   /**
    * @brief Set server name
    * @param sname the servers name
    * @param locale the language the server name represents - defaults to
    * "en-US"
    */
   void setName(std::string sname, std::string locale = "en-US");

   /**
    * @brief Return the open62541 server struct
    * @return
    */
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

   /**
    * @brief Reset the config to a basic state
    * @note Can only be called before executing th run() method
    */
   void resetBaseConfig();
   /**
    * Call this after you set base settings for the server and in before you add
    * the server to a OpcUANodeHandler
    * @brief setBaseConfigDone
    */
   void setBaseConfigDone();

   /**
    * @brief Set the server description
    * @param value the description
    */
   void setDescription(const std::string value);

   /**
    * @brief Add a capability the server adervertises with its config
    * @param cap
    */
   void addCapabilities(std::string cap);

   /**
    * @brief Remove a capability the server adervertises with its config
    * @param cap
    */
   void removeCapability(std::string cap);

   /**
    * @brief Remove all capabilites the server adervertises with its config
    */
   void removeAllCapabilites();

   /**
    * @brief Set the application URI for the server description
    * @param URI the URI to set
    */
   void setURI(std::string URI);

   /**
    * @brief Remove the appication URI from the server description
    */
   void removeURI();

   /**
    * @brief Register the server at a local discovery service (LDS)
    * @param ldsServerURI the server URI to register at (e.g.
    * "opc.tcp://localhost:4840")
    * @return true on success, else false
    */
   bool registerAtLDS(std::string ldsServerURI);

   /**
    * @brief Unregister the server at a previously registered local discovery
    *  service (LDS)
    * @return true on success, else false
    */
   bool unregisterAtLDS();

   /**
    * @brief Add a new namespace to the server
    * @param ns the name for the new namespace
    * @return the Namespace index for nodes to attach
    */
   uint16_t addNamespace(std::string ns);
};

} /* namespace n_opcua */

#endif /* SRC_OPCUASERVER_H_ */
