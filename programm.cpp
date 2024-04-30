#pragma once
#include "programm.h"
#include "objects/particles/effects.h"
#include <future>
#include "objects/game_objects/gameObject.h"
//#include "PhysX/PhysX.h"
#include "PhysX/PhysX_glm_conversion.h"

programm::programm(class init_storage* p_storage) : p_storage(p_storage), gameObjects(p_storage->gameObjects)
{
    p_vk = p_storage->p_vk;
    VkExtent2D currentExtent = {
        static_cast<uint32_t>(1024),
        static_cast<uint32_t>(1024)
    };
    p_vk->shadowExtent = currentExtent;
    p_vk->textureExtent = currentExtent;

    //-------------------------------------PhysX
    p_PhysX = new class PhysX(p_vk);
    p_PhysX->init();
    p_PhysX->createSimulation();
    p_PhysX->mMaterial = p_PhysX->mPhysics->createMaterial(100.0f, 10.0f, 0.01f);

    uint32_t location_id = p_vk->adduniqueModel("somnus_engine/res/models/loca.fbx");
    model* location_model = p_vk->models[location_id];
    gameObject* location_GO = new gameObject(location_model, p_storage, p_vk, p_PhysX);
    location_GO->createShape("somnus_engine/res/models/loca.obj", TYPE::UNDEFINED, TYPE::UNDEFINED);
    gameObjects.push_back(location_GO);
    Location* location = new Location(gameObjects[0]);
    p_storage->objects.push_back(location);
    gameObjects[0]->addStaticInstance(location, physx::PxVec3(0.0f, -1.0f, 0.0f));

    //print_g(p_vk->physicalObjectsInfo.size());
    uint32_t bomb_id = p_vk->adduniqueModel("somnus_engine/res/models/bomb.fbx");
    model* bomb_model = p_vk->models[bomb_id];
    gameObject* bomb_GO = new gameObject(bomb_model, p_storage, p_vk, p_PhysX);
    bomb_GO->createShape(0.5f, 1.0f, TYPE::UNDEFINED, TYPE::UNDEFINED);
    gameObjects.push_back(bomb_GO);

    uint32_t barrel_id = p_vk->adduniqueModel("somnus_engine/res/models/gbarrel.fbx");
    model* barrel_model = p_vk->models[barrel_id];
    gameObject* barrel_GO = new gameObject(barrel_model, p_storage, p_vk, p_PhysX);
    barrel_GO->createShape("somnus_engine/res/models/gbarrel_collider.obj", 1.0f, TYPE::Damagable, TYPE::Damaging);
    gameObjects.push_back(barrel_GO);

    uint32_t sparkles_id = p_vk->adduniqueModel("somnus_engine/res/models/sparkle.fbx");
    model* sparkle_model = p_vk->models[sparkles_id];
    particle* local_particles = new particle(sparkle_model, p_vk);
    p_storage->particles.push_back(local_particles);

    gameObject* blast_GO = new gameObject(p_storage, p_vk, p_PhysX);
    blast_GO->createShape(2.0f, 0.0f, TYPE::Damaging, TYPE::Damagable, true);
    gameObjects.push_back(blast_GO);

    uint32_t player_id = p_vk->adduniqueModel("somnus_engine/res/models/character.fbx");
    model* player_model = p_vk->models[player_id];
    gameObject* player_GO = new gameObject(player_model, p_storage, p_vk, p_PhysX);
    player_GO->createShape(physx::PxVec3(0.25f, 0.85f, 0.25f), 1.0f, TYPE::Damagable, TYPE::Damaging);
    gameObjects.push_back(player_GO);
    Player* player = new Player(gameObjects.back(), 100);
    p_storage->objects.push_back(player);
    gameObjects.back()->addDynamicInstance(player, physx::PxVec3(0));

    //uint32_t book_id = p_vk->adduniqueModel("somnus_engine/res/models/book.fbx");
    //model* book_model = p_vk->models[book_id];
    //gameObject* book_GO = new gameObject(book_model, p_storage, p_vk, p_PhysX);
    //book_GO->createShape(0.5f, 0.0f, TYPE::UNDEFINED, TYPE::UNDEFINED);
    //gameObjects.push_back(book_GO);
    //Barrel* book = new Barrel(gameObjects.back(), 2);
    //p_storage->objects.push_back(book);
    //gameObjects.back()->addStaticInstance(book);

    //Animation bookAnimation("somnus_engine/res/models/book.fbx", book_model);
    //Animator animator(&bookAnimation);
    //animations.push_back(&bookAnimation);
    //animators.push_back(&animator);

    //model bomb("somnus_engine/res/models/bomb.fbx", my_vk);
    //models.push_back(&bomb);
    //animators.push_back(&animator);

    //class button* middle_button = new button(glm::vec2(0, 0.7), 0.3, 0.3, "somnus_engine/res/textures/px.jpg", "middle", my_vk);
    //class button* right_button = new button(glm::vec2(0.7, 0.7), 0.3, 0.3, "somnus_engine/res/textures/almond_blossom.jpg", "right", my_vk);
    //class button* left_button = new button(glm::vec2(-0.7, 0.7), 0.3, 0.3, "somnus_engine/res/textures/retro_glitch_tokyo.jpg", "left", my_vk);
    //two_d_objects.push_back(middle_button->button_object);
    //two_d_objects.push_back(right_button->button_object);
    //two_d_objects.push_back(left_button->button_object);
    //
    //buttons.push_back(left_button);
    //buttons.push_back(right_button);
    //buttons.push_back(middle_button);

    std::vector<std::string> info;
    info.push_back("seva durak");
    fps_counter = new two_d_object(p_vk);
    textElements(0, 0, info, p_vk->mWindow, fps_counter);
    two_d_objects.push_back(fps_counter);

    p_vk->p_camera->horizontalAngle += glm::radians(225.0f);
    p_vk->p_camera->verticalAngle += glm::radians(-60.0f / 2);
    p_vk->p_camera->updateVectors();

    p_vk->lightSourcesInfo.resize(MAX_LIGHT_SOURCES);
    lightSourceInfo currentLightSourceInfo1{};
    currentLightSourceInfo1.proj = glm::perspective(glm::radians(45.f), 1.f, .1f, 100.f);
    currentLightSourceInfo1.view = glm::lookAt(p_vk->p_camera->position, p_vk->p_camera->position + p_vk->p_camera->direction, glm::vec3(0, 1, 0));
    currentLightSourceInfo1.lightPos = glm::vec3(1, 0, 1);
    currentLightSourceInfo1.lightDir = p_vk->p_camera->direction;
    currentLightSourceInfo1.intensity = 0.5f;
    p_vk->p_camera->horizontalAngle += glm::radians(180.0f);
    p_vk->p_camera->updateVectors();
    lightSourceInfo currentLightSourceInfo2{};
    currentLightSourceInfo2.proj = glm::perspective(glm::radians(45.f), 1.f, .1f, 100.f);
    currentLightSourceInfo2.view = glm::lookAt(glm::vec3( -p_vk->p_camera->position.x, p_vk->p_camera->position.y, -p_vk->p_camera->position.z), glm::vec3(-p_vk->p_camera->position.x, p_vk->p_camera->position.y, -p_vk->p_camera->position.z) + p_vk->p_camera->direction, glm::vec3(0, 1, 0));
    currentLightSourceInfo2.lightPos = glm::vec3(1, 0, 0);
    currentLightSourceInfo2.lightDir = p_vk->p_camera->direction;
    currentLightSourceInfo2.intensity = 0.5f;
    p_vk->lightSourcesInfo[0] = currentLightSourceInfo1;
    p_vk->lightSourcesInfo[1] = currentLightSourceInfo2;
    p_vk->current_light_sources = 2;

    p_vk->p_camera->horizontalAngle += glm::radians(180.0f);
    p_vk->p_camera->updateVectors();

    p_vk->buttons = buttons;

    p_vk->init_engine();

    mainLoop();
}

programm::~programm()
{
    for (int i = 0; i < p_vk->all_meshes.size(); i++) {
        delete p_vk->all_meshes[i];
    }
    for (int i = 0; i < buttons.size(); i++) {
        delete buttons[i];
    }
}

void programm::move(int index) {
    float multiplier = .5f;
    if (Events::pressed(GLFW_KEY_X) && Events::pressed(GLFW_KEY_MINUS)) {
        p_vk->physicalObjectsInfo[index].velocity_mass -= glm::vec4(1, 0, 0, 0) * multiplier;
    }
    if (Events::pressed(GLFW_KEY_X) && Events::pressed(GLFW_KEY_EQUAL)) {
        p_vk->physicalObjectsInfo[index].velocity_mass += glm::vec4(1, 0, 0, 0) * multiplier;
    }
    if (Events::pressed(GLFW_KEY_C) && Events::pressed(GLFW_KEY_MINUS)) {
        p_vk->physicalObjectsInfo[index].velocity_mass -= glm::vec4(0, 1, 0, 0) * multiplier;
    }
    if (Events::pressed(GLFW_KEY_C) && Events::pressed(GLFW_KEY_EQUAL)) {
        p_vk->physicalObjectsInfo[index].velocity_mass += glm::vec4(0, 1, 0, 0) * multiplier;
    }
    if (Events::pressed(GLFW_KEY_Z) && Events::pressed(GLFW_KEY_MINUS)) {
        p_vk->physicalObjectsInfo[index].velocity_mass -= glm::vec4(0, 0, 1, 0) * multiplier;
    }
    if (Events::pressed(GLFW_KEY_Z) && Events::pressed(GLFW_KEY_EQUAL)) {
        p_vk->physicalObjectsInfo[index].velocity_mass += glm::vec4(0, 0, 1, 0) * multiplier;
    }
}

void programm::mainLoop() {
    float deltaTime = 0;
    float lastFrame = 0;
    std::vector<std::string> info;
    info.push_back("");
    info.push_back("");
    info.push_back("");
    bool nextFrame = true;
    bool isPaused = false;
    bool isFullscreen = false;
    bool wait = false;
    p_vk->transferPhysicalObjectsInfo();
    std::chrono::steady_clock::time_point high_res_clock_start_point;
    while (!glfwWindowShouldClose(p_vk->window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (clock() - p_vk->second_timer < 1000) {
            ++p_vk->frames;
        }
        else {
            p_vk->second_timer = clock();
            info[0] = std::to_string(p_vk->frames);
            textElements(0, 0, info, p_vk->mWindow, fps_counter);
            p_vk->applyChangedObject(fps_counter->objectIndex);
            p_vk->frames = 0;
        }
        //info[1] = std::to_string(p_vk->verts_in_render);
        //textElements(0, 0, &info, p_vk->mWindow, fps_counter);
        //p_vk->applyChangedObject(fps_counter->objectIndex);

        p_vk->updateCameraBuffer();

        p_vk->updateUniformBuffer();

        //animators[0]->UpdateAnimation(deltaTime);
        //p_vk->transforms = animators[0]->GetFinalBoneMatrices();
        //p_vk->transferAnimationInfo();

        if (nextFrame) {
            p_vk->transferPhysicalObjectsInfo();
            p_vk->runPhysics();
            p_vk->waitForPhysicsFinish();
            //std::chrono::steady_clock::time_point high_res_clock_start_point = std::chrono::high_resolution_clock::now();
            p_vk->retrievePhysicalObjectsInfo();
            p_vk->prepareParticlesClusterized();
            //p_vk->prepareParticlesIndividualy();
            p_PhysX->run();
            p_PhysX->fetchObjectInfo();
            //std::cout << "\033[38;2;0;255;0m" << std::chrono::high_resolution_clock::duration(std::chrono::high_resolution_clock::now() - high_res_clock_start_point) << "\033[0m\n";
        }

        p_vk->prepareRender();

        p_vk->updateCameraInfoBuffers();
        p_vk->updateShadowMapBuffers();
        p_vk->drawShadowMaps();

        //std::chrono::steady_clock::time_point high_res_clock_start_point = std::chrono::high_resolution_clock::now();
        //std::this_thread::sleep_for(std::chrono::nanoseconds(10'000'000));
        p_vk->waitForShadowMapRenderFinish();
        p_vk->drawFrame();
        //std::cout << "\033[38;2;0;255;0m" << std::chrono::high_resolution_clock::duration(std::chrono::high_resolution_clock::now() - high_res_clock_start_point) << "\033[0m\n";

        Events::pollEvents();
        //if (!Events::_cursor_locked) {
        //    for (class button* current_button : buttons) {
        //        if (!current_button->button_object->info.is_interacted && Events::x > current_button->pixel_pos_bottom_left[0] && Events::x < current_button->pixel_pos_top_right[0] && Events::y > current_button->pixel_pos_bottom_left[1] && Events::y < current_button->pixel_pos_top_right[1]) {
        //            current_button->button_object->info.is_interacted = true;
        //        }
        //        else if (current_button->button_object->info.is_interacted && !(Events::x > current_button->pixel_pos_bottom_left[0] && Events::x < current_button->pixel_pos_top_right[0] && Events::y > current_button->pixel_pos_bottom_left[1] && Events::y < current_button->pixel_pos_top_right[1])) {
        //            current_button->button_object->info.is_interacted = false;
        //        }
        //        if (current_button->button_object->info.is_interacted) {
        //            if (current_button->name == "right") {
        //                move(0);
        //            }
        //            else if (current_button->name == "left") {
        //                move(2);
        //            }
        //            else if (current_button->name == "middle") {
        //                move(1);
        //                if (Events::pressed(GLFW_KEY_R))
        //                sphericalExplosion(&testExplosion, my_vk);
        //            }
        //        }
        //        my_vk->applyChangedObject(current_button->button_object->objectIndex);
        //    }
        //}
        //std::cout << '\n';
        //high_res_clock_start_point = std::chrono::high_resolution_clock::now();
        Ñamera* p_camera = p_vk->p_camera;
        glm::vec3 delta_pos(0);
        float cameraSpeed = p_vk->cameraSpeed * deltaTime;
        if (Events::jclicked(GLFW_MOUSE_BUTTON_1)) {
            physicalObjectInfo& clicked = p_vk->getClickInfo(Events::x, Events::y);

            info[1] = std::to_string(clicked.lifeTime);
            info[1].resize(6);
            textElements(0, 0, info, p_vk->mWindow, fps_counter);
            p_vk->applyChangedObject(fps_counter->objectIndex);
        }
        if (Events::pressed(GLFW_KEY_D)) {
            delta_pos += p_camera->relative_right * cameraSpeed;
        }
        if (Events::pressed(GLFW_KEY_A)) {
            delta_pos -= p_camera->relative_right * cameraSpeed;
        }
        if (Events::pressed(GLFW_KEY_W)) {
            delta_pos.x += p_camera->relative_right.z * cameraSpeed;
            delta_pos.z -= p_camera->relative_right.x * cameraSpeed;
        }
        if (Events::pressed(GLFW_KEY_S)) {
            delta_pos.x -= p_camera->relative_right.z * cameraSpeed;
            delta_pos.z += p_camera->relative_right.x * cameraSpeed;
        }
        //std::cout << (std::chrono::high_resolution_clock::duration(std::chrono::high_resolution_clock::now() - high_res_clock_start_point)) << '\n';
        if (Events::pressed(GLFW_KEY_SPACE)) {
            delta_pos.y += cameraSpeed;
            //reinterpret_cast<physx::PxRigidDynamic*>(gameObjects[4]->actors[0])->addForce(physx::PxVec3(0.0F, 10.0F, 0.0F));
        }
        if (Events::pressed(GLFW_KEY_LEFT_SHIFT)) {
            delta_pos.y -= cameraSpeed;
        }
        p_camera->position += delta_pos;
        if (Events::jpressed(GLFW_KEY_TAB)) {
            Events::toggleCursor();
        }
        //if (Events::pressed(GLFW_KEY_UP)) {
        //    physx::PxVec3 current_transform = (*p_PhysX->rigids_dynamic)[0]->getLinearVelocity();
        //    (*p_PhysX->rigids_dynamic)[0]->setLinearVelocity(current_transform + physx::PxVec3(0, 0.2f, 0));
        //}
        if (Events::jpressed(GLFW_KEY_F)) {
            if (isFullscreen) {
                isFullscreen = false;
                p_vk->mWindow->setWindowed(1024, 1024);
            }
            else {
                isFullscreen = true;
                p_vk->mWindow->setFullscreen();
            }
        }
        //if (Events::_cursor_locked) {
        //    my_vk->mWindow->setCursorPosition(glm::vec2(0,0));//??? how the fuck this work(cause some bugs when call delta func)
        //}
        if (isPaused) {
            nextFrame = false;
        }
        if (Events::jpressed(GLFW_KEY_RIGHT)) {
            nextFrame = true;
        }
        if (Events::jpressed(GLFW_KEY_P)) {
            isPaused = !isPaused;
            nextFrame = false;
        }
        if (Events::jpressed(GLFW_KEY_F2)) {
            p_vk->screenshot_should_be_saved = true;
        }
        if (Events::jpressed(GLFW_KEY_Q)) {
            Bomb* bomb = new Bomb(gameObjects[1], 2);
            p_storage->objects.push_back(bomb);
            gameObjects[1]->addDynamicInstance(bomb, vec3_to_PxVec3(p_camera->position), physx::PxQuat(0, 0, 0, 1), vec3_to_PxVec3(p_camera->direction) * 10, physx::PxVec3(0), 2.0f);
        }
        if (Events::jpressed(GLFW_KEY_E)) {
            Barrel* barrel = new Barrel(gameObjects[2], 2);
            p_storage->objects.push_back(barrel);
            gameObjects[2]->addDynamicInstance(barrel, physx::PxVec3(0), physx::PxQuat(-0.7071067811865475, 0, 0, 0.7071067811865475));
        }
        if (Events::pressed(GLFW_KEY_R)) {

        }
        if (Events::_cursor_locked) {
            float& horizontalAngle = p_camera->horizontalAngle;
            if (horizontalAngle >= glm::radians(360.f)) {
                horizontalAngle -= glm::radians(360.f);
            }
            else if (horizontalAngle <= glm::radians(-360.f)) {
                horizontalAngle += glm::radians(360.f);
            }
            horizontalAngle -= static_cast<float>(Events::deltaX) * p_vk->mouseSensitivity;

            if (custom_maths::abs(p_camera->verticalAngle - static_cast<float>(Events::deltaY) * p_vk->mouseSensitivity) <= glm::radians(90.0f)) {
                p_camera->verticalAngle -= static_cast<float>(Events::deltaY) * p_vk->mouseSensitivity;
            }
            p_camera->updateVectors();

        }
    }

    vkDeviceWaitIdle(p_vk->device);
}