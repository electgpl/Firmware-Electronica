#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Electgpl - Calculadora de Resistencia Térmica PCB - 4 Capas
Desarrollado para cálculo de disipación térmica en PCBs multicapa
Versión 1.0 - 2025
"""

import tkinter as tk
from tkinter import ttk, messagebox
import math
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np

class ThermalCalculatorGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Electgpl - Calculadora Térmica PCB v1.0")
        self.root.geometry("1000x700")
        
        # Variables de tkinter
        self.setup_variables()
        
        # Crear interfaz
        self.create_widgets()
        
        # Valores por defecto
        self.set_default_values()
        
    def setup_variables(self):
        # Dimensiones PCB
        self.pcb_x = tk.DoubleVar()
        self.pcb_y = tk.DoubleVar()
        self.thickness = tk.DoubleVar()
        
        # Cobre
        self.cu_thickness = tk.DoubleVar()
        self.cu_oz = tk.StringVar()
        
        # Vias
        self.vias_ep = tk.IntVar()  # Vias en el EP (Enhanced Pad)
        self.vias_stitching = tk.IntVar()  # Vias de stitching
        self.via_drill = tk.DoubleVar()
        self.via_plating = tk.DoubleVar()
        
        # Térmicos
        self.r_case_pad = tk.DoubleVar()
        self.r_spread = tk.DoubleVar()
        self.h_conv = tk.DoubleVar()
        self.t_ambient = tk.DoubleVar()
        self.tj_max = tk.DoubleVar()
        
        # Configuración
        self.layers = tk.StringVar()
        self.airflow = tk.StringVar()
        
    def create_widgets(self):
        # Notebook para pestañas
        notebook = ttk.Notebook(self.root)
        notebook.pack(fill='both', expand=True, padx=10, pady=10)
        
        # Pestaña de parámetros
        params_frame = ttk.Frame(notebook)
        notebook.add(params_frame, text="Parámetros PCB")
        
        # Pestaña de resultados
        results_frame = ttk.Frame(notebook)
        notebook.add(results_frame, text="Resultados y Gráficos")
        
        self.create_params_widgets(params_frame)
        self.create_results_widgets(results_frame)
        
    def create_params_widgets(self, parent):
        # Frame principal con scroll
        canvas = tk.Canvas(parent)
        scrollbar = ttk.Scrollbar(parent, orient="vertical", command=canvas.yview)
        scrollable_frame = ttk.Frame(canvas)
        
        scrollable_frame.bind(
            "<Configure>",
            lambda e: canvas.configure(scrollregion=canvas.bbox("all"))
        )
        
        canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)
        
        # Sección: Dimensiones PCB
        pcb_frame = ttk.LabelFrame(scrollable_frame, text="Dimensiones PCB", padding=10)
        pcb_frame.pack(fill='x', padx=5, pady=5)
        
        ttk.Label(pcb_frame, text="Largo (mm):").grid(row=0, column=0, sticky='w', padx=5)
        ttk.Entry(pcb_frame, textvariable=self.pcb_x, width=15).grid(row=0, column=1, padx=5)
        
        ttk.Label(pcb_frame, text="Ancho (mm):").grid(row=0, column=2, sticky='w', padx=5)
        ttk.Entry(pcb_frame, textvariable=self.pcb_y, width=15).grid(row=0, column=3, padx=5)
        
        ttk.Label(pcb_frame, text="Espesor (mm):").grid(row=1, column=0, sticky='w', padx=5)
        ttk.Entry(pcb_frame, textvariable=self.thickness, width=15).grid(row=1, column=1, padx=5)
        
        ttk.Label(pcb_frame, text="Capas:").grid(row=1, column=2, sticky='w', padx=5)
        layers_combo = ttk.Combobox(pcb_frame, textvariable=self.layers, width=12)
        layers_combo['values'] = ('2 Capas', '4 Capas')
        layers_combo.grid(row=1, column=3, padx=5)
        
        # Sección: Cobre
        cu_frame = ttk.LabelFrame(scrollable_frame, text="Especificaciones de Cobre", padding=10)
        cu_frame.pack(fill='x', padx=5, pady=5)
        
        ttk.Label(cu_frame, text="Onzas Cu:").grid(row=0, column=0, sticky='w', padx=5)
        cu_combo = ttk.Combobox(cu_frame, textvariable=self.cu_oz, width=12)
        cu_combo['values'] = ('0.5 oz', '1 oz', '2 oz', '3 oz')
        cu_combo.grid(row=0, column=1, padx=5)
        cu_combo.bind('<<ComboboxSelected>>', self.update_cu_thickness)
        
        ttk.Label(cu_frame, text="Espesor Cu (μm):").grid(row=0, column=2, sticky='w', padx=5)
        cu_entry = ttk.Entry(cu_frame, textvariable=self.cu_thickness, width=15)
        cu_entry.grid(row=0, column=3, padx=5)
        
        # Sección: Vias
        via_frame = ttk.LabelFrame(scrollable_frame, text="Configuración de Vias", padding=10)
        via_frame.pack(fill='x', padx=5, pady=5)
        
        ttk.Label(via_frame, text="Vias en EP:").grid(row=0, column=0, sticky='w', padx=5)
        ttk.Entry(via_frame, textvariable=self.vias_ep, width=15).grid(row=0, column=1, padx=5)
        
        ttk.Label(via_frame, text="Vias Stitching:").grid(row=0, column=2, sticky='w', padx=5)
        ttk.Entry(via_frame, textvariable=self.vias_stitching, width=15).grid(row=0, column=3, padx=5)
        
        ttk.Label(via_frame, text="Taladro (mm):").grid(row=1, column=0, sticky='w', padx=5)
        ttk.Entry(via_frame, textvariable=self.via_drill, width=15).grid(row=1, column=1, padx=5)
        
        ttk.Label(via_frame, text="Plating (μm):").grid(row=1, column=2, sticky='w', padx=5)
        ttk.Entry(via_frame, textvariable=self.via_plating, width=15).grid(row=1, column=3, padx=5)
        
        # Sección: Parámetros Térmicos
        thermal_frame = ttk.LabelFrame(scrollable_frame, text="Parámetros Térmicos", padding=10)
        thermal_frame.pack(fill='x', padx=5, pady=5)
        
        ttk.Label(thermal_frame, text="R Case->Pad (°C/W):").grid(row=0, column=0, sticky='w', padx=5)
        ttk.Entry(thermal_frame, textvariable=self.r_case_pad, width=15).grid(row=0, column=1, padx=5)
        
        ttk.Label(thermal_frame, text="R Spreading (°C/W):").grid(row=0, column=2, sticky='w', padx=5)
        ttk.Entry(thermal_frame, textvariable=self.r_spread, width=15).grid(row=0, column=3, padx=5)
        
        ttk.Label(thermal_frame, text="Conv. coef. h (W/m²K):").grid(row=1, column=0, sticky='w', padx=5)
        ttk.Entry(thermal_frame, textvariable=self.h_conv, width=15).grid(row=1, column=1, padx=5)
        
        ttk.Label(thermal_frame, text="Flujo aire:").grid(row=1, column=2, sticky='w', padx=5)
        air_combo = ttk.Combobox(thermal_frame, textvariable=self.airflow, width=12)
        air_combo['values'] = ('Natural', 'Forzado 1m/s', 'Forzado 2m/s')
        air_combo.grid(row=1, column=3, padx=5)
        air_combo.bind('<<ComboboxSelected>>', self.update_h_conv)
        
        # Sección: Temperaturas
        temp_frame = ttk.LabelFrame(scrollable_frame, text="Temperaturas", padding=10)
        temp_frame.pack(fill='x', padx=5, pady=5)
        
        ttk.Label(temp_frame, text="T Ambiente (°C):").grid(row=0, column=0, sticky='w', padx=5)
        ttk.Entry(temp_frame, textvariable=self.t_ambient, width=15).grid(row=0, column=1, padx=5)
        
        ttk.Label(temp_frame, text="Tj Máx (°C):").grid(row=0, column=2, sticky='w', padx=5)
        ttk.Entry(temp_frame, textvariable=self.tj_max, width=15).grid(row=0, column=3, padx=5)
        
        # Botones
        button_frame = ttk.Frame(scrollable_frame)
        button_frame.pack(fill='x', padx=5, pady=20)
        
        ttk.Button(button_frame, text="Calcular", command=self.calculate, 
                  style='Accent.TButton').pack(side='left', padx=10)
        ttk.Button(button_frame, text="Valores por Defecto", 
                  command=self.set_default_values).pack(side='left', padx=10)
        ttk.Button(button_frame, text="Exportar Resultado", 
                  command=self.export_results).pack(side='left', padx=10)
        
        canvas.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")
        
    def create_results_widgets(self, parent):
        # Frame para resultados
        self.results_text = tk.Text(parent, height=15, width=50, font=('Consolas', 10))
        self.results_text.pack(side='left', fill='both', expand=False, padx=10, pady=10)
        
        # Frame para gráficos
        self.fig, (self.ax1, self.ax2) = plt.subplots(2, 1, figsize=(8, 10))
        self.canvas = FigureCanvasTkAgg(self.fig, parent)
        self.canvas.get_tk_widget().pack(side='right', fill='both', expand=True, padx=10, pady=10)
        
    def set_default_values(self):
        """Establece valores por defecto basados en tu diseño actual"""
        self.pcb_x.set(42.0)
        self.pcb_y.set(20.0)
        self.thickness.set(1.6)
        self.layers.set('4 Capas')
        
        self.cu_oz.set('1 oz')
        self.cu_thickness.set(35)
        
        self.vias_ep.set(45)
        self.vias_stitching.set(20)
        self.via_drill.set(0.30)
        self.via_plating.set(25)
        
        self.r_case_pad.set(2.0)
        self.r_spread.set(8.0)
        self.airflow.set('Natural')
        self.h_conv.set(12.0)
        
        self.t_ambient.set(25.0)
        self.tj_max.set(125.0)
        
    def update_cu_thickness(self, event=None):
        """Actualiza el espesor de cobre según las onzas seleccionadas"""
        oz_to_um = {'0.5 oz': 17.5, '1 oz': 35, '2 oz': 70, '3 oz': 105}
        selected = self.cu_oz.get()
        if selected in oz_to_um:
            self.cu_thickness.set(oz_to_um[selected])
            
    def update_h_conv(self, event=None):
        """Actualiza el coeficiente de convección según el flujo de aire"""
        h_values = {'Natural': 12.0, 'Forzado 1m/s': 25.0, 'Forzado 2m/s': 40.0}
        selected = self.airflow.get()
        if selected in h_values:
            self.h_conv.set(h_values[selected])
    
    def calculate(self):
        """Realiza el cálculo térmico principal"""
        try:
            # Obtener valores
            pcb_x_mm = self.pcb_x.get()
            pcb_y_mm = self.pcb_y.get()
            thickness_mm = self.thickness.get()
            num_layers = 4 if self.layers.get() == '4 Capas' else 2
            
            cu_thick_um = self.cu_thickness.get()
            vias_ep = self.vias_ep.get()
            vias_stitch = self.vias_stitching.get()
            via_drill_mm = self.via_drill.get()
            via_plating_um = self.via_plating.get()
            
            r_case_pad = self.r_case_pad.get()
            r_spread = self.r_spread.get()
            h_conv = self.h_conv.get()
            t_amb = self.t_ambient.get()
            tj_max = self.tj_max.get()
            
            # Conversiones a metros
            pcb_area_m2 = (pcb_x_mm / 1000) * (pcb_y_mm / 1000)
            thickness_m = thickness_mm / 1000
            cu_thick_m = cu_thick_um / 1e6
            via_drill_m = via_drill_mm / 1000
            via_plating_m = via_plating_um / 1e6
            
            # Cálculos
            results = self.thermal_calculations(
                pcb_area_m2, thickness_m, cu_thick_m, num_layers,
                vias_ep, vias_stitch, via_drill_m, via_plating_m,
                r_case_pad, r_spread, h_conv, t_amb, tj_max
            )
            
            # Mostrar resultados
            self.display_results(results, pcb_x_mm, pcb_y_mm, thickness_mm, num_layers)
            
            # Generar gráficos
            self.plot_results(results, t_amb, tj_max)
            
        except Exception as e:
            messagebox.showerror("Error", f"Error en el cálculo: {str(e)}")
    
    def thermal_calculations(self, area_m2, thickness_m, cu_thick_m, num_layers,
                           vias_ep, vias_stitch, via_drill_m, via_plating_m,
                           r_case_pad, r_spread, h_conv, t_amb, tj_max):
        """Realiza los cálculos térmicos detallados"""
        
        # Constantes
        k_cu = 400.0  # W/mK - Conductividad térmica del cobre
        
        # Resistencia de vias
        def r_via_single(L_m, D_m, t_p_m, k=k_cu):
            return L_m / (k * math.pi * D_m * t_p_m)
        
        def r_via_parallel(N, L_m, D_m, t_p_m, k=k_cu):
            if N == 0:
                return float('inf')
            r_single = r_via_single(L_m, D_m, t_p_m, k)
            return r_single / N
        
        # Cálculo de resistencias
        R_vias_ep = r_via_parallel(vias_ep, thickness_m, via_drill_m, via_plating_m)
        R_vias_stitch = r_via_parallel(vias_stitch, thickness_m, via_drill_m, via_plating_m)
        
        # Resistencia de convección (mejorada para múltiples capas)
        # Para 4 capas, asumimos que inner1 e inner2 contribuyen a la disipación
        if num_layers == 4:
            # Factor de mejora por capas internas (estimación empírica)
            layer_factor = 1.5  # Las capas internas mejoran la disipación
            effective_area = area_m2 * 2 * layer_factor  # Top + Bottom + contribución inner
        else:
            effective_area = area_m2 * 2  # Solo top + bottom
        
        R_conv = 1.0 / (h_conv * effective_area)
        
        # Resistencia total del PCB
        # En paralelo: vias EP y stitching, en serie con spreading y convección
        R_vias_total = 1 / (1/R_vias_ep + 1/R_vias_stitch) if R_vias_stitch != float('inf') else R_vias_ep
        
        R_theta_pcb = r_case_pad + R_vias_total + r_spread + R_conv
        
        # Cálculo de potencia máxima
        delta_T_max = tj_max - t_amb
        P_max = delta_T_max / R_theta_pcb
        
        # Preparar resultados
        results = {
            'R_case_pad': r_case_pad,
            'R_vias_ep': R_vias_ep,
            'R_vias_stitch': R_vias_stitch,
            'R_vias_total': R_vias_total,
            'R_spread': r_spread,
            'R_conv': R_conv,
            'R_theta_pcb': R_theta_pcb,
            'P_max': P_max,
            'effective_area': effective_area,
            'layer_factor': layer_factor if num_layers == 4 else 1.0
        }
        
        return results
    
    def display_results(self, results, pcb_x, pcb_y, thickness, num_layers):
        """Muestra los resultados en el área de texto"""
        self.results_text.delete(1.0, tk.END)
        
        output = f"""
═══════════════════════════════════════════
    ELECTGPL - CALCULADORA TÉRMICA PCB v1.0
═══════════════════════════════════════════

CONFIGURACIÓN PCB:
─────────────────────────────────────────
• Dimensiones: {pcb_x:.1f} × {pcb_y:.1f} mm
• Espesor: {thickness:.2f} mm
• Configuración: {num_layers} capas
• Área efectiva: {results['effective_area']*1e6:.1f} mm²

RESISTENCIAS TÉRMICAS:
─────────────────────────────────────────
• R Case → Pad:      {results['R_case_pad']:.3f} °C/W
• R Vias EP:         {results['R_vias_ep']:.3f} °C/W
• R Vias Stitching:  {results['R_vias_stitch']:.3f} °C/W
• R Vias Total:      {results['R_vias_total']:.3f} °C/W
• R Spreading:       {results['R_spread']:.3f} °C/W
• R Convección:      {results['R_conv']:.3f} °C/W

RESULTADO FINAL:
─────────────────────────────────────────
• R_θ PCB Total:     {results['R_theta_pcb']:.3f} °C/W
• Potencia Máxima:   {results['P_max']:.1f} W

RECOMENDACIONES DE CORRIENTE POR TENSIÓN:
─────────────────────────────────────────
"""
        
        # Calcular corrientes recomendadas para diferentes tensiones
        voltages = [3.3, 5.0, 9.0, 12.0, 15.0, 24.0]
        for v in voltages:
            i_max = results['P_max'] / v
            output += f"• {v:4.1f}V → {i_max:.2f}A máx\n"
        
        output += f"""
═══════════════════════════════════════════
Electgpl - Diseño Electrónico Avanzado
www.electgpl.com - Calculadora Térmica v1.0
═══════════════════════════════════════════
"""
        
        self.results_text.insert(1.0, output)
    
    def plot_results(self, results, t_amb, tj_max):
        """Genera gráficos de temperatura vs potencia"""
        self.ax1.clear()
        self.ax2.clear()
        
        # Gráfico 1: Temperatura de Junction vs Potencia
        powers = np.linspace(0, results['P_max'] * 1.2, 100)
        temps = t_amb + powers * results['R_theta_pcb']
        
        self.ax1.plot(powers, temps, 'b-', linewidth=2, label='Tj calculada')
        self.ax1.axhline(y=tj_max, color='r', linestyle='--', linewidth=2, label=f'Tj máx ({tj_max}°C)')
        self.ax1.axvline(x=results['P_max'], color='g', linestyle='--', linewidth=2, 
                        label=f'P máx ({results["P_max"]:.1f}W)')
        self.ax1.fill_between(powers, temps, tj_max, where=(temps <= tj_max), 
                             color='lightgreen', alpha=0.3, label='Zona segura')
        self.ax1.fill_between(powers, temps, tj_max, where=(temps > tj_max), 
                             color='lightcoral', alpha=0.3, label='Zona peligrosa')
        
        self.ax1.set_xlabel('Potencia (W)')
        self.ax1.set_ylabel('Temperatura Junction (°C)')
        self.ax1.set_title('Temperatura de Junction vs Potencia')
        self.ax1.grid(True, alpha=0.3)
        self.ax1.legend()
        
        # Gráfico 2: Corriente máxima por tensión
        voltages = np.array([3.3, 5.0, 9.0, 12.0, 15.0, 24.0, 48.0])
        currents = results['P_max'] / voltages
        
        self.ax2.bar(voltages, currents, color='steelblue', alpha=0.7, width=1.0)
        self.ax2.set_xlabel('Tensión de Salida (V)')
        self.ax2.set_ylabel('Corriente Máxima (A)')
        self.ax2.set_title('Corriente Máxima por Tensión de Salida')
        self.ax2.grid(True, alpha=0.3)
        
        # Añadir valores en las barras
        for v, i in zip(voltages, currents):
            if i > 0.1:  # Solo mostrar si es significativo
                self.ax2.text(v, i + 0.1, f'{i:.2f}A', ha='center', va='bottom', fontsize=9)
        
        self.fig.tight_layout()
        self.canvas.draw()
    
    def export_results(self):
        """Exporta los resultados a un archivo de texto"""
        try:
            content = self.results_text.get(1.0, tk.END)
            filename = f"electgpl_resultado_termico_pcb_{self.pcb_x.get():.0f}x{self.pcb_y.get():.0f}mm.txt"
            
            with open(filename, 'w', encoding='utf-8') as f:
                f.write(content)
            
            messagebox.showinfo("Exportación", f"Resultados guardados en: {filename}")
            
        except Exception as e:
            messagebox.showerror("Error", f"Error al exportar: {str(e)}")

def main():
    root = tk.Tk()
    
    # Configurar estilo
    style = ttk.Style()
    style.theme_use('clam')
    
    app = ThermalCalculatorGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()