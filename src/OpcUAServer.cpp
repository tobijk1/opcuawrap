/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. 
 *
 * Copyright (C) 2018 Tobias Klausmann 
 * <tobias.johannes.klausmann@mni.thm.de>
 */

#include "OpcUAServer.h"

namespace n_opcua {

using namespace std;

OpcServerRole OpcUAServer::getRole() {
   return role;
}

void OpcUAServer::setRole(const OpcServerRole value) {
   role = value;
   if (value == OpcServerRole::RoleServer)
      config->applicationDescription.applicationType =
            UA_APPLICATIONTYPE_SERVER;
   if (value == OpcServerRole::RoleClientServer)
      config->applicationDescription.applicationType =
            UA_APPLICATIONTYPE_CLIENTANDSERVER;
}

void OpcUAServer::resetBaseConfig() {
   if (server)
      UA_Server_delete(server);
   server = nullptr;

   UA_ServerConfig_delete(config);

   config = nullptr;
   config = UA_ServerConfig_new_minimal(port, cert);
}

void OpcUAServer::setBaseConfigDone() {
   if (server)
      UA_Server_delete(server);

   server = UA_Server_new(config);
}

void OpcUAServer::setDescription(const std::string value) {
   description = value;
}

bool OpcUAServer::registerAtLDS(std::string ldsServerURI) {

   _ldsServerURI = ldsServerURI;

   ldsRegisterClient = UA_Client_new(UA_ClientConfig_default);

   // periodic server register after 10 Minutes, delay first register for 500ms
   UA_StatusCode ret =
       UA_Server_addPeriodicServerRegisterCallback(server,
                                                   ldsRegisterClient,
                                                   _ldsServerURI.c_str(),
                                                   10 * 60 * 1000, 500,
                                                   nullptr);

   if(ret != UA_STATUSCODE_GOOD) {
       UA_Client_disconnect(ldsRegisterClient);
       UA_Client_delete(ldsRegisterClient);
       ldsRegisterClient = nullptr;
       _ldsServerURI = "";

       return false;
   }
   return true;
}

bool OpcUAServer::unregisterAtLDS() {

   if (!ldsRegisterClient)
      return false;

   UA_StatusCode ret = UA_Server_unregister_discovery(server,
                                                      ldsRegisterClient);

   UA_Client_disconnect(ldsRegisterClient);
   UA_Client_delete(ldsRegisterClient);
   ldsRegisterClient = nullptr;

   if(ret != UA_STATUSCODE_GOOD)
      return false;
   return true;
}

OpcUAServer::OpcUAServer(uint16_t sport) :
   running(true),
   port(sport),
   server(nullptr), cert(nullptr), ldsRegisterClient(nullptr) {

   config = UA_ServerConfig_new_minimal(port, cert);

   role = OpcServerRole::RoleServer;
   config->applicationDescription.applicationType = UA_APPLICATIONTYPE_SERVER;
}

OpcUAServer::~OpcUAServer() {

   unregisterAtLDS();

   if (server)
      UA_Server_delete(server);
   UA_ServerConfig_delete(config);
}

void OpcUAServer::run() {
   UA_Server_run(server, &running);
}

void OpcUAServer::terminate() {
   running = false;
}

void OpcUAServer::setName(string sname, string slocale) {
   name = sname;
   locale = slocale;

   UA_LocalizedText_deleteMembers(&config->applicationDescription.applicationName);
   UA_String_deleteMembers(&config->customHostname);
   UA_String_deleteMembers(&config->mdnsServerName);

   config->applicationDescription.applicationName =
         UA_LOCALIZEDTEXT_ALLOC((char *)(locale.c_str()),
                                (char *) name.c_str());

   config->customHostname = UA_String_fromChars(sname.c_str());
   config->mdnsServerName = UA_String_fromChars(sname.c_str());

   for (uint64_t i = 0; i < config->endpointsSize; i++) {
      UA_LocalizedText_deleteMembers(
               &config->endpoints[i].endpointDescription.server.applicationName);
      config->endpoints[i].endpointDescription.server.applicationName =
            UA_LOCALIZEDTEXT_ALLOC((char *)(locale.c_str()),
                                   (char *) name.c_str());

   }

}

} /* namespace n_opcua */
