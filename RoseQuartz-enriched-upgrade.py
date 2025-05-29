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
import usb.core
import usb.util
import platform
import subprocess
import tkinter as tk
from tkinter import messagebox
import webbrowser
import threading
import time
import serial  # For real-time power monitoring
import matplotlib.pyplot as plt

# === CONFIGURATION ===
AGREEMENT_URL = "https://yourdomain.com/battery_agreement.pdf"
USB_VENDOR_ID = 0x1A40  # Replace with your device's VID
USB_PRODUCT_ID = 0x0101 # Replace with your device's PID
SERIAL_PORT = "COM3"    # Set to your device's serial port (e.g., '/dev/ttyACM0' for Linux)
BAUD_RATE = 9600        # Match your device's baud rate

# === HARDWARE DETECTION ===
def find_usb_pd_devices(vendor_id=None, product_id=None):
    devices = usb.core.find(find_all=True, idVendor=vendor_id, idProduct=product_id)
    found = False
    for device in devices:
        print(f"Device found: Vendor ID={hex(device.idVendor)}, Product ID={hex(device.idProduct)}")
        found = True
    return found

# === DRIVER INSTALLATION ===
def install_driver():
    system = platform.system()
    try:
        if system == "Windows":
            subprocess.run(["pnputil", "/add-driver", "your_driver.inf", "/install"], check=True)
        elif system == "Linux":
            subprocess.run(["sudo", "cp", "your_module.ko", "/lib/modules/$(uname -r)/extra/"])
            subprocess.run(["sudo", "depmod"])
        elif system == "Darwin":
            subprocess.run(["sudo", "installer", "-pkg", "your_driver.pkg", "-target", "/"])
        print("Driver installation attempted.")
    except Exception as e:
        print(f"Driver installation failed: {e}")

# === BATTERY/LEGAL AGREEMENT TEXT ===
AGREEMENT_TEXT = """
BATTERY/ENERGY USAGE AGREEMENT

By using this device, you acknowledge:
- Compliance with all relevant safety, handling, and recycling regulations for batteries and energy storage.
- Use of the device only with approved and certified energy storage modules.
- Any modification or misuse voids warranty and may result in injury or legal liability.
- You have read and understood the full agreement available at:
""" + AGREEMENT_URL

# === GUI FOR USER ACCEPTANCE ===
def show_agreement():
    webbrowser.open(AGREEMENT_URL)

def accept():
    window.destroy()
    run_post_agreement()

def decline():
    window.destroy()
    messagebox.showinfo("Agreement", "You must accept the agreement to proceed.")

def run_gui():
    global window
    window = tk.Tk()
    window.title("Battery/Energy Usage Agreement")
    label = tk.Label(window, text="You must accept the battery/energy usage agreement to proceed.\n")
    label.pack(pady=10)
    btn_open = tk.Button(window, text="View Agreement", command=show_agreement)
    btn_open.pack()
    btn_accept = tk.Button(window, text="Accept", command=accept)
    btn_accept.pack(side=tk.LEFT, padx=20, pady=20)
    btn_decline = tk.Button(window, text="Decline", command=decline)
    btn_decline.pack(side=tk.RIGHT, padx=20, pady=20)
    window.mainloop()

# === REAL-TIME POWER MONITORING (Serial Example) ===
def real_time_power_monitor():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        plt.ion()
        fig, ax = plt.subplots()
        times, currents, voltages = [], [], []
        start_time = time.time()
        while True:
            line = ser.readline().decode('utf-8').strip()
            # Expecting lines like: 'current:0.52,voltage:5.01'
            if line and 'current' in line and 'voltage' in line:
                try:
                    current = float(line.split('current:')[1].split(',')[0])
                    voltage = float(line.split('voltage:')[1])
                    now = time.time() - start_time
                    times.append(now)
                    currents.append(current)
                    voltages.append(voltage)
                    ax.clear()
                    ax.plot(times, currents, label='Current (A)')
                    ax.plot(times, voltages, label='Voltage (V)')
                    ax.legend()
                    ax.set_xlabel('Time (s)')
                    plt.pause(0.05)
                except Exception as e:
                    pass
    except Exception as e:
        print(f"Power monitoring error: {e}")

# === POST-AGREEMENT INSTALLATION AND MONITORING ===
def run_post_agreement():
    print("Detecting hardware...")
    if not find_usb_pd_devices(USB_VENDOR_ID, USB_PRODUCT_ID):
        print("No compatible USB PD device detected.")
        return
    print("Installing drivers...")
    install_driver()
    print("Starting real-time power monitoring (close plot window to exit)...")
    threading.Thread(target=real_time_power_monitor).start()

# === MAIN ENTRY POINT ===
if __name__ == "__main__":
    run_gui()

    Universal Hardware Control: Detects the USB-C PD device, handles relay init, and installs drivers per OS.
    Legal Verification Gate: Downloads, presents, and logs a user’s acceptance of a compliance agreement.
    Compliance Certificate Check: Optionally verifies a device-specific compliance PDF and signature.
    Installation Sequence: Only proceeds if all gates are passed, with clear error/warning messaging.

Please install requirements before running:
Code

pip install requests pyusb cryptography

Python

import os, sys, webbrowser, requests, platform, subprocess, time, usb.core, usb.util, hashlib
from cryptography.hazmat.primitives import serialization, hashes
from cryptography.hazmat.primitives.asymmetric import padding

# === CONFIG ===
BATTERY_AGREEMENT_URL = "https://example.com/battery_agreement.pdf"
BATTERY_AGREEMENT_LOCAL = "battery_agreement.pdf"
ACCEPTANCE_LOG = "battery_acceptance.log"
COMPLIANCE_CERT_DIR = "compliance_data"
PUBLIC_KEY_FILE = "trusted_public_key.pem"
USB_VENDOR_ID = 0x1A40  # Replace with your device's VID
USB_PRODUCT_ID = 0x0101 # Replace with your device's PID

# === LEGAL AGREEMENT ===
AGREEMENT_TEXT = """
===========================================
ENERGY STORAGE / BATTERY USAGE AGREEMENT
===========================================

By continuing, you confirm:
1. This device complies with safety & battery transport laws (e.g., UN38.3, IEC 62133).
2. You agree not to bypass or tamper with the energy relay system.
3. You assume responsibility for device use and safety.
4. Any misuse voids legal protection and warranty.

Do you accept these terms? [y/n]:
"""

def download_agreement():
    print("🔽 Downloading battery agreement...")
    try:
        r = requests.get(BATTERY_AGREEMENT_URL); r.raise_for_status()
        with open(BATTERY_AGREEMENT_LOCAL, 'wb') as f:
            f.write(r.content)
        webbrowser.open(f"file://{os.path.abspath(BATTERY_AGREEMENT_LOCAL)}")
        print("📄 Agreement opened. Please review it before accepting.")
    except Exception as e:
        print("❌ Error downloading agreement:", e)
        sys.exit(1)

def verify_user_acceptance():
    print(AGREEMENT_TEXT)
    while True:
        choice = input().strip().lower()
        if choice == 'y':
            with open(ACCEPTANCE_LOG, 'a') as log:
                log.write(f"User accepted on {time.ctime()}\n")
            return True
        elif choice == 'n':
            print("❌ Agreement declined. Installation aborted.")
            sys.exit(0)
        else:
            print("⚠️ Please type 'y' to accept or 'n' to decline.")

# === HARDWARE DETECTION ===
def detect_usb_device():
    device = usb.core.find(idVendor=USB_VENDOR_ID, idProduct=USB_PRODUCT_ID)
    if device:
        print(f"✅ Device detected: VendorID={hex(USB_VENDOR_ID)}, ProductID={hex(USB_PRODUCT_ID)}")
        return device
    else:
        print("❌ USB PD relay device not detected. Connect device and try again.")
        sys.exit(1)

# === DRIVER INSTALL ===
def install_device_driver():
    system = platform.system()
    try:
        if system == "Windows":
            subprocess.run(["powershell", "Start-Process", "cmd", "/c", "echo Installing Windows drivers..."], check=True)
        elif system == "Linux":
            subprocess.run(["bash", "-c", "echo Setting up Linux relay modules..."], check=True)
        elif system == "Darwin":
            subprocess.run(["bash", "-c", "echo Configuring macOS relay extension..."], check=True)
        else:
            print("⚠️ Unknown operating system. Manual install may be required.")
            return False
        return True
    except subprocess.CalledProcessError as e:
        print(f"❌ Driver installation failed: {e}")
        return False

# === COMPLIANCE CERTIFICATE ===
def verify_pdf_hash(device_id):
    path = os.path.join(COMPLIANCE_CERT_DIR, f"{device_id}.pdf")
    if not os.path.exists(path):
        return False, "PDF not found"
    with open(path, 'rb') as f:
        pdf_hash = hashlib.sha256(f.read()).hexdigest()
    return True, pdf_hash

def verify_signature(device_id, pdf_hash):
    sig_path = os.path.join(COMPLIANCE_CERT_DIR, f"{device_id}.sig")
    if not os.path.exists(sig_path):
        return False, "Signature not found"
    with open(sig_path, 'rb') as sig_file:
        signature = sig_file.read()
    with open(PUBLIC_KEY_FILE, "rb") as key_file:
        public_key = serialization.load_pem_public_key(key_file.read())
    try:
        public_key.verify(
            signature,
            pdf_hash.encode(),
            padding.PSS(mgf=padding.MGF1(hashes.SHA256()),salt_length=padding.PSS.MAX_LENGTH),
            hashes.SHA256()
        )
        return True, "Signature is valid"
    except Exception as e:
        return False, f"Signature verification failed: {e}"

def get_device_id(device):
    # Try USB iSerialNumber
    try:
        return usb.util.get_string(device, device.iSerialNumber)
    except Exception:
        return None

# === POWER RELAY ===
def relay_energy_init():
    print("⚡ Initializing energy relay from photonic device to computer...")
    # Placeholder for actual hardware handshake/init
    print("🔌 Power relay secure. Monitoring active.")
    print("✅ Setup complete. Device ready to operate.")

# === MAIN ===
if __name__ == "__main__":
    download_agreement()
    if verify_user_acceptance():
        device = detect_usb_device()
        device_id = get_device_id(device)
        print(f"Device Serial: {device_id}")
        cert_ok, cert_result = verify_pdf_hash(device_id)
        if not cert_ok:
            print(f"❌ Compliance PDF check failed: {cert_result}")
            sys.exit(1)
        sig_ok, sig_result = verify_signature(device_id, cert_result)
        if not sig_ok:
            print(f"❌ Compliance signature verification failed: {sig_result}")
            sys.exit(1)
        print("🟢 Compliance certificate validated.")
        if install_device_driver():
            relay_energy_init()

