#pragma once
#include "programm.h"
#include "somnus_engine/vulkan/vk.h"
#include "somnus_engine/GLFW/Events.h"
#include "somnus_engine/render_objects/button.h"
#include <thread>
#include <vector>
#include <boost/pool/pool_alloc.hpp>

class init_storage
{
public:
    class init_storage* my_storage;
    class vk* p_vk;
    class programm* my_programm;

    std::list<class Object*, boost::fast_pool_allocator<Object*>> objects;
    std::vector<class gameObject*> gameObjects{};
    std::vector<class particle*> particles{};

    unsigned int max_threads_available = 0;
    std::vector<std::thread> p_threads;

    init_storage();
    ~init_storage();

    void start_programm();
};

