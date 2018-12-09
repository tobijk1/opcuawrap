/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. 
 *
 * Copyright (C) 2018 Tobias Klausmann 
 * <tobias.johannes.klausmann@mni.thm.de>
 */

#include "OpcUANodeHandler.h"

namespace n_opcua {

OpcUANodeHandler::OpcUANodeHandler(OpcUAServer *server): _server(server) {

}

OpcUANodeHandler::~OpcUANodeHandler() {
   deleteAllNodes();
}

bool OpcUANodeHandler::findNodeInIndex(UA_NodeId* node, nodeMapIterator *mapIter) {
   nodeMapIterator lIter;
   lIter = nodemap.find(node);
   if (lIter == nodemap.end())
      return false;

   if (mapIter)
      *mapIter = lIter;
   return true;
}

bool OpcUANodeHandler::addNodeToIndex(UA_NodeId *node, OpcUANodeContext *ctx) {
   if (findNodeInIndex(node))
      // Node already in index
      return false;

   using namespace std;

   pair<unordered_map<UA_NodeId*, OpcUANodeContext*>::iterator, bool> p;
   p = nodemap.emplace(node, ctx);
   return p.second;
}

bool OpcUANodeHandler::addNodeToIndex(OpcUANodeContext *ctx) {
   return addNodeToIndex(ctx->getNodeId(), ctx);
}

bool OpcUANodeHandler::removeNodeFromIndex(UA_NodeId *node) {
   nodeMapIterator lIter;
   if (!findNodeInIndex(node, &lIter))
      // Node not found in Index, we can't remove it
      return false;
   nodemap.erase(lIter);
   return true;

}

bool OpcUANodeHandler::getNodePairFromIndex(UA_NodeId *node, nodeMapPair *p) {
   nodeMapIterator lIter;
   if (!findNodeInIndex(node, &lIter)) {
      // Not found in the Index, we can't return it
      return false;
   }

   if (p) {
      p->first = lIter->first;
      p->second = lIter->second;
   }
   return true;
}

bool OpcUANodeHandler::getCtxFromIndexByNode(UA_NodeId *node, OpcUANodeContext *ctx) {
   nodeMapIterator lIter;
   if (!findNodeInIndex(node, &lIter)) {
      // Not found in the Index, we can't return it
      return false;
   }

   ctx = lIter->second;
   return true;
}

/**
 *
 */
OpcUANodeContext *OpcUANodeHandler::initNewNodeAndAddToIndex(OpcUANodeContext *ctx) {
   if (!ctx)
      ctx = new OpcUANodeContext(this);

   if (!addNodeToIndex(ctx->getNodeId(), ctx)) {
      delete ctx;
      return NULL;
   }
   return ctx;
}

bool OpcUANodeHandler::addVariableCallbackNodeDataSourceToServer(OpcUAVarNodeContext *ctx) {

   if (!checkServer())
      return false;

   if (!ctx->getReadable() && !ctx->getWriteable())
      return false;

   ctx->setServer(getServer());

   UA_DataSource dataSource;
   dataSource.read = readCallback;
   dataSource.write = writeCallback;

   UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
   UA_NodeId variableTypeNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE);

   UA_Server_addDataSourceVariableNode(_server->getServer(),
                                       *ctx->getNodeId(),
                                       *ctx->getParent(),
                                       parentReferenceNodeId,
                                       *ctx->getQualifiedName(),
                                       variableTypeNodeId,
                                       *ctx->getVariableAttr(),
                                       dataSource,
                                       ctx,
                                       nullptr);
   return true;

}

bool OpcUANodeHandler::addObjectNodeToServer(OpcUAObjectNodeContext *ctx) {
   if (!checkServer())
      return false;

   UA_Server_addObjectNode(_server->getServer(), UA_NODEID_NULL, *ctx->getParent(),
            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), *ctx->getQualifiedName(),
            UA_NODEID_NUMERIC(0, ctx->getObjectType()), *ctx->getObjectAttr(),
            ctx, ctx->getNodeId());

   return true;
}

bool OpcUANodeHandler::addMethodNodeToServer(OpcUAMethodNodeContext *ctx) {
   if (!checkServer())
      return false;

   ctx->setServer(getServer());

   UA_MethodCallback callback;
   callback = &onMethodCallCallback;

   UA_Server_addMethodNode(_server->getServer(), UA_NODEID_NULL,
                           *ctx->getParent(),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASORDEREDCOMPONENT),
                           *ctx->getQualifiedName(),
                           *ctx->getMethodAttr(), callback,
                           ctx->getInputArgumentCount(), ctx->getInputArguments(),
                           ctx->getOutputArgumentCount(),
                           ctx->getOutputArguments(),
                           ctx,
                           ctx->getNodeId());

   return true;
}

/**
 * Delete a node, also deletes it from the server if the server is set
 * \ node The node to delete
 */
bool OpcUANodeHandler::deleteNode(UA_NodeId *node) {
   nodeMapPair pair;
   OpcUANodeContext *ctx = nullptr;
   if (getNodePairFromIndex(node, &pair)) {
      ctx = pair.second;
      removeNodeFromIndex(node);
   }

   // TODO: Update parent and all children and modify in the server instance and
   // change the below

   if (checkServer())
      UA_Server_deleteNode(_server->getServer(), *node, true);

   if (ctx)
      delete ctx;

   return true;

}

void OpcUANodeHandler::deleteAllNodes() {
   nodeMapIterator lIter = nodemap.begin();

   while (lIter != nodemap.end()) {
      UA_NodeId *node = lIter->first;
      OpcUANodeContext *ctx = lIter->second;
      deleteNode(node);
      lIter = nodemap.begin();
   }
}

UA_StatusCode OpcUANodeHandler::readCallback(UA_Server *server, const UA_NodeId *sessionId,
                           void *sessionContext, const UA_NodeId *nodeId,
                           void *nodeContext, UA_Boolean includeSourceTimeStamp,
                           const UA_NumericRange *range, UA_DataValue *value) {
   OpcUAVarNodeContext *obj = static_cast<OpcUAVarNodeContext*>(nodeContext);

   bool ret = false;

   if (obj && obj->getRead()) {
      ret = obj->getRead()(sessionId, sessionContext, includeSourceTimeStamp,
                           range, value);
      if (ret)
         return UA_STATUSCODE_GOOD;
      return UA_STATUSCODE_BADMETHODINVALID;
   }
   if (obj && obj->getReadSimple()) {
      ret = obj->getReadSimple()(value);
      if (includeSourceTimeStamp) {
         obj->setOPCSourceTimeStampNow(value);
      }
      if (ret)
         return UA_STATUSCODE_GOOD;
      return UA_STATUSCODE_BADMETHODINVALID;
   }
   return UA_STATUSCODE_BADMETHODINVALID;
}

UA_StatusCode OpcUANodeHandler::writeCallback(UA_Server *server, const UA_NodeId *sessionId,
                            void *sessionContext, const UA_NodeId *nodeId,
                            void *nodeContext, const UA_NumericRange *range,
                            const UA_DataValue *value) {
   OpcUAVarNodeContext *obj = static_cast<OpcUAVarNodeContext*>(nodeContext);
   bool ret = false;

   if (obj && obj->getWrite()) {
      ret = obj->getWrite()(sessionId, sessionContext, range, value);
      if (ret)
         return UA_STATUSCODE_GOOD;
      return UA_STATUSCODE_BADMETHODINVALID;
   }
   if (obj && obj->getWriteSimple()) {
      ret = obj->getWriteSimple()(value);
      if (ret)
         return UA_STATUSCODE_GOOD;
      return UA_STATUSCODE_BADMETHODINVALID;
   }

   return UA_STATUSCODE_BADMETHODINVALID;
}

UA_StatusCode OpcUANodeHandler::onMethodCallCallback(UA_Server *server,
                                                     const UA_NodeId *sessionId,
                                                     void *sessionContext,
                                                     const UA_NodeId *methodId,
                                                     void *nodeContext,
                                                     const UA_NodeId *objectId,
                                                     void *objectContext,
                                                     size_t inputSize,
                                                     const UA_Variant *input,
                                                     size_t outputSize,
                                                     UA_Variant *output) {

   OpcUAMethodNodeContext *obj = static_cast<OpcUAMethodNodeContext*>(nodeContext);
   bool ret = false;

   if (obj && obj->getCallback()) {
      ret = obj->getCallback()(sessionId, sessionContext,
                           objectId, objectContext, inputSize,
                           input, outputSize, output);
      if (ret)
         return UA_STATUSCODE_GOOD;
      return UA_STATUSCODE_BADMETHODINVALID;
   }
   if (obj && obj->getCallbackSimple()) {
      ret = obj->getCallbackSimple()(inputSize, input, outputSize, output);
      if (ret)
         return UA_STATUSCODE_GOOD;
      return UA_STATUSCODE_BADMETHODINVALID;
   }

   return UA_STATUSCODE_BADMETHODINVALID;
}

} /* namespace n_opcua */
