# Electgpl - 2 Layer Thermal Calc
import math

# Parametros (ajustables)
pcb_x_mm = 42.0
pcb_y_mm = 20.0
thickness_m = 1.6e-3     # mm -> m
t_cu_m = 35e-6           # 1 oz
vias_count = 24
via_drill_m = 0.30e-3
via_plating_m = 25e-6
R_case_to_pad = 2        # °C/W (case -> pad soldado)
R_spread = 8.0           # °C/W (spreading desde pad a plano)
h = 12.0                 # W/m2K (conveccion natural, "medio")
T_amb = 24.0             # °C

# Areas
A_face_m2 = (pcb_x_mm/1000.0) * (pcb_y_mm/1000.0)
A_total_m2 = 2.0 * A_face_m2
area_top_pad_mm2 = 10.0 * 10.0

# Propiedades
k_cu = 400.0

# Funciones
def r_via_single(L_m, D_m, t_p_m, k=k_cu):
    return L_m / (k * math.pi * D_m * t_p_m)

def r_via_array(N, L_m, D_m, t_p_m, k=k_cu):
    r1 = r_via_single(L_m, D_m, t_p_m, k)
    return r1 / max(1, N)

def r_conv_air(h_local, A_m2):
    return 1.0 / (h_local * A_m2)

# Calculo
R_vias = r_via_array(vias_count, thickness_m, via_drill_m, via_plating_m)
R_conv = r_conv_air(h, A_total_m2)

R_theta_pcb = R_case_to_pad + R_vias + R_spread + R_conv

# Output
print("Parametros del PCB:")
print(f" PCB: {pcb_x_mm} x {pcb_y_mm} mm, 2 capas, t_cu={t_cu_m*1e6:.0f} um")
print(f" Vías: {vias_count}, taladro {via_drill_m*1e3:.2f} mm, plating {via_plating_m*1e6:.0f} um")
print("")
print("Resultados:")
print(f" R_case->pad = {R_case_to_pad:.3f} °C/W")
print(f" R_vias (array) = {R_vias:.3f} °C/W")
print(f" R_spread = {R_spread:.3f} °C/W")
print(f" R_conv (amb) = {R_conv:.3f} °C/W")
print(f" R_theta_pcb (case -> amb) = {R_theta_pcb:.3f} °C/W")
