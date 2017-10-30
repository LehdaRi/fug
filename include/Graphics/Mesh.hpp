#ifndef FUG_GRAPHICS_MESH_HPP
#define FUG_GRAPHICS_MESH_HPP



#include <GL/glew.h>
#include <Core/ResourceManager.hpp>
#include <Core/MathTypes.hpp>
#include <Graphics/Material.hpp>



namespace fug {

    class FUG_RESOURCE(Mesh) {
    public:
        //  Resource init and destroy template member function declarations
        FUG_RESOURCE_INIT_DESTROY_DECL

        Mesh(void);

        const ResourcePointer<Material>& getMaterialPointer(void) const;
        GLuint getVAO() const;
        uint32_t getIndexCount() const;

    private:
        ResourcePointer<Material> _material;

        GLuint _vertexArrayObjectId;

        uint32_t _nIndices;

        GLuint _positionBufferId;
        GLuint _texCoordBufferId;
        GLuint _normalBufferId;
        GLuint _elementBufferId;
    };

} // namespace fug


#endif // FUG_GRAPHICS_MESH_HPP
