#!/usr/bin/env python3
"""
Calculadora Térmica para PCB con AP64501
Calcula θJA basado en parámetros específicos del PCB y realiza análisis térmico completo
Autor: Electgpl
"""

import math
import numpy as np
import matplotlib.pyplot as plt

class ThermalCalculator:
    def __init__(self):
        # Constantes del AP64501
        self.theta_jc = 5.0  # °C/W - del datasheet
        self.tj_max = 125.0  # °C - temperatura máxima de junción
        self.rds_on_hs = 45e-3  # Ω - High-side MOSFET
        self.rds_on_ls = 20e-3  # Ω - Low-side MOSFET
        self.fsw = 570e3  # Hz - frecuencia de switching
        self.iq = 25e-6  # A - corriente quiescente
        
        # Constantes térmicas
        self.k_fr4 = 120  # Constante empírica para FR-4 con convección natural
        self.via_thermal_resistance = 0.8  # °C/W por vía (estimado)
        
    def calculate_copper_thickness(self, oz):
        """Convierte onzas de cobre a micrones"""
        return oz * 35e-6  # metros
    
    def calculate_effective_layers(self, layer_config):
        """
        Calcula el número efectivo de capas térmicas
        layer_config: dict con keys 'top', 'inner1', 'inner2', 'bottom'
        valores: 'signal', 'gnd', 'power', 'mixed'
        """
        weights = {
            'signal': 0.3,  # Capa de señal contribuye menos
            'gnd': 1.0,     # Plano de GND contribuye completamente
            'power': 0.8,   # Plano de power contribuye bien
            'mixed': 0.5    # Capa mixta contribuye moderadamente
        }
        
        effective_layers = 0
        for layer, type_layer in layer_config.items():
            if layer == 'top':
                effective_layers += 0.5  # Top siempre cuenta menos por componentes
            else:
                effective_layers += weights.get(type_layer, 0.5)
                
        return effective_layers
    
    def calculate_via_improvement_factor(self, ep_vias, ep_area_mm2, stitching_vias, total_area_mm2):
        """
        Calcula el factor de mejora por vías térmicas
        """
        # Densidad de vías en EP (más importante)
        ep_density = ep_vias / ep_area_mm2 if ep_area_mm2 > 0 else 0
        
        # Densidad de vías stitching en el resto del PCB
        remaining_area = max(total_area_mm2 - ep_area_mm2, 1)
        stitching_density = stitching_vias / remaining_area
        
        # Factor de mejora basado en densidades
        # EP vías tienen más impacto (peso 0.7) que stitching (peso 0.3)
        improvement_ep = min(ep_density * 0.1, 0.3)  # Máximo 30% mejora por EP
        improvement_stitching = min(stitching_density * 0.05, 0.1)  # Máximo 10% por stitching
        
        total_improvement = improvement_ep + improvement_stitching
        
        # Factor multiplicador (1.0 = sin mejora, menor es mejor)
        factor = 1.0 - min(total_improvement, 0.35)  # Máximo 35% mejora total
        
        return factor, improvement_ep * 100, improvement_stitching * 100
    
    def calculate_theta_ca(self, pcb_length_mm, pcb_width_mm, copper_oz, layer_config, 
                          ep_vias, ep_area_mm2, stitching_vias):
        """
        Calcula la resistencia térmica Case-to-Ambient del PCB
        """
        # Área efectiva en cm²
        area_cm2 = (pcb_length_mm * pcb_width_mm) / 100
        
        # Número efectivo de capas
        n_layers = self.calculate_effective_layers(layer_config)
        
        # Espesor de cobre en metros
        t_cu = self.calculate_copper_thickness(copper_oz)
        
        # Cálculo básico de θCA
        denominator = area_cm2 * n_layers * math.sqrt(t_cu * 1e6)  # convertir a micrones para sqrt
        theta_ca_basic = self.k_fr4 / denominator
        
        # Factor de mejora por vías
        total_area_mm2 = pcb_length_mm * pcb_width_mm
        via_factor, ep_improvement, stitching_improvement = self.calculate_via_improvement_factor(
            ep_vias, ep_area_mm2, stitching_vias, total_area_mm2)
        
        theta_ca = theta_ca_basic * via_factor
        
        return theta_ca, theta_ca_basic, via_factor, ep_improvement, stitching_improvement, n_layers
    
    def calculate_theta_ja(self, pcb_length_mm, pcb_width_mm, copper_oz, layer_config, 
                          ep_vias, ep_area_mm2, stitching_vias, safety_factor=1.2):
        """
        Calcula el θJA total con factor de seguridad
        """
        theta_ca, theta_ca_basic, via_factor, ep_imp, stitch_imp, n_layers = self.calculate_theta_ca(
            pcb_length_mm, pcb_width_mm, copper_oz, layer_config, 
            ep_vias, ep_area_mm2, stitching_vias)
        
        # θJA total sin factor de seguridad
        theta_ja_calculated = self.theta_jc + theta_ca
        
        # θJA con factor de seguridad para diseño
        theta_ja_design = theta_ja_calculated * safety_factor
        
        return {
            'theta_ja_calculated': theta_ja_calculated,
            'theta_ja_design': theta_ja_design,
            'theta_jc': self.theta_jc,
            'theta_ca': theta_ca,
            'theta_ca_basic': theta_ca_basic,
            'via_factor': via_factor,
            'ep_improvement': ep_imp,
            'stitching_improvement': stitch_imp,
            'effective_layers': n_layers,
            'safety_factor': safety_factor
        }
    
    def calculate_power_losses(self, vin, vout, iout):
        """
        Calcula las pérdidas de potencia en el IC
        """
        duty_cycle = vout / vin
        
        # Estimación de eficiencia basada en curvas típicas
        if iout <= 0.1:
            efficiency = 0.75  # Baja eficiencia en light load
        elif iout <= 1.0:
            efficiency = 0.85
        elif iout <= 3.0:
            efficiency = 0.88
        elif iout <= 5.0:
            efficiency = 0.87
        else:
            efficiency = 0.85  # Eficiencia baja en overcurrent
        
        pout = vout * iout
        pin = pout / efficiency
        
        # Pérdidas en MOSFETs
        irms_hs = iout * math.sqrt(duty_cycle)
        irms_ls = iout * math.sqrt(1 - duty_cycle)
        
        p_hs = irms_hs**2 * self.rds_on_hs
        p_ls = irms_ls**2 * self.rds_on_ls
        
        # Pérdidas por switching (estimación)
        p_switching = 0.02 * pout + 0.05  # Aproximación empírica
        
        # Pérdidas de control
        p_control = vin * self.iq
        
        # Pérdidas gate drive (estimación)
        p_gate_drive = 0.08  # Valor típico
        
        total_ic_losses = p_hs + p_ls + p_switching + p_control + p_gate_drive
        
        return {
            'efficiency': efficiency,
            'pin': pin,
            'pout': pout,
            'total_losses': pin - pout,
            'ic_losses': total_ic_losses,
            'p_hs': p_hs,
            'p_ls': p_ls,
            'p_switching': p_switching,
            'p_control': p_control,
            'p_gate_drive': p_gate_drive,
            'duty_cycle': duty_cycle
        }
    
    def thermal_analysis(self, thermal_params, vin, vout, iout, t_ambient):
        """
        Análisis térmico completo
        """
        power_data = self.calculate_power_losses(vin, vout, iout)
        
        theta_ja = thermal_params['theta_ja_design']
        ic_losses = power_data['ic_losses']
        
        # Temperatura de junción
        delta_t = ic_losses * theta_ja
        tj = t_ambient + delta_t
        
        # Margen térmico
        thermal_margin = self.tj_max - tj
        
        # Potencia máxima disipable
        max_dissipatable_power = (self.tj_max - t_ambient) / theta_ja
        
        # Estimación de potencia de salida máxima
        # Asumiendo pérdidas = 15% de la potencia de salida en promedio
        max_output_power = max_dissipatable_power / 0.15
        
        return {
            'tj': tj,
            'delta_t': delta_t,
            'thermal_margin': thermal_margin,
            'max_dissipatable_power': max_dissipatable_power,
            'max_output_power': max_output_power,
            'is_safe': thermal_margin > 0,
            'power_data': power_data
        }
    
    def generate_power_derating_table(self, thermal_params, vin_list, vout_list, max_current=6):
        """
        Genera tabla de derating de potencia para diferentes condiciones
        """
        results = []
        
        for vin in vin_list:
            for vout in vout_list:
                if vout >= vin:
                    continue
                
                # Encontrar corriente máxima para diferentes temperaturas
                for t_amb in [25, 50, 75, 85]:
                    max_power_25c = self.thermal_analysis(thermal_params, vin, vout, 1, 25)['max_output_power']
                    max_power_tamb = self.thermal_analysis(thermal_params, vin, vout, 1, t_amb)['max_output_power']
                    
                    max_current_25c = min(max_power_25c / vout, max_current)
                    max_current_tamb = min(max_power_tamb / vout, max_current)
                    
                    results.append({
                        'vin': vin,
                        'vout': vout,
                        't_ambient': t_amb,
                        'max_power': max_power_tamb,
                        'max_current': max_current_tamb
                    })
        
        return results
    
    def plot_thermal_analysis(self, thermal_params, vin, vout, t_ambient_list):
        """
        Genera gráficos de análisis térmico
        """
        currents = np.linspace(0.1, 6, 50)
        
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('Electgpl - Analisis RθPCB para DCDC Multilayer', fontsize=16, fontweight='bold')
        
        for t_amb in t_ambient_list:
            tj_list = []
            efficiency_list = []
            power_loss_list = []
            thermal_margin_list = []
            
            for iout in currents:
                analysis = self.thermal_analysis(thermal_params, vin, vout, iout, t_amb)
                tj_list.append(analysis['tj'])
                efficiency_list.append(analysis['power_data']['efficiency'] * 100)
                power_loss_list.append(analysis['power_data']['ic_losses'])
                thermal_margin_list.append(analysis['thermal_margin'])
            
            label = f'T_amb = {t_amb}°C'
            ax1.plot(currents, tj_list, label=label)
            ax2.plot(currents, efficiency_list, label=label)
            ax3.plot(currents, power_loss_list, label=label)
            ax4.plot(currents, thermal_margin_list, label=label)
        
        # Configuración de gráficos
        ax1.axhline(y=125, color='r', linestyle='--', label='Tj max')
        ax1.set_xlabel('Corriente de Salida (A)')
        ax1.set_ylabel('Temperatura de Junción (°C)')
        ax1.set_title('Temperatura de Junción vs Corriente')
        ax1.legend()
        ax1.grid(True)
        
        ax2.set_xlabel('Corriente de Salida (A)')
        ax2.set_ylabel('Eficiencia (%)')
        ax2.set_title('Eficiencia vs Corriente')
        ax2.legend()
        ax2.grid(True)
        
        ax3.set_xlabel('Corriente de Salida (A)')
        ax3.set_ylabel('Pérdidas IC (W)')
        ax3.set_title('Pérdidas en IC vs Corriente')
        ax3.legend()
        ax3.grid(True)
        
        ax4.axhline(y=0, color='r', linestyle='--', label='Límite térmico')
        ax4.set_xlabel('Corriente de Salida (A)')
        ax4.set_ylabel('Margen Térmico (°C)')
        ax4.set_title('Margen Térmico vs Corriente')
        ax4.legend()
        ax4.grid(True)
        
        plt.tight_layout()
        plt.show()

# Función principal de ejemplo
def main():
    calc = ThermalCalculator()
    
    print("=== CALCULADORA TÉRMICA PCB - AP64501 ===\n")
    
    # Parámetros del PCB (ejemplo: tu diseño)
    pcb_length_mm = 42
    pcb_width_mm = 20
    copper_oz = 1  # Cambiar a 2 para 2oz
    
    # Configuración de capas
    layer_config = {
        'top': 'signal',    # Top layer con componentes
        'inner1': 'gnd',    # Plano GND
        'inner2': 'gnd',    # Plano GND
        'bottom': 'gnd'     # Plano GND
    }
    
    # Parámetros de vías
    ep_vias = 45           # Vías en el área del EP
    ep_area_mm2 = 9        # Área del EP (3mm x 3mm)
    stitching_vias = 50    # Vías de stitching en el resto del PCB
    
    # Calcular θJA
    thermal_params = calc.calculate_theta_ja(
        pcb_length_mm, pcb_width_mm, copper_oz, layer_config,
        ep_vias, ep_area_mm2, stitching_vias
    )
    
    print(f"=== RESULTADOS TÉRMICOS ===")
    print(f"Dimensiones PCB: {pcb_length_mm}mm × {pcb_width_mm}mm = {pcb_length_mm*pcb_width_mm}mm²")
    print(f"Cobre: {copper_oz}oz")
    print(f"Capas efectivas: {thermal_params['effective_layers']:.1f}")
    print(f"Vías EP: {ep_vias} en {ep_area_mm2}mm²")
    print(f"Vías stitching: {stitching_vias}")
    print(f"\nθJC (del IC): {thermal_params['theta_jc']:.1f}°C/W")
    print(f"θCA (básico): {thermal_params['theta_ca_basic']:.1f}°C/W")
    print(f"Factor de vías: {thermal_params['via_factor']:.3f}")
    print(f"  - Mejora EP: {thermal_params['ep_improvement']:.1f}%")
    print(f"  - Mejora stitching: {thermal_params['stitching_improvement']:.1f}%")
    print(f"θCA (con vías): {thermal_params['theta_ca']:.1f}°C/W")
    print(f"θJA calculado: {thermal_params['theta_ja_calculated']:.1f}°C/W")
    print(f"θJA diseño (con seguridad): {thermal_params['theta_ja_design']:.1f}°C/W")
    
    # Análisis para caso específico
    vin, vout, iout = 12, 5, 3
    t_ambient = 25
    
    analysis = calc.thermal_analysis(thermal_params, vin, vout, iout, t_ambient)
    
    print(f"\n=== ANÁLISIS CASO ESPECÍFICO ===")
    print(f"VIN: {vin}V, VOUT: {vout}V, IOUT: {iout}A")
    print(f"Temperatura ambiente: {t_ambient}°C")
    print(f"\nEficiencia: {analysis['power_data']['efficiency']*100:.1f}%")
    print(f"Potencia salida: {analysis['power_data']['pout']:.1f}W")
    print(f"Pérdidas IC: {analysis['power_data']['ic_losses']:.3f}W")
    print(f"Temperatura junción: {analysis['tj']:.1f}°C")
    print(f"Margen térmico: {analysis['thermal_margin']:.1f}°C")
    print(f"¿Es seguro? {'SÍ' if analysis['is_safe'] else 'NO'}")
    
    # Potencias máximas
    print(f"\n=== POTENCIAS MÁXIMAS ===")
    for t_amb in [25, 50, 75, 85]:
        max_analysis = calc.thermal_analysis(thermal_params, 12, 5, 1, t_amb)
        print(f"T_amb {t_amb}°C: {max_analysis['max_output_power']:.1f}W máximo")
    
    # Generar gráficos (opcional)
    print(f"\n¿Generar gráficos? (y/n): ", end="")
    if input().lower() == 'y':
        calc.plot_thermal_analysis(thermal_params, vin, vout, [25, 50, 85])

if __name__ == "__main__":
    main()