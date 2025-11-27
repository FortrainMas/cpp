add_library(utils
    utils/RandomGenerator.cpp
)

add_library(simulation
    simulation/Loader.cpp
    simulation/Warehouse/Warehouse.cpp
    simulation/Warehouse/AccountingSystem.cpp
)

target_include_directories(utils PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(simulation PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(main main.cpp)
target_link_libraries(simulation utils)
target_link_libraries(main simulation)
