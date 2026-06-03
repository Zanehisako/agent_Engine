file(GLOB_RECURSE GAME_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
)

add_library(Game SHARED
    ${GAME_SOURCES}
)

target_link_libraries(
    Game
    PRIVATE
        AgentEngine
)
