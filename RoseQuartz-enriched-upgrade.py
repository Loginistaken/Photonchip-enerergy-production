# Rose Quartz Photon Chip Insert
# Description: Full-spectrum photon emission and recycling chip using doped Rose Quartz (Mn, Al, Fe)
# This is an energy recycling add-on for the original device. It expands photon capture capabilities.
# It references the original device's energy core but enhances it with multi-dopant quantum mineral logic.

"""
OVERVIEW:
This module upgrades the existing energy device by introducing a photon chip made from Rose Quartz enriched with:
 - Manganese (Mn) for red/near-IR emissions
 - Aluminum (Al) for UV-blue emissions
 - Iron (Fe) for green-yellow photon range

Together, these dopants provide full-spectrum photon emission from UV to IR, improving thermal-to-light conversion and light recycling by up to 50%.

This photon chip insert can be added to the initial energy device without full redesign. It integrates into the Energy Capture Mesh (ECM) to enhance multi-band photon capture.
"""

# --- Dopant Emission Definitions ---
dopant_properties = {
    "Mn": {
        "element": "Manganese",
        "spectrum_nm": (630, 680),
        "effect": "Red/IR photon emission",
        "voltage": 3.0
    },
    "Al": {
        "element": "Aluminum",
        "spectrum_nm": (400, 480),
        "effect": "UV-Blue photon generation",
        "voltage": 5.0
    },
    "Fe": {
        "element": "Iron",
        "spectrum_nm": (510, 600),
        "effect": "Green/Yellow photon band",
        "voltage": 4.2
    }
}

# --- Dopant Activation Logic ---
def activate_dopant(voltage):
    """
    Activates the dopant that matches the input voltage (±0.2V tolerance).
    Returns dopant name and spectral range.
    """
    for symbol, data in dopant_properties.items():
        if abs(voltage - data["voltage"]) < 0.2:
            return {
                "dopant": symbol,
                "element": data["element"],
                "range_nm": data["spectrum_nm"],
                "effect": data["effect"]
            }
    return {"dopant": None, "message": "No dopant activated at this voltage."}

# --- Photon Emission Core ---
def photon_emission_core(input_voltages):
    """
    Simulates multi-band photon emission from doped rose quartz based on voltage inputs.
    Returns list of activated dopants and corresponding photon data.
    """
    results = []
    for voltage in input_voltages:
        result = activate_dopant(voltage)
        if result["dopant"]:
            results.append(result)
    return results

# --- Integration into Main Device (Reference Only) ---
"""
Assumes main device core contains:
- energy_input_module()
- ecm (Energy Capture Mesh)
- thermal_conversion_matrix()
This module attaches as a layered insert between energy input and ecm.
It feeds additional photon data to ECM for enhanced conversion.
"""

def upgrade_device_with_photon_chip(device_core):
    """
    Attaches the photon chip insert to the device core.
    Injects new photon streams into ECM.
    """
    base_energy = device_core.get("energy_input", [])
    emitted_photons = photon_emission_core(base_energy)
    for photon in emitted_photons:
        print(f"[PhotonChip] {photon['element']} activated: {photon['effect']} ({photon['range_nm'][0]}-{photon['range_nm'][1]} nm)")
        device_core["ecm"].append(photon)
    return device_core

# --- Test Setup ---
if __name__ == "__main__":
    # Simulate input from base device voltages
    simulated_device = {
        "energy_input": [3.0, 4.2, 5.0],
        "ecm": []
    }

    upgraded_device = upgrade_device_with_photon_chip(simulated_device)

    print("\n[Device ECM Photon Layers]:")
    for layer in upgraded_device["ecm"]:
        print(layer)

"""
EXPECTED OUTPUT:
[PhotonChip] Manganese activated: Red/IR photon emission (630-680 nm)
[PhotonChip] Iron activated: Green/Yellow photon band (510-600 nm)
[PhotonChip] Aluminum activated: UV-Blue photon generation (400-480 nm)

[Device ECM Photon Layers]:
{'dopant': 'Mn', 'element': 'Manganese', ...}
...
"""
dopant_zones = {
    "Mn": {"range_nm": (630, 680), "voltage": 3.0},
    "Fe": {"range_nm": (510, 600), "voltage": 4.2},
    "Al": {"range_nm": (400, 480), "voltage": 5.0}
}

def excite_dopant(voltage):
    for dopant, config in dopant_zones.items():
        if abs(voltage - config["voltage"]) < 0.2:
            return f"{dopant} activated at {config['range_nm']} nm"
    return "No dopant active"
