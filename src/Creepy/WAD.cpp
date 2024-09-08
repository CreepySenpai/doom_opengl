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
constexpr int LineDefsIndex{2};
constexpr int SideDefsIndex{3};
constexpr int VertexesIndex{4};
constexpr int SegsIndex{5};
constexpr int SSectorsIndex{6};
constexpr int NodesIndex{7};
constexpr int SectorsIndex{8};


static void readVertices(Map& map, const Lump& lump);
static void readLineDefs(Map& map, const Lump& lump);
static void readSideDefs(Map& map, const Lump& lump);
static void readSectors(Map& map, const Lump& lump);

std::optional<Map> WAD::readMap(std::string_view mapName, const WAD& wadFile) {
    Map map{};
    const int mapIndex = findLump(mapName, wadFile);

    if(mapIndex < 0){
        return std::nullopt;
    }

    std::println("Found Map: {}", mapIndex);

    readVertices(map, wadFile.lumps.at(mapIndex + VertexesIndex));
    
    readLineDefs(map, wadFile.lumps.at(mapIndex + LineDefsIndex));

    readSideDefs(map, wadFile.lumps.at(mapIndex + SideDefsIndex));

    readSectors(map, wadFile.lumps.at(mapIndex + SectorsIndex));

    return map;
}

template <typename T>
T readBytes(std::span<const std::byte> data, size_t index){
    return static_cast<T>(data.at(index)) | static_cast<T>(data.at(index + 1)) << 8;
}

void readVertices(Map& map, const Lump& lump){
    map.vertices.resize(lump.size / 4);     // X Y: 4 bytes

    map.min.x = std::numeric_limits<float>::infinity();
    map.min.y = std::numeric_limits<float>::infinity();

    map.max.x = -std::numeric_limits<float>::infinity();
    map.max.y = -std::numeric_limits<float>::infinity();
    
    for(size_t i{}, j{}; i < lump.size; i += 4, ++j){
        const int16_t valX = readBytes<int16_t>(lump.data, i);
        const int16_t valY = readBytes<int16_t>(lump.data, i + 2);
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

void readLineDefs(Map& map, const Lump& lump) {
    map.lineDefs.resize(lump.size / 14);    // Each LineDef: 14 bytes

    std::println("Lump Size: {}", lump.size);
    std::println("Lump Size Div: {}", lump.size / 14);
    for(size_t i{}, j{}; i < lump.size; i += 14, ++j){
        map.lineDefs.at(j).startIndex = readBytes<uint16_t>(lump.data, i);
        map.lineDefs.at(j).endIndex = readBytes<uint16_t>(lump.data, i + 2);
        map.lineDefs.at(j).flags = readBytes<uint16_t>(lump.data, i + 4);
        map.lineDefs.at(j).frontSideDef = readBytes<uint16_t>(lump.data, i + 10);
        map.lineDefs.at(j).backSideDef = readBytes<uint16_t>(lump.data, i + 12);
    }
}

void readSideDefs(Map& map, const Lump& lump) {
    map.sideDefs.resize(lump.size / 30);    // Each SideDef: 30 bytes

    std::println("Side Def: {}", map.sideDefs.size());

    for(size_t i{}, j{}; i < lump.size; i += 30, ++j){
        map.sideDefs.at(j).sectorIndex = readBytes<uint16_t>(lump.data, i + 28);
    }
}

void readSectors(Map& map, const Lump& lump) {
    map.sectors.resize(lump.size / 26);     // Each Sector: 26 bytes

    for(size_t i{}, j{}; i < lump.size; i += 26, ++j){
        map.sectors.at(j).floor = readBytes<int16_t>(lump.data, i);
        map.sectors.at(j).ceiling = readBytes<int16_t>(lump.data, i + 2);
    }
}