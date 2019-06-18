/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. 
 *
 * Copyright (C) 2018 Tobias Klausmann 
 * <tobias.johannes.klausmann@mni.thm.de>
 */

#include <unordered_map>
#include <unordered_set>
#include "OpcUANodeContext.h"
#include "OpcUAServer.h"


#ifndef SRC_OPCUANODEHANDLER_H_
#define SRC_OPCUANODEHANDLER_H_

namespace n_opcua {

typedef std::unordered_map<UA_NodeId*, OpcUANodeContext*>::iterator nodeMapIterator;
typedef std::pair<UA_NodeId*, OpcUANodeContext*> nodeMapPair;

class OpcUANodeHandler {
private:
   std::unordered_map<UA_NodeId*, OpcUANodeContext*> nodemap;
   OpcUAServer *_server;

public:
   /**
    * @brief Default OpcUANodeHandler constructor
    * @param server the server to set
    */
   OpcUANodeHandler(OpcUAServer *server = NULL);
   /**
    * @brief Set the server for our nodes, only works if the server is not set already
    * @param server the server to set
    * @return true if the server was set, else false
    */
   bool setServer(OpcUAServer *server) {
      if (_server)
         return false;
      if (!server)
         return false;

      _server = server;
      return true;
   }

   /**
    * @brief Get the OPCUAServer
    * @return the server
    */
   OpcUAServer *getServer() {
      return _server;
   }

   /**
    * @brief Check if a valid server is set
    * @return true if set, else false
    */
   bool checkServer() {
      if (!_server || !_server->getServer())
         return false;
      return true;
   }

   /**
    * @brief Default deconstructor for OpcUANodeHandler
    */
   virtual ~OpcUANodeHandler();
   /**
    * @brief Find a indexed node
    * @param node the node to find
    * @param mapIter the mapIterator to use
    * @return true if the node was found, else false
    */
   bool findNodeInIndex(UA_NodeId* node, nodeMapIterator *mapIter = NULL);
   /**
    * @brief Add a node to the index
    * @param node the node to add
    * @param ctx the context for the node
    * @return if the action was successful
    */
   bool addNodeToIndex(UA_NodeId *node, OpcUANodeContext *ctx);
   /**
    * @brief Add a node to the index by its context
    * @param ctx the context for the node
    * @return if the action was successful
    */
   bool addNodeToIndex(OpcUANodeContext *ctx);
   /**
    * @brief Remove a node from the index
    * @param node the node to remove
    * @return if the action was successful
    */
   bool removeNodeFromIndex(UA_NodeId *node);
   /**
    * @brief Remove a node from the index by its context
    * @param ctx the context of the node
    * @return if the action was successful
    */
   bool inline removeNodeFromIndex(OpcUANodeContext *ctx) {
      return removeNodeFromIndex(ctx->getNodeId());
   }
   /**
    * @brief Get the node and the context from the index by searching for the node
    * @param node the node to look for
    * @param p the return pair
    * @return if the action was successful
    */
   bool getNodePairFromIndex(UA_NodeId *node,  nodeMapPair *p = NULL);
   /**
    * @brief Get a node context from the index by a node
    * @param node the node for the context
    * @param ctx the retuning context
    * @return if the action was successful
    */
   bool getCtxFromIndexByNode(UA_NodeId *node,  OpcUANodeContext *ctx);
   /**
    * @brief Initialize a new node and add it to the index
    * @param ctx the context for the node to use, if any
    * @return the nodes context after the action
    */
   OpcUANodeContext *initNewNodeAndAddToIndex(OpcUANodeContext *ctx = NULL);
   /**
    * @brief Add a node to a server
    * @return true if added, else false
    */
   virtual bool addNodeToServer() { return false; }
   /**
    * @brief Add a Variable Node with a Callback to the server
    * @param ctx the context of the variable
    * @return true if added, else false
    */
   bool addVariableCallbackNodeDataSourceToServer(OpcUAVarNodeContext *ctx);
   /**
    * @brief Add a Object Node to the server
    * @param ctx the context of the object
    * @return true if added, else false
    */
   bool addObjectNodeToServer(OpcUAObjectNodeContext *ctx);
   /**
    * @brief Add a Method Node to the server
    * @param ctx the context of the method
    * @return true if added, else false
    */
   bool addMethodNodeToServer(OpcUAMethodNodeContext *ctx);

   /**
    * @brief Delete a node
    * @param node the node to delete
    * @return true if deleted, else false
    */
   bool deleteNode(UA_NodeId *node);
   /**
    * @brief Delete a node by its context
    * @param ctx the nodes context
    * @return true if deleted, else false
    */
   bool inline deleteNode(OpcUANodeContext *ctx) {
      return deleteNode(ctx->getNodeId());
   }
   /**
    * @brief Delete all nodes on the index
    */
   void deleteAllNodes();
   /**
    * @brief Map a datatype name to a name integer (open62541)
    * @param datatypename the name to match
    * @return the integer representation if found
    */
   int16_t mapDataTypeToName(const char* datatypename);

   /**
    * @brief Static read callback for a opcua variable
    */
   static UA_StatusCode readCallback(UA_Server *server, const UA_NodeId *sessionId,
                              void *sessionContext, const UA_NodeId *nodeId,
                              void *nodeContext, UA_Boolean includeSourceTimeStamp,
                              const UA_NumericRange *range, UA_DataValue *value);
   /**
    * @brief Static write callback for a opc ua variable
    */
   static UA_StatusCode writeCallback(UA_Server *server, const UA_NodeId *sessionId,
                               void *sessionContext, const UA_NodeId *nodeId,
                               void *nodeContext, const UA_NumericRange *range,
                               const UA_DataValue *value);

   /**
    * @brief Static call callback for a opc ua method
    */
   static UA_StatusCode onMethodCallCallback(UA_Server *server,
                                             const UA_NodeId *sessionId,
                                             void *sessionContext,
                                             const UA_NodeId *methodId,
                                             void *methodContext,
                                             const UA_NodeId *objectId,
                                             void *objectContext,
                                             size_t inputSize,
                                             const UA_Variant *input,
                                             size_t outputSize,
                                             UA_Variant *output);
};

} /* namespace n_opcua */

#endif /* SRC_OPCUANODEHANDLER_H_ */
