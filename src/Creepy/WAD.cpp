#include <print>
#include <fstream>
#include <Creepy/WAD.hpp>

std::optional<WAD> WAD::loadFromFile(const std::filesystem::path& filePath) {
    std::ifstream fileIn{filePath, std::ios::binary | std::ios::ate};

    if(!fileIn.good()){
        std::println("File Not Found");
        return std::nullopt;
    }
    
    auto fileSize = fileIn.tellg();
    fileIn.seekg(0u);

    if(fileSize < 12){
        std::println("File Failed");
        return std::nullopt;
    }

    WAD wadFile{};

    fileIn.read(wadFile.id, 4);
    wadFile.id[4] = '\0';
    fileIn.seekg(4u);
    fileIn.read(std::bit_cast<char*>(&wadFile.numLumps), 4);
    fileIn.seekg(8u);

    uint32_t directoryOffset{};
    fileIn.read(std::bit_cast<char*>(&directoryOffset), 4);
    
    wadFile.lumps.resize(wadFile.numLumps);

    for(uint32_t i{}; i < wadFile.numLumps; ++i){
        const uint32_t offset = directoryOffset + (i * 16);
        fileIn.seekg(offset);
        uint32_t lumpsOffset{};
        fileIn.read(std::bit_cast<char*>(&lumpsOffset), 4);

        fileIn.seekg(offset + 4);
        fileIn.read(std::bit_cast<char*>(&wadFile.lumps.at(i).size), 4);

        fileIn.seekg(offset + 8);
        fileIn.read(wadFile.lumps.at(i).name, 8);
        wadFile.lumps.at(i).name[8] = '\0';

        wadFile.lumps.at(i).data.resize(wadFile.lumps.at(i).size);
        fileIn.seekg(lumpsOffset);
        fileIn.read(std::bit_cast<char*>(wadFile.lumps.at(i).data.data()), wadFile.lumps.at(i).size);
    }

    return wadFile;
}

int WAD::findLump(std::string_view lumpName, const WAD& wadFile) {
    for(int i{}; const auto& lump : wadFile.lumps){
        if(lumpName == lump.name){
            return i;
        }
        ++i;
    }

    return -1;
}

constexpr int ThingsIndex{1};
constexpr int SideDefsIndex{2};
constexpr int LineDefsIndex{3};
constexpr int VertexesIndex{4};
constexpr int SegsIndex{5};
constexpr int SSectors{6};
constexpr int Nodes{7};
constexpr int Sectors{8};


static void readVertices(Map& map, const Lump& lump);

std::optional<Map> WAD::readMap(std::string_view mapName, const WAD& wadFile) {
    Map map{};
    const int mapIndex = findLump(mapName, wadFile);

    if(mapIndex < 0){
        return std::nullopt;
    }

    std::println("Found Map: {}", mapIndex);

    readVertices(map, wadFile.lumps.at(mapIndex + VertexesIndex));

    for(auto v : map.vertices){
        std::println("{} - {}", v.x, v.y);
    }

    return map;
}

void readVertices(Map& map, const Lump& lump){
    map.vertices.resize(lump.size / 4);

    map.min.x = std::numeric_limits<float>::infinity();
    map.min.y = std::numeric_limits<float>::infinity();

    map.max.x = -std::numeric_limits<float>::infinity();
    map.max.y = -std::numeric_limits<float>::infinity();
    
    for(size_t i{}, j{}; i < lump.size; i += 4, ++j){
        const int16_t valX = static_cast<int16_t>(lump.data.at(i)) | static_cast<int16_t>(lump.data.at(i + 1)) << 8;
        const int16_t valY = static_cast<int16_t>(lump.data.at(i + 2)) | static_cast<int16_t>(lump.data.at(i + 3)) << 8;
        map.vertices.at(j).x = static_cast<float>(valX);
        map.vertices.at(j).y = static_cast<float>(valY);

        if(map.vertices.at(j).x < map.min.x){
            map.min.x = map.vertices.at(j).x;
        }
        if(map.vertices.at(j).y < map.min.y){
            map.min.y = map.vertices.at(j).y;
        }
        if(map.vertices.at(j).x > map.max.x){
            map.max.x = map.vertices.at(j).x;
        }
        if(map.vertices.at(j).y > map.max.y){
            map.max.y = map.vertices.at(j).y;
        }
    }
}