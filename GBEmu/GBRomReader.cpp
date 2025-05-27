#include "GBRomReader.hpp"


void GBRomReader::load(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error("File open failed");

    romBuffer.resize(file.tellg());
    file.seekg(0);
    file.read(reinterpret_cast<char*>(romBuffer.data()), romBuffer.size());

    parseHeader();
}

const GBRomReader::Header& GBRomReader::getHeader() const
{ 
    return header; 
}

std::vector<uint8_t> GBRomReader::getOpcodes() const
{
    if (romBuffer.size() <= 0x150) return {};
    return { romBuffer.begin() + 0x150, romBuffer.end() };
}

void GBRomReader::parseHeader() {
    if (romBuffer.size() < 0x150)
        throw std::runtime_error("Invalid ROM file");

    const size_t headerStart = 0x100;

    // Extraction du titre
    std::memcpy(header.title, &romBuffer[headerStart + 0x34], 16);
    header.title[15] = '\0';

    // Métadonnées
    header.cartridgeType = romBuffer[headerStart + 0x47];
    header.romSize = romBuffer[headerStart + 0x48];
    header.ramSize = romBuffer[headerStart + 0x49];

    // Vérification checksum
    uint16_t checksum = 0;
    for (size_t i = headerStart + 0x34; i <= headerStart + 0x4C; ++i) {
        checksum += romBuffer[i];
    }
    header.checksumValid = (checksum & 0xFF) == romBuffer[headerStart + 0x4D];
}