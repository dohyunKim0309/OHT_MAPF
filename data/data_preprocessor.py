#!/usr/bin/env python3
"""
SMAT 2022 OHT network -> directed graph input for the OHT_MAPF C++ project.

Reads:
    data/SMAT2022_node.csv  (cols: idx, ENCODED_ID, count)
    data/SMAT2022_link.csv  (cols: idx, ENCODED_ID, ENCODED_FROM_NODE,
                                   ENCODED_TO_NODE, ENCODED_SPEED, ENCODED_DISTANCE)

Writes (into data/):
    edges.txt     : "N M" header, then "from_idx to_idx int_weight" per line
    node_map.csv  : "idx,ENCODED_ID"

Weight conversion: int_weight = max(1, round(distance / V)).
"""
import argparse
import csv
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))

# Expected ground-truth values (asserted, not trusted blindly)
EXP_NODES = 2858
EXP_EDGES = 3424


def load_nodes(path):
    """Return (id_to_idx dict, ordered list of ENCODED_ID)."""
    ids = []
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            ids.append(row["ENCODED_ID"])
    id_to_idx = {eid: i for i, eid in enumerate(ids)}
    if len(id_to_idx) != len(ids):
        sys.exit(f"ERROR: duplicate ENCODED_ID in node CSV "
                 f"({len(ids)} rows, {len(id_to_idx)} unique).")
    return id_to_idx, ids


def load_edges(path, id_to_idx):
    """Return list of (from_idx, to_idx, distance)."""
    edges = []
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            fr = row["ENCODED_FROM_NODE"]
            to = row["ENCODED_TO_NODE"]
            dist = float(row["ENCODED_DISTANCE"])
            if fr not in id_to_idx:
                sys.exit(f"ERROR: FROM node '{fr}' not in node set.")
            if to not in id_to_idx:
                sys.exit(f"ERROR: TO node '{to}' not in node set.")
            edges.append((id_to_idx[fr], id_to_idx[to], dist))
    return edges


def verify_topology(n_nodes, edges):
    """Check expectations: counts, self-loops, reverse pairs. Returns dict."""
    self_loops = sum(1 for a, b, _ in edges if a == b)
    directed_set = {(a, b) for a, b, _ in edges}
    reverse_pairs = sum(1 for (a, b) in directed_set if (b, a) in directed_set) // 2
    duplicate_directed = len(edges) - len(directed_set)
    return {
        "n_nodes": n_nodes,
        "n_edges": len(edges),
        "self_loops": self_loops,
        "reverse_pairs": reverse_pairs,
        "duplicate_directed": duplicate_directed,
    }


def convert_weights(edges, V):
    """Return (list of int_weights, stats dict) for given V."""
    int_weights = []
    rel_errors = []
    for _, _, dist in edges:
        iw = max(1, round(dist / V))
        int_weights.append(iw)
        if dist != 0:
            rel_errors.append(abs(iw * V - dist) / dist)
        else:
            rel_errors.append(0.0)
    stats = {
        "V": V,
        "min_weight": min(int_weights),
        "max_weight": max(int_weights),
        "mean_rel_err": sum(rel_errors) / len(rel_errors),
        "max_rel_err": max(rel_errors),
    }
    return int_weights, stats


def expand_unweighted(n_nodes, edges, int_weights):
    """Stage 2: expand each weighted edge into a chain of unit (length-1) edges.

    A weighted edge from->to with weight w becomes:
        w == 1 : from -> to                                   (no virtual node)
        w >  1 : from -> v1 -> v2 -> ... -> v(w-1) -> to       (w-1 virtual nodes)
    Virtual nodes get fresh indices appended after the original nodes
    (n_nodes, n_nodes+1, ...). All resulting edges have length 1.

    Returns (total_node_count, list of (from_idx, to_idx)).
    """
    unit_edges = []
    next_idx = n_nodes  # first virtual node index
    for (a, b, _), w in zip(edges, int_weights):
        if w <= 1:
            unit_edges.append((a, b))
            continue
        prev = a
        for _ in range(w - 1):
            v = next_idx
            next_idx += 1
            unit_edges.append((prev, v))
            prev = v
        unit_edges.append((prev, b))  # last hop into the real target
    total_nodes = next_idx
    return total_nodes, unit_edges


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--V", type=float, default=100.0,
                    help="base unit; int_weight = max(1, round(distance/V))")
    ap.add_argument("--node-csv", default=os.path.join(HERE, "SMAT2022_node.csv"))
    ap.add_argument("--link-csv", default=os.path.join(HERE, "SMAT2022_link.csv"))
    ap.add_argument("--out-edges", default=os.path.join(HERE, "edges.txt"))
    ap.add_argument("--out-expanded", default=os.path.join(HERE, "edges_expanded.txt"))
    ap.add_argument("--out-map", default=os.path.join(HERE, "node_map.csv"))
    ap.add_argument("--scan", action="store_true",
                    help="print stats for several candidate V values and exit")
    args = ap.parse_args()

    id_to_idx, ids = load_nodes(args.node_csv)
    edges = load_edges(args.link_csv, id_to_idx)
    topo = verify_topology(len(ids), edges)

    print("=== Topology verification ===")
    print(f"  nodes              : {topo['n_nodes']}  (expected {EXP_NODES})  "
          f"{'OK' if topo['n_nodes'] == EXP_NODES else 'MISMATCH'}")
    print(f"  edges              : {topo['n_edges']}  (expected {EXP_EDGES})  "
          f"{'OK' if topo['n_edges'] == EXP_EDGES else 'MISMATCH'}")
    print(f"  self-loops         : {topo['self_loops']}  (expected 0)  "
          f"{'OK' if topo['self_loops'] == 0 else 'MISMATCH'}")
    print(f"  reverse pairs      : {topo['reverse_pairs']}  (expected 0)  "
          f"{'OK' if topo['reverse_pairs'] == 0 else 'MISMATCH'}")
    print(f"  duplicate directed : {topo['duplicate_directed']}")

    dist_min = min(d for _, _, d in edges)
    dist_max = max(d for _, _, d in edges)
    print(f"  distance range     : {dist_min} .. {dist_max}")

    if args.scan:
        print("\n=== V scan ===")
        print(f"{'V':>10} {'max_w':>8} {'min_w':>6} {'mean_err':>10} {'max_err':>9}")
        for V in [10, 25, 50, 100, 200, 416.666, 500]:
            _, s = convert_weights(edges, V)
            print(f"{V:>10.3f} {s['max_weight']:>8} {s['min_weight']:>6} "
                  f"{s['mean_rel_err']*100:>9.2f}% {s['max_rel_err']*100:>8.2f}%")
        return

    int_weights, stats = convert_weights(edges, args.V)
    print(f"\n=== Weight conversion (V = {stats['V']}) ===")
    print(f"  min int_weight : {stats['min_weight']}")
    print(f"  max int_weight : {stats['max_weight']}  (= TEG time layers driver)")
    print(f"  mean rel error : {stats['mean_rel_err']*100:.2f}%")
    print(f"  max  rel error : {stats['max_rel_err']*100:.2f}%")
    zero_clamped = sum(1 for _, _, d in edges if round(d / args.V) < 1)
    print(f"  edges clamped to 1 by max(1,...): {zero_clamped}")

    # Write edges.txt
    with open(args.out_edges, "w") as f:
        f.write(f"{len(ids)} {len(edges)}\n")
        for (a, b, _), w in zip(edges, int_weights):
            f.write(f"{a} {b} {w}\n")
    # Write node_map.csv
    with open(args.out_map, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["idx", "ENCODED_ID"])
        for i, eid in enumerate(ids):
            w.writerow([i, eid])

    # Stage 2: unweighted expansion (for BFS+TEG).
    total_nodes, unit_edges = expand_unweighted(len(ids), edges, int_weights)
    n_virtual = total_nodes - len(ids)
    print(f"\n=== Unweighted expansion ===")
    print(f"  original nodes : {len(ids)}")
    print(f"  virtual nodes  : {n_virtual}")
    print(f"  total nodes    : {total_nodes}")
    print(f"  unit edges     : {len(unit_edges)}  (all length 1)")
    # Write edges_expanded.txt: "N M" header, then "from to" per line (weight omitted = 1).
    with open(args.out_expanded, "w") as f:
        f.write(f"{total_nodes} {len(unit_edges)}\n")
        for a, b in unit_edges:
            f.write(f"{a} {b}\n")

    print(f"\nWrote {args.out_edges}, {args.out_expanded}, and {args.out_map}.")


if __name__ == "__main__":
    main()
