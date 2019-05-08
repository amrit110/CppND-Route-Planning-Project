#include "route_model.h"
#include <iostream>

RouteModel::RouteModel(const std::vector<std::byte> &xml) : Model(xml) {
    int node_idx = 0;
    for(Model::Node node : this->Nodes()) {
        RouteModel::m_Nodes.push_back(RouteModel::Node(node_idx, this, node));
        node_idx++;
    }
}