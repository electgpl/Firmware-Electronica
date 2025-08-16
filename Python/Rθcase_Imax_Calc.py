# Electgpl - R_case y Imax
import numpy as np
import matplotlib.pyplot as plt

# Parametros (ajustables)
Vin = 12.0
Vout = 5.0
Iout = 2.0
R_theta_PCB = 66.677
R_theta_JC = 2.0
T_amb = 25.0
eta = 0.82
Tj_max = 125.0

# Disipacion
def calcular_temperaturas(I):
    Pout = Vout * I
    Ptotal = Pout * (1/eta - 1)
    Pic = 0.64 * Ptotal 
    Tcase = T_amb + Pic * R_theta_PCB
    Tj = Tcase + Pic * R_theta_JC
    return Pout, Ptotal, Pic, Tcase, Tj

# Corriente de entrada Iout
Pout, Ptotal, Pic, Tcase, Tj = calcular_temperaturas(Iout)
print("=== Resultados para Iout = {:.2f} A ===".format(Iout))
print(f"Pout    = {Pout:.2f} W")
print(f"Ptotal  = {Ptotal:.2f} W")
print(f"Pic     = {Pic:.2f} W")
print(f"Tcase   = {Tcase:.2f} °C")
print(f"Tj      = {Tj:.2f} °C")

# Inverso: Imax para Tj_max
Pic_max = (Tj_max - T_amb) / (R_theta_PCB + R_theta_JC)
Ptotal_max = Pic_max / 0.64
I_max = Ptotal_max / (Vout * (1/eta - 1))
print("\n=== Corriente maxima para Tj_max = {:.0f} °C ===".format(Tj_max))
print(f"I_max   = {I_max:.3f} A")
print(f"Ptotal  = {Ptotal_max:.2f} W")
print(f"Pic     = {Pic_max:.2f} W")
Tcase_max = T_amb + Pic_max * R_theta_PCB
print(f"Tcase   = {Tcase_max:.2f} °C")
Tj_max_calc = Tcase_max + Pic_max * R_theta_JC
print(f"Tj      = {Tj_max_calc:.2f} °C")