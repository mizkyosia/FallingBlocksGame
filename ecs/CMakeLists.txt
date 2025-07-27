add_library(ECS World.cpp ECS.hpp Archetype.hpp Archetype.inl Archetype.cpp Commands.hpp Commands.inl CommandsImpl.hpp EntityEdit.hpp Global.hpp World.hpp World.inl Column.hpp Column.inl)
add_subdirectory(query)

target_include_directories(ECS PUBLIC .)

target_link_libraries(ECS PUBLIC Boost::core)