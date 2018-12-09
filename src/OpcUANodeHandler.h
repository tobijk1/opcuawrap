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
   OpcUANodeHandler(OpcUAServer *server = NULL);
   /**
    * Set the server for our nodes, only works if the server is not set already
    * \return true if the server was set, else false
    */
   bool setServer(OpcUAServer *server) {
      if (_server)
         return false;
      if (!server)
         return false;

      _server = server;
      return true;
   }

   OpcUAServer *getServer() {
      return _server;
   }

   bool checkServer() {
      if (!_server || !_server->getServer())
         return false;
      return true;
   }

   virtual ~OpcUANodeHandler();
   bool findNodeInIndex(UA_NodeId* node, nodeMapIterator *mapIter = NULL);
   bool addNodeToIndex(UA_NodeId *node, OpcUANodeContext *ctx);
   bool addNodeToIndex(OpcUANodeContext *ctx);
   bool removeNodeFromIndex(UA_NodeId *node);
   bool inline removeNodeFromIndex(OpcUANodeContext *ctx) {
      return removeNodeFromIndex(ctx->getNodeId());
   }
   bool getNodePairFromIndex(UA_NodeId *node,  nodeMapPair *p = NULL);
   bool getCtxFromIndexByNode(UA_NodeId *node,  OpcUANodeContext *ctx);
   OpcUANodeContext *initNewNodeAndAddToIndex(OpcUANodeContext *ctx = NULL);
   virtual bool addNodeToServer() { return false; }
   bool addVariableCallbackNodeDataSourceToServer(OpcUAVarNodeContext *ctx);
   bool addObjectNodeToServer(OpcUAObjectNodeContext *ctx);
   bool addMethodNodeToServer(OpcUAMethodNodeContext *ctx);

   bool deleteNode(UA_NodeId *node);
   bool inline deleteNode(OpcUANodeContext *ctx) {
      return deleteNode(ctx->getNodeId());
   }
   void deleteAllNodes();
   int16_t mapDataTypeToName(const char* datatypename);

   static UA_StatusCode readCallback(UA_Server *server, const UA_NodeId *sessionId,
                              void *sessionContext, const UA_NodeId *nodeId,
                              void *nodeContext, UA_Boolean includeSourceTimeStamp,
                              const UA_NumericRange *range, UA_DataValue *value);
   static UA_StatusCode writeCallback(UA_Server *server, const UA_NodeId *sessionId,
                               void *sessionContext, const UA_NodeId *nodeId,
                               void *nodeContext, const UA_NumericRange *range,
                               const UA_DataValue *value);

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
