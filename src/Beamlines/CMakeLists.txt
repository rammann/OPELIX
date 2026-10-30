set (_SRCS
    Beamline.cpp
    FlaggedElmPtr.cpp
    )

set (HDRS
    Beamline.h
    FlaggedElmPtr.h
    )

target_include_directories(opalx PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(opalx PRIVATE ${_SRCS})

install (FILES ${HDRS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/Beamlines")