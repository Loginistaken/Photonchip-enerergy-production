#include <iostream>
#include <cmath>
#include <string>
#include <map>
#include <iomanip>
#include <algorithm>

// -------- PHYSICAL CONSTANTS --------
constexpr double PLANCK = 6.626e-34;          // Planck's constant (J*s)
constexpr double C = 3.0e8;                    // Speed of light (m/s)
constexpr double K_BOLTZ = 1.38e-23;          // Boltzmann constant (J/K)
constexpr double IR_WAVELENGTH_NM = 1000.0;   // IR photon wavelength (chip design, nm)
constexpr double IR_WAVELENGTH_M = IR_WAVELENGTH_NM * 1e-9;
constexpr double DEFAULT_EFFICIENCY = 0.85;   // Quantum efficiency target
constexpr double DEFAULT_TEMP = 300.0;        // Room temp Kelvin
constexpr int PHOTONS_PER_GAS_UNIT = 42;      // Scaling factor (k)
constexpr double TEMP_COEFF = 0.002;          // Temperature scaling alpha

// -------- CHIP HARDWARE SPEC --------
struct ChipSpec {
    double wavelength_nm = IR_WAVELENGTH_NM;
    double efficiency = DEFAULT_EFFICIENCY;
    double temperature = DEFAULT_TEMP;
    double supply_voltage = 1.1;
    double clock_freq_GHz = 2.0;
    double max_photon_rate = 1e9;
    double power_consumption_W = 0.05;
    int photons_per_gas_unit = PHOTONS_PER_GAS_UNIT;
    double temp_coeff = TEMP_COEFF;
};

// -------- PHOTONIC CHIP MODEL --------
class PhotonicChip {
public:
    ChipSpec spec;
    bool auto_mode = true;

    PhotonicChip(const ChipSpec& s = ChipSpec()) : spec(s) {}

    double photon_frequency() const {
        return C / (spec.wavelength_nm * 1e-9);
    }

    double photon_energy() const {
        return PLANCK * photon_frequency();
    }

    double thermal_loss_factor() const {
        double loss = std::exp(-(spec.temperature - 300.0) / 100.0);
        return std::max(loss, 0.1);
    }

    double quantum_efficiency() const {
        return spec.efficiency * thermal_loss_factor();
    }

    double temp_emission_scaling() const {
        return 1.0 + spec.temp_coeff * (spec.temperature - 300.0);
    }

    uint64_t photons_emitted(uint64_t gas_used) const {
        double n_photons = spec.photons_per_gas_unit * gas_used * quantum_efficiency() * temp_emission_scaling();
        return static_cast<uint64_t>(n_photons);
    }

    double total_energy_emitted(uint64_t gas_used) const {
        return photons_emitted(gas_used) * photon_energy();
    }

    void simulate_operation(uint64_t gas_used, const std::string& caller = "user") {
        auto n_photons = photons_emitted(gas_used);
        auto e_total = total_energy_emitted(gas_used);
        std::cout << std::fixed << std::setprecision(7);
        std::cout << "[Chip] Mode: " << (auto_mode ? "AUTO" : "MANUAL")
                  << " | Gas used: " << gas_used
                  << " | Photons emitted: " << n_photons
                  << " | Photon energy (J): " << photon_energy()
                  << " | Total energy (J): " << e_total
                  << " | Caller: " << caller << std::endl;
    }

    void set_mode(bool is_auto) { auto_mode = is_auto; }
};

// -------- ORBIMINT ERC-20 STYLE TOKEN SIMULATION --------
class OrbimintToken {
public:
    struct Account {
        uint64_t balance = 0;
        uint64_t photons_emitted = 0;
        double energy_emitted = 0.0;
    };

    std::map<std::string, Account> accounts;
    uint8_t decimals = 7;  // Orbimint uses 7 decimals
    uint64_t totalSupply;
    PhotonicChip& chip;

    OrbimintToken(uint64_t initial_supply, const std::string& owner, PhotonicChip& chip_model)
        : chip(chip_model) {
        totalSupply = initial_supply * static_cast<uint64_t>(std::pow(10, decimals));
        accounts[owner].balance = totalSupply;

        // Reserve 25% supply in medieval kindships vault (example address)
        std::string vault = "0xMedievalKindshipVault";
        accounts[vault].balance = totalSupply / 4;
        accounts[owner].balance -= accounts[vault].balance;
    }

    // Transfer tokens and simulate photon emission based on gas used
    bool transfer(const std::string& from, const std::string& to, uint64_t value, uint64_t gasUsed) {
        if (accounts[from].balance < value) {
            std::cout << "[Error] Insufficient balance in " << from << std::endl;
            return false;
        }
        accounts[from].balance -= value;
        accounts[to].balance += value;

        uint64_t photons = chip.photons_emitted(gasUsed);
        double energy = chip.total_energy_emitted(gasUsed);

        accounts[from].photons_emitted += photons;
        accounts[from].energy_emitted += energy;

        // Event log
        std::cout << "[Event] Transfer: " << value << " tokens from " << from << " to " << to << std::endl;
        std::cout << "[Event] PhotonEmission: " << photons << " photons, " << energy << " J from " << from << std::endl;

        return true;
    }

    void print_account(const std::string& addr) {
        auto& acc = accounts[addr];
        std::cout << "Account: " << addr
                  << " | Balance: " << acc.balance / std::pow(10, decimals)
                  << " | Photons emitted: " << acc.photons_emitted
                  << " | Energy emitted (J): " << acc.energy_emitted << std::endl;
    }
};

// -------- MAIN DEMO: Combine Token + Photonic Chip --------
int main() {
    // Initialize chip specs and chip
    ChipSpec chip_spec;
    PhotonicChip chip(chip_spec);

    // Initialize Orbimint token with 65 million initial supply (from prior conversation)
    uint64_t initial_supply = 65'000'000;
    std::string owner_address = "0xOwner";
    OrbimintToken orbimint(initial_supply, owner_address, chip);

    // Demonstrate transfer & photon emission
    chip.set_mode(true); // AUTO mode

    // Transfer example: owner -> user1, with gas usage (simulate gas spent on tx)
    std::string user1 = "0xUser1";
    uint64_t transfer_amount = 1'000 * static_cast<uint64_t>(std::pow(10, orbimint.decimals));
    uint64_t gas_used = 100'000;

    orbimint.transfer(owner_address, user1, transfer_amount, gas_used);

    // Show account states after transfer
    orbimint.print_account(owner_address);
    orbimint.print_account(user1);

    // Manual chip operation for diagnostics
    chip.set_mode(false);
    chip.simulate_operation(250'000, "manual_test");

    // Interactive CLI for multiple commands
    std::string cmd;
    while (true) {
        std::cout << "\nCommand? (transfer/manual/auto/status/quit): ";
        std::cin >> cmd;

        if (cmd == "quit") break;

        if (cmd == "transfer") {
            std::string from, to;
            uint64_t value, gas;
            std::cout << "From: "; std::cin >> from;
            std::cout << "To: "; std::cin >> to;
            std::cout << "Amount (tokens): "; std::cin >> value;
            std::cout << "Gas used: "; std::cin >> gas;
            bool success = orbimint.transfer(from, to, value * static_cast<uint64_t>(std::pow(10, orbimint.decimals)), gas);
            if (!success) std::cout << "Transfer failed." << std::endl;
        }
        else if (cmd == "manual") {
            uint64_t gas;
            std::cout << "Gas used: "; std::cin >> gas;
            chip.set_mode(false);
            chip.simulate_operation(gas, "manual_CLI");
        }
        else if (cmd == "auto") {
            chip.set_mode(true);
            std::cout << "Switched to AUTO mode.\n";
        }
        else if (cmd == "status") {
            std::string addr;
            std::cout << "Account address to check: "; std::cin >> addr;
            orbimint.print_account(addr);
        }
        else {
            std::cout << "Unknown command." << std::endl;
        }
    }

    return 0;
}
