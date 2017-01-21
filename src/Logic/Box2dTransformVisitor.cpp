#include <Eigen/Geometry>
#include "Logic/Box2dTransformVisitor.hpp"


using namespace fug;

void Box2dTransformVisitor::operator()(Box2dBodyComponent& body, TransformComponent& transform)
{
    auto pos = body.body->GetPosition();
    transform.transform = Eigen::Affine3f(Eigen::Translation3f(pos.x, pos.y, 0) * Eigen::AngleAxisf(body.body->GetAngle(), Vector3f(0, 0, -1))).matrix();
}