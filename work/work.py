import random
import pandas as pd
import matplotlib.pyplot as plt
from statistics import mean

# -----------------------------
# Алгоритмы сортировки
# -----------------------------

def insertion_sort_count(arr):
    a = arr[:]
    comparisons = swaps = 0
    n = len(a)
    for j in range(1, n):
        key = a[j]
        i = j - 1
        comparisons += 1
        while i >= 0 and a[i] > key:
            comparisons += 1
            swaps += 1
            a[i + 1] = a[i]
            i -= 1
        a[i + 1] = key
        swaps += 1
    return comparisons, swaps


def shell_sort_count(arr, gaps):
    a = arr[:]
    comparisons = swaps = 0
    n = len(a)
    for gap in gaps:
        for i in range(gap, n):
            temp = a[i]
            j = i
            comparisons += 1
            while j >= gap and a[j - gap] > temp:
                comparisons += 1
                swaps += 1
                a[j] = a[j - gap]
                j -= gap
            a[j] = temp
            swaps += 1
    return comparisons, swaps


# -----------------------------
# Конфигурация эксперимента
# -----------------------------

SIZES = [100, 500, 2000]
TRIALS = 7
GAPS_CIURA = [701, 301, 132, 57, 23, 10, 4, 1]

raw_results = []

for n in SIZES:
    for trial in range(1, TRIALS + 1):
        arr = [random.randint(0, 10**6) for _ in range(n)]
        ins_cmp, ins_swaps = insertion_sort_count(arr)
        sh_cmp, sh_swaps = shell_sort_count(arr, GAPS_CIURA)

        raw_results.append({
            "n": n,
            "trial": trial,
            "insertion_comparisons": ins_cmp,
            "insertion_swaps": ins_swaps,
            "shell_comparisons": sh_cmp,
            "shell_swaps": sh_swaps,
            "gaps_used": str(GAPS_CIURA)
        })

# -----------------------------
# Сохранение CSV
# -----------------------------

df_raw = pd.DataFrame(raw_results)
df_raw.to_csv("sorting_results.csv", index=False)

df_agg = df_raw.groupby("n").agg({
    "insertion_comparisons": "mean",
    "insertion_swaps": "mean",
    "shell_comparisons": "mean",
    "shell_swaps": "mean"
}).reset_index()
df_agg.to_csv("sorting_agg.csv", index=False)

print("CSV сохранены: sorting_results.csv и sorting_agg.csv")

# -----------------------------
# Построение диаграмм
# -----------------------------

def plot_metric(df, metric, ylabel, filename):
    plt.figure(figsize=(6,4))
    for algo in ["insertion", "shell"]:
        plt.plot(df["n"], df[f"{algo}_{metric}"], marker='o', label=f"{algo.capitalize()} Sort")
    plt.xlabel("Размер массива (n)")
    plt.ylabel(ylabel)
    plt.xscale("log")
    plt.yscale("log")
    plt.legend()
    plt.grid(True, which="both", ls="--", lw=0.5)
    plt.savefig(filename, dpi=200, bbox_inches="tight")
    plt.close()
    print(f"Сохранён график: {filename}")

# Сравнения
plot_metric(df_agg, "comparisons", "Число сравнений (лог. шкала)", "comparisons.png")

# Перестановки
plot_metric(df_agg, "swaps", "Число перестановок (лог. шкала)", "swaps.png")

# Суммарные операции
df_agg["insertion_total"] = df_agg["insertion_comparisons"] + df_agg["insertion_swaps"]
df_agg["shell_total"] = df_agg["shell_comparisons"] + df_agg["shell_swaps"]

plt.figure(figsize=(6,4))
plt.plot(df_agg["n"], df_agg["insertion_total"], marker='o', label="Insertion Sort")
plt.plot(df_agg["n"], df_agg["shell_total"], marker='o', label="Shell Sort")
plt.xlabel("Размер массива (n)")
plt.ylabel("Суммарные операции (лог. шкала)")
plt.xscale("log")
plt.yscale("log")
plt.legend()
plt.grid(True, which="both", ls="--", lw=0.5)
plt.savefig("total_ops.png", dpi=200, bbox_inches="tight")
plt.close()
print("Сохранён график: total_ops.png")
