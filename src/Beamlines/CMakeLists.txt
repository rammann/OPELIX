set (_SRCS
    Beamline.cpp
    )

set (HDRS
    Beamline.h
    )

target_include_directories(opalx PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(opalx PRIVATE ${_SRCS})

install (FILES ${HDRS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/Beamlines")