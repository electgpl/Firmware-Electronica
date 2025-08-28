#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
GUI Tkinter para calculadora térmica PCB
Basado en tu clase ThermalCalculator de θJA_PCB_AP64501_Calc.py
"""

import tkinter as tk
from tkinter import ttk, messagebox
import runpy
import matplotlib
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt

# Importar tu clase desde el script original
module = runpy.run_path('θJA_PCB_AP64501_Calc.py')
ThermalCalculator = module['ThermalCalculator']
calc = ThermalCalculator()

def calcular():
    try:
        # Lectura de parámetros
        L = float(entry_L.get())
        W = float(entry_W.get())
        oz = float(combo_oz.get())
        layer_config = {
            'top': combo_top.get(),
            'inner1': combo_inner1.get(),
            'inner2': combo_inner2.get(),
            'bottom': combo_bottom.get()
        }
        epv = int(entry_epv.get())
        epa = float(entry_epa.get())
        st = int(entry_stitch.get())
        vin = float(entry_vin.get())
        vout = float(entry_vout.get())
        iout = float(entry_iout.get())
        Ta = float(entry_ta.get())

        # Cálculo térmico
        thermal_params = calc.calculate_theta_ja(L, W, oz, layer_config, epv, epa, st)
        analysis = calc.thermal_analysis(thermal_params, vin, vout, iout, Ta)

        # Mostrar resultados
        texto = []
        texto.append(f"θJC (datasheet): {thermal_params['theta_jc']:.2f} °C/W")
        texto.append(f"θCA básico: {thermal_params['theta_ca_basic']:.3f} °C/W")
        texto.append(f"θCA con vías: {thermal_params['theta_ca']:.3f} °C/W")
        texto.append(f"θJA calculado: {thermal_params['theta_ja_calculated']:.3f} °C/W")
        texto.append(f"θJA diseño: {thermal_params['theta_ja_design']:.3f} °C/W")
        texto.append(f"Pérdidas IC: {analysis['power_data']['ic_losses']:.3f} W")
        texto.append(f"Tj estimada: {analysis['tj']:.2f} °C")
        texto.append(f"Margen térmico: {analysis['thermal_margin']:.2f} °C")
        texto.append(f"P_disipable máx IC: {analysis['max_dissipatable_power']:.2f} W")
        texto.append(f"Pout máx estimada: {analysis['max_output_power']:.1f} W")
        results_text.delete(1.0, tk.END)
        results_text.insert(tk.END, "\n".join(texto))

        # Graficar Tj vs Iout
        currents = [i/10 for i in range(1, 101)]
        tj_list = []
        for i in currents:
            a = calc.thermal_analysis(thermal_params, vin, vout, i, Ta)
            tj_list.append(a['tj'])

        fig.clear()
        ax = fig.add_subplot(111)
        ax.plot(currents, tj_list)
        ax.axhline(125, linestyle='--', color='r', label='Tj max')
        ax.set_xlabel("Iout (A)")
        ax.set_ylabel("Tj (°C)")
        ax.grid(True)
        ax.legend()
        canvas.draw()

    except Exception as e:
        messagebox.showerror("Error", f"Entrada inválida: {e}")

# --- GUI Tkinter ---
root = tk.Tk()
root.title("Calculadora Térmica PCB - Tkinter")

frame_inputs = ttk.Frame(root, padding=5)
frame_inputs.grid(row=0, column=0, sticky="w")

# PCB params
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

# Layers
ttk.Label(frame_inputs, text="top:").grid(row=1, column=0)
combo_top = ttk.Combobox(frame_inputs, values=["signal","gnd","power","mixed"], width=7)
combo_top.set("signal")
combo_top.grid(row=1, column=1)

ttk.Label(frame_inputs, text="inner1:").grid(row=1, column=2)
combo_inner1 = ttk.Combobox(frame_inputs, values=["signal","gnd","mixed"], width=7)
combo_inner1.set("gnd")
combo_inner1.grid(row=1, column=3)

ttk.Label(frame_inputs, text="inner2:").grid(row=1, column=4)
combo_inner2 = ttk.Combobox(frame_inputs, values=["signal","gnd","mixed"], width=7)
combo_inner2.set("gnd")
combo_inner2.grid(row=1, column=5)

ttk.Label(frame_inputs, text="bottom:").grid(row=1, column=6)
combo_bottom = ttk.Combobox(frame_inputs, values=["signal","gnd","mixed"], width=7)
combo_bottom.set("gnd")
combo_bottom.grid(row=1, column=7)

# Vías
ttk.Label(frame_inputs, text="Vías EP:").grid(row=2, column=0)
entry_epv = ttk.Entry(frame_inputs, width=6)
entry_epv.insert(0, "45")
entry_epv.grid(row=2, column=1)

ttk.Label(frame_inputs, text="Área EP (mm²):").grid(row=2, column=2)
entry_epa = ttk.Entry(frame_inputs, width=6)
entry_epa.insert(0, "9")
entry_epa.grid(row=2, column=3)

ttk.Label(frame_inputs, text="Vías stitching:").grid(row=2, column=4)
entry_stitch = ttk.Entry(frame_inputs, width=6)
entry_stitch.insert(0, "50")
entry_stitch.grid(row=2, column=5)

# Condiciones eléctricas
ttk.Label(frame_inputs, text="VIN (V):").grid(row=3, column=0)
entry_vin = ttk.Entry(frame_inputs, width=6)
entry_vin.insert(0, "12")
entry_vin.grid(row=3, column=1)

ttk.Label(frame_inputs, text="VOUT (V):").grid(row=3, column=2)
entry_vout = ttk.Entry(frame_inputs, width=6)
entry_vout.insert(0, "5")
entry_vout.grid(row=3, column=3)

ttk.Label(frame_inputs, text="IOUT (A):").grid(row=3, column=4)
entry_iout = ttk.Entry(frame_inputs, width=6)
entry_iout.insert(0, "3")
entry_iout.grid(row=3, column=5)

ttk.Label(frame_inputs, text="T amb (°C):").grid(row=3, column=6)
entry_ta = ttk.Entry(frame_inputs, width=6)
entry_ta.insert(0, "25")
entry_ta.grid(row=3, column=7)

# Botón calcular
ttk.Button(frame_inputs, text="Calcular", command=calcular).grid(row=4, column=0, pady=5)

# Resultados
results_text = tk.Text(root, height=10, width=80)
results_text.grid(row=1, column=0, padx=5, pady=5)

# Gráfico matplotlib
fig = plt.Figure(figsize=(6,3), dpi=100)
canvas = FigureCanvasTkAgg(fig, master=root)
canvas.get_tk_widget().grid(row=2, column=0)

root.mainloop()
