#include "route_model.h"
#include <iostream>
#include <vector>

/* Create RouteModel Nodes. Uses the "this" pointer to get the Nodes
from the parent Model and create RouteModel Nodes from them. */
RouteModel::RouteModel(const std::vector<std::byte> &xml) : Model(xml) {
    int node_idx = 0;
    for(Model::Node node : this->Nodes()) {
        this->m_Nodes.push_back(RouteModel::Node(node_idx, this, node));
        node_idx++;
    }
    CreateNodeToRoadHashmap();
}

/* Create a hash table of Node index values to a vector of Road pointers 
that those nodes belong to. */
void RouteModel::CreateNodeToRoadHashmap() {
    for (const Model::Road &road : this->Roads()) {
        if (road.type != Model::Road::Type::Footway) {
            for (int node_idx : this->Ways()[road.way].nodes) {
                if (this->node_to_road.find(node_idx) == this->node_to_road.end()) {
                    this->node_to_road[node_idx] = std::vector<const Model::Road *> ();
                }
                this->node_to_road[node_idx].push_back(&road);
            }
        }
    }
}

/* Find the closest unvisited node from a vector of node indices. */
RouteModel::Node *RouteModel::Node::FindNeighbor(std::vector<int> node_indices) {
    RouteModel::Node *closest_node = nullptr;
    RouteModel::Node node;
    for (int node_idx : node_indices) {
        node = this->parent_model->SNodes()[node_idx];
        if ((this->distance(node) != 0) && (!node.visited)) {
            if ((closest_node == nullptr) || ((this->distance(node) < this->distance(*closest_node)))) {
                closest_node = &this->parent_model->SNodes()[node_idx];
            }
        }
    }
    return closest_node;
}

// Find closest neighbor along all possible roads (directions) that a node is associated with.
void RouteModel::Node::FindNeighbors() {
    for (auto &road : this->parent_model->node_to_road[this->index]) {
        RouteModel::Node *node = this->FindNeighbor(this->parent_model->Ways()[road->way].nodes);
        if (node != nullptr) {
            this->neighbors.push_back(node);
        }
    }
}

// Given a coordinate in the map, find the closest node so that search can start/end there.
RouteModel::Node &RouteModel::FindClosestNode(float x, float y) {
    RouteModel::Node input;
    input.x = x;
    input.y = y;

    float min_dist = std::numeric_limits<float>::max();
    int closest_idx;

    for (const Model::Road &road : Roads()) {
        if (road.type != Model::Road::Type::Footway) {
            for (int node_idx : Ways()[road.way].nodes) {
                auto dist = input.distance(SNodes()[node_idx]);
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_idx = node_idx;
                }
            }
        }
    }
    return this->SNodes()[closest_idx];
}