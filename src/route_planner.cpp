#include "route_planner.h"
#include <algorithm>

// RoutePlanner constructor, accepts input coordinates and scale values to percentages.

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    this->start_node = &m_Model.FindClosestNode(start_x, start_y);
    this->end_node = &m_Model.FindClosestNode(end_x, end_y);
}

/* Starting from a node (end node), iteratively traverse sequence of parent nodes 
storing them until the starting node is reached. */

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    std::vector<RouteModel::Node> path_found;
    distance = 0.0;
    RouteModel::Node parent_node;
    while (current_node->parent != nullptr) {
        path_found.push_back(*current_node);
        parent_node = *(current_node->parent);
        distance += current_node->distance(parent_node);
        current_node = current_node->parent;
    }
    path_found.push_back(*current_node);
    distance *= m_Model.MetricScale();

    return path_found;
}

// A* search function, starts from a node and iterates until a path is found.

void RoutePlanner::AStarSearch() {
    start_node->visited = true;
    open_list.push_back(start_node);
    RouteModel::Node *current_node = nullptr;

    while(open_list.size() > 0) {
        current_node = NextNode();
        if (current_node->distance(*end_node) == 0) {
            m_Model.path = ConstructFinalPath(current_node);
            return;
        }
        AddNeighbors(current_node);
    }

}

/* Heuristic function for A* search, here simply the euclidean distance
from a node to the end node. */

float RoutePlanner::CalculateHValue(const RouteModel::Node *node) {
    return node->distance(*(this->end_node));
}

/*Sort the list of open nodes in the A* search, 
return the node with the lowest f-value, and remove the node from the 
list of open nodes. */

RouteModel::Node *RoutePlanner::NextNode() {
    std::sort(open_list.begin(), open_list.end(), [](const auto &first_node, const auto &second_node) {
       return ((first_node->h_value + first_node->g_value) < (second_node->h_value + second_node->g_value)); 
    });
    RouteModel::Node *lowest_f_node = open_list.front();
    open_list.erase(open_list.begin());
    return lowest_f_node;
}

// Add neighbors, getting their h and g values.

void RoutePlanner::AddNeighbors(RouteModel::Node * current_node) {
    current_node->FindNeighbors();
    for (auto neighbor : current_node->neighbors) {
        neighbor->parent = current_node;
        neighbor->g_value = current_node->g_value + current_node->distance(*neighbor);
        neighbor->h_value = CalculateHValue(neighbor);
        open_list.push_back(neighbor);
        neighbor->visited = true;
    } 

}