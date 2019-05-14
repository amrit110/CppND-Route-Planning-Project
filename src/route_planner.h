#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "route_model.h"


class RoutePlanner {
  public:
    RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y);
    // Add public variables or methods declarations here.
    void AStarSearch();
    float GetDistance() {
      return distance;
    }

  private:
    // Add private variables or methods declarations here.
    std::vector<RouteModel::Node *> open_list;
    RouteModel::Node *NextNode();
    void AddNeighbors(RouteModel::Node *);
    float CalculateHValue(const RouteModel::Node *);
    std::vector<RouteModel::Node> ConstructFinalPath(RouteModel::Node *);
    RouteModel::Node *start_node;
    RouteModel::Node *end_node;
    float distance;
    RouteModel &m_Model;
};
