
set(SOURCE_HEADER
   ${SOURCE_HEADER}
   ${CMAKE_CURRENT_LIST_DIR}/OpcUANodeContext.h
   ${CMAKE_CURRENT_LIST_DIR}/OpcUANodeHandler.h
   ${CMAKE_CURRENT_LIST_DIR}/OpcUAServer.h
)

# Our Common Sources
set(SOURCE
   ${SOURCE}
   ${SOURCE_HEADER}
   ${CMAKE_CURRENT_LIST_DIR}/OpcUANodeContext.cpp
   ${CMAKE_CURRENT_LIST_DIR}/OpcUANodeHandler.cpp
   ${CMAKE_CURRENT_LIST_DIR}/OpcUAServer.cpp
)
