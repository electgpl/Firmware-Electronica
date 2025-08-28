# Electgpl - 4 Layer Thermal Calc
import math

# Parametros (ajustables)
pcb_x_mm = 42.0
pcb_y_mm = 20.0
thickness_total_m = 1.6e-3   # Espesor total del PCB
t_cu_outer_m = 35e-6         # 1 oz capas externas (top/bottom)
t_cu_inner_m = 35e-6         # 1 oz capas internas (inner1/inner2)
vias_count = 24
via_drill_m = 0.30e-3
via_plating_m = 25e-6
R_case_to_pad = 2.0          # °C/W (case -> pad soldado)
R_spread_top = 8.0           # °C/W (spreading desde pad a plano top)
h = 12.0                     # W/m2K (conveccion natural)
T_amb = 24.0                 # °C

# Distribución de espesores (típico para 4 capas)
# Top: t_cu_outer, Core1, Inner1: t_cu_inner, Prepreg, Inner2: t_cu_inner, Core2, Bottom: t_cu_outer
core1_thickness_m = 0.4e-3   # Core entre top e inner1
prepreg_thickness_m = 0.4e-3 # Prepreg entre inner1 e inner2
core2_thickness_m = 0.4e-3   # Core entre inner2 y bottom

# Areas
A_face_m2 = (pcb_x_mm/1000.0) * (pcb_y_mm/1000.0)
A_total_outer_m2 = 2.0 * A_face_m2  # Solo top y bottom expuestas al aire
area_top_pad_mm2 = 10.0 * 10.0

# Propiedades
k_cu = 400.0
k_fr4 = 0.3  # W/mK (conductividad térmica del FR4)

# Funciones
def r_via_single_4layer(L_total_m, D_m, t_p_m, k=k_cu):
    """Resistencia térmica de un via que atraviesa las 4 capas"""
    return L_total_m / (k * math.pi * D_m * t_p_m)

def r_via_array(N, L_m, D_m, t_p_m, k=k_cu):
    """Resistencia de array de vias en paralelo"""
    r1 = r_via_single_4layer(L_m, D_m, t_p_m, k)
    return r1 / max(1, N)

def r_conv_air(h_local, A_m2):
    """Resistencia térmica convectiva"""
    return 1.0 / (h_local * A_m2)

def r_copper_plane_spreading(t_cu, area_ratio=0.8):
    """Resistencia de spreading en planos de cobre internos
    area_ratio: fracción del área total cubierta por cobre"""
    # Estimación simplificada para spreading en planos internos
    return R_spread_top * 0.6 * (35e-6 / t_cu) / area_ratio

def r_thermal_parallel(r1, r2):
    """Resistencia térmica de dos caminos en paralelo"""
    return (r1 * r2) / (r1 + r2)

def r_thermal_series(*resistances):
    """Resistencia térmica de caminos en serie"""
    return sum(resistances)

# Cálculos principales

# 1. Resistencia de vias (atraviesan las 4 capas)
R_vias = r_via_array(vias_count, thickness_total_m, via_drill_m, via_plating_m)

# 2. Resistencia de convección (solo superficies top y bottom expuestas)
R_conv = r_conv_air(h, A_total_outer_m2)

# 3. Resistencia de spreading en capas internas
R_spread_inner1 = r_copper_plane_spreading(t_cu_inner_m, area_ratio=0.9)
R_spread_inner2 = r_copper_plane_spreading(t_cu_inner_m, area_ratio=0.9)
R_spread_bottom = R_spread_top  # Asumiendo similar spreading en bottom

# 4. Caminos térmicos en paralelo desde los vias hacia las superficies
# Camino 1: Via -> Top surface
R_path_top = r_thermal_series(R_spread_top, R_conv/2)  # R_conv/2 porque hay dos superficies

# Camino 2: Via -> Bottom surface (a través de capas internas)
R_path_bottom = r_thermal_series(R_spread_bottom, R_conv/2)

# Caminos adicionales a través de capas internas (conducción lateral + vias)
# Simplificación: las capas internas actúan como caminos adicionales en paralelo
R_path_inner_equiv = r_thermal_series(
    r_thermal_parallel(R_spread_inner1, R_spread_inner2),
    R_conv  # Las internas deben llegar a superficie para disipar
)

# 5. Resistencia total desde vias hasta ambiente
R_vias_to_amb = r_thermal_parallel(
    r_thermal_parallel(R_path_top, R_path_bottom),
    R_path_inner_equiv
)

# 6. Resistencia térmica total del PCB
R_theta_pcb_4layer = R_case_to_pad + R_vias + R_vias_to_amb

# Comparación con 2 capas (cálculo original)
R_vias_2layer = r_via_array(vias_count, thickness_total_m, via_drill_m, via_plating_m)
R_conv_2layer = r_conv_air(h, A_total_outer_m2)
R_theta_pcb_2layer = R_case_to_pad + R_vias_2layer + R_spread_top + R_conv_2layer

# Mejora térmica
mejora_porcentual = ((R_theta_pcb_2layer - R_theta_pcb_4layer) / R_theta_pcb_2layer) * 100

# Output
print("=== ANÁLISIS TÉRMICO PCB 4 CAPAS vs 2 CAPAS ===")
print(f"PCB: {pcb_x_mm} x {pcb_y_mm} mm")
print(f"Espesor total: {thickness_total_m*1e3:.1f} mm")
print(f"Cobre externo: {t_cu_outer_m*1e6:.0f} μm, interno: {t_cu_inner_m*1e6:.0f} μm")
print(f"Vías: {vias_count} unidades, ø{via_drill_m*1e3:.2f} mm, plating {via_plating_m*1e6:.0f} μm")
print()

print("--- RESISTENCIAS TÉRMICAS 4 CAPAS ---")
print(f"R_case->pad     = {R_case_to_pad:.3f} °C/W")
print(f"R_vias          = {R_vias:.3f} °C/W")
print(f"R_spread_top    = {R_spread_top:.3f} °C/W")
print(f"R_spread_inner1 = {R_spread_inner1:.3f} °C/W")
print(f"R_spread_inner2 = {R_spread_inner2:.3f} °C/W")
print(f"R_vias->amb     = {R_vias_to_amb:.3f} °C/W")
print(f"R_conv          = {R_conv:.3f} °C/W")
print()

print("--- COMPARACIÓN FINAL ---")
print(f"R_theta_pcb (2 capas) = {R_theta_pcb_2layer:.3f} °C/W")
print(f"R_theta_pcb (4 capas) = {R_theta_pcb_4layer:.3f} °C/W")
print(f"Mejora térmica        = {mejora_porcentual:.1f}%")
print()

print("--- EJEMPLO DE DISIPACIÓN ---")
potencia_test_w = 2.0  # Watts
delta_t_2layer = potencia_test_w * R_theta_pcb_2layer
delta_t_4layer = potencia_test_w * R_theta_pcb_4layer
print(f"Para {potencia_test_w:.1f}W de disipación:")
print(f"ΔT con 2 capas = {delta_t_2layer:.1f} °C")
print(f"ΔT con 4 capas = {delta_t_4layer:.1f} °C")
print(f"Reducción de temperatura = {delta_t_2layer - delta_t_4layer:.1f} °C")