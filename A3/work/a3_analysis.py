import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Загрузка данных
random = pd.read_csv('times_random.txt', delim_whitespace=True)
nearly = pd.read_csv('times_nearly.txt', delim_whitespace=True)
reversed_df = pd.read_csv('times_reversed.txt', delim_whitespace=True)
sorted_df = pd.read_csv('times_sorted.txt', delim_whitespace=True)

# Удобные имена
def plot_type(df, title):
    plt.figure(figsize=(12, 7))
    plt.plot(df.n, df.std_sort, label='std::sort', color='black', linewidth=2.5, linestyle='--')
    plt.plot(df.n, df.quick_random, label='QuickSort (random pivot)', color='tab:blue', linewidth=2)
    plt.plot(df.n, df.intro_random, label='Introsort (random pivot)', color='tab:green', linewidth=2)
    plt.plot(df.n, df.quick_det, label='QuickSort (det. pivot)', color='tab:orange', linewidth=2)
    plt.plot(df.n, df.intro_det, label='Introsort (det. pivot)', color='tab:red', linewidth=2)
    plt.plot(df.n, df.heap, label='HeapSort', color='gray', linewidth=2)

    plt.title(title, fontsize=16, fontweight='bold')
    plt.xlabel('Размер массива n')
    plt.ylabel('Время (мкс)')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.show()

plot_type(random, 'Случайный массив')
plot_type(nearly, 'Почти отсортированный массив (n/25 свопов)')
plot_type(reversed_df, 'Обратно отсортированный массив (плохие случаи)')
plot_type(sorted_df, 'Полностью отсортированный массив (плохие случаи)')

# Дополнительно — только плохие случаи крупным планом
fig, axes = plt.subplots(1, 2, figsize=(15, 6))
for ax, df, title in zip(axes, [sorted_df, reversed_df], ['Отсортированный', 'Обратно отсортированный']):
    ax.plot(df.n, df.quick_det / 1e6, label='QuickSort (det. pivot) — O(n²)', color='red', linewidth=3)
    ax.plot(df.n, df.intro_det / 1e6, label='Introsort (det. pivot)', color='green', linewidth=3)
    ax.plot(df.n, df.heap / 1e6, label='HeapSort', color='gray')
    ax.set_title(title)
    ax.set_xlabel('n')
    ax.set_ylabel('Время (мс)')
    ax.legend()
    ax.grid(True)

plt.suptitle('Дегенерация QuickSort и спасение Introsort', fontsize=16, fontweight='bold')
plt.tight_layout()
plt.show()