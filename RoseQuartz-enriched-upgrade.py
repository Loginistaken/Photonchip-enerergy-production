import math
from typing import List, Optional

# --- Physical Constants ---
PLANCK = 6.62607015e-34        # Planck constant (J·s)
C = 299_792_458                # Speed of light (m/s)
E_CHARGE = 1.602e-19           # Coulombs

# --- Dopant Layer Definitions ---
DOPANT_LAYERS = [
    {"name": "Mn", "element": "Manganese",   "spectrum_nm": (630, 680),  "base_wavelength": 660e-9, "voltage": 3.0},
    {"name": "P",  "element": "Phosphorus",  "spectrum_nm": (420, 480),  "base_wavelength": 450e-9, "voltage": 5.0},
    {"name": "Fe", "element": "Iron",        "spectrum_nm": (510, 600),  "base_wavelength": 550e-9, "voltage": 4.2},
    # Extendable: add {"name": "Ce", ...}, etc.
]

# --- Tuning Parameters ---
DEFAULT_TOLERANCE = 0.2
VOLTAGE_EFFECT = 0.04  # Wavelength shift per voltage mismatch

# --- Wavelength to Visible Color (approx.) ---
def wavelength_to_color(wavelength_nm: float) -> str:
    if 380 <= wavelength_nm <= 450:
        return "Violet"
    elif 451 <= wavelength_nm <= 495:
        return "Blue"
    elif 496 <= wavelength_nm <= 570:
        return "Green"
    elif 571 <= wavelength_nm <= 590:
        return "Yellow"
    elif 591 <= wavelength_nm <= 620:
        return "Orange"
    elif 621 <= wavelength_nm <= 750:
        return "Red"
    return "Infrared/Out of visible"

# --- Layer Class ---
class DopedLayer:
    def __init__(self, dopant: dict, voltage_tolerance: float = DEFAULT_TOLERANCE):
        self.dopant = dopant
        self.base_voltage = dopant["voltage"]
        self.voltage_tolerance = voltage_tolerance
        self.reset()

    def reset(self):
        self.active_voltage: Optional[float] = None
        self.shifted_wavelength: Optional[float] = None
        self.photon_energy: Optional[float] = None

    def excite(self, applied_voltage: float) -> bool:
        self.reset()
        delta = abs(applied_voltage - self.base_voltage)
        if delta < self.voltage_tolerance:
            shift_factor = 1 - VOLTAGE_EFFECT * (applied_voltage - self.base_voltage)
            self.shifted_wavelength = self.dopant["base_wavelength"] * shift_factor
            self.photon_energy = PLANCK * C / self.shifted_wavelength
            self.active_voltage = applied_voltage
            return True
        return False

    def emission_data(self, units: str = "eV") -> Optional[dict]:
        if self.photon_energy is None:
            return None
        energy = self.photon_energy / E_CHARGE if units == "eV" else self.photon_energy
        wavelength_nm = self.shifted_wavelength * 1e9
        return {
            "dopant": self.dopant["name"],
            "element": self.dopant["element"],
            "range_nm": self.dopant["spectrum_nm"],
            "wavelength_nm": wavelength_nm,
            "energy": energy,
            "units": units,
            "activated_voltage": self.active_voltage,
            "color": wavelength_to_color(wavelength_nm)
        }

# --- Quantum Chip ---
class QuantumPhotonChip:
    def __init__(self, dopants: List[dict], voltage_tolerance: float = DEFAULT_TOLERANCE):
        self.layers = [DopedLayer(dopant, voltage_tolerance) for dopant in dopants]

    def excite_layers(self, applied_voltages: List[float], units: str = "eV") -> List[dict]:
        emissions = []
        for layer, voltage in zip(self.layers, applied_voltages):
            if layer.excite(voltage):
                data = layer.emission_data(units)
                if data:
                    emissions.append(data)
        return emissions

# --- Quantum Device Pipeline ---
class QuantumPhotonDevice:
    def __init__(
        self,
        chip: QuantumPhotonChip,
        gain_medium: str = "InGaAs",
        photovoltaic_type: str = "Perovskite",
        tpv_type: str = "GaSb TPV Cell",
        pv_eff: float = 0.27,
        tpv_eff: float = 0.16,
        gain_factor: float = 2.0,
        units: str = "eV"
    ):
        self.chip = chip
        self.gain_medium = gain_medium
        self.photovoltaic_type = photovoltaic_type
        self.tpv_type = tpv_type
        self.pv_eff = pv_eff
        self.tpv_eff = tpv_eff
        self.gain_factor = gain_factor
        self.energy_collected = 0.0   # Cumulative electrical output (in units)
        self.units = units

    def run_cycle(self, input_voltages: List[float]):
        print("\n[Quantum Photon Device Cycle Initiated]")
        print(f"Applied voltages (per layer): {input_voltages}")

        # 1. Excite each dopant layer independently
        emissions = self.chip.excite_layers(input_voltages, self.units)
        if not emissions:
            print("No layers activated: voltage mismatch.")
            return

        print("\n[Photon Emissions (Layer-resolved)]:")
        for e in emissions:
            print(
                f"Layer {e['dopant']} ({e['element']}): "
                f"{e['wavelength_nm']:.1f} nm, {e['energy']:.3f} {e['units']} (V={e['activated_voltage']}) | Color: {e['color']}"
            )

        # 2. Optical Gain (simulated amplification)
        amplified = []
        for e in emissions:
            amp = e.copy()
            amp["energy"] *= self.gain_factor
            amplified.append(amp)
        print("\n[Amplified Photon Output]:")
        for a in amplified:
            print(
                f"Layer {a['dopant']}: {a['energy']:.3f} {a['units']} (amplified) | Color: {a['color']}"
            )

        # 3. Energy Conversion (PV & TPV)
        pv_energy = sum(a["energy"] for a in amplified if a["wavelength_nm"] < 700)
        tpv_energy = sum(a["energy"] for a in amplified if a["wavelength_nm"] >= 700)
        elec_output = pv_energy * self.pv_eff + tpv_energy * self.tpv_eff

        self.energy_collected += elec_output
        print(f"\n[PV ({self.photovoltaic_type}) efficiency]: {self.pv_eff*100:.1f}%")
        print(f"[TPV ({self.tpv_type}) efficiency]: {self.tpv_eff*100:.1f}%")
        print(f"[Total Electrical Output this cycle]: {elec_output:.3f} {self.units}")
        print(f"[Cumulative Device Output]: {self.energy_collected:.3f} {self.units}")

        # 4. Photon Recycling (40% simulated feedback)
        recycled = int(len(amplified) * 0.4)
        print(f"\n[Photon Recycling]: {recycled} photons recycled (simulated)")

        # 5. OLED/Gas Display Output
        print("\n[OLED/Gas Frequency Display]:")
        for a in amplified:
            print(
                f"{a['dopant']} ({a['element']}): {a['wavelength_nm']:.1f} nm | "
                f"{a['color']} | {a['energy']:.3f} {a['units']}"
            )

# --- Run a full simulation cycle ---
if __name__ == "__main__":
    # Example: three voltages, one for each layer (no overlap)
    input_voltages = [3.0, 5.0, 4.2]
    chip = QuantumPhotonChip(DOPANT_LAYERS)
    device = QuantumPhotonDevice(chip)
    device.run_cycle(input_voltages)
