#ifndef FUG_GRAPHICS_RENDERER_HPP
#define FUG_GRAPHICS_RENDERER_HPP


#include "Core/MathTypes.hpp"
#include "Core/Visitor.hpp"
#include "Logic/TransformComponent.hpp"
#include "Graphics/ModelComponent.hpp"
#include "Graphics/Camera.hpp"

namespace fug {

    // NOTE: a second component here containing animation frame info!!
    class Renderer : public Visitor<Renderer, ModelComponent, TransformComponent>{
    public:
        Renderer(const Vector3Glf& position, const Vector3Glf& forward, const Vector3Glf& up,
                 const float& fov, const float& aspectRatio,
                 const float& near, const float& far);

        bool operator()(ModelComponent& mesh, TransformComponent& transform);

        Camera _cam;
    private:

    };

} // namespace fug


#endif // FUG_GRAPHICS_RENDERER_HPP
