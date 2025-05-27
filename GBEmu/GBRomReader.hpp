#ifndef __GBROAMREADER_HPP__
#define __GBROAMREADER_HPP__


#include <fstream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <string>

class GBRomReader {
public:
    struct Header {
        char title[16] = { 0 };
        uint8_t cartridgeType = 0;
        uint8_t romSize = 0;
        uint8_t ramSize = 0;
        bool checksumValid = false;
    };

    void load(const std::string& filePath);

    const Header& getHeader() const;

    std::vector<uint8_t> getOpcodes() const;

private:
    std::vector<uint8_t> romBuffer;
    Header header;

    void parseHeader();
};

#endif 
