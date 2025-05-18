# energy_photon_chip_design.py

import math

class QuantumPhotonChip:
    PLANCK_CONSTANT = 6.62607015e-34  # Js
    SPEED_OF_LIGHT = 2.998e8          # m/s
    DEFAULT_WAVELENGTH = 700e-9       # meters (visible red)

    def __init__(self):
        self.mode = 0  # 0 = Manual, 1 = Auto
        self.gas_input = 0  # In gas units
        self.photon_scaling = 42  # Default emission factor
        self.temperature = 300  # Kelvin
        self.error_flags = 0b00000000
        self.interrupt_enabled = False
        self.efficiency = 0.85

        # Multi-frequency setup
        self.wavelengths = [850e-9, 700e-9]
        self.frequencies = [self.SPEED_OF_LIGHT / wl for wl in self.wavelengths]
        self.energies = [self.PLANCK_CONSTANT * f for f in self.frequencies]

    def thermal_loss_factor(self):
        # Simulated thermal loss coefficient
        return 1 - 0.002 * (self.temperature - 300)

    def adjusted_photon_yield(self, gas_units):
        temp_coeff = self.thermal_loss_factor()
        env_noise = 1 + 0.02 * (self.temperature - 300)
        return (gas_units * self.efficiency * temp_coeff) / env_noise

    def simulate_operation(self):
        if self.mode == 0:
            print("Manual mode active. Waiting for command...")
            return

        photons_emitted = self.adjusted_photon_yield(self.gas_input) * self.photon_scaling
        print(f"Gas Used: {self.gas_input}, Photons Emitted: {photons_emitted:.2f}")
        return photons_emitted

    def update_registers(self, addr, value):
        if addr == 0x00:
            self.mode = value
        elif addr == 0x01:
            self.gas_input = (self.gas_input & 0xFF00) | value
        elif addr == 0x02:
            self.gas_input = (self.gas_input & 0x00FF) | (value << 8)
        elif addr == 0x03:
            self.photon_scaling = value
        elif addr == 0x06:
            self.interrupt_enabled = bool(value)

    def read_register(self, addr):
        if addr == 0x00:
            return self.mode
        elif addr == 0x04:
            return 1  # Assume emission complete for demo
        elif addr == 0x05:
            return self.error_flags
        elif addr == 0x06:
            return int(self.interrupt_enabled)
        return 0


# Suggested SPI Register Map (for documentation/reference)
SPI_REGISTER_MAP = {
    0x00: "MODE_SELECT",
    0x01: "GAS_INPUT_LOW",
    0x02: "GAS_INPUT_HIGH",
    0x03: "PHOTON_SCALING",
    0x04: "EMISSION_STATUS",
    0x05: "ERROR_FLAGS",
    0x06: "INTERRUPT_ENABLE"
}

# Energy Budget Planning Table (example values for benchmarking)
ENERGY_BUDGET = [
    {"operation": "Transfer", "gas": 21000, "photons": 882000, "energy_uJ": 0.46},
    {"operation": "Smart Contract Call", "gas": 50000, "photons": 2100000, "energy_uJ": 1.1},
    {"operation": "Contract Deployment", "gas": 150000, "photons": 6300000, "energy_uJ": 3.3}
]
