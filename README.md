# High-Performance Graph Analytics: Pointer vs. CSR Memory Layouts

**[Link to Full Cache Profiling & Performance Report](https://drive.google.com/file/d/1djYDhasvPS32_2A0Rt9pcG-8gQHDt3Ul/view?usp=sharing)**

This repository explores the critical impact of memory layout on algorithmic performance. It implements Breadth-First Search (BFS) over two distinct graph representations—a pointer-based adjacency list and a Compressed Sparse Row (CSR) array layout. While both implementations share an identical $O(V+E)$ theoretical complexity, real-world execution time varies by up to 5x purely due to hardware cache interactions, spatial locality, and memory access patterns.

## Getting Started

### 1. Build the Project

Compile the benchmark executable using the provided Makefile:

```bash
make

```

### 2. Generate Test Graphs

Use the Python script to generate graphs of varying topologies (Erdős-Rényi, Grid, Chain, Star) and sizes.
*Example: Generate a random Erdős-Rényi graph with 10,000 vertices and an average degree of 8.*

```bash
python3 scripts/gen_graph.py --kind er --n 10000 --deg 8 --seed 1 --out data/test.txt

```

### 3. Run Benchmarks

Execute BFS using either the `pointer` or `csr` implementation. Use the `--repeat` flag to amortize initialization noise.

```bash
./graph_bench --impl=pointer --graph=data/test.txt --source=0 --repeat=5
./graph_bench --impl=csr --graph=data/test.txt --source=0 --repeat=5

```

### 4. Cache Profiling

Profile low-level memory behavior (L1 and Last-Level cache misses) using Cachegrind:

```bash
valgrind --tool=cachegrind ./graph_bench --impl=pointer --graph=data/test.txt --source=0

```

---

## Experiments Conducted

The provided report details extensive benchmarking across different graph topologies and simulated CPU cache configurations. Key experiments include:

* **Topology-Driven Runtime Analysis:** Compared traversal speeds across Erdős-Rényi (random), Grid (bounded degree), Chain (degree 1), and Star (hub-and-spoke) graphs.
* **Cache Parameter Sweeps:** Utilized Cachegrind (`--D1=size,assoc,line`) to systematically measure the impact of modifying L1 data cache capacity, set associativity, and cache line size on miss rates.
* **Memory Locality Verification:** Tracked D1 and LLd (Last-Level data) cache misses to prove that CSR's contiguous integer arrays eliminate the pointer-chasing overhead and heap fragmentation inherent to adjacency lists.

### Key Insights

1. **CSR dominates high-degree and random graphs:** On Erdős-Rényi graphs, CSR drastically reduces LLd misses by fitting more working sets into the cache and prefetching effectively. Pointer layouts suffer extreme performance degradation as edge counts exceed L2 cache capacity.
2. **Pointer layouts can win on chains:** On strict chain graphs (degree 1), the pointer implementation often outperforms CSR. Because edges are allocated sequentially during graph construction, the hardware prefetcher effectively masks pointer-chasing overhead, while CSR pays an unamortized penalty for `row_ptr` bounds checking.
3. **Hardware-Software-Data Co-design:** Optimizing an algorithm without knowing the underlying data topology is futile. Performance is dictated by the alignment of the data layout, the traversal pattern, and the CPU's cache architecture.

---

## Interesting Things to Try

If you are exploring this repository, here are several phenomenons observed in the study that you can replicate and investigate further:

* **The Grid Shape Anomaly:** Generate grids of the same total vertex count but different dimensions (e.g., `25 x 1600` vs `1600 x 25`). Run them through Cachegrind. You will see that wide-short grids are slower than narrow-tall grids due to cache stride pollution caused by the row-major vertex ID layout.
* **The Pointer Conflict Spike:** Run the pointer implementation on a chain graph and sweep the size `n` from 4000 to 6000. You should observe a massive latency spike around `n=5000`. This is likely a local cache-layout artifact where hot pointer streams perfectly conflict in the cache sets.
* **Cache Line Pollution:** Run a Grid graph using CSR with a very large cache line size (`valgrind --tool=cachegrind --D1=32768,8,128`). Observe how D1 misses actually *increase* compared to a 64-byte line, as the larger fetch pulls in irrelevant adjacent nodes and pollutes the limited L1 capacity.
