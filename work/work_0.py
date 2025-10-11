import math
import networkx as nx
import matplotlib.pyplot as plt

def build_tree(n, max_nodes=2000):
    G = nx.DiGraph()
    labels = {}
    sizes = {}
    node_id = 0
    def add_node(size):
        nonlocal node_id
        nid = node_id
        node_id += 1
        G.add_node(nid)
        labels[nid] = str(size)
        sizes[nid] = size
        return nid
    root = add_node(n)
    stack = [(root, n)]
    while stack and G.number_of_nodes() < max_nodes:
        nid, size = stack.pop()
        if size <= 1:
            continue
        if size % 2 == 0:
            a = size // 2
            left = add_node(a)
            right = add_node(a)
            G.add_edge(nid, left)
            G.add_edge(nid, right)
            stack.append((left, a))
            stack.append((right, a))
        else:
            a = size - 1
            left = add_node(a)
            right = add_node(a)
            G.add_edge(nid, left)
            G.add_edge(nid, right)
            stack.append((left, a))
            stack.append((right, a))
    return G, labels, sizes

def draw_tree(n, filename='tree.png', dpi=200):
    G, labels, sizes = build_tree(n)
    try:
        pos = nx.nx_agraph.graphviz_layout(G, prog='dot')
    except:
        pos = nx.spring_layout(G, k=0.5, iterations=200)
    plt.figure(figsize=(12,8))
    node_colors = []
    node_sizes = []
    for node in G.nodes():
        s = sizes[node]
        node_colors.append(math.log2(s+1) if s>0 else 0.1)
        node_sizes.append(300 + 2*math.log2(s+1)*200)
    nx.draw(G, pos, labels=labels, with_labels=True, node_size=node_sizes, cmap=plt.cm.viridis, node_color=node_colors, arrows=False)
    plt.title(f"Рекурсивное дерево для n={n}")
    plt.tight_layout()
    plt.savefig(filename, dpi=dpi)
    print("Saved", filename)

if __name__ == '__main__':
    n = 31  # замените на желаемое n
    draw_tree(n, filename='recursion_tree_n31.png')
