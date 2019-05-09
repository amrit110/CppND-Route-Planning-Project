#include "route_model.h"
#include <iostream>
#include <vector>

RouteModel::RouteModel(const std::vector<std::byte> &xml) : Model(xml) {
    int node_idx = 0;
    for(Model::Node node : this->Nodes()) {
        RouteModel::m_Nodes.push_back(RouteModel::Node(node_idx, this, node));
        node_idx++;
    }
    CreateNodeToRoadHashmap();
}

void RouteModel::CreateNodeToRoadHashmap() {
    for (const Model::Road &road : Roads()) {
        if (road.type != Model::Road::Type::Footway) {
            for (int node_idx : Ways()[road.way].nodes) {
                if (node_to_road.find(node_idx) == node_to_road.end()) {
                    node_to_road[node_idx] = std::vector<const Model::Road *> ();
                }
                node_to_road[node_idx].push_back(&road);
            }
        }
    }
}

RouteModel::Node* RouteModel::Node::FindNeighbor(std::vector<int> node_indices) {
    Node* closest_node = nullptr;
    Node node;
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

void RouteModel::Node::FindNeighbors() {
    for (auto &road : parent_model->node_to_road[this->index]) {
        RouteModel::Node *node = this->FindNeighbor(parent_model->Ways()[road->way].nodes);
        if (node != nullptr) {
            this->neighbors.push_back(node);
        }
    }
}

RouteModel::Node &RouteModel::FindClosestNode(float x, float y) {
    Node input;
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
    return SNodes()[closest_idx];;
}