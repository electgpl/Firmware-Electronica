import matplotlib.pyplot as plt

# Parámetros de tiempos (s)
t_deep_before = 2
t_proc = 5
t_tx = 1.5
t_deep_after = 2

# Corrientes (mA)
i_deep = 0.024  # 24 µA
i_proc = 9
i_tx = 18

# Construcción de secuencia de tiempo y corriente
t = [0]
i = [i_deep]

# Deep sleep inicial
t.append(t[-1] + t_deep_before); i.append(i_deep)

# Procesamiento
t.append(t[-1]); i.append(i_proc)
t.append(t[-1] + t_proc); i.append(i_proc)

# Transmisión
t.append(t[-1]); i.append(i_tx)
t.append(t[-1] + t_tx); i.append(i_tx)

# Deep sleep final
t.append(t[-1]); i.append(i_deep)
t.append(t[-1] + t_deep_after); i.append(i_deep)

# --- Cálculos de consumo diario ---
eventos_dia = 100
t_total = 24*3600
t_proc_total = eventos_dia * t_proc
t_tx_total = eventos_dia * t_tx
t_deep_total = t_total - (t_proc_total + t_tx_total)

consumo_deep = i_deep * (t_deep_total/3600)
consumo_proc = i_proc * (t_proc_total/3600)
consumo_tx = i_tx * (t_tx_total/3600)
consumo_total = consumo_deep + consumo_proc + consumo_tx

cap_bateria = 1500  # mAh
autonomia_dias = cap_bateria / consumo_total

# --- Gráfico ---
plt.figure(figsize=(12,5))
plt.step(t, i, where='post', linewidth=2, color="blue", label="Consumo")

# Colorear regiones
plt.axhspan(0, i_deep, facecolor="green", alpha=0.2, label="Deep Sleep")
plt.axhspan(i_deep, i_proc, facecolor="orange", alpha=0.2, label="Procesamiento")
plt.axhspan(i_proc, i_tx, facecolor="red", alpha=0.2, label="Transmisión")

# Etiquetas y estilo
plt.xlabel("Tiempo [s]")
plt.ylabel("Corriente [mA]")
plt.title("Oscilograma de Consumo durante un Evento")
plt.ylim(0, 20)
plt.grid(True, linestyle="--", alpha=0.6)

# Texto con cálculos
texto = (f"Consumo diario:\n"
         f"- Deep Sleep: {consumo_deep:.3f} mAh/día\n"
         f"- Procesamiento: {consumo_proc:.2f} mAh/día\n"
         f"- Transmisión: {consumo_tx:.2f} mAh/día\n"
         f"- Total: {consumo_total:.2f} mAh/día\n"
         f"Autonomía estimada: {autonomia_dias:.0f} días (~{autonomia_dias/365:.1f} años)")

plt.text(9, 10, texto, fontsize=10, bbox=dict(facecolor="white", alpha=0.8))

# Agregar etiquetas de corriente en la cima de cada escalón
etiquetas = ["24 µA", "9 mA", "18 mA"]
posiciones_y = [i_deep, i_proc, i_tx]
posiciones_x = [1, 4.5, 8]  # posiciones aproximadas dentro de cada segmento

for x, y, label in zip(posiciones_x, posiciones_y, etiquetas):
    plt.text(x, y + 0.5, label, ha="center", va="bottom", fontsize=10, color="black",
             bbox=dict(facecolor="white", alpha=0.7, edgecolor="none"))

plt.legend()
plt.show()
