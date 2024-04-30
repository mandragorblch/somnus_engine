#pragma once
#ifndef VK_MISC
#define VK_MISC

#include <array>
#include <set>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <glm/gtx/hash.hpp>
#include <boost/pool/pool_alloc.hpp>

#define MAX_HITBOXES 16
#define MAX_INTERSECTIONS 128
#define MAX_BONESMATRICES 256
#define MAX_COMMANDS 10'0000u//limit for render batch
#define CLUSTER_SIZE (MAX_COMMANDS / 100)

const std::vector<glm::vec3> normal_vertices = { //(from observer)
{1, -1, -1},//back
{1, 1, -1},
{-1, 1, -1},
{-1, -1, -1},

{-1, -1, 1},//face
{-1, 1, 1},
{1, 1, 1},
{1, -1, 1},

{-1, -1, -1},//left
{-1, 1, -1},
{-1, 1, 1},
{-1, -1, 1},

{1, -1, 1},//right
{1, 1, 1},
{1, 1, -1},
{1, -1, -1},

{-1, -1, -1},//down
{-1, -1, 1},
{1, -1, 1},
{1, -1, -1},

{-1, 1, 1},//top
{-1, 1, -1},
{1, 1, -1},
{1, 1, 1}

};
const std::vector<glm::vec2> standart_uv = {
    {0, 1},
    {0, 0},
    {1, 0},
    {1, 1}
};
const std::vector<unsigned int> standart_indices = {
    3, 2, 1,//back
    3, 1, 0
};

struct Vertex {
    alignas(16) glm::vec3 pos;
    alignas(16) glm::vec3 normal;
    alignas(16) glm::vec3 tangent;
    //glm::vec3 bitangent;
    alignas(8) glm::vec2 texCoord;
    alignas(16) glm::ivec4 boneIds;
    alignas(16) glm::vec4 weights;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 6> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 6> attributeDescriptions{};
        unsigned int index = 0;
        attributeDescriptions[index].binding = 0;
        attributeDescriptions[index].location = index;
        attributeDescriptions[index].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[index].offset = offsetof(Vertex, pos);

        index++;
        attributeDescriptions[index].binding = 0;
        attributeDescriptions[index].location = index;
        attributeDescriptions[index].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[index].offset = offsetof(Vertex, normal);

        index++;
        attributeDescriptions[index].binding = 0;
        attributeDescriptions[index].location = index;
        attributeDescriptions[index].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[index].offset = offsetof(Vertex, tangent);

        //index++;
        //attributeDescriptions[index].binding = 0;
        //attributeDescriptions[index].location = index;
        //attributeDescriptions[index].format = VK_FORMAT_R32G32B32_SFLOAT;
        //attributeDescriptions[index].offset = offsetof(Vertex, bitangent);

        index++;
        attributeDescriptions[index].binding = 0;
        attributeDescriptions[index].location = index;
        attributeDescriptions[index].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[index].offset = offsetof(Vertex, texCoord);

        index++;
        attributeDescriptions[index].binding = 0;
        attributeDescriptions[index].location = index;
        attributeDescriptions[index].format = VK_FORMAT_R32G32B32A32_SINT;
        attributeDescriptions[index].offset = offsetof(Vertex, boneIds);

        index++;
        attributeDescriptions[index].binding = 0;
        attributeDescriptions[index].location = index;
        attributeDescriptions[index].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[index].offset = offsetof(Vertex, weights);

        return attributeDescriptions;
    }

    bool operator==(const Vertex& other) const {
        return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
    }

    bool operator!=(const Vertex& other) const {
        return pos != other.pos && normal != other.normal && texCoord != other.texCoord;
    }
};

struct ui_Vertex {
    alignas(8) glm::vec2 pos;
    alignas(8) glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(ui_Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(ui_Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(ui_Vertex, texCoord);

        return attributeDescriptions;
    }

    bool operator==(const ui_Vertex& other) const {
        return pos == other.pos && texCoord == other.texCoord;
    }
};
//non gpu
struct indirectBatch {
    class model* p_model;
    uint32_t firstMeshIndex;
    uint32_t instanceIndex;
    uint32_t* instance_count;
    int32_t mesh_count;
    std::list<uint32_t, boost::fast_pool_allocator<uint32_t>>* PO_global_indexes;
};

struct indirectBatchCluster {
    class model* p_model;
    uint32_t firstMeshIndex;
    uint32_t instanceIndex;
    uint32_t* instance_count;
    int32_t mesh_count;
    typedef std::list<uint32_t, boost::fast_pool_allocator<uint32_t>>* p_uint32_list;
    std::list<p_uint32_list, boost::fast_pool_allocator<p_uint32_list>>* clustered_PO_global_indexes;
};

struct ProjViewMatrix {
    alignas(16) glm::mat4 projView;
};

struct UniformBufferObject {
    ProjViewMatrix* projView;
    void* data;
    UniformBufferObject(ProjViewMatrix* projView) : projView(projView), data(reinterpret_cast<void*>(projView)) {}
};

struct lightGlobalInfo { 
    alignas(unsigned int) unsigned int lightSourcesCount; 
};

struct index_wrapper {
    alignas(4) uint32_t index;
};

struct twoDObjectInfo {
    alignas(8) glm::vec2 pos;
    alignas(16) glm::vec3 rotation;
    alignas(bool) bool is_interacted;
    alignas(unsigned int) unsigned int texture_index;
};

//struct computeCubeHitbox {
//    alignas(16) glm::vec4 rot;
//    alignas(16) glm::vec4 pos;
//    alignas(16) glm::vec4 side_length_index_of_object;
//    alignas(16) glm::vec4 intersection_posTime;
//    alignas(16) glm::vec4 velocity_goinInside;
//    alignas(float) float verticeIndeces[8];
//};
//struct computeRectangularParallelepipedHitbox {
//    alignas(16) glm::vec3 top;
//    alignas(16) glm::vec3 bottom;
//
//    alignas(16) glm::vec3 rot;
//    alignas(16) glm::vec3 pos;
//    alignas(unsigned int) unsigned int index;
//};
//struct computeSphereHitbox {
//    alignas(unsigned int) unsigned int radius;
//
//    alignas(16) glm::vec3 rot;
//    alignas(16) glm::vec3 pos;
//    alignas(unsigned int) unsigned int index;
//};
//struct computeCylinderHitbox {
//    alignas(unsigned int) unsigned int height;
//    alignas(unsigned int) unsigned int radius;
//
//    alignas(16) glm::vec3 rot;
//    alignas(16) glm::vec3 pos;
//    alignas(unsigned int) unsigned int index;
//};

enum class PO_TYPE : uint32_t{
    TRUE_PO = 1,
    PARTICLE = 2
};

struct physicalObjectInfo {
    alignas(16) glm::vec4 pos_index;
    alignas(16) glm::vec4 quat_rot;
    alignas(16) glm::vec4 velocity_mass;
    alignas(16) glm::vec4 angularVelocity_momentOfInertia;
    alignas(16) glm::vec4 force;
    alignas(4)  PO_TYPE type;
    alignas(4)  float lifeTime;
    alignas(4)  bool should_be_deleted;
};

struct clusterInfo {
    uint32_t index;
    uint32_t size;
    std::list<uint32_t, boost::fast_pool_allocator<uint32_t>> PO;
    typedef std::list<uint32_t, boost::fast_pool_allocator<uint32_t>>* p_uint32_list;
    std::list<p_uint32_list, boost::fast_pool_allocator<p_uint32_list>>::iterator PO_iterator;
};

struct physicInfo {
    alignas(16) glm::vec4 deltaTime;
};

struct cameraInfo {
    alignas(16) glm::vec3 cameraPos;
    alignas(16) glm::vec3 viewDir;
    alignas(16) glm::vec3 viewPos;

    alignas(16) glm::vec4 flashlight_diffused;
};//and this for final render

struct lightSourceInfo {//this is for shadow casting(actually this is required for final rendering too)
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::vec3 lightPos;
    alignas(16) glm::vec3 lightDir;
    alignas(4) float intensity;
};

struct BoneInfo
{
    BoneInfo() : id((std::numeric_limits<int>::min)()) {

    }
    /*id is index in finalBoneMatrices*/
    alignas(4) int id;

    /*offset matrix transforms vertex from model space to bone space*/
    alignas(16) glm::mat4 offset;
    
    BoneInfo(int id, const glm::mat4& offset) : id(id), offset(offset) {

    }
};

struct finalBonesMatrices {
    alignas (16) glm::mat4 matrices[MAX_BONESMATRICES];
};

struct hitboxVertice {
    alignas(16) glm::vec4 pos_index;
    alignas(16) glm::vec4 velocity_intersectionTime;
    alignas(4) bool goingInside;
};

namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}
////WTF IS THAT FCKNG SHIT
//namespace std {
//    template<> struct hash<ui_Vertex> {
//        size_t operator()(ui_Vertex const& vertex) const {
//            return (hash<glm::vec2>()(vertex.pos) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
//        }
//    };
//}

#endif