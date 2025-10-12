import matplotlib.pyplot as plt
import networkx as nx
from typing import Dict, Any
import math

plt.rcParams.update({
    'font.family': 'DejaVu Sans',
    'font.size': 10,
    'axes.titlesize': 14,
    'figure.titlesize': 14,
    'text.usetex': False
})

def build_recursion_tree_case_a(n: int = 100, max_depth: int = 4) -> nx.DiGraph:
    """T(n) = 13·T(n/21) + n^1.5 — один ребёнок с пометкой '13 подзадач'"""
    G = nx.DiGraph()
    
    def _build(node_n: int, depth: int, node_id: str):
        if depth >= max_depth or node_n < 21:
            G.add_node(node_id, label=f"T({node_n})\n(Θ(1))", level=depth)
            return
        
        cost = f"{node_n}^1.5"
        G.add_node(node_id, label=f"T({node_n})\n(Θ({cost}))", level=depth)
        
        child_n = node_n // 21
        child_id = f"{node_id}_child"
        G.add_edge(node_id, child_id, label="13 подзадач")
        _build(child_n, depth + 1, child_id)
    
    _build(n, 0, "root")
    return G

def build_recursion_tree_case_b(n: int = 20, max_depth: int = 5) -> nx.DiGraph:
    """T(n) = T(n-2) + T(n-3) + n — два ребёнка"""
    G = nx.DiGraph()
    
    def _build(node_n: int, depth: int, node_id: str):
        if depth >= max_depth or node_n <= 3:
            G.add_node(node_id, label=f"T({node_n})\n(Θ(1))", level=depth)
            return
        
        G.add_node(node_id, label=f"T({node_n})\n(Θ({node_n}))", level=depth)
        
        left_id = f"{node_id}_L"
        G.add_edge(node_id, left_id, label="n-2")
        _build(node_n - 2, depth + 1, left_id)
        
        right_id = f"{node_id}_R"
        G.add_edge(node_id, right_id, label="n-3")
        _build(node_n - 3, depth + 1, right_id)
    
    _build(n, 0, "root")
    return G

def build_recursion_tree_case_c(n: int = 256, max_depth: int = 4) -> nx.DiGraph:
    """T(n) = 12·T(n/4) + n^(7/4) — один ребёнок с пометкой '12 подзадач'"""
    G = nx.DiGraph()
    
    def _build(node_n: int, depth: int, node_id: str):
        if depth >= max_depth or node_n < 4:
            G.add_node(node_id, label=f"T({node_n})\n(Θ(1))", level=depth)
            return
        
        G.add_node(node_id, label=f"T({node_n})\n(Θ({node_n}^(7/4)))", level=depth)
        
        child_n = node_n // 4
        child_id = f"{node_id}_child"
        G.add_edge(node_id, child_id, label="12 подзадач")
        _build(child_n, depth + 1, child_id)
    
    _build(n, 0, "root")
    return G

def build_recursion_tree_3a(n: int = 256, max_depth: int = 4) -> nx.DiGraph:
    """T(n) = T(n/4) + 2·T(n/16) + n·log n — два типа детей"""
    G = nx.DiGraph()
    
    def _build(node_n: int, depth: int, node_id: str):
        if depth >= max_depth or node_n < 16:
            G.add_node(node_id, label=f"T({node_n})\n(Θ(1))", level=depth)
            return
        
        log_val = max(1, int(math.log2(node_n))) if node_n > 0 else 1
        G.add_node(node_id, label=f"T({node_n})\n(Θ({node_n}·{log_val}))", level=depth)
        
        child1_n = node_n // 4
        child1_id = f"{node_id}_A"
        G.add_edge(node_id, child1_id, label="T(n/4)")
        _build(child1_n, depth + 1, child1_id)
        
        child2_n = node_n // 16
        child2_id = f"{node_id}_B"
        G.add_edge(node_id, child2_id, label="2·T(n/16)")
        _build(child2_n, depth + 1, child2_id)
    
    _build(n, 0, "root")
    return G

def build_recursion_tree_3b(n: int = 1000, max_depth: int = 3) -> nx.DiGraph:
    """T(n) = 3T(n/2) + 6T(n/5) + T(n/10) + n²/ln n — три типа детей (глубина ограничена)"""
    G = nx.DiGraph()
    
    def _build(node_n: int, depth: int, node_id: str):
        if depth >= max_depth or node_n < 10:
            G.add_node(node_id, label=f"T({node_n})\n(Θ(1))", level=depth)
            return
        
        if node_n > 1:
            cost_label = f"{node_n}^2/ln({node_n})"
        else:
            cost_label = "1"
        G.add_node(node_id, label=f"T({node_n})\n(Θ({cost_label}))", level=depth)
        
        child1_n = node_n // 2
        child1_id = f"{node_id}_C1"
        G.add_edge(node_id, child1_id, label="3·T(n/2)")
        _build(child1_n, depth + 1, child1_id)
        
        child2_n = node_n // 5
        child2_id = f"{node_id}_C2"
        G.add_edge(node_id, child2_id, label="6·T(n/5)")
        _build(child2_n, depth + 1, child2_id)
        
        child3_n = node_n // 10
        child3_id = f"{node_id}_C3"
        G.add_edge(node_id, child3_id, label="T(n/10)")
        _build(child3_n, depth + 1, child3_id)
    
    _build(n, 0, "root")
    return G

def hierarchy_pos(G: nx.DiGraph, root: str = None, width: float = 1., vert_gap: float = 0.3, 
                  vert_loc: float = 0, xcenter: float = 0.5, pos: Dict = None, parent: str = None) -> Dict:
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
        pos = hierarchy_pos(G, child, width=dx*1.2, vert_gap=vert_gap,
                            vert_loc=vert_loc - vert_gap, xcenter=nextx,
                            pos=pos, parent=root)
    return pos

def draw_tree(G: nx.DiGraph, title: str, filename: str):
    """Отрисовка дерева рекурсии с метками на рёбрах."""
    root = "root"
    pos = hierarchy_pos(G, root=root, width=3.0, vert_gap=0.4)
    
    plt.figure(figsize=(18, 11))
    
    edge_labels = nx.get_edge_attributes(G, 'label')
    nx.draw_networkx_edges(
        G, pos, 
        arrowstyle='->', 
        arrowsize=14, 
        edge_color='dimgray',
        width=1.1,
        alpha=0.8
    )
    
    nx.draw_networkx_edge_labels(
        G, pos, 
        edge_labels, 
        font_size=9, 
        font_color='darkred',
        bbox=dict(boxstyle="round,pad=0.2", facecolor="white", alpha=0.7)
    )
    
    labels = nx.get_node_attributes(G, 'label')
    nx.draw_networkx_nodes(
        G, pos, 
        node_color='#f0f8ff',
        node_size=5000, 
        edgecolors='navy', 
        linewidths=1.4,
        alpha=0.95
    )
    nx.draw_networkx_labels(G, pos, labels, font_size=10, font_weight='normal', font_color='black')
    
    plt.title(title, fontsize=16, pad=25, fontweight='bold', color='navy')
    plt.axis('off')
    plt.tight_layout()
    plt.savefig(filename, dpi=300, bbox_inches='tight', facecolor='white')
    plt.close()

if __name__ == "__main__":
    tree_a = build_recursion_tree_case_a(n=100, max_depth=4)
    draw_tree(
        tree_a, 
        "Дерево рекурсии для случая (a)\nT(n) = 13 · T(n/21) + n^1.5", 
        "recursion_tree_case_a.png"
    )
    
    tree_b = build_recursion_tree_case_b(n=20, max_depth=5)
    draw_tree(
        tree_b, 
        "Дерево рекурсии для случая (b)\nT(n) = T(n-2) + T(n-3) + n", 
        "recursion_tree_case_b.png"
    )
    
    tree_c = build_recursion_tree_case_c(n=256, max_depth=4)
    draw_tree(
        tree_c, 
        "Дерево рекурсии для случая (c)\nT(n) = 12 · T(n/4) + n^(7/4)", 
        "recursion_tree_case_c.png"
    )
    
    tree_3a = build_recursion_tree_3a(n=256, max_depth=4)
    draw_tree(
        tree_3a, 
        "Дерево рекурсии для задачи 3(a)\nT(n) = T(n/4) + 2 · T(n/16) + n · log₂ n", 
        "recursion_tree_3a.png"
    )
    
    tree_3b = build_recursion_tree_3b(n=1000, max_depth=3)
    draw_tree(
        tree_3b, 
        "Дерево рекурсии для задачи 3(b)\nT(n) = 3T(n/2) + 6T(n/5) + T(n/10) + n^2 / ln n", 
        "recursion_tree_3b.png"
    )
    
    print("✅ Все диаграммы деревьев рекурсии успешно созданы:")
    print("   - recursion_tree_case_a.png")
    print("   - recursion_tree_case_b.png")
    print("   - recursion_tree_case_c.png")
    print("   - recursion_tree_3a.png")
    print("   - recursion_tree_3b.png")