#include "src/vk.h"
#include "GLFW/Events.h"
#include "tools/three_d_object.h"
#include "tools/two_d_object.h"

int main() {

    try {
        class three_d_object* room = new three_d_object("somnus_engine/models/viking_room.obj", "somnus_engine/textures/viking_room.png", 0);
        class three_d_object* eye = new three_d_object("somnus_engine/models/eye.obj", "somnus_engine/textures/pxfuel.jpg", 1);
        eye->info.pos = glm::vec3(2, 2, 2);
        class three_d_object* sheesh = new three_d_object("somnus_engine/models/sheesh.obj", "somnus_engine/textures/almond_blossom.jpg", 2);
        class three_d_object* roomAlmond = new three_d_object("somnus_engine/models/viking_room.obj", "somnus_engine/textures/retro_glitch_tokyo.jpg", 3);
        roomAlmond->info.pos = glm::vec3(1, 1, 1);
        class two_d_object* square = new two_d_object("somnus_engine/textures/almond_blossom.jpg", 0);
        class vk* app = new class vk;
        app->p_vk = app;
        app->threeDObjects.push_back(room);
        app->threeDObjects.push_back(eye);
        app->threeDObjects.push_back(sheesh);
        app->threeDObjects.push_back(roomAlmond);
        app->twoDObjects.push_back(square);
        app->run();
        delete app;
        delete room;
        delete sheesh;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}