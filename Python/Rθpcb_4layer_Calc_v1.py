# Electgpl - Thermal Calc (2 o 4 layers)
import math

# =============================
# Parametros (ajustables)
# =============================
pcb_x_mm = 42.0
pcb_y_mm = 20.0
thickness_m = 1.6e-3     # mm -> m
t_cu_m = 35e-6           # 1 oz
vias_count = 45
via_drill_m = 0.30e-3
via_plating_m = 25e-6
R_case_to_pad = 2.0      # °C/W (case -> pad soldado)
R_spread_base = 8.0      # °C/W (spreading desde pad a plano, caso 2 capas)
h = 12.0                 # W/m2K (conveccion natural, "medio")
T_amb = 24.0             # °C
layers = 4               # elegir: 2 o 4

# =============================
# Areas
# =============================
A_face_m2 = (pcb_x_mm/1000.0) * (pcb_y_mm/1000.0)
A_total_m2 = 2.0 * A_face_m2   # solo top+bottom expuestos

# Propiedades
k_cu = 400.0

# =============================
# Funciones
# =============================
def r_via_single(L_m, D_m, t_p_m, k=k_cu):
    return L_m / (k * math.pi * D_m * t_p_m)

def r_via_array(N, L_m, D_m, t_p_m, k=k_cu):
    r1 = r_via_single(L_m, D_m, t_p_m, k)
    return r1 / max(1, N)

def r_conv_air(h_local, A_m2):
    return 1.0 / (h_local * A_m2)

# =============================
# Calculo vias
# =============================
R_vias = r_via_array(vias_count, thickness_m, via_drill_m, via_plating_m)

if layers == 4:
    # Tres planos disipativos conectados (bottom + inner1 + inner2)
    R_vias = R_vias / 3.0
    # Mejora de spreading con planos internos
    R_spread = R_spread_base / 2.0   # más conservador que /3
else:
    R_spread = R_spread_base

# =============================
# Calculo global
# =============================
R_conv = r_conv_air(h, A_total_m2)

R_theta_pcb = R_case_to_pad + R_vias + R_spread + R_conv

# =============================
# Output
# =============================
print("Parametros del PCB:")
print(f" PCB: {pcb_x_mm} x {pcb_y_mm} mm, {layers} capas, t_cu={t_cu_m*1e6:.0f} um")
print(f" Vías: {vias_count}, taladro {via_drill_m*1e3:.2f} mm, plating {via_plating_m*1e6:.0f} um")
print("")
print("Resultados:")
print(f" R_case->pad = {R_case_to_pad:.3f} °C/W")
print(f" R_vias (array, ajustado) = {R_vias:.3f} °C/W")
print(f" R_spread = {R_spread:.3f} °C/W")
print(f" R_conv (amb) = {R_conv:.3f} °C/W")
print(f" R_theta_pcb (case -> amb) = {R_theta_pcb:.3f} °C/W")
