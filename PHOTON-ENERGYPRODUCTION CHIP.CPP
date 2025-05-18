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

    /// SPDX-License-Identifier: MIT
// Orbimint: Eco-Friendly, AI-Compatible Multi-Chain Blockchain Coin with Photon Chip Energy Extension
// Language: C++
// Combines Orbimint core blockchain with Photon Chip energy production from blockchain interaction & gas

#include <iostream>
#include <map>
#include <string>
#include <cstdint>
#include <cmath>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>
#include <random>

// Crypto++ for RSA Sign/Verify (install Crypto++ library)
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/pssr.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>

// HTTP client cpr (install cpr library for HTTP requests)
#include <cpr/cpr.h>

// Placeholder includes for compression (future: integrate zstd, lz4)
#include <zlib.h>

// --- Orbimint Blockchain Configurations ---

constexpr uint8_t DECIMALS = 7;                          // 7 decimals precision (low gas, eco-friendly)
constexpr uint64_t INITIAL_SUPPLY = 700000000000000000; // 7 billion ORBIM with 7 decimals (7 * 10^17)
const std::string COIN_NAME = "Orbimint";
const std::string COIN_SYMBOL = "ORBIM";
constexpr uint64_t BLOCK_INTERVAL_MS = 3000;            // 3 seconds block time (fast & low gas)

// --- Thread Safety ---
std::mutex ledger_mutex;
std::mutex tx_mutex;

// --- Ledger & Account Management ---

class Account {
public:
    uint64_t balance = 0;            // Balance stored as integer with DECIMALS precision
    std::string publicKeyHash = "";  // Address hash (supports sha3-64 or Ethereum 40-char)
    std::string addressType = "";    // "sha3-64" or "eth-42"
    uint64_t energyCredits = 0;      // Photon Chip energy credits from blockchain interaction
};

std::map<std::string, Account> ledger;      // Ledger mapping address -> Account
std::vector<std::string> activeAccounts;    // List of active accounts for pruning, display, etc.

// Normalize addresses to lowercase and validate
std::string normalizeAddress(const std::string& addr) {
    if (addr.substr(0, 2) != "0x" || (addr.length() != 42 && addr.length() != 66)) {
        throw std::invalid_argument("Invalid address format. Must start with 0x and length 42 or 66.");
    }
    std::string lowerAddr = addr;
    std::transform(lowerAddr.begin(), lowerAddr.end(), lowerAddr.begin(), ::tolower);
    return lowerAddr;
}

// --- RSA Signing & Verification for Transactions ---

std::string signTransaction(const std::string& data, const CryptoPP::RSA::PrivateKey& privateKey) {
    CryptoPP::AutoSeededRandomPool rng;
    std::string signature;
    CryptoPP::RSASSA_PKCS1v15_SHA_Signer signer(privateKey);
    CryptoPP::StringSource ss(data, true,
        new CryptoPP::SignerFilter(rng, signer, new CryptoPP::StringSink(signature)));
    return signature;
}

bool verifyTransaction(const std::string& data, const std::string& signature, const CryptoPP::RSA::PublicKey& publicKey) {
    CryptoPP::RSASSA_PKCS1v15_SHA_Verifier verifier(publicKey);
    bool result = false;
    try {
        CryptoPP::StringSource ss(signature + data, true,
            new CryptoPP::SignatureVerificationFilter(
                verifier,
                new CryptoPP::ArraySink((CryptoPP::byte*)&result, sizeof(result)),
                CryptoPP::SignatureVerificationFilter::THROW_EXCEPTION | CryptoPP::SignatureVerificationFilter::SIGNATURE_AT_BEGIN));
    } catch (...) {
        return false;
    }
    return result;
}

// --- AI Block Approval Simulation ---

bool approveBlockAI(const std::string& blockData) {
    std::cout << "[AI] Analyzing block data...\n";
    std::vector<std::string> suspiciousKeywords = {"fraud", "invalid", "error", "malicious"};
    for (const auto& kw : suspiciousKeywords) {
        if (blockData.find(kw) != std::string::npos) {
            std::cerr << "[AI] Block rejected due to suspicious keyword: " << kw << std::endl;
            return false;
        }
    }
    if (blockData.size() < 100 || blockData.size() > 10000) {
        std::cerr << "[AI] Block rejected due to invalid data length.\n";
        return false;
    }
    std::cout << "[AI] Block approved.\n";
    return true;
}

// --- Blockchain Core Functions ---

// Transfer tokens from one account to another
void transfer(const std::string& from, const std::string& to, uint64_t amount) {
    std::lock_guard<std::mutex> lock(ledger_mutex);
    if (ledger[from].balance < amount) {
        std::cerr << "[Transfer] Insufficient balance from " << from << std::endl;
        return;
    }
    ledger[from].balance -= amount;
    ledger[to].balance += amount;
    if (std::find(activeAccounts.begin(), activeAccounts.end(), to) == activeAccounts.end())
        activeAccounts.push_back(to);

    // Photon Chip Energy Production: Gain energy credits proportional to gas saved (simulated)
    uint64_t energyProduced = amount / 1000; // Simplified: 0.1% of transfer amount as energy credits
    ledger[to].energyCredits += energyProduced;

    std::cout << "Transferred " << amount << " ORBIM from " << from << " to " << to << std::endl;
    std::cout << "Photon Chip Energy Credits produced: " << energyProduced << std::endl;
}

// Mint new tokens to an account
void mint(const std::string& to, uint64_t amount, const std::string& pubKeyHash = "") {
    std::lock_guard<std::mutex> lock(ledger_mutex);
    ledger[to].balance += amount;
    if (!pubKeyHash.empty()) {
        ledger[to].publicKeyHash = normalizeAddress(pubKeyHash);
        ledger[to].addressType = (pubKeyHash.length() == 66) ? "sha3-64" : "eth-42";
    }
    if (std::find(activeAccounts.begin(), activeAccounts.end(), to) == activeAccounts.end())
        activeAccounts.push_back(to);
    std::cout << "Minted " << amount << " ORBIM tokens to " << to << std::endl;
}

// Burn tokens from an account
void burn(const std::string& from, uint64_t amount) {
    std::lock_guard<std::mutex> lock(ledger_mutex);
    if (ledger[from].balance < amount) {
        std::cerr << "[Burn] Insufficient balance from " << from << std::endl;
        return;
    }
    ledger[from].balance -= amount;
    std::cout << "Burned " << amount << " ORBIM tokens from " << from << std::endl;
}

// Show balance with decimal formatting
void showBalance(const std::string& user) {
    std::lock_guard<std::mutex> lock(ledger_mutex);
    if (ledger.find(user) == ledger.end()) {
        std::cout << "No such account: " << user << std::endl;
        return;
    }
    double formatted = static_cast<double>(ledger[user].balance) / std::pow(10, DECIMALS);
    std::cout << "Balance of " << user << ": " << formatted << " " << COIN_SYMBOL << std::endl;
    if (!ledger[user].publicKeyHash.empty())
        std::cout << "Public Key Hash: " << ledger[user].publicKeyHash << std::endl;
    std::cout << "Energy Credits: " << ledger[user].energyCredits << std::endl;
}

// --- Ethereum Bridge for Cross-Chain Interactions ---

class EthereumBridge {
public:
    std::string ethNodeURL = "https://mainnet.infura.io/v3/YOUR_INFURA_PROJECT_ID";
    std::string ethContractAddress = "0xYourEthContractAddressHere";

    std::string buildEthTransactionJSON(const std::string& from, const std::string& to, uint64_t value) {
        std::ostringstream oss;
        oss << R"({
            "jsonrpc": "2.0",
            "method": "eth_sendTransaction",
            "params": [{
                "from": ")" << from << R"(",
                "to": ")" << to << R"(",
                "value": "0x)" << std::hex << value << R"(",
                "gas": "0x5208",
                "gasPrice


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
