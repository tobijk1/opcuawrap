/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. 
 *
 * Copyright (C) 2018 Tobias Klausmann 
 * <tobias.johannes.klausmann@mni.thm.de>
 */

#include "OpcUANodeContext.h"
#include "OpcUANodeHandler.h"

namespace n_opcua {

/*
 * OpcUANodeContext
 */

void OpcUANodeContext::initDefault() {
   setName("");
   setDescription("");
   setQualifiedName("");
   setDefaultParent();
   setLocale("en-US");

   _nodeHandler->addNodeToIndex(this);
}

void OpcUANodeContext::setDefaultParent() {
   _default_parent->namespaceIndex = 0;
   _default_parent->identifierType = UA_NODEIDTYPE_NUMERIC;
   _default_parent->identifier.numeric = UA_NS0ID_ROOTFOLDER;
}

OpcUANodeContext::OpcUANodeContext(UA_NodeId* node,
                                   OpcUANodeHandler *nodeHandler) :
   _node(node),
   _parent(nullptr),
   _default_parent(nullptr),
   server(nullptr),
   _nodeHandler(nodeHandler),
   _dataTypeNr(-1),
   _readable(false),
   _writeable(false),
   _active(false) {
   _default_parent = new UA_NodeId();
   initDefault();
}

OpcUANodeContext::OpcUANodeContext(OpcUANodeHandler *nodeHandler) :
   _parent(nullptr),
   _default_parent(nullptr),
   server(nullptr),
   _nodeHandler(nodeHandler),
   _dataTypeNr(-1),
   _readable(false),
   _writeable(false),
   _active(false) {
   _node = new UA_NodeId();
   _default_parent = new UA_NodeId();
   initDefault();
}

OpcUANodeContext::~OpcUANodeContext() {

   _nodeHandler->removeNodeFromIndex(this);

   if (_node)
      delete _node;
   if (_default_parent)
      delete _default_parent;
}

bool OpcUANodeContext::setNode(UA_NodeId *node) {
   if (_node)
      return false;
   _node = node;
   return true;
}

bool OpcUANodeContext::setParent(OpcUANodeContext *parent_ctx) {
   if (!parent_ctx || !parent_ctx->getNodeId() || _parent != nullptr)
      return false;

   _parent = parent_ctx->getNodeId();
   return true;
}

bool OpcUANodeContext::removeParent() {
   if (!_parent)
      return false;
   _parent = nullptr;
   return true;
}

UA_NodeId *OpcUANodeContext::getParent() {
   if (!_parent)
      return _default_parent;
   return _parent;
}

bool OpcUANodeContext::setServer(OpcUAServer *serv) {
   if (server)
      return false;
   if (!serv)
      return false;

   server = serv;
   return true;
}

bool OpcUANodeContext::setName(std::string name) {
   _name = name;
   /* Set node name */
   if (!_node)
      return false;
   *_node = UA_NODEID_STRING(1, (char *) _name.c_str());

   setAttrName();

   return true;
}

void OpcUANodeContext::setDescription(std::string description) {
   _description = description;
   setAttrDescription();
}

void OpcUANodeContext::setQualifiedName(std::string qualifiedName) {
   _qualifiedNameStr = qualifiedName;
   /* Set qualified name for node */
   _qualifiedName = UA_QUALIFIEDNAME(1, (char *)_qualifiedNameStr.c_str());
}

bool OpcUANodeContext::addChild(OpcUANodeContext *child) {
   if (isChild(child))
      return false;

   child->setParent(this);
   childset.insert(child);
   return true;
}

bool OpcUANodeContext::removeChild(OpcUANodeContext *child) {
   if (!isChild(child))
      return false;

   child->removeParent();
   childset.erase(child);
   return true;
}

bool OpcUANodeContext::isChild(OpcUANodeContext *node) {
   if (childset.find(node) != childset.end())
      return true;
   return false;
}

void OpcUANodeContext::setDataTypeNumber(int16_t dataTypeNr) {
   _dataTypeNr = dataTypeNr;
   setAttrDataType();
}

void OpcUANodeContext::convertFromOPC(std::string *value,
                                      const UA_Variant *opcval) {
   UA_String *str = static_cast<UA_String *>(opcval->data);
   value->append(reinterpret_cast<char *>(str->data), str->length);
   value->append("\0");
}

void OpcUANodeContext::convertToOPC(UA_Variant *value,
                                    const std::string *userval) {
   int datatypenr = convertTypeToOpen62541Type(*userval);
   UA_String str = UA_String_fromChars(userval->c_str());
   UA_Variant_setScalarCopy(value, &str, &UA_TYPES[datatypenr]);
   UA_String_deleteMembers(&str);
}

void OpcUANodeContext::convertToOPC(UA_Variant *value,
                                    const std::vector<std::string> *uservec) {
   uint64_t arraycount = 0;

   if (uservec->size() < 1)
      return;

   int datatypenr = convertTypeToOpen62541Type(*uservec->begin());

    UA_String *arr =
          reinterpret_cast<UA_String*>(UA_Array_new(uservec->size(),
                                                    &UA_TYPES[datatypenr]));

   if (value == nullptr)
      return;

   typename std::vector<std::string>::const_iterator it = uservec->begin();

   while (it != uservec->end()) {

      arr[arraycount] = UA_String_fromChars((*it).c_str());
      arraycount++;
      it++;
   }

   UA_Variant_setArray(value, arr, uservec->size(), &UA_TYPES[datatypenr]);
}



void OpcUANodeContext::setOPCSourceTimeStamp(UA_DataValue *value,
                                             time_t time_point) {
   value->sourceTimestamp = UA_DateTime_fromUnixTime(time_point);
}

void OpcUANodeContext::setOPCSourceTimeStampNow(UA_DataValue *value) {
   std::time_t time_point = std::time(nullptr);
   setOPCSourceTimeStamp(value, time_point);
}

void OpcUANodeContext::writeToServer(UA_Variant var) {
   UA_Server_writeValue(server->getServer(), *getNodeId(), var);
}

/*
 * OpcUAVarNodeContext
 */

void OpcUAVarNodeContext::setAttrName() {
   varAttr.displayName = UA_LOCALIZEDTEXT_ALLOC((char *) _locale.c_str(),
                                                (char *) _name.c_str());
}

void OpcUAVarNodeContext::setAttrDescription() {
   varAttr.description = UA_LOCALIZEDTEXT_ALLOC((char *) _locale.c_str(),
                                                (char *) _description.c_str());
}

void OpcUAVarNodeContext::setAttrDataType() {
   varAttr.dataType = UA_TYPES[_dataTypeNr].typeId;
}

void OpcUAVarNodeContext::setAttrReadable() {
   /* Set Access level mask */
   if (_readable)
      varAttr.accessLevel |= UA_ACCESSLEVELMASK_READ;
   else
      varAttr.accessLevel |= ~UA_ACCESSLEVELMASK_READ;
}

void OpcUAVarNodeContext::setAttrWriteable() {
   /* Set Access level mask */
   if (_writeable)
      varAttr.accessLevel |= UA_ACCESSLEVELMASK_WRITE;
   else
      varAttr.accessLevel |= ~UA_ACCESSLEVELMASK_WRITE;
}

void OpcUAVarNodeContext::setReadable(bool readable) {
   _readable = readable;
   setAttrReadable();
}

void OpcUAVarNodeContext::setWriteable(bool writeable) {
   _writeable = writeable;
   setAttrWriteable();
}

/*
 * OpcUAObjectNodeContext
 */

int8_t OpcUAObjectNodeContext::checkTypeName(std::string objtypename) {
   if (objtypename.compare("base") == 0)
      return UA_NS0ID_BASEOBJECTTYPE;
   if (objtypename.compare("folder") == 0)
      return UA_NS0ID_FOLDERTYPE;
   return -1;
}

bool OpcUAObjectNodeContext::checkTypeNumber(int8_t objecttypenr) {
   if (objecttypenr == UA_NS0ID_BASEOBJECTTYPE)
      return true;
   if (objecttypenr == UA_NS0ID_FOLDERTYPE)
      return true;
   return false;
}

bool OpcUAObjectNodeContext::setObjectType(std::string objecttypename) {
   int8_t tmp = checkTypeName(objecttypename);
   if (tmp < 0)
      return false;

   objtype = tmp;
   return true;
}

bool OpcUAObjectNodeContext::setObjectType(int8_t objectTypeNr) {
   if (!checkTypeNumber(objectTypeNr))
      return false;
   objtype = objectTypeNr;
   return true;
}

void OpcUAObjectNodeContext::setAttrName() {
   objAttr.displayName = UA_LOCALIZEDTEXT_ALLOC((char *) _locale.c_str(),
                                                (char *) _name.c_str());

}

void OpcUAObjectNodeContext::setAttrDescription() {
   objAttr.description = UA_LOCALIZEDTEXT_ALLOC((char *) _locale.c_str(),
                                                (char *) _description.c_str());
}

/*
 * OpcUAMethodNodeContext
 */

void OpcUAMethodNodeContext::freeInputArguments() {
   if (inputArguments)
      free(inputArguments);
}

void OpcUAMethodNodeContext::freeOutputArguments() {
   if (outputArguments)
      free(outputArguments);
}

UA_Argument *OpcUAMethodNodeContext::allocArguments(uint64_t argCount) {
   UA_Argument *args = nullptr;
   if (argCount > 0)
      args = static_cast<UA_Argument *> (calloc(argCount, sizeof(UA_Argument)));
   return args;
}

void OpcUAMethodNodeContext::initArguments(uint64_t argCount,
                                           UA_Argument *args) {
   uint64_t count = 0;

   if (argCount == 0)
      return;

   while (count < argCount) {
      UA_Argument_init(&args[count]);
      args[count].valueRank = -1; // set to scalar by default
      std::string str = "var" + std::to_string(count);
      args[count].description = UA_LOCALIZEDTEXT((char *)_locale.c_str(),
                                                 (char *)_description.c_str());
      args[count].name = UA_STRING((char *)(_name.c_str()));
      args[count].name = UA_STRING((char *)(_name.c_str()));
      count++;
   }
}

OpcUAMethodNodeContext::~OpcUAMethodNodeContext() {
   freeInputArguments();
   freeOutputArguments();
}

void OpcUAMethodNodeContext::setExecutable(bool executable) {
   methodAttr.executable = executable;
}

void OpcUAMethodNodeContext::setUserExecutable(bool uexecutable) {
   methodAttr.userExecutable = uexecutable;
}

void OpcUAMethodNodeContext::initOutputArguments(uint64_t outArgCount) {

   outArgumentCount = outArgCount;

   freeOutputArguments();

   outputArguments = allocArguments(outArgCount);

   initArguments(outArgCount, outputArguments);
}

void OpcUAMethodNodeContext::initInputArguments(uint64_t inArgCount) {

   inArgumentCount = inArgCount;

   freeInputArguments();

   inputArguments = allocArguments(inArgCount);

   initArguments(inArgCount, inputArguments);
}

bool OpcUAMethodNodeContext::setInputArgumentRank(uint64_t argNum,
                                                  int32_t rank) {
   if (argNum > inArgumentCount)
      return false;

   inputArguments[argNum].valueRank = rank;
   return true;
}

bool OpcUAMethodNodeContext::setOutputArgumentRank(uint64_t argNum,
                                                   int32_t rank) {
   if (argNum > outArgumentCount)
      return false;

   outputArguments[argNum].valueRank = rank;
   return true;
}

bool OpcUAMethodNodeContext::setInputArgumentName(uint64_t argNum,
                                                  std::string name) {
   if (argNum > inArgumentCount)
      return false;

   inputArguments[argNum].name = UA_String_fromChars(name.c_str());
   return true;
}

bool OpcUAMethodNodeContext::setOutputArgumentName(uint64_t argNum,
                                                   std::string name) {
   if (argNum > outArgumentCount)
      return false;

   outputArguments[argNum].name = UA_String_fromChars(name.c_str());
   return true;
}

void OpcUAMethodNodeContext::setAttrName() {
   methodAttr.displayName = UA_LOCALIZEDTEXT_ALLOC(
            static_cast< const char *>(_locale.c_str()),
            static_cast<const char *>(_name.c_str()));
}

void OpcUAMethodNodeContext::setAttrDescription() {
   methodAttr.description = UA_LOCALIZEDTEXT_ALLOC(
            static_cast<const char *>(_locale.c_str()),
            static_cast<const char *>(_description.c_str()));
}

void OpcUAMethodNodeContext::setAttrExecutable(bool executable) {
   methodAttr.executable = executable;
}

void OpcUAMethodNodeContext::setAttrUserExecutable(bool uexecutable) {
   methodAttr.userExecutable = uexecutable;
}

void OpcUAMethodNodeContext::setAttrWriteMask(uint32_t writemask) {
   methodAttr.writeMask = writemask;
}

void OpcUAMethodNodeContext::setAttrUserWriteMask(uint32_t uwritemask) {
   methodAttr.userWriteMask = uwritemask;
}

} /* namespace n_opcua */
