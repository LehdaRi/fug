#include "Core/ResourceManager.hpp"

#include "Core/Binary.hpp"
#include "Core/Binary_Init_File.hpp"
#include "Core/Text.hpp"
#include "Core/Text_Init_File.hpp"

#include "Graphics/Material.hpp"
#include "Graphics/Material_Init.hpp"

#include "Graphics/Mesh.hpp"
#include "Graphics/Mesh_Init.hpp"
#include "Graphics/MeshComponent.hpp"

#include "Graphics/ShaderObject.hpp"
#include "Graphics/ShaderObject_Init_Text.hpp"
#include "Graphics/ShaderProgram.hpp"
#include "Graphics/ShaderProgram_Init_Default.hpp"

#include "Graphics/SpriteMeshComponent.hpp"
#include "Graphics/SpriteMaterial.hpp"
#include "Graphics/SpriteMaterial_Init.hpp"
#include "Graphics/SpriteMesh.hpp"
#include "Graphics/SpriteMesh_Init.hpp"
#include "Graphics/SpriteMeshComponent.hpp"

#include "Graphics/Texture.hpp"
#include "Graphics/Texture_Init_Binary.hpp"

#include "Graphics/VertexData.hpp"
#include "Graphics/VertexData_Init_Text.hpp"

#include "Graphics/Canvas_SFML.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/SpriteRenderer.hpp"

#include "Core/EventManager.hpp"
#include "Core/Scene.hpp"

#include "main.hpp"
#include "ControlVisitor.hpp"

int main()
{
    Canvas_SFML canvas;
    
    TransformComponent tran_comp;
    tran_comp.transform << 1.f, 0.f, 0.f, 0.f,
                           0.f, 1.f, 0.f, 0.f,
                           0.f, 0.f, 1.f, 1.f,
                           0.f, 0.f, 0.f, 1.f;

    ControlMapComponent ctrl_comp;
    ctrl_comp.map = {{sf::Keyboard::W, ControlMapComponent::Action::MoveUp},
                     {sf::Keyboard::S, ControlMapComponent::Action::MoveDown},
                     {sf::Keyboard::A, ControlMapComponent::Action::MoveLeft},
                     {sf::Keyboard::D, ControlMapComponent::Action::MoveRight}};

    FUG_SCENE.addEntity();
    FUG_SCENE.addComponent(loadCubeMeshComponent());
    FUG_SCENE.addComponent(std::move(tran_comp));
    FUG_SCENE.addComponent(std::move(ctrl_comp)); 

    ControlVisitor control_visitor;

    Renderer render_visitor(Vector3Glf(0.f, 0.f, -3.f), Vector3Glf(0.f, 0.f, 1.f),
                            Vector3Glf(0.f, 1.f, 0.f), 90.f, 1280/720.f, 1.f, 10.f);

    while (canvas.isOpen()) {

        // Handle window events
        canvas.handleEvents();

        // Clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Handle input
        FUG_SCENE.accept(control_visitor);

        // Draw
        FUG_SCENE.accept(render_visitor);

        // End the current frame (internally swaps the front and back buffers)
        canvas.display();

        // Flush event buffers
        FUG_EVENT_MANAGER.flushEvents<sf::Event>(sf::Event::KeyPressed);
        FUG_EVENT_MANAGER.flushEvents<sf::Event>(sf::Event::KeyReleased);
    }
    return 0;
}
