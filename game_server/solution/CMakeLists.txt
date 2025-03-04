cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)


include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(ModelGameLilrary STATIC
    src/model/dog.cpp
    src/model/game.cpp
    src/model/game_session.cpp
    src/model/map.cpp
    src/model/road_movement.cpp
    src/model/loot/loot_generator.cpp
    src/model/collision_detector/collision_detector.cpp
)

target_include_directories(ModelGameLilrary PUBLIC ${ZLIB_INCLUDES} 
    src/model 
    src/utils
)

target_link_libraries(ModelGameLilrary PUBLIC Threads::Threads CONAN_PKG::boost)


add_executable(game_server
    src/application/application.cpp
    src/database/postgres/postgres.cpp
    src/http_server/session.cpp
    src/json_loader/json_loader.cpp
    src/logger/logger.cpp
    src/request/static_file_request_handler.cpp
    src/request/api_request_handler.cpp
    src/boost_json.cpp
    src/main.cpp
)

target_include_directories(game_server PRIVATE
    src                  
    src/application      
    src/constants
    src/database        
    src/http_server      
    src/json_loader      
    src/logger           
    src/parse_command_line  
    src/request          
    src/utils            
)


target_link_libraries(game_server PRIVATE 
    Threads::Threads    
    CONAN_PKG::boost
    CONAN_PKG::libpq
    CONAN_PKG::libpqxx    
    ModelGameLilrary    
)

