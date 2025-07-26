add_library(ECS World.cpp ECS.hpp Archetype.hpp Archetype.inl Commands.hpp Commands.inl CommandsImpl.hpp EntityEdit.hpp Global.hpp World.hpp World.inl)
add_subdirectory(query)
add_subdirectory(assets)

target_include_directories(ECS PUBLIC .)

target_link_libraries(ECS PUBLIC Boost::core)