# Rose Quartz Quantum Photon Chip – Layered Multi-Dopant Simulation

import math

# --- Physical Constants ---
PLANCK = 6.62607015e-34        # Planck constant (J·s)
C = 299_792_458                # Speed of light (m/s)
E_CHARGE = 1.602e-19           # Coulombs

# --- Dopant Layer Definitions ---
DOPANT_LAYERS = [
    {"name": "Mn", "element": "Manganese",   "spectrum_nm": (630, 680),  "base_wavelength": 660e-9, "voltage": 3.0},
    {"name": "P",  "element": "Phosphorus",  "spectrum_nm": (420, 480),  "base_wavelength": 450e-9, "voltage": 5.0},
    {"name": "Fe", "element": "Iron",        "spectrum_nm": (510, 600),  "base_wavelength": 550e-9, "voltage": 4.2},
    # To extend: add {"name": "Ce", ...}, etc.
]

VOLTAGE_TOLERANCE = 0.2
VOLTAGE_EFFECT = 0.04  # Tuning factor: how voltage shifts bandgap

# --- Layered Crystal Model ---
class DopedLayer:
    def __init__(self, dopant, base_voltage):
        self.dopant = dopant
        self.base_voltage = base_voltage
        self.active_voltage = None
        self.shifted_wavelength = None
        self.photon_energy = None

    def excite(self, applied_voltage):
        """Excite layer if voltage matches; shift wavelength accordingly."""
        if abs(applied_voltage - self.base_voltage) < VOLTAGE_TOLERANCE:
            # Bandgap tuning: shift wavelength by applied voltage
            shift_factor = 1 - VOLTAGE_EFFECT * (applied_voltage - self.base_voltage)
            self.shifted_wavelength = self.dopant["base_wavelength"] * shift_factor
            self.photon_energy = PLANCK * C / self.shifted_wavelength
            self.active_voltage = applied_voltage
            return True
        else:
            self.shifted_wavelength = None
            self.photon_energy = None
            self.active_voltage = None
            return False

    def emission_data(self):
        if self.photon_energy is None:
            return None
        return {
            "dopant": self.dopant["name"],
            "element": self.dopant["element"],
            "range_nm": self.dopant["spectrum_nm"],
            "wavelength_nm": self.shifted_wavelength * 1e9,
            "energy_eV": self.photon_energy / E_CHARGE,
            "activated_voltage": self.active_voltage
        }

# --- Quantum Photon Chip Core (Layered) ---
class QuantumPhotonChip:
    def __init__(self, dopant_layers):
        self.layers = [DopedLayer(dopant, dopant["voltage"]) for dopant in dopant_layers]

    def excite_layers(self, applied_voltages):
        """Apply voltages to each layer separately (layered doping model)."""
        emissions = []
        for layer, voltage in zip(self.layers, applied_voltages):
            if layer.excite(voltage):
                emissions.append(layer.emission_data())
        return emissions

# --- Quantum Device Pipeline ---
class QuantumPhotonDevice:
    def __init__(self, chip: QuantumPhotonChip):
        self.chip = chip
        self.gain_medium = "InGaAs"  # Example; could be modular
        self.photovoltaic_type = "Perovskite"
        self.tpv_type = "GaSb TPV Cell"
        self.energy_collected = 0.0

    def run_cycle(self, input_voltages):
        print("\n[Quantum Photon Device Cycle Initiated]")
        print(f"Applied voltages (per layer): {input_voltages}")

        # 1. Excite each dopant layer independently
        emissions = self.chip.excite_layers(input_voltages)
        if not emissions:
            print("No layers activated: voltage mismatch.")
            return

        print("\n[Photon Emissions (Layer-resolved)]:")
        for e in emissions:
            print(f"Layer {e['dopant']} ({e['element']}): {e['wavelength_nm']:.1f} nm, {e['energy_eV']:.3f} eV (V={e['activated_voltage']})")

        # 2. Optical Gain (simulated amplification)
        gain_factor = 2.0
        amplified = []
        for e in emissions:
            amp = e.copy()
            amp["energy_eV"] *= gain_factor
            amplified.append(amp)
        print("\n[Amplified Photon Output]:")
        for a in amplified:
            print(f"Layer {a['dopant']}: {a['energy_eV']:.3f} eV (amplified)")

        # 3. Energy Conversion (PV & TPV)
        pv_eff = 0.27
        tpv_eff = 0.16
        pv_energy = sum(a["energy_eV"] for a in amplified if a["wavelength_nm"] < 700)
        tpv_energy = sum(a["energy_eV"] for a in amplified if a["wavelength_nm"] >= 700)
        elec_output = pv_energy * pv_eff + tpv_energy * tpv_eff

        self.energy_collected += elec_output
        print(f"\n[Total Electrical Output this cycle]: {elec_output:.3f} eV")
        print(f"[Cumulative Device Output]: {self.energy_collected:.3f} eV")

        # 4. Photon Recycling (40% simulated feedback)
        recycled = int(len(amplified) * 0.4)
        print(f"\n[Photon Recycling]: {recycled} photons recycled (simulated)")

        # 5. OLED/Gas Display Output
        print("\n[OLED/Gas Frequency Display]:")
        for a in amplified:
            print(f"{a['dopant']} ({a['element']}): {a['wavelength_nm']:.1f} nm | {a['energy_eV']:.3f} eV")

# --- Run a full simulation cycle ---
if __name__ == "__main__":
    # Example: three voltages, one for each layer (no overlap)
    input_voltages = [3.0, 5.0, 4.2]
    chip = QuantumPhotonChip(DOPANT_LAYERS)
    device = QuantumPhotonDevice(chip)
    device.run_cycle(input_voltages)
