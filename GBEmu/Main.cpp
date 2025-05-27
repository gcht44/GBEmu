#include <iostream>
#include "GBRomReader.hpp"


int main() {
    GBRomReader reader;

    try {
        reader.load("Pokemon_Rouge.gb");
        const auto& header = reader.getHeader();
        auto opcodes = reader.getOpcodes();

        std::cout << "Titre: " << header.title << "\n"
            << "Type cartouche: 0x" << std::hex << (int)header.cartridgeType << "\n"
            << "Checksum valide: " << std::boolalpha << header.checksumValid << "\n"
            << "Nombre d'opcodes: " << std::dec << opcodes.size() << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur: " << e.what() << "\n";
    }

    return 0;
}