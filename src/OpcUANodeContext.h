/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. 
 *
 * Copyright (C) 2018 Tobias Klausmann 
 * <tobias.johannes.klausmann@mni.thm.de>
 */

#ifndef OPCUANODECONTEXT_H
#define OPCUANODECONTEXT_H

#include <unordered_set>
#include <vector>
#include <functional>
#include <cassert>
#include <chrono>
#include "OpcUAServer.h"


namespace n_opcua {

class OpcUANodeHandler;

/* Class to hold additional information to a UA_Node */
class OpcUANodeContext {
private:
   /* the node our Context belongs to */
   UA_NodeId *_node;
   /* the qualified name of the node */
   std::string _qualifiedNameStr;
   UA_QualifiedName _qualifiedName;

   /* The parent Node of ourself */
   UA_NodeId *_parent;
   UA_NodeId *_default_parent;

   OpcUAServer *server;
   OpcUANodeHandler *_nodeHandler;

   std::unordered_set<OpcUANodeContext *> childset;

   /**
    * @brief Set a default node parent
    */
   void setDefaultParent();

   /**
    * @brief Initialize the default node state
    */
   void initDefault();
protected:
   /* the name of the node */
   std::string _name;
   /* the description for our node */
   std::string _description;
   /* the locale of our strings */
   std::string _locale;
   /* the datatype Number we want to use */
   int16_t _dataTypeNr;
   /* if the node can be read */
   bool _readable;
   /* if the node can be written to */
   bool _writeable;

   bool _active;

   uint16_t nsID;

public:
   /**
    * @brief Constructor for OpcUANodeContext with direct initialization of our node
    * @param node The node our context belongs to
    * @param nodeHandler the nodeHandler to use
    * @return a new OpcUANodeContext object
    */
   OpcUANodeContext(UA_NodeId* node, OpcUANodeHandler *nodeHandler);

   /**
    * @brief Constructor for OpcUANodeContext with the node initialized to NULL
    * @return a new OpcUANodeContext object
    */
   OpcUANodeContext(OpcUANodeHandler *nodeHandler);
   /**
    * Our default deconstructor for OpcUANodeContext, destroys the node on its
    * way
    */
   virtual ~OpcUANodeContext();

   /**
    * @brief Set a node the Context belongs to, only works if the node is not
    * already set
    * @param node the node to set
    * @return true if the node was set, false if it was not set
    */
   bool setNode(UA_NodeId *node);

   /**
    * @brief Set the parent node of our object in the Node tree
    * @param parent_ctx The parent of ourself
    * @return true if the parent was set, else false
    */
   bool setParent(OpcUANodeContext *parent_ctx);

   /**
    * @brief Remove the previously set parent node
    * @return true if the node was removed, else false
    */
   bool removeParent();

   /**
    * @brief Returns the parent node
    * @return The parent node, if set, else NULL
    */
   UA_NodeId *getParent();

   /**
    * @brief Set a server the node will be used with
    * @param serv the server to set
    * @return if the server was set
    */
   bool setServer(OpcUAServer *serv);

   /**
    * @brief Return the server the node is used with
    * @return the server object
    */
   OpcUAServer *getServer() {
      return server;
   }

   /**
    * @brief Set the locale of this node
    * @param locale The locale short - e.g "de-DE"
    */
   void setLocale(std::string locale) {
      _locale = locale;
   }

   /**
    * @brief return the set locale
    * @return the lcoale
    */
   std::string getLocale() {
      return _locale;
   }

   /**
    * @brief Set a new name for the node, only works if the node is set
    * @param name the new name to set
    * @return true if the name was set, else false
    */
   bool setName(std::string name);


   /**
    * @brief set the node description
    * @param description the description string to set
    */
   void setDescription(std::string description);

   /**
    * @brief Set a new qualified name for the node, only works if the node is
    * set
    * @param qualifiedName the new name to set
    * @return true if the qualified name was set, else false
    */
   void setQualifiedName(std::string qualifiedName);

   /**
    * @brief Return the qualified node name
    * @return the nodes qualified name
    */
   UA_QualifiedName *getQualifiedName() {
      return &_qualifiedName;
   }

   /**
    * @brief Returns the node for our context
    * @return the node
    */
   UA_NodeId* getNodeId() {
      return _node;
   }

   /**
    * @brief Add a new child node to our index
    * @param child the child node
    * @return true if the child was added, else false
    */
   bool addChild(OpcUANodeContext *child);

   /**
    * @brief Remove a child from the index
    * @param child The child to remove
    * @return true if removed, else false
    */
   bool removeChild(OpcUANodeContext *child);

   /**
    * @brief Check if a node is really our child
    * @param node the Node in question
    * @return true if it is our child, else false
    */
   bool isChild(OpcUANodeContext *node);

   /**
    * @brief return if the node is active
    * @return true if the node is active, else false
    */
   bool isActive() {return _active; }

   /**
    * @brief Set the node as active
    * @param active if the node is active
    */
   void setActive(bool active) {
      _active = active;
   }

   /**
    * @brief Set the type number by guessing the righ type internally, see
    * UA_DataType to set it directly with setDataTypeNumber()
    * @param dattype the dattye to set
    * @return true if the type got matched an set, else false
    */
   template <typename T>
   bool setDataType(T dattype) {
      int16_t type = convertTypeToOpen62541Type(dattype);
      if (type == -1)
         return false;
      _dataTypeNr = type;

      setAttrDataType();

      return true;
   }

   /**
    * @brief Set the type number directly, see UA_DataType for more information
    * @param dataTypeNr the number to set
    */
   void setDataTypeNumber(int16_t dataTypeNr);

   /**
    * @brief Return the data type number reflecting the open62541 type
    * @return the datatypenr as used in open62541
    */
   int16_t getDataTypeNumber() {
      return _dataTypeNr;
   }

   /**
    * @brief Set the namespace id this node will get added to
    * @param namespaceID The ID of the namespace this node should be added to
    */
   void setNamespace(uint16_t namespaceID);

   /**
    * @brief Return the ID of the namespace this node belongs to
    * @return
    */
   uint16_t getNamespace();

   /* Methods to implement */
   virtual void setAttrName() {
      //assert("Make sure nobody calls this virtual method" == "");
   }
   virtual void deleteAttrName() {
      //assert("Make sure nobody calls this virtual method" == "");
   }
   virtual void setAttrDescription() {
      //assert("Make sure nobody calls this virtual method" == "");
   }
   virtual void deleteAttrDescription() {
      //assert("Make sure nobody calls this virtual method" == "");
   }
   virtual void setAttrDataType() {
      //assert("Make sure nobody calls this virtual method" == "");
   }
   virtual void setAttrReadable() {
      //assert("Make sure nobody calls this virtual method" == "");
   }
   virtual void setAttrWriteable() {
      //assert("Make sure nobody calls this virtual method" == "");
   }
   virtual UA_VariableAttributes *getVariableAttr() {
      assert("Make sure nobody calls this virtual method" == "");
      return nullptr;
   }
   virtual UA_ObjectAttributes *getObjectAttr() {
      assert("Make sure nobody calls this virtual method" == "");
      return nullptr;
   }
   virtual UA_MethodAttributes* getMethodAttr() {
      assert("Make sure nobody calls this virtual method" == "");
      return nullptr;
   }


   template <typename T>
   /**
    * @brief Convert a c++ type to an open62541 representation
    * @param type
    * @return open62541 type definition or -1 if not found
    *
    * Return an open62514 type according to the typeid
    *
    * New types have to be implemented, as well as the corresponding setters for
    * the new types!
    */
   int16_t convertTypeToOpen62541Type(T type) {
      int16_t ret = -1;
      /* bool */
      if (typeid(type) == typeid(bool))
            return UA_TYPES_BOOLEAN;
      /* int8_t */
      if (typeid(type) == typeid(int8_t))
            return UA_TYPES_SBYTE;
      if (typeid(type) == typeid(short))
            return UA_TYPES_SBYTE;
      if (typeid(type) == typeid(signed char))
            return UA_TYPES_SBYTE;
      /* uint8_t */
      if (typeid(type) == typeid(uint8_t))
            return UA_TYPES_BYTE;
      if (typeid(type) == typeid(ushort))
            return UA_TYPES_BYTE;
      if (typeid(type) == typeid(char))
            return UA_TYPES_BYTE;
      if (typeid(type) == typeid(unsigned char))
            return UA_TYPES_BYTE;
      /* int16_t */
      if (typeid(type) == typeid(int16_t))
            return UA_TYPES_INT16;
      /* uint16_t */
      if (typeid(type) == typeid(uint16_t))
            return UA_TYPES_UINT16;
      /* int32_t */
      if (typeid(type) == typeid(int32_t))
            return UA_TYPES_INT32;
      if (typeid(type) == typeid(int))
            return UA_TYPES_INT32;
      /* uint32_t */
      if (typeid(type) == typeid(uint32_t))
            return UA_TYPES_UINT32;
      if (typeid(type) == typeid(uint))
            return UA_TYPES_UINT32;
      /* int64_t */
      if (typeid(type) == typeid(int64_t))
            return UA_TYPES_INT64;
      /* uint64_t */
      if (typeid(type) == typeid(uint64_t))
            return UA_TYPES_UINT64;
      /* float */
      if (typeid(type) == typeid(float))
            return UA_TYPES_FLOAT;
      /* double */
      if (typeid(type) == typeid(double))
            return UA_TYPES_DOUBLE;
      /* strings */
      if (typeid(type) == typeid(std::string))
            return UA_TYPES_STRING;
      if (typeid(type) == typeid(char *))
            return UA_TYPES_STRING;
      if (typeid(type) == typeid(const char *))
            return UA_TYPES_STRING;

      return ret;
   }

   /**
    * @brief Convert a UA_String to a string variable
    * @param value the string to fill
    * @param opcval the value to copy from
    */
   void convertFromOPC(std::string *value, const UA_Variant *opcval);

   template <typename V>
   /**
    * @brief Convert a value to a templateable value
    * @param value the templateable value, e.g. int
    * @param opcval the value to copy from
    */
   void convertFromOPC(V *value, const UA_Variant *opcval) {
      V *v = reinterpret_cast<V *>(opcval->data);
      *value = *v;
   }

   template <typename V>
   /**
    * @brief Convert a value to a templateable value
    * @param value the templateable value, e.g. int
    * @param opcval the value to copy from
    */
   void convertFromOPC(std::vector<V> *value, const UA_Variant *opcval) {

      if (opcval->arrayLength < 1)
         return;

      if (opcval->arrayDimensionsSize > 1)
         return;

      V *vec = reinterpret_cast<V>(opcval->data);

      for (uint64_t i = 0; i < opcval->arrayLength; i++) {
         *value->push_back(vec[i]);
      }
   }

   template <typename V>
   /**
    * @brief Convert a value to a templateable value
    * @param value the templateable value, e.g. int
    * @param opcval the value to copy from
    */
   void convertFromOPC(V *value, const UA_DataValue *opcval) {
      if (opcval->hasValue) {
         convertFromOPC(value, &opcval->value);
      }
   }

   template <typename W>
   /**
    * @brief Convert a templateable value to a Open62541 value
    * @param value the value to copy to
    * @param userval the value to copy from
    */
   void convertToOPC(UA_Variant *value, const W *userval) {
      int datatypenr = convertTypeToOpen62541Type(*userval);
      UA_Variant_setScalarCopy(value, userval,
                           &UA_TYPES[datatypenr]);
   }

   template <typename W>
   /**
    * @brief Convert a templateable value to a Open62541 value
    * @param value the value to copy to
    * @param userval the value to copy from
    */
   void convertToOPC(UA_DataValue *value, const W *userval) {
      convertToOPC(&value->value, userval);
      value->hasValue = true;
   }

   /**
    * @brief Convert a string to a open62541 Variant
    * @param value the variant to fill
    * @param userval the string to copy from
    */
   void convertToOPC(UA_Variant *value, const std::string *userval);

   template <typename V>
   /**
    * @brief Convert a std::vector to a open62541 Variant array
    * @param value the variant to fill
    * @param uservec the vector to copy from
    */
   void convertToOPC(UA_Variant *value, const std::vector<V> *uservec) {
      uint64_t arraycount = 0;
      V val;

      if (uservec->size() < 1)
         return;

      int datatypenr = convertTypeToOpen62541Type(val);

      V *arr = static_cast<V*>(UA_Array_new(uservec->size(), &UA_TYPES[datatypenr]));

      if (value == nullptr)
         return;

      typename std::vector<V>::const_iterator it = uservec->begin();

      while (it != uservec->end()) {
         arr[arraycount] = (*it);
         arraycount++;
         it++;
      }

      UA_Variant_setArray(value, arr, uservec->size(), &UA_TYPES[datatypenr]);
   }

   /**
    * @brief Convert a std::vector to a open62541 Variant array
    * @param value the variant to fill
    * @param uservec the vector to copy from
    */
   void convertToOPC(UA_Variant *value,
                     const std::vector<std::string> *uservec);

   /**
    * @brief Check if a open62541 Varaiant convertable to a std::vector
    * @param opcval the value to check
    * @return if the value is convertable
    */
   bool isVectorizable(UA_Variant *opcval) {
      if (opcval->arrayDimensionsSize == 1)
         return true;
      return false;
   }

   /**
    * @brief Check if a open62541 Variant is a array
    * @see isVectorizable for 1 dimensional arrays
    * @param opcval the value to check
    * @return if the value contains an array
    */
   bool isArray(UA_Variant *opcval) {
      if (opcval->arrayDimensionsSize > 0)
         return true;
      return false;
   }

   /**
    * @brief setOPCSourceTimeStamp
    * @param value A value the timestamp should be set to
    * @param time_point The timestamp to set
    *
    * Set the source time stamp of value to the given time in time_point
    */
   void setOPCSourceTimeStamp(UA_DataValue *value, time_t time_point);

   /**
    * @brief setOPCSourceTimeStampNow
    * @param value A value the timestamp should be set to
    *
    * Set the source timestamp of a value to the current system time
    */
   void setOPCSourceTimeStampNow(UA_DataValue *value);

   /**
    * @brief writeToServer
    * @param var the varialbe to write to the server
    *
    * Write a OPC Variable back to the server
    */
   void writeToServer(UA_Variant var);

};

/*
 *
 */

/**
 * @brief Callback for a open62541 variable read method
 */
typedef std::function<bool(const UA_NodeId *sessionId,
                           void *sessionContext,
                           UA_Boolean includeSourceTimeStamp,
                           const UA_NumericRange *range,
                           UA_DataValue *value)>
OpcUAVarDataSourceReadCallback;

/**
 * @brief Simple callback for a open62541 variable read method
 */
typedef std::function<bool(UA_DataValue *value)>
OpcUAVarDataSourceReadCallbackSimple;

/**
 * @brief Callback for a open62541 variable write method
 */
typedef std::function<bool(const UA_NodeId *sessionId,
                           void *sessionContext,
                           const UA_NumericRange *range,
                           const UA_DataValue *value)>
OpcUAVarDataSourceWriteCallback;

/**
 * @brief Simple callback for a open62541 variable write method
 */
typedef std::function<bool(const UA_DataValue *value)>
OpcUAVarDataSourceWriteCallbackSimple;

class OpcUAVarNodeContext: public OpcUANodeContext {
private:
   /**
    * @brief Write callback method for this variable
    */
   OpcUAVarDataSourceWriteCallback write;
   /**
    * @brief Read callback method for this variable
    */
   OpcUAVarDataSourceReadCallback read;

   /**
    * @brief Simple write callback method for this variable
    */
   OpcUAVarDataSourceWriteCallbackSimple write_simple;
   /**
    * @brief Simple read callback method for this variable
    */
   OpcUAVarDataSourceReadCallbackSimple read_simple;

   /**
    * @brief The variable node arrtibutes as used in open62541
    */
   UA_VariableAttributes varAttr;
public:
   /**
    * @brief Constructor for this class
    * @param node A preinitialized node
    * @param nodeHandler The nodeHandler this node will be handled
    */
   OpcUAVarNodeContext(UA_NodeId *node, OpcUANodeHandler *nodeHandler):
      OpcUANodeContext(node, nodeHandler),
      varAttr(UA_VariableAttributes_default) {}

   /**
    * @brief Constructor for this class
    * @param nodeHandler The nodeHandler this node will be handled
    */
   OpcUAVarNodeContext(OpcUANodeHandler *nodeHandler):
      OpcUANodeContext(nodeHandler),
      varAttr(UA_VariableAttributes_default) {}

   /**
    * @brief Default deconstructor
    */
   virtual ~OpcUAVarNodeContext();

   /**
    * @brief Set the read method for this variable
    * @param method The read callback method
    */
   void setReadMethod(OpcUAVarDataSourceReadCallback method) { read = method; }

   /**
    * @brief Set the write method for this variable
    * @param method The write callback method
    */
   void setWriteMethod(OpcUAVarDataSourceWriteCallback method) {
      write = method;
   }

   /**
    * @brief Set the simple read method for this variable
    * @param method The simple read callback method
    */
   void setReadMethodSimple(OpcUAVarDataSourceReadCallbackSimple method) {
      read_simple = method;
   }

   /**
    * @brief Set the simple write method for this variable
    * @param method The simple write callback method
    */
   void setWriteMethodSimple(OpcUAVarDataSourceWriteCallbackSimple method) {
      write_simple = method;
   }

   /**
    * @brief Return the write callback method
    * @return The write callback method
    */
   OpcUAVarDataSourceWriteCallback getWrite() {return write; }

   /**
    * @brief Return the read callback method
    * @return The read callback method
    */
   OpcUAVarDataSourceReadCallback getRead() { return read; }

   /**
    * @brief Return the simple write callback method
    * @return The simple write callback method
    */
   OpcUAVarDataSourceWriteCallbackSimple getWriteSimple() {
      return write_simple;
   }

   /**
    * @brief Return the simple read callback method
    * @return The simple read callback method
    */
   OpcUAVarDataSourceReadCallbackSimple getReadSimple() {
      return read_simple;
   }


   /**
    * @brief Set the attribute name to the node
    */
   void setAttrName();

   /**
    * @brief Delete the attribute name of the node
    */
   void deleteAttrName();

   /**
    * @brief Set the attribute description to the node
    */
   void setAttrDescription();

   /**
    * @brief Delete the attribute description of the node
    */

   void deleteAttrDescription();
   /**
    * @brief Set the data type for the node
    */
   void setAttrDataType();

   /**
    * @brief Set the attributes readable status of the node
    */
   void setAttrReadable();

   /**
    * @brief Set the attributes writable status of the node
    */
   void setAttrWriteable();

   /**
    * @brief Return the Attributes for this variable node
    * @return The Variable Attriibute structure
    */
   UA_VariableAttributes *getVariableAttr() {
      return &varAttr;
   }

   /**
    * @brief Set the variables readable status
    * @param readable true if the node should be readable, else false
    */
   void setReadable(bool readable);

   /**
    * @brief Return the readable status of this node
    * @return true if the node is readable, else false
    */
   bool getReadable() {
      return _readable;
   }

   /**
    * @brief Set the variables writable status
    * @param writeable true if the node should be writeable, else false
    */
   void setWriteable(bool writeable);

   /**
    * @brief Return the writeable status of this node
    * @return true if the node is writeable, else false
    */
   bool getWriteable() {
      return _writeable;
   }
};

/*
 *
 */

class OpcUAObjectNodeContext: public OpcUANodeContext {
private:
   /**
    * @brief The objects attribute structure given to open62541
    */
   UA_ObjectAttributes objAttr;

   /**
    * @brief The objects type number, as used in open62541
    */
   int8_t objtype;

   /**
    * @brief Check the type name of the object and return the object type number
    * @param objtypename the name to check
    * @return the object number or -1 on error
    */
   int8_t checkTypeName(std::string objtypename);

   /**
    * @brief Check the object type number against a open62541 object number
    * @param objecttypenr the number to check
    * @return true on equality, else false
    */
   bool checkTypeNumber(int8_t objecttypenr);

public:
   /**
    * @brief Constructor for this class
    * @param node A preinitialized node
    * @param nodeHandler The node handler this node gets handled at
    */
   OpcUAObjectNodeContext(UA_NodeId *node, OpcUANodeHandler *nodeHandler):
      OpcUANodeContext(node, nodeHandler),
      objAttr(UA_ObjectAttributes_default),
      objtype(UA_NS0ID_BASEOBJECTTYPE) {}

   /**
    * @brief Constructor for this class
    * @param nodeHandler The node handler this node gets handled at
    */
   OpcUAObjectNodeContext(OpcUANodeHandler *nodeHandler):
      OpcUANodeContext(nodeHandler),
      objAttr(UA_ObjectAttributes_default),
      objtype(UA_NS0ID_BASEOBJECTTYPE) {}

   /**
    * @brief The default deconstructor for this class
    */
   virtual ~OpcUAObjectNodeContext();

   /**
    * Set the Type by name of the objecttype, at the time of writting this
    * there are two possibilities: "base" and "folder"
    * @brief Set the object type
    * @param objecttypename The name of the type of our object
    * @return True if found and set, else false
    */
   bool setObjectType(std::string objecttypename);

   /**
    * @brief Set the object Type directly by its coresponding number
    * @param objectTypeNr The Number to set
    * @return True if set, else false
    */
   bool setObjectType(int8_t objectTypeNr);

   /**
    * @brief Return the Object type
    * @return the object type number
    */
   int8_t getObjectType() {
      return objtype;
   }

   /**
    * @brief Returns the objects attrubute structure
    * @return the object structure
    */
   UA_ObjectAttributes *getObjectAttr() {
      return &objAttr;
   }

   /**
    * @brief Set the attribute name of the node
    */
   void setAttrName();

   /**
    * @brief Delete the attribute name of the node
    */
   void deleteAttrName();

   /**
    * @brief Set the attribute description
    */
   void setAttrDescription();

   /**
    * @brief Delete the attribue description of the node
    */
   void deleteAttrDescription();
};

/*
 *
 */

/**
 * @brief Callback method for remote proecure calls
 */
typedef std::function<bool(const UA_NodeId *sessionId,
                           void *sessionContext,
                           const UA_NodeId *objectId,
                           void *objectContext,
                           size_t inputSize,
                           const UA_Variant *input,
                           size_t outputSize,
                           UA_Variant *output)> OpcUAMethodCallback;

/**
 * @brief Simple callback method for remote proecure calls
 */
typedef std::function<bool(size_t inputSize,
                           const UA_Variant *input,
                           size_t outputSize,
                           UA_Variant *output)> OpcUAMethodCallbackSimple;


class OpcUAMethodNodeContext: public OpcUANodeContext {
public:

private:
   /**
    * @brief The method attribute structure for open62541
    */
   UA_MethodAttributes methodAttr;

   /**
    * @brief The input arguments (parameters) for the remote procedure calls
    */
   UA_Argument *inputArguments;

   /**
    * @brief The output arguments (return arguments) for the remote procedure
    * call
    */
   UA_Argument *outputArguments;

   /**
    * @brief The count of the input arguments
    */
   uint64_t inArgumentCount;

   /**
    * @brief The count of the output arguments
    */
   uint64_t outArgumentCount;

   /**
    * @brief The callback method for this remote procedure call
    */
   OpcUAMethodCallback callback;

   /**
    * @brief The simple callback method for this remote procedure call
    */
   OpcUAMethodCallbackSimple callbackSimple;

   /**
    * @brief Free all input arguments
    */
   void freeInputArguments();

   /**
    * @brief Free all output arguments
    */
   void freeOutputArguments();

   /**
    * @brief Allocate argCount arguments
    * @param argCount The argument count to allocate
    * @return an array of arguments or NULL
    */
   UA_Argument *allocArguments(uint64_t argCount);

   /**
    * @brief Initialize the arguments with default values
    * @param argCount The argument count to initialize
    * @param args The arguments to initialize
    */
   void initArguments(uint64_t argCount, UA_Argument *args);

public:
   /**
    * @brief The constructor for this class
    * @param node A preinitialized node
    * @param nodeHandler The nodehandler, this node is handled at
    */
   OpcUAMethodNodeContext(UA_NodeId *node, OpcUANodeHandler *nodeHandler):
      OpcUANodeContext(node, nodeHandler),
      methodAttr(UA_MethodAttributes_default),
      inputArguments(nullptr), outputArguments(nullptr),
      inArgumentCount(0), outArgumentCount(0),
      callback(nullptr), callbackSimple(nullptr) {}

   /**
    * @brief The constructor for this class
    * @param nodeHandler The nodehandler, this node is handled at
    */
   OpcUAMethodNodeContext(OpcUANodeHandler *nodeHandler):
      OpcUANodeContext(nodeHandler),
      methodAttr(UA_MethodAttributes_default),
      inputArguments(nullptr), outputArguments(nullptr),
      inArgumentCount(0), outArgumentCount(0),
      callback(nullptr), callbackSimple(nullptr) {}

   /**
    * @brief Default deconstructor
    */
   ~OpcUAMethodNodeContext();

   /**
    * @brief Return the Method nodes attribute structure
    * @return the method node structure
    */
   UA_MethodAttributes *getMethodAttr() {
      return &methodAttr;
   }

   /**
    * @brief Set if the Method is executable
    * @param executable true if the method should be executable, else false
    */
   void setExecutable(bool executable);

   /**
    * @brief Set if the method should be user executable
    * @param uexecutable true if it method should be executable, else false
    */
   void setUserExecutable(bool uexecutable);

   /**
    * @brief Initialize the output arguments for this remote procedure call
    * @param outArgCount The count of the arguments to initialize
    */
   void initOutputArguments(uint64_t outArgCount = 0);

   /**
    * @brief Initialize the input arguments for this remote procedure call
    * @param inArgCount The count of the arguments to initialize
    */
   void initInputArguments(uint64_t inArgCount = 0);

   template <typename T>
   /**
    * @brief Initialize the input argument type for an argument
    * @param argNum The number of the argument (0 - n)
    * @param argType The templateble type of the argument
    * @return True if the argument got initialized, else false
    */
   bool initInputArgumentType(uint64_t argNum, T argType) {
      if (argNum > inArgumentCount)
         return false;

      int16_t type = convertTypeToOpen62541Type(argType);
      if ((type < 0) && (type > UA_TYPES_COUNT))
         return false;

      inputArguments[argNum].dataType = UA_TYPES[type].typeId;

      return true;
   }

   template <typename T>
   /**
    * @brief Initialize the input argument type for an argument
    * @param argNum The number of the argument (0 - n)
    * @param argType The templateble type of the argument
    * @return True if the argument got initialized, else false
    */
   bool initInputArgumentType(uint64_t argNum, std::vector<T> argType) {
      if (argNum > inArgumentCount)
         return false;

      T element;

      int16_t type = convertTypeToOpen62541Type(element);
      if ((type < 0) && (type > UA_TYPES_COUNT))
         return false;

      inputArguments[argNum].dataType = UA_TYPES[type].typeId;

      return true;
   }

   template <typename T>
   /**
    * @brief Initialize the output argument type for an argument
    * @param argNum The number of the argument (0 - n)
    * @param argType The templateble type of the argument
    * @return True if the argument got initialized, else false
    */
   bool initOutputArgumentType(uint64_t argNum, T argType) {
      if (argNum > outArgumentCount)
         return false;

      int16_t type = convertTypeToOpen62541Type(argType);
      if ((type < 0) && (type > UA_TYPES_COUNT))
         return false;

      outputArguments[argNum].dataType = UA_TYPES[type].typeId;

      return true;
   }

   template <typename T>
   /**
    * @brief Initialize the input argument type for an argument
    * @param argNum The number of the argument (0 - n)
    * @param argType The templateble type of the argument
    * @return True if the argument got initialized, else false
    */
   bool initOutputArgumentType(uint64_t argNum, std::vector<T> argType) {
      if (argNum > outArgumentCount)
         return false;

      T element;

      int16_t type = convertTypeToOpen62541Type(element);

      if ((type < 0) && (type > UA_TYPES_COUNT))
         return false;

      outputArguments[argNum].dataType = UA_TYPES[type].typeId;

      return true;
   }

   /**
    * @brief Set the input argument rank
    * @param argNum The number of the argument (0 - n)
    * @param rank The rank of the argument
    * @return True if the rank got set, else false
    */
   bool setInputArgumentRank(uint64_t argNum, int32_t rank);

   /**
    * @brief Set the input argument rank
    * @param argNum The number of the argument (0 - n)
    * @param rank The rank of the argument
    * @return True if the rank got set, else false
    */
   bool setOutputArgumentRank(uint64_t argNum, int32_t rank);

   /**
    * @brief Set the name of the input argument
    * @param argNum The number of the argument (0 - n)
    * @param name The name to set
    * @return True if the name was set, else false
    */
   bool setInputArgumentName(uint64_t argNum, std::string name);

   /**
    * @brief Set the name of the output argument
    * @param argNum The number of the argument (0 - n)
    * @param name The name to set
    * @return True if the name was set, else false
    */
   bool setOutputArgumentName(uint64_t argNum, std::string name);

   /**
    * @brief Set the node attibute name of the node
    */
   void setAttrName();

   /**
    * @brief Delete the node attribute name of the node
    */
   void deleteAttrName();

   /**
    * @brief Set the attibute description of the node
    */
   void setAttrDescription();

   /**
    * @brief Delete the attribute description of the node
    */
   void deleteAttrDescription();

   /**
    * @brief Set the attributes executable status
    * @param executable True if the method should be executable, else false
    */
   void setAttrExecutable(bool executable);

   /**
    * @brief Set the attributes user executable status
    * @param uexecutable
    */
   void setAttrUserExecutable(bool uexecutable);

   /**
    * @brief Set the attributes write mask
    * @param writemask The writemask to set
    */
   void setAttrWriteMask(uint32_t writemask);

   /**
    * @brief Set the attributes user write mask
    * @param uwritemask The writemask to set
    */
   void setAttrUserWriteMask(uint32_t uwritemask);

   /**
    * @brief Return the input argument count of this remote procedure call
    * @return The input argument count
    */
   uint64_t getInputArgumentCount() {
      return inArgumentCount;
   }

   /**
    * @brief Return the output argument count of this remote procedure call
    * @return The output argument count
    */
   uint64_t getOutputArgumentCount() {
      return outArgumentCount;
   }

   /**
    * @brief Returns the input arguments
    * @return The input argument array
    */
   UA_Argument *getInputArguments() {
      return inputArguments;
   }

   /**
    * @brief Returns the output arguments
    * @return The output argument array
    */
   UA_Argument *getOutputArguments() {
      return outputArguments;
   }

   /**
    * @brief Set the callback for this remote procedure call
    * @param method The method to set
    */
   void setCallback(OpcUAMethodCallback method) {
      callback = method;
   }

   /**
    * @brief Set the simple callback for this remote procedure call
    * @param method The method to set
    */
   void setCallbackSimple(OpcUAMethodCallbackSimple method) {
      callbackSimple = method;
   }

   /**
    * @brief Returns the callback for this remote procedure call
    * @return The set callback
    */
   OpcUAMethodCallback getCallback() {
      return callback;
   }

   /**
    * @brief Returns the simple callback for this remote procedure call
    * @return The set simple callback
    */
   OpcUAMethodCallbackSimple getCallbackSimple() {
      return callbackSimple;
   }
};

} /* namespace n_opcua */

#endif // OPCUANODECONTEXT_H
