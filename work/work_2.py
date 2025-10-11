import matplotlib.pyplot as plt
import networkx as nx

def build_recurrence_tree(n, G=None, parent=None, depth=0, max_depth=8):
    """Рекурсивно строит дерево T(n) с учётом чётности и ограничением по глубине."""
    if G is None:
        G = nx.DiGraph()
    if n <= 1 or depth >= max_depth:
        return G
    if parent is not None:
        G.add_edge(parent, n)
    # Чётное n — одна ветвь
    if n % 2 == 0:
        child = n // 2
        G.add_edge(n, child)
        build_recurrence_tree(child, G, n, depth + 1, max_depth)
    else:
        left = (n - 1) // 2
        right = (n + 1) // 2
        G.add_edge(n, left)
        G.add_edge(n, right)
        build_recurrence_tree(left, G, n, depth + 1, max_depth)
        build_recurrence_tree(right, G, n, depth + 1, max_depth)
    return G

def hierarchy_pos(G, root=None, width=1.0, vert_gap=0.3, vert_loc=0, xcenter=0.5, pos=None, parent=None):
    """Рекурсивная разметка уровней для NetworkX."""
    if pos is None:
        pos = {root: (xcenter, vert_loc)}
    else:
        pos[root] = (xcenter, vert_loc)
    children = list(G.successors(root))
    if not children:
        return pos
    dx = width / len(children)
    nextx = xcenter - width/2 - dx/2
    for child in children:
        nextx += dx
        pos = hierarchy_pos(G, root=child, width=dx, vert_gap=vert_gap,
                            vert_loc=vert_loc - vert_gap, xcenter=nextx, pos=pos, parent=root)
    return pos

def draw_tree(n0=31, max_depth=8, figsize=(12, 8), filename="recurrence_full_tree.png"):
    """Строит и визуализирует рекурсивное дерево T(n) с подсветкой четности."""
    G = build_recurrence_tree(n0, max_depth=max_depth)
    pos = hierarchy_pos(G, root=n0)

    # Цвета узлов
    node_colors = []
    for node in G.nodes:
        if node % 2 == 0:
            node_colors.append("#5cb85c")  # зелёный — чётные
        else:
            node_colors.append("#f0ad4e")  # оранжевый — нечётные

    plt.figure(figsize=figsize)
    nx.draw(
        G, pos,
        with_labels=True,
        node_color=node_colors,
        node_size=900,
        font_size=9,
        arrows=False,
        linewidths=1,
        edgecolors="black"
    )

    plt.title(f"Рекурсивное дерево T(n) с учётом чётности (n₀ = {n0})", fontsize=14)
    plt.axis("off")
    plt.tight_layout()
    plt.savefig(filename, dpi=300)
    plt.show()

if __name__ == "__main__":
    draw_tree(n0=31, max_depth=8)