import math
import networkx as nx
import matplotlib.pyplot as plt
from collections import deque

def build_tree(n, max_nodes=20000):
    G = nx.DiGraph()
    labels = {}
    sizes = {}
    depth = {}
    node_id = 0
    def new_node(size, d):
        nonlocal node_id
        nid = node_id
        node_id += 1
        G.add_node(nid)
        labels[nid] = f"{size}"
        sizes[nid] = size
        depth[nid] = d
        return nid
    root = new_node(n, 0)
    q = deque([(root, n, 0)])
    while q and G.number_of_nodes() < max_nodes:
        nid, size, d = q.popleft()
        if size <= 1:
            continue
        if size % 2 == 0:
            a = size // 2
            left = new_node(a, d+1)
            right = new_node(a, d+1)
            G.add_edge(nid, left)
            G.add_edge(nid, right)
            q.append((left, a, d+1))
            q.append((right, a, d+1))
        else:
            a = size - 1
            left = new_node(a, d+1)
            right = new_node(a, d+1)
            G.add_edge(nid, left)
            G.add_edge(nid, right)
            q.append((left, a, d+1))
            q.append((right, a, d+1))
    return G, labels, sizes, depth

def draw_tree(n, filename='rec_tree.png', dpi=200):
    G, labels, sizes, depth = build_tree(n)
    try:
        pos = nx.nx_agraph.graphviz_layout(G, prog='dot')
    except Exception:
        pos = nx.spring_layout(G, k=0.5, iterations=200)

    node_colors = [depth[u] for u in G.nodes()]
    node_sizes = [200 + 15*math.log2(sizes[u]+1) for u in G.nodes()]

    plt.figure(figsize=(14,10))
    nx.draw(G, pos, labels=labels, with_labels=True, node_size=node_sizes,
            cmap=plt.cm.viridis, node_color=node_colors, arrows=False)
    plt.title(f"Рекурсивное дерево для n={n}")
    plt.tight_layout()
    plt.savefig(filename, dpi=dpi)
    print("Saved", filename)

if __name__ == '__main__':
    n = 31
    draw_tree(n, filename=f'recursion_tree_n{n}.png')