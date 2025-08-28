# Electgpl - Thermal PCB 2Layer Calc:
import math

# ----------------------------- R_theta_pcb -----------------------------

def r_via_single(L_m, D_m, t_p_m, k_cu=400.0):
    return L_m / (k_cu * math.pi * D_m * t_p_m)

def r_via_array(N, L_m, D_m, t_p_m, k_cu=400.0):
    r1 = r_via_single(L_m, D_m, t_p_m, k_cu)
    return r1 / max(1, N)

def r_conv_air(h, A_m2):
    return 1.0 / (h * A_m2)

def calc_rtheta_pcb(
    pcb_x_mm=42.0,
    pcb_y_mm=20.0,
    thickness_m=1.6e-3,
    t_cu_m=35e-6,
    area_top_pad_mm2=10.0*10.0,
    vias_count=24,
    via_drill_m=0.30e-3,
    via_plating_m=25e-6,
    R_case_to_pad=1.5,
    R_spread=8.0,
    h=12.0
):
    A_face_m2 = (pcb_x_mm/1000.0) * (pcb_y_mm/1000.0)
    A_total_m2 = 2.0 * A_face_m2   # ambas caras participando
    R_vias = r_via_array(vias_count, thickness_m, via_drill_m, via_plating_m)
    R_conv = r_conv_air(h, A_total_m2)
    R_total = R_case_to_pad + R_vias + R_spread + R_conv
    return {
        "R_case_to_pad": R_case_to_pad,
        "R_vias": R_vias,
        "R_spread": R_spread,
        "R_conv": R_conv,
        "R_theta_pcb": R_total,
        "A_face_m2": A_face_m2,
        "A_total_m2": A_total_m2
    }

# ----------------------------- Ejemplo Circuito DCDC -----------------------------

def losses_from_params(I,
                       V_in=12.0, V_out=5.0, Vf_diode=0.50, Vsat_switch=0.8,
                       tr_tf=200e-9, f_sw=150e3, ESR_inductor=0.05, P_other_fixed=0.0):
    """Calcula pérdidas del convertidor y P_ic (pérdidas en IC) para una corriente I."""
    D = V_out / V_in
    P_diode = (1.0 - D) * I * Vf_diode
    P_sw_cond = I * Vsat_switch * D
    P_sw_trans = V_in * I * tr_tf * f_sw
    P_ind_esr = ESR_inductor * I * I
    P_total = P_diode + P_sw_cond + P_sw_trans + P_ind_esr + P_other_fixed
    P_ic = P_sw_cond + P_sw_trans
    return {
        "P_diode": P_diode,
        "P_sw_cond": P_sw_cond,
        "P_sw_trans": P_sw_trans,
        "P_ind_esr": P_ind_esr,
        "P_total": P_total,
        "P_ic": P_ic,
        "D": D
    }

def compute_Tcase_Tj_from_Rpcb(I, R_theta_pcb, theta_jc, T_amb=18.0, **loss_params):
    """Dado I y R_theta_pcb (case->amb excluyendo theta_jc), calcula T_case y T_j."""
    comps = losses_from_params(I, **loss_params)
    P_ic = comps["P_ic"]
    T_case = T_amb + R_theta_pcb * P_ic
    T_j = T_case + theta_jc * P_ic
    return T_j, T_case, comps

def find_Imax_for_Tj(R_theta_pcb, Tj_max, theta_jc,
                     I_min=0.01, I_max=10.0, tol=1e-3, max_iter=100,
                     T_amb=18.0, **loss_params):
    """Busca la corriente máxima tal que Tj(I) <= Tj_max, usando bisección."""
    def Tj_of_I(I):
        Tj, _, _ = compute_Tcase_Tj_from_Rpcb(I, R_theta_pcb, theta_jc, T_amb=T_amb, **loss_params)
        return Tj
    a, b = I_min, I_max
    Ta = Tj_of_I(a)
    Tb = Tj_of_I(b)

    iter_e = 0
    while Tb < Tj_max and iter_e < 50:
        b *= 2.0
        Tb = Tj_of_I(b)
        iter_e += 1
    if Tb < Tj_max:
        return b, compute_Tcase_Tj_from_Rpcb(b, R_theta_pcb, theta_jc, T_amb=T_amb, **loss_params)
    for _ in range(max_iter):
        m = 0.5*(a+b)
        Tm = Tj_of_I(m)
        if abs(Tm - Tj_max) < tol:
            return m, compute_Tcase_Tj_from_Rpcb(m, R_theta_pcb, theta_jc, T_amb=T_amb, **loss_params)
        if Tm > Tj_max:
            b = m
        else:
            a = m
    m = 0.5*(a+b)
    return m, compute_Tcase_Tj_from_Rpcb(m, R_theta_pcb, theta_jc, T_amb=T_amb, **loss_params)

# ----------------------------- EJEMPLO / DEMO -----------------------------
if __name__ == "__main__":
    # parámetros del PCB (tu ejemplo)
    pcb_params = dict(
        pcb_x_mm = 42.0,
        pcb_y_mm = 20.0,
        thickness_m = 1.6e-3,
        t_cu_m = 35e-6,
        area_top_pad_mm2 = 10.0*10.0,
        vias_count = 24,
        via_drill_m = 0.30e-3,
        via_plating_m = 25e-6,
        R_case_to_pad = 1.5,
        R_spread = 8.0,
        h = 12.0
    )
    rpcb = calc_rtheta_pcb(**pcb_params)
    print("=== RESULTADO SCRIPT PCB (R_theta_pcb) ===")
    for k,v in rpcb.items():
        if isinstance(v, float):
            if abs(v) > 1e-2:
                print(f"{k}: {v:.3f}")
            else:
                print(f"{k}: {v}")
    print("")

    # ahora usamos R_theta_pcb en solver de fuente
    R_theta_pcb_value = rpcb["R_theta_pcb"]
    solver_params = dict(
        V_in=12.0, V_out=5.0, Vf_diode=0.50, Vsat_switch=0.8,
        tr_tf=200e-9, f_sw=150e3, ESR_inductor=0.05, P_other_fixed=0.0
    )
    theta_jc = 2.0
    I_test = 2.0
    Tj, Tcase, comps = compute_Tcase_Tj_from_Rpcb(I_test, R_theta_pcb_value, theta_jc, T_amb=18.0, **solver_params)
    print("=== EJEMPLO: I = 2.0 A ===")
    print(f"P_total = {comps['P_total']:.3f} W, P_ic = {comps['P_ic']:.3f} W")
    print(f"T_case = {Tcase:.2f} °C, T_j = {Tj:.2f} °C\n")

    # Buscamos I_max para Tj_max = 125 °C
    Imax_125, state_125 = find_Imax_for_Tj(R_theta_pcb_value, 125.0, theta_jc, I_min=0.01, I_max=5.0, T_amb=18.0, **solver_params)
    print("=== BÚSQUEDA I_max para Tj_max = 125 °C ===")
    print(f"I_max ≈ {Imax_125:.3f} A")
    print(f"P_total = {state_125[2]['P_total']:.3f} W, P_ic = {state_125[2]['P_ic']:.3f} W")
    print(f"T_case = {state_125[1]:.2f} °C, T_j = {state_125[0]:.2f} °C")
