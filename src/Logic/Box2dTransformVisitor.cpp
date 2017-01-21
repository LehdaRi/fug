#include <Eigen/Geometry>
#include "Logic/Box2dTransformVisitor.hpp"

#include <iostream> // TEMP

using namespace fug;

void Box2dTransformVisitor::operator()(TransformComponent& transform, Box2dBodyComponent& body)
{
    auto bodyPtr = body.body;
    auto pos = bodyPtr->GetPosition();
    transform.transform = Eigen::Affine3f(Eigen::Translation3f(pos.x, pos.y, 0) * Eigen::AngleAxisf(bodyPtr->GetAngle(), Vector3f(0, 0, -1))).matrix();
    std::cout << pos.x << " " << pos.y << " " << bodyPtr->GetMass() << " " << bodyPtr->IsActive() << " " << bodyPtr->IsAwake() << std::endl;
}
