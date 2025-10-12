import matplotlib.pyplot as plt
import networkx as nx
from typing import Dict

plt.rcParams.update({
    'font.family': 'DejaVu Sans',
    'font.size': 10,
    'axes.titlesize': 14,
    'figure.titlesize': 14
})

def build_algorithm1_tree(n: int, max_depth: int = 5) -> nx.DiGraph:
    """Дерево рекурсии: T(n) = T(n-5) + T(n-8) + Θ(n²)"""
    G = nx.DiGraph()
    
    def _build(node_n: int, depth: int, node_id: str):
        if depth >= max_depth or node_n <= 20:
            G.add_node(node_id, label=f"T({node_n})\n(Θ(1))")
            return
        
        G.add_node(node_id, label=f"T({node_n})\n(Θ({node_n}^2))")
        
        left_id = f"{node_id}_L"
        G.add_edge(node_id, left_id)
        _build(node_n - 5, depth + 1, left_id)
        
        right_id = f"{node_id}_R"
        G.add_edge(node_id, right_id)
        _build(node_n - 8, depth + 1, right_id)
    
    _build(n, 0, "root")
    return G

def build_algorithm2_tree(n: int, max_depth: int = 5) -> nx.DiGraph:
    """Дерево рекурсии: T(n) = 2·T(n//4) + Θ(n)"""
    G = nx.DiGraph()
    
    def _build(node_n: int, depth: int, node_id: str):
        if depth >= max_depth or node_n <= 50:
            G.add_node(node_id, label=f"T({node_n})\n(Θ(1))")
            return
        
        G.add_node(node_id, label=f"T({node_n})\n(Θ({node_n}))")
        
        child_n = node_n // 4
        left_id = f"{node_id}_L"
        right_id = f"{node_id}_R"
        
        G.add_edge(node_id, left_id)
        _build(child_n, depth + 1, left_id)
        
        G.add_edge(node_id, right_id)
        _build(child_n, depth + 1, right_id)
    
    _build(n, 0, "root")
    return G

def hierarchy_pos(G, root=None, width=1., vert_gap=0.2, vert_loc=0, xcenter=0.5, pos=None, parent=None):
    """Иерархическая раскладка дерева."""
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
        pos = hierarchy_pos(G, child, width=dx, vert_gap=vert_gap,
                            vert_loc=vert_loc - vert_gap, xcenter=nextx,
                            pos=pos, parent=root)
    return pos

def draw_tree(G: nx.DiGraph, title: str, filename: str):
    """Отрисовка дерева рекурсии."""
    root = "root"
    pos = hierarchy_pos(G, root=root, width=2.0, vert_gap=0.35)
    
    plt.figure(figsize=(16, 10))
    
    nx.draw_networkx_edges(
        G, pos, 
        arrowstyle='-|>', 
        arrowsize=12, 
        edge_color='dimgray',
        alpha=0.7
    )
    
    labels = nx.get_node_attributes(G, 'label')
    nx.draw_networkx_nodes(
        G, pos, 
        node_color='white', 
        node_size=4200, 
        edgecolors='black', 
        linewidths=1.2,
        alpha=0.95
    )
    nx.draw_networkx_labels(G, pos, labels, font_size=10, font_weight='normal')
    
    plt.title(title, fontsize=16, pad=25, fontweight='bold')
    plt.axis('off')
    plt.tight_layout()
    plt.savefig(filename, dpi=300, bbox_inches='tight', facecolor='white')
    plt.close()

if __name__ == "__main__":
    tree1 = build_algorithm1_tree(n=50, max_depth=4)
    draw_tree(
        tree1, 
        "Дерево рекурсии для Алгоритма 1\nT(n) = T(n-5) + T(n-8) + Θ(n²)", 
        "algorithm1_recursion_tree.png"
    )
    
    tree2 = build_algorithm2_tree(n=256, max_depth=4)
    draw_tree(
        tree2, 
        "Дерево рекурсии для Алгоритма 2\nT(n) = 2·T(n/4) + Θ(n)", 
        "algorithm2_recursion_tree.png"
    )
    
    print("✅ Диаграммы деревьев рекурсии созданы:")
    print("   - algorithm1_recursion_tree.png")
    print("   - algorithm2_recursion_tree.png")