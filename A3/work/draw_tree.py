import matplotlib.pyplot as plt
import networkx as nx
from typing import Dict
import math

plt.rcParams.update({
    'font.family': 'DejaVu Sans',
    'font.size': 10,
    'axes.titlesize': 14,
    'figure.titlesize': 14
})

def build_strassen_tree(n: int = 64, max_depth: int = 3) -> nx.DiGraph:
    """Дерево рекурсии для алгоритма Штрассена: T(N) = 7·T(N/2) + Θ(N²)"""
    G = nx.DiGraph()
    
    def _build(size: int, depth: int, node_id: str):
        if depth >= max_depth or size < 2:
            G.add_node(node_id, label=f"T({size})\n(Θ(1))")
            return
        
        G.add_node(node_id, label=f"T({size})\n(Θ({size}^2))")
        
        child_size = size // 2
        for i in range(7):
            child_id = f"{node_id}_S{i+1}"
            G.add_edge(node_id, child_id)
            _build(child_size, depth + 1, child_id)
    
    _build(n, 0, "root")
    return G

def build_mult_tree(a: int, n: int = 64, max_depth: int = 3) -> nx.DiGraph:
    """Дерево рекурсии для алгоритма MULT: T(N) = a·T(N/4) + Θ(N²)"""
    G = nx.DiGraph()
    
    def _build(size: int, depth: int, node_id: str):
        if depth >= max_depth or size < 4:
            G.add_node(node_id, label=f"T({size})\n(Θ(1))")
            return
        
        G.add_node(node_id, label=f"T({size})\n(Θ({size}^2))")
        
        child_size = size // 4
        for i in range(a):
            child_id = f"{node_id}_M{i+1}"
            G.add_edge(node_id, child_id)
            _build(child_size, depth + 1, child_id)
    
    _build(n, 0, "root")
    return G

def hierarchy_pos(G, root=None, width=1., vert_gap=0.25, vert_loc=0, xcenter=0.5, pos=None, parent=None):
    """Иерархическая раскладка дерева с учётом множества детей."""
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
        pos = hierarchy_pos(G, child, width=dx*0.9, vert_gap=vert_gap,
                            vert_loc=vert_loc - vert_gap, xcenter=nextx,
                            pos=pos, parent=root)
    return pos

def draw_tree(G: nx.DiGraph, title: str, filename: str):
    """Отрисовка дерева рекурсии."""
    root = "root"
    pos = hierarchy_pos(G, root=root, width=4.0, vert_gap=0.3)
    
    plt.figure(figsize=(18, 10))
    
    nx.draw_networkx_edges(G, pos, arrowstyle='->', arrowsize=10, 
                           edge_color='gray', alpha=0.7)
    
    labels = nx.get_node_attributes(G, 'label')
    nx.draw_networkx_nodes(G, pos, node_color='lightblue', node_size=3500,
                           edgecolors='black', linewidths=1.0)
    nx.draw_networkx_labels(G, pos, labels, font_size=9)
    
    plt.title(title, fontsize=16, pad=20, fontweight='bold')
    plt.axis('off')
    plt.tight_layout()
    plt.savefig(filename, dpi=300, bbox_inches='tight')
    plt.close()

if __name__ == "__main__":
    # Дерево для алгоритма Штрассена
    strassen_tree = build_strassen_tree(n=64, max_depth=3)
    draw_tree(
        strassen_tree,
        "Дерево рекурсии алгоритма Штрассена\nT(N) = 7 · T(N/2) + Θ(N²)",
        "strassen_recursion_tree.png"
    )
    
    # Дерево для MULT с a=48 (максимально допустимое значение)
    mult_tree_48 = build_mult_tree(a=48, n=64, max_depth=2)  # Глубина 2, так как 48 детей
    draw_tree(
        mult_tree_48,
        "Дерево рекурсии алгоритма MULT (a=48)\nT(N) = 48 · T(N/4) + Θ(N²)",
        "mult_recursion_tree_a48.png"
    )
    
    # Дерево для MULT с a=16 (граничный случай)
    mult_tree_16 = build_mult_tree(a=16, n=64, max_depth=3)
    draw_tree(
        mult_tree_16,
        "Дерево рекурсии алгоритма MULT (a=16)\nT(N) = 16 · T(N/4) + Θ(N²)",
        "mult_recursion_tree_a16.png"
    )
    
    print("✅ Диаграммы деревьев рекурсии созданы:")
    print("   - strassen_recursion_tree.png")
    print("   - mult_recursion_tree_a48.png")
    print("   - mult_recursion_tree_a16.png")