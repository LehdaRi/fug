#include "imgui-fug/imgui/imgui.h"
#include "imgui-fug/imgui_impl_sfml_gl3.h"

#include "Core/ResourceManager.hpp"
#include "Core/Binary.hpp"
#include "Core/Binary_Init_File.hpp"
#include "Core/Scene.hpp"
#include "Core/Text.hpp"
#include "Core/Text_Init_File.hpp"

#include "Engine/ResourceLoader.hpp"

#include "Graphics/GBuffer.hpp"
#include "Graphics/DirectionalLightPass.hpp"
#include "Graphics/LightShaderBinding.hpp"
#include "Graphics/LightShaderBinding_Init.hpp"
#include "Graphics/Material.hpp"
#include "Graphics/Material_Init.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Mesh_Init.hpp"
#include "Graphics/ModelComponent.hpp"
#include "Graphics/ShaderObject.hpp"
#include "Graphics/ShaderObject_Init_Text.hpp"
#include "Graphics/ShaderProgram.hpp"
#include "Graphics/ShaderProgram_Init_Default.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Texture_Init_Binary.hpp"
#include "Graphics/Texture_Init_Color.hpp"
#include "Graphics/VertexData.hpp"
#include "Graphics/VertexData_Init_Text.hpp"
#include "Graphics/VertexData_Init_UVSphere.hpp"
#include "Graphics/Canvas_SFML.hpp"
#include "Graphics/Renderer.hpp"

using namespace fug;

namespace {
    const float FOV = 90.f * PI / 180.f;
    const float Z_NEAR = 0.1f;
    const float Z_FAR = 100.f;
    const float MAX_PITCH = PI * 0.4999f;
    int CENTER_X = 1280 * 0.5;
    int CENTER_Y = 720 * 0.5;

    // Homogenous corner vectors for a full screen quad
    // TODO: Check why this doesn't match the quad's vert order
    const std::vector<Vector4Glf> ndcCorners{ Vector4Glf( 1.f, -1.f, 0.f, 1.f),
                                              Vector4Glf( 1.f,  1.f, 0.f, 1.f),
                                              Vector4Glf(-1.f, -1.f, 0.f, 1.f),
                                              Vector4Glf(-1.f,  1.f, 0.f, 1.f) };
    std::vector<GLfloat> getHomogenousVectors(const Matrix4Glf& normalToView)
    {
        std::vector<GLfloat> vecs(8);
        for (auto i = 0u; i < 4; ++i) {
            auto v = ndcCorners[i];
            v = normalToView * v;
            v /= v[3];
            v /= v[2];
            vecs[i*2] = v[0];
            vecs[i*2 + 1] = v[1];
        }
        return vecs;
    }

}

int main(void)
{
    Canvas_SFML c;
    sf::Window* window = c.getWindow();

    ImGui_ImplSFMLGL3_Init(window);

    // Load resources
    ResourceLoader resourceLoader("deferred_pbr.stfu");
    resourceLoader.load();

    auto ironMaterialResPtr = FUG_RESOURCE_MANAGER.getResource<Material>(FUG_RESOURCE_ID_MAP.getId("material_iron"));
    auto copperMaterialResPtr = FUG_RESOURCE_MANAGER.getResource<Material>(FUG_RESOURCE_ID_MAP.getId("material_copper"));
    auto redPlasticMaterialResPtr = FUG_RESOURCE_MANAGER.getResource<Material>(FUG_RESOURCE_ID_MAP.getId("material_red_plastic"));
    auto sphereMeshResPtr = FUG_RESOURCE_MANAGER.getResource<Mesh>(FUG_RESOURCE_ID_MAP.getId("mesh_uvsphere"));
    auto quadMeshResPtr = FUG_RESOURCE_MANAGER.getResource<Mesh>(FUG_RESOURCE_ID_MAP.getId("mesh_quad"));
    auto dirLightBindResPtr = FUG_RESOURCE_MANAGER.getResource<LightShaderBinding>(FUG_RESOURCE_ID_MAP.getId("light_shader_binding_directional"));


    // Set up scene
    Matrix4Glf translation;
    translation << 1.f, 0.f, 0.f,  0.f,
                   0.f, 1.f, 0.f,  0.f,
                   0.f, 0.f, 1.f, -2.f,
                   0.f, 0.f, 0.f,  1.f;
    Matrix4Glf rotMatX;
    rotMatX << 1.f,         0.f,          0.f,  0.f,
               0.f, cosf(1.57f), -sinf(1.57f), -1.f,
               0.f, sinf(1.57f),  cosf(1.57f),  0.f,
               0.f ,        0.f,          0.f,  1.f;
    Matrix4Glf scaleMat;
    scaleMat << 10.f,  0.f,  0.f, 0.f,
                 0.f, 10.f,  0.f, 0.f,
                 0.f,  0.f, 10.f, 0.f,
                 0.f,  0.f,  0.f, 1.f;
    TransformComponent floorTransform;
    floorTransform.transform = translation * rotMatX * scaleMat;
    FUG_SCENE.addEntity();
    FUG_SCENE.addComponent(ModelComponent({redPlasticMaterialResPtr, quadMeshResPtr}));
    FUG_SCENE.addComponent(std::move(floorTransform));
    TransformComponent copperBallTransform;
    copperBallTransform.transform << 1.f, 0.f, 0.f, -1.5f,
                                      0.f, 1.f, 0.f,  0.5f,
                                      0.f, 0.f, 1.f,   0.f,
                                      0.f, 0.f, 0.f,   1.f;
    FUG_SCENE.addEntity();
    FUG_SCENE.addComponent(ModelComponent({copperMaterialResPtr, sphereMeshResPtr}));
    FUG_SCENE.addComponent(std::move(copperBallTransform));
    TransformComponent ironBallTransform;
    ironBallTransform.transform << 1.f, 0.f, 0.f, 1.5f,
                                   0.f, 1.f, 0.f, 0.5f,
                                   0.f, 0.f, 1.f,  0.f,
                                   0.f, 0.f, 0.f,  1.f;
    FUG_SCENE.addEntity();
    FUG_SCENE.addComponent(ModelComponent({ironMaterialResPtr, sphereMeshResPtr}));
    FUG_SCENE.addComponent(std::move(ironBallTransform));
    FUG_SCENE.addEntity();
    FUG_SCENE.addComponent(DirectionalLightComponent({ dirLightBindResPtr,
                                                       Vector3Glf(-0.5, -0.5, 1).normalized(),
                                                       { 1.0, 1.0, 1.0 }, { 0.05, 0.05, 0.05 }}));

    // Set up camera movement
    bool cameraActive = false;
    Vector3Glf camPos(0.f, 0.f, -3.f);
    float camPitch = 0.f;
    float camYaw = 0.f;
    Matrix4Glf camPitchMat = Matrix4Glf::Identity();
    Matrix4Glf camYawMat = Matrix4Glf::Identity();
    Vector3Glf camFwd = Vector3Glf(0.f, 0.f, 1.f);
    float lastMouseX = sf::Mouse::getPosition().x;
    float lastMouseY = sf::Mouse::getPosition().y;

    // Rendering control
    int currentMode = 0;
    const char* renderModes[] = { "Shaded", "Depth", "Normals", "Albedo", "Roughness",
                                  "Metalness" };


    Renderer renderer(camPos, Vector3Glf(0.f, 0.f, 1.f), Vector3Glf(0.f, 1.f, 0.f),
                      FOV, float(window->getSize().x) / window->getSize().y, Z_NEAR, Z_FAR);
    Matrix4Glf normalToView = renderer._cam.getView().transpose().inverse();
    DirectionalLightPass dirLightPass(quadMeshResPtr, normalToView,
                                      getHomogenousVectors(normalToView), 0);

    GBuffer gBuffer(1280, 720, { GL_R32F, GL_RGB32F, GL_RGBA16,  GL_R8,  GL_R8 },
                               {  GL_RED,    GL_RGB,   GL_RGBA, GL_RED, GL_RED } );

    bool running = true;
    while (running)
    {
        // handle events
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                // end the program
                running = false;
            } else if (event.type == sf::Event::Resized) {
                // adjust the viewport when the window is resized
                glViewport(0, 0, event.size.width, event.size.height);
                renderer._cam.projection(FOV, float(event.size.width) / event.size.height,
                                         Z_NEAR, Z_FAR);
                gBuffer.resize(event.size.width, event.size.height);
                CENTER_X = event.size.width * 0.5;
                CENTER_Y = event.size.height * 0.5;
            } else if (cameraActive || !ImGui::IsMouseHoveringAnyWindow()) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    // Enable mouse control and hide cursor
                    cameraActive = !cameraActive;
                    sf::Mouse::setPosition({CENTER_X, CENTER_Y}, *window);
                }
            }
            ImGui_ImplSFMLGL3_HandleEvent(event);
        }

        // Update cam position
        if (cameraActive) {
            sf::Vector2i mPos = sf::Mouse::getPosition(*window);
            if (mPos.x != CENTER_X || mPos.y != CENTER_Y) {
                camYaw += float(mPos.x - CENTER_X) / window->getSize().x;
                camPitch += float(mPos.y - CENTER_Y) / window->getSize().y;
                // Limit pitch
                camPitch = std::min(std::max(camPitch, -MAX_PITCH), MAX_PITCH);
                // Calculate new fwd vector
                camPitchMat << 1.f,            0.f,             0.f, 0.f,
                                0.f, cosf(camPitch), -sinf(camPitch), 0.f,
                                0.f, sinf(camPitch),  cosf(camPitch), 0.f,
                                0.f,            0.f,             0.f, 1.f;
                camYawMat <<  cosf(camYaw), 0.f, sinf(camYaw), 0.f,
                                        0.f, 1.f,          0.f, 0.f,
                                -sinf(camYaw), 0.f, cosf(camYaw), 0.f,
                                        0.f, 0.f,          0.f, 1.f;
                Vector4Glf fwd = camYawMat * camPitchMat * Vector4Glf(0.f, 0.f, 1.f, 0.f);
                camFwd = Vector3Glf(fwd[0], fwd[1], fwd[2]);
                sf::Mouse::setPosition({CENTER_X, CENTER_Y}, *window);
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            camPos += 0.05f * renderer._cam.getZAxis();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            camPos -= 0.05f * renderer._cam.getZAxis();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            camPos += 0.05f * renderer._cam.getXAxis();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            camPos -= 0.05f * renderer._cam.getXAxis();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            camPos += 0.05f * renderer._cam.getYAxis();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            camPos -= 0.05f * renderer._cam.getYAxis();
        renderer._cam.orient(camPos, camFwd, Vector3Glf(0.f, 1.f, 0.f));
        dirLightPass._normalToView = renderer._cam.getView().transpose().inverse();
        dirLightPass._hCorners = getHomogenousVectors(renderer._cam.getProj().inverse());

        // Handle basic camera control
        // Simple imgui-window
        ImGui_ImplSFMLGL3_NewFrame();
        // mousepos could be passed to imgui before event-handling and check this
        // to not pass mouse-events to program if hovering
        ImGui::GetIO().MouseDrawCursor = ImGui::IsMouseHoveringAnyWindow();
        {
            ImGui::ListBox("", &currentMode, renderModes, 6, 6);
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        // Draw geometry to gbuffer
        gBuffer.bindWrite();
        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        FUG_SCENE.accept(renderer);

        // Draw contents of buffer
        gBuffer.bindRead();
        dirLightPass._currentMode = currentMode;
        dirLightPass.initPass();
        FUG_SCENE.accept(dirLightPass);

        ImGui::Render();
        // end the current frame (internally swaps the front and back buffers)
        window->display();
    }
    return 0;
}