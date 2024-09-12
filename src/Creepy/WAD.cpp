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
constexpr T readBytes(std::span<const std::byte> data, size_t index){
    if constexpr(sizeof(T) == 2u){
        return static_cast<T>(data.at(index)) | static_cast<T>(data.at(index + 1)) << 8;
    }
    else if constexpr(sizeof(T) == 4u){
        return static_cast<T>(data.at(index)) | static_cast<T>(data.at(index + 1)) << 8 | 
        static_cast<T>(data.at(index + 2)) << 16 | static_cast<T>(data.at(index + 3)) << 24;
    }
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

        // std::println("{} - {}", map.lineDefs.at(j).frontSideDef, map.lineDefs.at(j).backSideDef);
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
        map.sectors.at(j).lightLevel = readBytes<int16_t>(lump.data, i + 20);
    }
}

constexpr int GLVerticesIndex{1};
constexpr int GLSegsIndex{2};
constexpr int GLSSectorsIndex{3};
constexpr int GLNodesIndex{4};


static void readGLVertices(GLMap& glMap, const Lump& lump);
static void readGLSegments(GLMap& glMap, const Lump& lump);
static void readGLSubSectors(GLMap& glMap, const Lump& lump);

std::optional<GLMap> WAD::readGLMap(std::string_view glMapName, const WAD& wadFile) {

    const int glMapIndex = findLump(glMapName, wadFile);

    if(glMapIndex < 0){
        return std::nullopt;
    }

    GLMap glMap{};
    std::println("Found GLMap: {}", glMapIndex);

    if(std::string{reinterpret_cast<const char*>(wadFile.lumps.at(glMapIndex + GLVerticesIndex).data.data()), 0, 4} != "gNd2"){
        std::println("Not Found gNd2: {}", std::string{reinterpret_cast<const char*>(wadFile.lumps.at(glMapIndex + GLVerticesIndex).data.data()), 0, 4});
        // return std::nullopt;
    }

    if(std::string{reinterpret_cast<const char*>(wadFile.lumps.at(glMapIndex + GLSegsIndex).data.data()), 0, 4} != "gNd3"){
        // std::println("Not Found gNd3");
        std::println("Not Found gNd3: {}", std::string{reinterpret_cast<const char*>(wadFile.lumps.at(glMapIndex + GLSegsIndex).data.data()), 0, 4});
        // return std::nullopt;
    }

    readGLVertices(glMap, wadFile.lumps.at(glMapIndex + GLVerticesIndex));
    readGLSegments(glMap, wadFile.lumps.at(glMapIndex + GLSegsIndex));
    readGLSubSectors(glMap, wadFile.lumps.at(glMapIndex + GLSSectorsIndex));

    return glMap;
}

void readGLVertices(GLMap& glMap, const Lump& lump) {
    glMap.vertices.resize((lump.size - 4) / 8);     // X Y: 8 bytes

    glMap.min.x = std::numeric_limits<float>::infinity();
    glMap.min.y = std::numeric_limits<float>::infinity();

    glMap.max.x = -std::numeric_limits<float>::infinity();
    glMap.max.y = -std::numeric_limits<float>::infinity();
    
    for(size_t i{4}, j{}; i < lump.size; i += 8, ++j){
        const float valX = static_cast<float>(readBytes<int32_t>(lump.data, i)) / (1 << 16);
        const float valY = static_cast<float>(readBytes<int32_t>(lump.data, i + 4)) / (1 << 16);

        glMap.vertices.at(j).x = valX;
        glMap.vertices.at(j).y = valY;

        if(glMap.vertices.at(j).x < glMap.min.x){
            glMap.min.x = glMap.vertices.at(j).x;
        }

        if(glMap.vertices.at(j).y < glMap.min.y){
            glMap.min.y = glMap.vertices.at(j).y;
        }

        if(glMap.vertices.at(j).x > glMap.max.x){
            glMap.max.x = glMap.vertices.at(j).x;
        }

        if(glMap.vertices.at(j).y > glMap.max.y){
            glMap.max.y = glMap.vertices.at(j).y;
        }
    }
}

void readGLSegments(GLMap& glMap, const Lump& lump) {
    glMap.segments.resize(lump.size / 10);     // Segment: 10 bytes

    for(uint32_t i{}, j{}; i < lump.size; i += 10, ++j){
        glMap.segments.at(j).startVertex = readBytes<uint16_t>(lump.data, i);
        glMap.segments.at(j).endVertex = readBytes<uint16_t>(lump.data, i + 2);
        glMap.segments.at(j).lineDef = readBytes<uint16_t>(lump.data, i + 4);
        glMap.segments.at(j).side = readBytes<uint16_t>(lump.data, i + 6);
    }
}

void readGLSubSectors(GLMap& glMap, const Lump& lump) {
    glMap.subSectors.resize(lump.size / 4);     // SubSector: 4 bytes

    for(uint32_t i{}, j{}; i < lump.size; i += 4, ++j){
        glMap.subSectors.at(j).numSegments = readBytes<uint16_t>(lump.data, i);
        glMap.subSectors.at(j).firstSegment = readBytes<uint16_t>(lump.data, i + 2);

        // std::println("{} - {}", glMap.subSectors.at(j).numSegments, glMap.subSectors.at(j).firstSegment);
    }
}