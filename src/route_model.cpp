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
    for (auto &road : this->Roads()) {
        if (road.type != Model::Road::Type::Footway) {
            for (int node_idx : this->Ways()[road.way].nodes) {
                if (this->node_to_road.find(node_idx) == this->node_to_road.end()) {
                    this->node_to_road[node_idx] = std::vector<const Model::Road*> ();
                }
                this->node_to_road[node_idx].push_back(&road);
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