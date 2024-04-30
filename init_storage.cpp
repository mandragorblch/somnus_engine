#pragma once
#include "init_storage.h"

void init_storage::start_programm()
{
	p_vk->p_vk = p_vk;
	p_vk->p_init_storage = my_storage;
	p_vk->initWindow();

	max_threads_available = std::thread::hardware_concurrency();
	//p_threads = new std::vector<std::thread>(max_threads_available);

	my_programm = new class programm(my_storage);
}

init_storage::init_storage()
{
	p_vk = new class vk();
}

init_storage::~init_storage()
{
	delete p_vk;
}