# add_executable(classifier_client_node classifier_client_node.cpp ${PROJECT_SOURCES})
# add_dependencies(classifier_client_node ${${PROJECT_NAME}_EXPORTED_TARGETS})
# target_link_libraries(classifier_client_node ${PROJECT_LIBRARIES})

# Locomanipulability Visualization
add_subdirectory(planner_nodes)
add_subdirectory(reachability_regions)
add_subdirectory(visualization_nodes)