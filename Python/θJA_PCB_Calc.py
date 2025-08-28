#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
GUI Tkinter para cálculo térmico de PCB (solo PCB)
Usa ThermalCalculator de tu script original, pero muestra solo θPCB
"""

import tkinter as tk
from tkinter import ttk, messagebox
import runpy

# Importar clase desde tu script original
module = runpy.run_path('θJA_PCB_AP64501_Calc.py')
ThermalCalculator = module['ThermalCalculator']
calc = ThermalCalculator()

def calcular():
    try:
        # Entradas
        L = float(entry_L.get())
        W = float(entry_W.get())
        oz = float(combo_oz.get())
        n_layers = int(combo_layers.get())

        if n_layers == 2:
            layer_config = {
                'top': combo_top.get(),
                'inner1': 'signal',  # no usada
                'inner2': 'signal',  # no usada
                'bottom': combo_bottom.get()
            }
        else:  # 4 capas
            layer_config = {
                'top': combo_top.get(),
                'inner1': combo_inner1.get(),
                'inner2': combo_inner2.get(),
                'bottom': combo_bottom.get()
            }

        epv = int(entry_epv.get())
        epa = float(entry_epa.get())
        st = int(entry_stitch.get())

        # Calcular
        thermal_params = calc.calculate_theta_ja(L, W, oz, layer_config, epv, epa, st)

        theta_pcb_calc = thermal_params['theta_ca']              # puro PCB
        theta_pcb_design = theta_pcb_calc * thermal_params['safety_factor']

        # Mostrar
        texto = []
        texto.append(f"θPCB calculado: {theta_pcb_calc:.3f} °C/W")
        texto.append(f"θPCB diseño:    {theta_pcb_design:.3f} °C/W")
        results_text.delete(1.0, tk.END)
        results_text.insert(tk.END, "\n".join(texto))

    except Exception as e:
        messagebox.showerror("Error", f"Entrada inválida: {e}")

# --- GUI ---
root = tk.Tk()
root.title("Cálculo θPCB - Tkinter")

frame_inputs = ttk.Frame(root, padding=5)
frame_inputs.grid(row=0, column=0, sticky="w")

# PCB size y cobre
ttk.Label(frame_inputs, text="Largo PCB (mm):").grid(row=0, column=0)
entry_L = ttk.Entry(frame_inputs, width=6)
entry_L.insert(0, "42")
entry_L.grid(row=0, column=1)

ttk.Label(frame_inputs, text="Ancho PCB (mm):").grid(row=0, column=2)
entry_W = ttk.Entry(frame_inputs, width=6)
entry_W.insert(0, "20")
entry_W.grid(row=0, column=3)

ttk.Label(frame_inputs, text="Cobre (oz):").grid(row=0, column=4)
combo_oz = ttk.Combobox(frame_inputs, values=["0.5", "1", "2"], width=5)
combo_oz.set("1")
combo_oz.grid(row=0, column=5)

# Número de capas
ttk.Label(frame_inputs, text="N° capas:").grid(row=1, column=0)
combo_layers = ttk.Combobox(frame_inputs, values=["2", "4"], width=5)
combo_layers.set("4")
combo_layers.grid(row=1, column=1)

# Configuración de capas
ttk.Label(frame_inputs, text="top:").grid(row=2, column=0)
combo_top = ttk.Combobox(frame_inputs, values=["signal","gnd","power","mixed"], width=7)
combo_top.set("signal")
combo_top.grid(row=2, column=1)

ttk.Label(frame_inputs, text="inner1:").grid(row=2, column=2)
combo_inner1 = ttk.Combobox(frame_inputs, values=["signal","gnd","mixed"], width=7)
combo_inner1.set("gnd")
combo_inner1.grid(row=2, column=3)

ttk.Label(frame_inputs, text="inner2:").grid(row=2, column=4)
combo_inner2 = ttk.Combobox(frame_inputs, values=["signal","gnd","mixed"], width=7)
combo_inner2.set("gnd")
combo_inner2.grid(row=2, column=5)

ttk.Label(frame_inputs, text="bottom:").grid(row=2, column=6)
combo_bottom = ttk.Combobox(frame_inputs, values=["signal","gnd","mixed"], width=7)
combo_bottom.set("gnd")
combo_bottom.grid(row=2, column=7)

# Vías
ttk.Label(frame_inputs, text="Vías EP:").grid(row=3, column=0)
entry_epv = ttk.Entry(frame_inputs, width=6)
entry_epv.insert(0, "45")
entry_epv.grid(row=3, column=1)

ttk.Label(frame_inputs, text="Área EP (mm²):").grid(row=3, column=2)
entry_epa = ttk.Entry(frame_inputs, width=6)
entry_epa.insert(0, "9")
entry_epa.grid(row=3, column=3)

ttk.Label(frame_inputs, text="Vías stitching:").grid(row=3, column=4)
entry_stitch = ttk.Entry(frame_inputs, width=6)
entry_stitch.insert(0, "50")
entry_stitch.grid(row=3, column=5)

# Botón calcular
ttk.Button(frame_inputs, text="Calcular θPCB", command=calcular).grid(row=4, column=0, pady=5)

# Resultados
results_text = tk.Text(root, height=5, width=50)
results_text.grid(row=1, column=0, padx=5, pady=5)

root.mainloop()
