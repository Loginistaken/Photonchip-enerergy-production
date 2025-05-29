#include <iostream>
#include <cmath>
#include <string>
#include <map>
#include <iomanip>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif
#include <string>
// photon_energy_relay.cpp
#include <cstring>
#include <cstdio>

// Replace with your actual device ID
#define DEVICE_ID "DEVICE_ID=EL40-SN-00012345\n"

// Stub: Replace with your actual USB Serial/TinyUSB/STM32Cube/ESP-IDF driver includes
#include "usb_serial.h" // Must implement: usb_serial_init(cb), usb_serial_write(data, len), usb_serial_task()

// Compliance PDF/Signature check could be delegated to host (Python/FastAPI portal)

void serial_rx_callback(const char* buf, size_t len) {
    if (strncmp(buf, "GET_DEVICE_ID", 13) == 0) {
        usb_serial_write(DEVICE_ID, strlen(DEVICE_ID));
    }
    // Extend: add further commands as needed
}

// --- Hardware relay stub functions ---
// (Replace with actual PD/relay control logic per platform)
void init_usb_power_delivery() {
    // For STM32: integrate ST USBPD middleware
    // For ESP32/RP2040: TinyUSB PD (or custom handshake with FUSB302/STUSB1602)
    printf("USB Power Delivery handshake initiated...\n");
    // Set power profiles, negotiate voltage/current as needed
}

void monitor_energy_relay() {
    // Real hardware: sample current, voltage, temperature, log as required
    printf("Monitoring energy relay (stub)...\n");
}

// --- Main Entry ---
int main() {
    usb_serial_init(serial_rx_callback); // Set up serial device ID responder

    // Start USB PD handshake/power relay (stub)
    init_usb_power_delivery();

    // Main loop: maintain relay, respond to GET_DEVICE_ID, monitor/protect
    while (1) {
        usb_serial_task();       // Handle USB CDC events
        monitor_energy_relay();  // Extend with actual monitoring logic
        // Add: Protection (overcurrent/thermal), relay control, compliance logging
        // Delay or sleep as appropriate for your MCU/RTOS
    }
    return 0;
}
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
// --- USB Device Detection ---
// Example IDs, replace with your actual device's vendor and product IDs
constexpr uint16_t PHOTON_VENDOR_ID = 0xC0DE;
constexpr uint16_t PHOTON_PRODUCT_ID = 0xB10C;

#ifdef _WIN32
#include <setupapi.h>
#pragma comment(lib, "setupapi.lib")
// For real deployment: use SetupDi APIs or libusb for robust detection
bool detect_photon_device() {
    // TODO: Implement full Windows USB device detection for production
    return true; // For now, always 'find' device
}
#else
#include <libusb-1.0/libusb.h>
bool detect_photon_device() {
    libusb_context* ctx = nullptr;
    libusb_device** devs;
    bool found = false;
    libusb_init(&ctx);
    ssize_t cnt = libusb_get_device_list(ctx, &devs);
    for (ssize_t i = 0; i < cnt; i++) {
        libusb_device_descriptor desc;
        libusb_get_device_descriptor(devs[i], &desc);
        if (desc.idVendor == PHOTON_VENDOR_ID && desc.idProduct == PHOTON_PRODUCT_ID) {
            found = true;
            break;
        }
    }
    libusb_free_device_list(devs, 1);
    libusb_exit(ctx);
    return found;
}
bool show_license_agreement() {
#ifdef _WIN32
    std::wstring license =
        L"PHOTON CHIP INSTALLATION & BLOCKCHAIN CONSENT\n\n"
        L"By clicking OK, you consent to the following:\n"
        L"  - Device activation and communication with your computer\n"
        L"  - Tracking of energy recycling and photon emissions for blockchain operations\n"
        L"  - Data exchanges related to token transfers and blockchain interactions\n"
        L"\n"
        L"You must accept this agreement for the chip and software to operate.\n"
        L"\n[Insert full legal/technical text here]\n";
    int result = MessageBoxW(NULL, license.c_str(), L"PhotonDevice License & Blockchain Agreement", MB_OKCANCEL | MB_ICONINFORMATION);
    if (result == IDOK) {
        std::cout << "[License] User accepted the agreement." << std::endl;
        return true;
    } else {
        std::cout << "[License] User declined the agreement." << std::endl;
        return false;
    }
#else
    // Linux/MacOS: Use zenity for graphical dialog
    std::string agreement_text =
        "PHOTON CHIP INSTALLATION & BLOCKCHAIN CONSENT\n\n"
        "By clicking OK, you consent to the following:\n"
        "  - Device activation and communication with your computer\n"
        "  - Tracking of energy recycling and photon emissions for blockchain operations\n"
        "  - Data exchanges related to token transfers and blockchain interactions\n"
        "\n"
        "You must accept this agreement for the chip and software to operate.\n"
        "\n[Insert full legal/technical text here]\n";
    std::string cmd = "zenity --question --width=500 --title='PhotonDevice License & Blockchain Agreement' --text=\"" +
        agreement_text + "\"";
    int result = system(cmd.c_str());
    if (result == 0) {
        std::cout << "[License] User accepted the agreement." << std::endl;
        return true;
    } else {
        std::cout << "[License] User declined the agreement." << std::endl;
        return false;
    }
#endif
}
#endif
#ifndef DEVICECONNECTOR_H
#define DEVICECONNECTOR_H

#include <iostream>
#include <string>

// Base class for device connections
class DeviceConnector {
public:
    virtual void connect() = 0;
    virtual void sendCommand(const std::string& cmd) = 0;
    virtual std::string receiveData() = 0;
    virtual ~DeviceConnector() {}
};
double n_photons = spec.photons_per_gas_unit * gas_used * quantum_efficiency() * temp_emission_scaling();
double effective_gas = gas_used * (1.0 - get_dynamic_recycle_efficiency());
double n_photons = spec.photons_per_gas_unit * effective_gas * quantum_efficiency() * temp_emission_scaling();
// Serial connection (for USB, UART, etc.)
class SerialDeviceConnector : public DeviceConnector {
public:
    void connect() override {
        std::cout << "[SerialDeviceConnector] Connecting to Photon Chip via serial..." << std::endl;
        // TODO: implement actual serial connection logic
    }

    void sendCommand(const std::string& cmd) override {
        std::cout << "[SerialDeviceConnector] Sending command: " << cmd << std::endl;
        // TODO: implement send logic
    }

    std::string receiveData() override {
        std::cout << "[SerialDeviceConnector] Receiving data..." << std::endl;
        // TODO: implement receive logic
        return "Mocked data from device";
    }
};
// Hardware Abstraction Layer for microcontroller/SoC support
class HardwareAbstraction {
public:
    virtual void setGPIO(int pin, bool value) = 0;
    virtual int readADC(int channel) = 0;
    virtual ~HardwareAbstraction() {}
};

// Example stub for future embedded/FPGA/SoC integration
class DummyHardware : public HardwareAbstraction {
public:
    void setGPIO(int pin, bool value) override {}
    int readADC(int channel) override { return 0; }
};
// (Optional) Future: add Raspberry Pi or other connectors here

#endif // DEVICECONNECTOR_H
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
#include <immintrin.h>
// Example: Accelerate photon energy computation (for batch arrays)
// (Add only if you have large array-based calculations)
void simd_photon_energy(const double* freqs, double* energies, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        energies[i] = PLANCK * freqs[i];
    }
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
    // Device detection and license agreement
    if (!detect_photon_device()) {
        std::cerr << "PhotonDevice not detected. Please connect via USB-C and retry." << std::endl;
        return 1;
    }
    if (!show_license_agreement()) {
        std::cerr << "Installation aborted. User did not accept the license agreement." << std::endl;
        return 1;
    }
    std::cout << "User accepted agreement. Proceeding with installation/activation..." << std::endl;
    // TODO: Add installation logic if needed (driver copy, config, etc.)
    void set_mode(bool is_auto) { auto_mode = is_auto; }
};
#include <thread>
#include <atomic>

class RealTimePhotonSimulator {
public:
    RealTimePhotonSimulator(PhotonicChip& chip) : chip_(chip), running_(false) {}
    void start(uint64_t gas_per_cycle, int interval_ms = 100) {
        running_ = true;
        worker_ = std::thread([this, gas_per_cycle, interval_ms]() {
            while (running_) {
                chip_.simulate_operation(gas_per_cycle, "real_time_thread");
                std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
            }
        });
    }
    void stop() {
        running_ = false;
        if (worker_.joinable()) worker_.join();
    }
    ~RealTimePhotonSimulator() { stop(); }
private:
    PhotonicChip& chip_;
    std::atomic<bool> running_;
    std::thread worker_;
};
// -------- ORBIMINT ERC-20 STYLE TOKEN SIMULATION --------
class OrbimintToken {
public:
    struct Account {
        uint64_t balance = 0;
        uint64_t photons_emitted = 0;
        double energy_emitted = 0.0;
    };
struct UserCredentials {
  std::map<std::string, UserCredentials> user_db;
std::string username;
    std::string email;
    std::string password_hash; // Never store plaintext!
    uint64_t token_amount;
    std::time_t timestamp;
    // Add more fields as needed (e.g., 2FA secret, device ID)
};
    std::map<std::string, Account> accounts;
    uint8_t decimals = 7;  // Orbimint uses 7 decimals
    uint64_t totalSupply;
    PhotonicChip& chip;
std::string device_signature;
std::string recovery_question;
std::string backup_email;
#include <libusb-1.0/libusb.h>
// ... (other includes)
bool verify_firmware_signature(const std::string& firmware_path);
// Call this before running chip logic

bool request_user_pin();
// Call this before sensitive operations
bool detect_photon_device(uint16_t vendor_id, uint16_t product_id) {
    libusb_context* ctx = NULL;
    libusb_device** devs;
    ssize_t cnt;
    bool found = false;
    libusb_init(&ctx);
    cnt = libusb_get_device_list(ctx, &devs);
    for (ssize_t i = 0; i < cnt; i++) {
        libusb_device_descriptor desc;
        libusb_get_device_descriptor(devs[i], &desc);
        if (desc.idVendor == vendor_id && desc.idProduct == product_id) {
            found = true;
            break;
        }
    }
    libusb_free_device_list(devs, 1);
    libusb_exit(ctx);
    return found;
}
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
// Securely zero memory for sensitive data (C++17 or above)
#include <cstring>

// Utility function: securely wipe sensitive data buffers
void secure_zero(void* p, size_t n) {
#if defined(_WIN32)
    SecureZeroMemory(p, n);
#else
    std::memset(p, 0, n);
    std::atomic_signal_fence(std::memory_order_seq_cst);
#endif
}
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
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>

// Utility function for SHA-256 hashing
std::string hash_password(const std::string& password, const std::string& salt) {
    std::string input = password + salt;
    CryptoPP::SHA256 hash;
    std::string digest;
    CryptoPP::StringSource s(input, true, new CryptoPP::HashFilter(hash, new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest))));
    return digest;
}
constexpr uint8_t DECIMALS = 7;                          // 7 decimals precision (low gas, eco-friendly)
constexpr uint64_t INITIAL_SUPPLY = 700000000000000000; // 7 billion ORBIM with 7 decimals (7 * 10^17)
const std::string COIN_NAME = "Orbimint";
const std::string COIN_SYMBOL = "ORBIM";
constexpr uint64_t BLOCK_INTERVAL_MS = 3000;            // 10 seconds block time (fast & low gas)

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
#include "DeviceConnector.h" // Add at the top if you create a separate file

int main() {
    // Create connector instance (can switch to other types in future)
    SerialDeviceConnector device;

    // Connect to device
    device.connect();

    // Send command to Photon Chip
    device.sendCommand("START_ENERGY_PRODUCTION");

    // Receive data from device
    std::string data = device.receiveData();
    std::cout << "Device responded: " << data << std::endl;

    // ... rest of your existing energy production logic ...
    return 0;
}
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
// If compiling with pybind11, expose key functions to Python
#ifdef BUILD_PYBIND
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(photonchip_cpp, m) {
    py::class_<ChipSpec>(m, "ChipSpec")
        .def(py::init<>())
        .def_readwrite("wavelength_nm", &ChipSpec::wavelength_nm)
        .def_readwrite("efficiency", &ChipSpec::efficiency)
        .def_readwrite("temperature", &ChipSpec::temperature);

    py::class_<PhotonicChip>(m, "PhotonicChip")
        .def(py::init<const ChipSpec&>())
        .def("photon_frequency", &PhotonicChip::photon_frequency)
        .def("photon_energy", &PhotonicChip::photon_energy)
        .def("photons_emitted", &PhotonicChip::photons_emitted)
        .def("total_energy_emitted", &PhotonicChip::total_energy_emitted);
}
#endif
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
}_t photons_absorbed(uint64_t incident_photons) const {
    return static_cast<uint64_t>(incident_photons * quantum_efficiency());
}
if (!verify_identity(tx.sender)) {
    show_error("Unauthorized Tx");
    continue;
}
// After transaction in transfer, or after simulate_operation
display.show_transaction(gas_used, effective_gas, n_photons, total_energy, ...);
double generated_current(uint64_t incident_photons) const {
    double charge = 1.602e-19; // electron charge (Coulombs)
    uint64_t absorbed = photons_absorbed(incident_photons);
    return absorbed * charge; // Amperes
}
