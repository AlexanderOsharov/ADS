import matplotlib.pyplot as plt
import networkx as nx

def build_symbolic_tree(G=None, root="T(n)", label="+n", depth=0, max_depth=4, even=True):
    """Рекурсивно строит универсальное рекурсивное дерево T(n) с обозначениями n, n/2, (n±1)/2."""
    if G is None:
        G = nx.DiGraph()

    root_id = f"{root}_{depth}"
    G.add_node(root_id, display=root, cost=label)

    # Условие остановки (листья)
    if depth >= max_depth:
        G.add_node(f"T(1)_{depth+1}", display="T(1)", cost="+1")
        G.add_edge(root_id, f"T(1)_{depth+1}", label="база", color="gray")
        return G

    if even:
        # Чётный случай
        child = f"T(n/2)_{depth+1}"
        G.add_edge(root_id, child, label="чётное: n → n/2", color="green")
        build_symbolic_tree(G, "T(n/2)", f"+n/{2**(depth+1)}", depth + 1, max_depth, even=False)
    else:
        # Нечётный случай: две ветви
        left = f"T((n-1)/2)_{depth+1}"
        mid = f"⋯_{depth+1}"
        right = f"T((n+1)/2)_{depth+1}"

        G.add_edge(root_id, left, label="нечётное: n → (n-1)/2", color="orange")
        G.add_edge(root_id, mid, label="", color="gray")
        G.add_edge(root_id, right, label="нечётное: n → (n+1)/2", color="orange")

        G.add_node(mid, display="⋯", cost="")
        build_symbolic_tree(G, "T((n-1)/2)", f"+n/{2**(depth+1)}", depth + 1, max_depth, even=True)
        build_symbolic_tree(G, "T((n+1)/2)", f"+n/{2**(depth+1)}", depth + 1, max_depth, even=True)

    return G


def hierarchy_pos(G, root=None, width=1., vert_gap=0.25, vert_loc=0, xcenter=0.5, pos=None, parent=None):
    """Располагает узлы NetworkX в иерархическом виде."""
    if pos is None:
        pos = {root: (xcenter, vert_loc)}
    else:
        pos[root] = (xcenter, vert_loc)

    children = [n for n in G.successors(root) if n != parent]
    if not children:
        return pos

    dx = width / len(children)
    nextx = xcenter - width / 2 - dx / 2
    for child in children:
        nextx += dx
        pos = hierarchy_pos(G, root=child, width=dx, vert_gap=vert_gap,
                            vert_loc=vert_loc - vert_gap, xcenter=nextx,
                            pos=pos, parent=root)
    return pos


def draw_symbolic_tree(filename="symbolic_tree_Tn_full.png", max_depth=4):
    """Рисует универсальное дерево T(n) с троеточиями, аннотацией и формулами."""
    G = build_symbolic_tree(max_depth=max_depth)
    root = list(G.nodes)[0]
    pos = hierarchy_pos(G, root=root)

    edge_colors = [G[u][v]["color"] for u, v in G.edges()]
    edge_labels = nx.get_edge_attributes(G, "label")

    plt.figure(figsize=(13, 9))
    nx.draw(
        G, pos,
        with_labels=False,
        node_color="#fefefe",
        node_size=2800,
        font_size=9,
        font_weight="bold",
        edgecolors="black",
        arrows=False,
        edge_color=edge_colors
    )

    # Подписи узлов
    labels = {n: f"{G.nodes[n]['display']}\n({G.nodes[n]['cost']})" for n in G.nodes}
    nx.draw_networkx_labels(G, pos, labels=labels, font_size=9, font_weight="bold")

    # Подписи рёбер
    nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels, font_size=8, label_pos=0.45)

    # Текст-аннотация
    plt.text(
        0.5, -1.15,
        "На каждом уровне суммарная стоимость ≈ n\n"
        "→ Всего ≈ n·log(n) для сбалансированных (чётных) случаев\n"
        "→ До n² при множественных нечётных ветвлениях",
        ha="center", va="center", fontsize=10,
        bbox=dict(boxstyle="round,pad=0.4", facecolor="#f6f6f6", edgecolor="gray")
    )

    plt.text(
        0.5, -1.45,
        r"$T(n) = 2\,T\!\left(\frac{n}{2}\right) + n \;\Rightarrow\; T(n) = \Theta(n \log n)$",
        ha="center", va="center", fontsize=11, color="green"
    )
    plt.text(
        0.5, -1.62,
        r"$T(n) = T\!\left(\frac{n-1}{2}\right) + T\!\left(\frac{n+1}{2}\right) + n \;\Rightarrow\; T(n) = O(n^2)$",
        ha="center", va="center", fontsize=11, color="orange"
    )

    plt.title("Универсальное рекурсивное дерево T(n)", fontsize=13)
    plt.axis("off")
    plt.tight_layout()
    plt.savefig(filename, dpi=300, bbox_inches="tight")
    plt.show()


if __name__ == "__main__":
    draw_symbolic_tree(max_depth=4)