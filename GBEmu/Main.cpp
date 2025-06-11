#include <iostream>
#include "GBRomReader.hpp"
#include "Emu.hpp"

void afficherOpcodes(const GBRomReader& reader) {
    auto opcodes = reader.getOpcodes();
    size_t addr = 0x0;

    for (uint16_t i = 0; i < opcodes.size(); ++i) {
        // Affiche l'adresse et l'opcode en hexadécimal
        printf("%04zx: %02X\n", addr + i, opcodes[i]);

        if (i == 0x200) { break; }
    }
}

int main() {
    GBRomReader reader;

    try {
        reader.load("04.gb");
        const auto& header = reader.getHeader();
        auto opcodes = reader.getOpcodes();

        std::cout << "Titre: " << header.title << "\n"
            << "Type cartouche: 0x" << std::hex << (int)header.cartridgeType << "\n"
            << "Checksum valide: " << std::boolalpha << header.checksumValid << "\n"
            << "Nombre d'opcodes: " << std::dec << opcodes.size() << "\n\n";

        Emu emu(reader.getOpcodes());
		emu.run(); // Démarre l'émulateur en mode debug

    }
    catch (const std::exception& e) {
        std::cerr << "Erreur: " << e.what() << "\n";
    }

    return 0;
}
