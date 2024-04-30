#pragma once
#include "utility/termcolor.hpp"
#include <string>
#include <iostream>
#include <typeinfo>
#include <glm/glm.hpp>

template <typename type>
void print_r(type message);

template <typename type>
void print_g(type message);

template <typename type>
void print_b(type message);

template<typename type> inline
void print_r(type message)
{
	if (typeid(message) == typeid(std::string)) {
		std::string* temp_txt = reinterpret_cast<std::string*>(&message);
		std::cout << termcolor::red << *temp_txt << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(size_t)) {
		size_t* temp_txt = reinterpret_cast<size_t*>(&message);
		std::cout << termcolor::red << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(int)) {
		int* temp_txt = reinterpret_cast<int*>(&message);
		std::cout << termcolor::red << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(long)) {
		long* temp_txt = reinterpret_cast<long*>(&message);
		std::cout << termcolor::red << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(long long)) {
		long long* temp_txt = reinterpret_cast<long long*>(&message);
		std::cout << termcolor::red << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(unsigned)) {
		unsigned* temp_txt = reinterpret_cast<unsigned*>(&message);
		std::cout << termcolor::red << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(unsigned long)) {
		unsigned long* temp_txt = reinterpret_cast<unsigned long*>(&message);
		std::cout << termcolor::red << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(unsigned long long)) {
		unsigned long long* temp_txt = reinterpret_cast<unsigned long long*>(&message);
		std::cout << termcolor::red << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(float)) {
		float* temp_txt = reinterpret_cast<float*>(&message);
		std::cout << termcolor::red << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(double)) {
		double* temp_txt = reinterpret_cast<double*>(&message);
		std::cout << termcolor::red << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(long double)) {
		long double* temp_txt = reinterpret_cast<long double*>(&message);
		std::cout << termcolor::red << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(glm::vec3)) {
		glm::vec3* temp_txt = reinterpret_cast<glm::vec3*>(&message);
		std::cout << termcolor::red << std::to_string(temp_txt->x) << " " << std::to_string(temp_txt->y) << " " << std::to_string(temp_txt->z) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(glm::vec4)) {
		glm::vec4* temp_txt = reinterpret_cast<glm::vec4*>(&message);
		std::cout << termcolor::red << std::to_string(temp_txt->x) << " " << std::to_string(temp_txt->y) << " " << std::to_string(temp_txt->z) << " " << std::to_string(temp_txt->w) << termcolor::white << "\n";
	}
}

template<typename type> inline
void print_g(type message)
{
	if (typeid(message) == typeid(std::string)) {
		std::string* temp_txt = reinterpret_cast<std::string*>(&message);
		std::cout << termcolor::green << *temp_txt << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(size_t)) {
		size_t* temp_txt = reinterpret_cast<size_t*>(&message);
		std::cout << termcolor::green << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(int)) {
		int* temp_txt = reinterpret_cast<int*>(&message);
		std::cout << termcolor::green << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(long)) {
		long* temp_txt = reinterpret_cast<long*>(&message);
		std::cout << termcolor::green << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(long long)) {
		long long* temp_txt = reinterpret_cast<long long*>(&message);
		std::cout << termcolor::green << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(unsigned)) {
		unsigned* temp_txt = reinterpret_cast<unsigned*>(&message);
		std::cout << termcolor::green << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(unsigned long)) {
		unsigned long* temp_txt = reinterpret_cast<unsigned long*>(&message);
		std::cout << termcolor::green << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(unsigned long long)) {
		unsigned long long* temp_txt = reinterpret_cast<unsigned long long*>(&message);
		std::cout << termcolor::green << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(float)) {
		float* temp_txt = reinterpret_cast<float*>(&message);
		std::cout << termcolor::green << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(double)) {
		double* temp_txt = reinterpret_cast<double*>(&message);
		std::cout << termcolor::green << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(long double)) {
		long double* temp_txt = reinterpret_cast<long double*>(&message);
		std::cout << termcolor::green << std::to_string(*temp_txt) << termcolor::white << "\n";
	} else if (typeid(message) == typeid(glm::vec3)) {
		glm::vec3* temp_txt = reinterpret_cast<glm::vec3*>(&message);
		std::cout << termcolor::green << std::to_string(temp_txt->x) << " " << std::to_string(temp_txt->y) << " " << std::to_string(temp_txt->z) << termcolor::white << "\n";
	} else if (typeid(message) == typeid(glm::vec4)) {
		glm::vec4* temp_txt = reinterpret_cast<glm::vec4*>(&message);
		std::cout << termcolor::green << std::to_string(temp_txt->x) << " " << std::to_string(temp_txt->y) << " " << std::to_string(temp_txt->z) << " " << std::to_string(temp_txt->w) << termcolor::white << "\n";
	}
}

template<typename type> inline
void print_b(type message)
{
	if (typeid(message) == typeid(std::string)) {
		std::string* temp_txt = reinterpret_cast<std::string*>(&message);
		std::cout << termcolor::blue << *temp_txt << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(int)) {
		int* temp_txt = reinterpret_cast<int*>(&message);
		std::cout << termcolor::blue << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(size_t)) {
		size_t* temp_txt = reinterpret_cast<size_t*>(&message);
		std::cout << termcolor::blue << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(long)) {
		long* temp_txt = reinterpret_cast<long*>(&message);
		std::cout << termcolor::blue << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(long long)) {
		long long* temp_txt = reinterpret_cast<long long*>(&message);
		std::cout << termcolor::blue << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(unsigned int)) {
		unsigned int* temp_txt = reinterpret_cast<unsigned int*>(&message);
		std::cout << termcolor::blue << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(unsigned long)) {
		unsigned long* temp_txt = reinterpret_cast<unsigned long*>(&message);
		std::cout << termcolor::blue << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(unsigned long long)) {
		unsigned long long* temp_txt = reinterpret_cast<unsigned long long*>(&message);
		std::cout << termcolor::blue << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(float)) {
		float* temp_txt = reinterpret_cast<float*>(&message);
		std::cout << termcolor::blue << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(double)) {
		double* temp_txt = reinterpret_cast<double*>(&message);
		std::cout << termcolor::blue << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(long double)) {
		long double* temp_txt = reinterpret_cast<long double*>(&message);
		std::cout << termcolor::blue << std::to_string(*temp_txt) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(glm::vec3)) {
		glm::vec3* temp_txt = reinterpret_cast<glm::vec3*>(&message);
		std::cout << termcolor::blue << std::to_string(temp_txt->x) << " " << std::to_string(temp_txt->y) << " " << std::to_string(temp_txt->z) << termcolor::white << "\n";
	}
	else if (typeid(message) == typeid(glm::vec4)) {
		glm::vec4* temp_txt = reinterpret_cast<glm::vec4*>(&message);
		std::cout << termcolor::blue << std::to_string(temp_txt->x) << " " << std::to_string(temp_txt->y) << " " << std::to_string(temp_txt->z) << " " << std::to_string(temp_txt->w) << termcolor::white << "\n";
	}
}