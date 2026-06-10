const pptxgen = require("pptxgenjs");
const pres = new pptxgen();
pres.layout = "LAYOUT_WIDE";          // 13.3 x 7.5
pres.author = "김도현";
pres.title = "MAPF on a Real Fab OHT Network";

// ---- palette: white background, charcoal ink, one restrained accent ----
const INK = "1F2933", MUTE = "7B8794", ACC = "0E7C86", FAINT = "CBD2D9";
const HEAD = "Georgia", BODY = "Calibri";
const ASSETS = "/sessions/youthful-relaxed-maxwell/mnt/OHT_MAPF/results/assets/";

function sectionTag(s, tag) {
  s.addText(tag.toUpperCase(), { x: 0.7, y: 0.55, w: 11, h: 0.3, margin: 0,
    fontFace: BODY, fontSize: 11, color: ACC, bold: true, charSpacing: 3 });
}
function title(s, t, o = {}) {
  s.addText(t, { x: 0.7, y: o.y ?? 0.95, w: o.w ?? 11.9, h: o.h ?? 1.0, margin: 0,
    fontFace: HEAD, fontSize: o.size ?? 32, color: INK, bold: true, valign: "top" });
}
function keyline(s, t, y = 2.05, o = {}) {
  s.addText(t, { x: 0.7, y, w: o.w ?? 11.9, h: o.h ?? 0.6, margin: 0,
    fontFace: BODY, fontSize: o.size ?? 16, color: o.color ?? INK, valign: "top" });
}
function visualBox(s, label, who, b) {
  s.addShape(pres.shapes.RECTANGLE, { x: b.x, y: b.y, w: b.w, h: b.h,
    fill: { color: "FFFFFF" }, line: { color: FAINT, width: 1.25, dashType: "dash" } });
  s.addText([
    { text: label, options: { fontFace: BODY, fontSize: 13, color: MUTE, italic: true, breakLine: true } },
    { text: who, options: { fontFace: BODY, fontSize: 10.5, color: ACC, bold: true } },
  ], { x: b.x + 0.2, y: b.y + b.h / 2 - 0.45, w: b.w - 0.4, h: 0.9, margin: 0, align: "center", valign: "middle" });
}
function imgBox(s, file, ow, oh, b) {
  const sc = Math.min(b.w / ow, b.h / oh), dw = ow * sc, dh = oh * sc;
  s.addImage({ path: ASSETS + file, x: b.x + (b.w - dw) / 2, y: b.y + (b.h - dh) / 2, w: dw, h: dh });
}
function codeNote(s, t, y) {
  s.addText(t, { x: 0.7, y, w: 11.9, h: 0.4, margin: 0, fontFace: BODY, fontSize: 12.5, color: MUTE, italic: true });
}
function newSlide() { const s = pres.addSlide(); s.background = { color: "FFFFFF" }; return s; }

// ============================================================ 1. INTRO
{ // 1.1 TITLE
  const s = newSlide();
  s.addText("Multi-Agent Path Finding", { x: 0.9, y: 2.2, w: 11.5, h: 0.9, margin: 0, fontFace: HEAD, fontSize: 40, bold: true, color: INK });
  s.addText("on a Real Fab OHT Network", { x: 0.9, y: 3.05, w: 11.5, h: 0.9, margin: 0, fontFace: HEAD, fontSize: 40, bold: true, color: INK });
  s.addText("Data Structure Project", { x: 0.9, y: 4.05, w: 11.5, h: 0.5, margin: 0, fontFace: BODY, fontSize: 18, italic: true, color: ACC });
  s.addShape(pres.shapes.LINE, { x: 0.95, y: 4.75, w: 2.2, h: 0, line: { color: FAINT, width: 1.5 } });
  s.addText("김도현  ·  202214223  ·  전기전자공학과", { x: 0.9, y: 4.95, w: 11.5, h: 0.4, margin: 0, fontFace: "NanumGothic", fontSize: 14, color: MUTE });
}
{ // 1.2 THE PROBLEM
  const s = newSlide(); sectionTag(s, "1 · Intro");
  title(s, "Hundreds of OHTs share one rail");
  keyline(s, "A collision stalls the line — and a stalled line stalls production.");
  imgBox(s, "FAB_OHT.png", 1200, 360, { x: 1.9, y: 3.3, w: 9.5, h: 2.9 });
}

// ============================================================ 2. PROBLEM SETUP
{ // 2.1 MOTION MODEL
  const s = newSlide(); sectionTag(s, "2 · Problem setup");
  title(s, "Discrete-time motion model");
  keyline(s, "Directed graph  ·  one step = move V·Δt or wait  ·  min separation V·Δt");
  imgBox(s, "fig_motion.png", 921, 461, { x: 1.6, y: 3.0, w: 10.1, h: 3.9 });
}
{ // 2.2 VIRTUAL NODES
  const s = newSlide(); sectionTag(s, "2 · Problem setup");
  title(s, "Edge → unit edges (virtual nodes)");
  keyline(s, "Split each edge into unit steps  →  collision = same (node, time).");
  imgBox(s, "fig_virtualnodes.png", 921, 421, { x: 1.6, y: 2.9, w: 10.1, h: 4.1 });
}
{ // 2.3 COLLISION (why time)
  const s = newSlide(); sectionTag(s, "2 · Problem setup");
  title(s, "The conflict: why time is part of the problem");
  imgBox(s, "fig_collision.png", 921, 541, { x: 2.3, y: 2.2, w: 8.7, h: 4.8 });
}

// ============================================================ 3. DATA ACQUISITION
{ // 3.1 WHAT DATA
  const s = newSlide(); sectionTag(s, "3 · Data acquisition");
  title(s, "A real fab network, not a toy grid");
  keyline(s, "SMAT 2022 OHT network — Choi et al., PLOS ONE 2024  ·  2,858 nodes / 3,424 edges");
  imgBox(s, "paper.png", 1426, 1733, { x: 1.4, y: 2.7, w: 4.4, h: 4.4 });
  imgBox(s, "github.png", 3150, 1966, { x: 6.3, y: 3.1, w: 5.9, h: 3.7 });
}
{ // 3.2 WEIGHTED → QUANTIZE
  const s = newSlide(); sectionTag(s, "3 · Data acquisition");
  s.addText([
    { text: "Directed ", options: { color: INK } },
    { text: "weighted", options: { color: ACC, bold: true } },
    { text: " graph", options: { color: INK } },
  ], { x: 0.9, y: 2.7, w: 11.5, h: 0.9, margin: 0, fontFace: HEAD, fontSize: 34, bold: true });
  s.addText("→  quantize to integer steps", { x: 0.9, y: 3.7, w: 11.5, h: 0.8, margin: 0, fontFace: HEAD, fontSize: 34, bold: true, color: INK });
}
{ // 3.3a PREPROCESSING CODE — stage 1
  const s = newSlide(); sectionTag(s, "3 · Data acquisition  ·  preprocessing");
  title(s, "Stage 1 — quantize (convert_weights)");
  keyline(s, "distance → integer steps: round(d / V), clamp ≥ 1.  Coding it is what lets us measure & tune the error.", 2.0, { size: 15 });
  imgBox(s, "python_stage1_convert.png", 879, 666, { x: 2.6, y: 2.7, w: 8.1, h: 4.3 });
}
{ // 3.3b PREPROCESSING CODE — stage 2
  const s = newSlide(); sectionTag(s, "3 · Data acquisition  ·  preprocessing");
  title(s, "Stage 2 — expand (expand_unweighted)");
  keyline(s, "each weight-w edge → a w−1 virtual-node chain of unit edges.", 2.0, { size: 15 });
  imgBox(s, "python_stage2_expand.png", 1006, 897, { x: 3.2, y: 2.6, w: 6.9, h: 4.3 });
}
{ // 3.4 QUANTIZATION TABLE
  const s = newSlide(); sectionTag(s, "3 · Data acquisition  ·  tuning result");
  title(s, "Quantization: smaller graph vs. more error");
  const hdr = (t) => ({ text: t, options: { fill: { color: INK }, color: "FFFFFF", bold: true, fontFace: BODY, fontSize: 12.5, align: "center", valign: "middle" } });
  const c = (t, a = false) => ({ text: t, options: { color: a ? ACC : INK, bold: a, fontFace: BODY, fontSize: 13, align: "center", valign: "middle" } });
  const row = (v, mw, ea, em, pe, n, a = false) => [c(v, a), c(mw, a), c(ea, a), c(em, a), c(pe, a), c(n, a)];
  s.addTable([
    [hdr("V"), hdr("max w"), hdr("edge avg err"), hdr("edge max err"), hdr("PATH avg err"), hdr("expanded nodes")],
    row("1000", "14", "11.8%", "33.3%", "1.6%", "10,312"),
    row("1417", "10", "16.8%", "60.7%", "2.8%", "7,578"),
    row("500  (chosen)", "28", "7.1%", "20.0%", "3.6%", "21,178", true),
    row("1800", "8", "31.3%", "104%", "8.1%", "6,146"),
  ], { x: 0.8, y: 2.45, w: 11.7, h: 2.9, rowH: 0.62, colW: [2.0, 1.3, 2.1, 2.1, 2.1, 2.1], border: { pt: 0.5, color: FAINT }, align: "center", valign: "middle" });
  keyline(s, "Path error is the real metric — edges over/under-round, but a path sums many edges so errors cancel.  Chosen V=500: max w = H = 28.", 5.6, { color: MUTE, size: 12.5, h: 0.8 });
}

// ============================================================ 4. THE BIG PICTURE (why needed)
{ // 4.1 PP
  const s = newSlide(); sectionTag(s, "4 · The big picture");
  title(s, "Prioritized Planning");
  keyline(s, "Plan agents one at a time, in priority order.  Each committed path becomes occupancy the next agent must avoid.");
  imgBox(s, "fig_pp.png", 921, 481, { x: 1.6, y: 2.95, w: 10.1, h: 4.0 });
}
{ // 4.2 THREE ROLES
  const s = newSlide(); sectionTag(s, "4 · The big picture");
  title(s, "Three roles, receding horizon");
  keyline(s, "Environment runs time (plan H, commit C, replan)  ·  PP orders agents  ·  PathFinder routes one agent.");
  imgBox(s, "fig_roles.png", 921, 441, { x: 2.3, y: 2.85, w: 8.7, h: 4.1 });
}
{ // 4.3 BFS + TEG (pathfinder)
  const s = newSlide(); sectionTag(s, "4 · The big picture  ·  the PathFinder");
  title(s, "Pathfinding: BFS over a Time-Expanded Graph");
  keyline(s, "Replicate the graph per step (TEG)  ·  avoid occupancy via isFree  ·  space = V × (H+1)");
  imgBox(s, "fig_teg.png", 921, 621, { x: 2.6, y: 2.55, w: 8.1, h: 4.5 });
}
{ // 4.4 WHAT THIS NEEDS (why)
  const s = newSlide(); sectionTag(s, "4 · The big picture  ·  what it needs");
  title(s, "What this picture needs — and why");
  const hdr = (t) => ({ text: t, options: { fill: { color: INK }, color: "FFFFFF", bold: true, fontFace: BODY, fontSize: 14, valign: "middle" } });
  const c = (t, a = false) => ({ text: t, options: { color: a ? ACC : INK, bold: a, fontFace: BODY, fontSize: 14, valign: "middle" } });
  s.addTable([
    [hdr("component"), hdr("why it's needed")],
    [c("BFS", true), c("expand the frontier level by level")],
    [c("TEG & layout", true), c("represent the graph (copied per time step)")],
    [c("reservation", true), c("store every path's occupancy, queried constantly")],
    [c("priority pull", true), c("PP takes the next agent by priority")],
  ], { x: 1.6, y: 2.7, w: 10.1, h: 2.9, rowH: 0.62, colW: [3.4, 6.7], border: { pt: 0.5, color: FAINT }, valign: "middle" });
  keyline(s, "Why each is needed — here.  The next section: what data structure realizes each, and why that one.", 5.8, { color: MUTE, size: 13 });
}

// ============================================================ 5. DATA STRUCTURES (what / why that)
{ // 5.1 COMPONENT → DATA STRUCTURE
  const s = newSlide(); sectionTag(s, "5 · Data structures");
  title(s, "Each component → a data structure");
  const hdr = (t) => ({ text: t, options: { fill: { color: INK }, color: "FFFFFF", bold: true, fontFace: BODY, fontSize: 14, valign: "middle" } });
  const c = (t, a = false) => ({ text: t, options: { color: a ? ACC : INK, bold: a, fontFace: BODY, fontSize: 14, valign: "middle" } });
  s.addTable([
    [hdr("need"), hdr("data structure")],
    [c("BFS frontier"), c("queue  (FIFO)", true)],
    [c("TEG & layout"), c("graph  (adjacency list, on dynamic_array)", true)],
    [c("reservation"), c("dynamic_array (per node) + AVL tree (intervals)", true)],
    [c("priority pull"), c("min-heap", true)],
  ], { x: 1.6, y: 2.7, w: 10.1, h: 2.9, rowH: 0.62, colW: [3.4, 6.7], border: { pt: 0.5, color: FAINT }, valign: "middle" });
  keyline(s, "Emphasis three — graph · AVL · min-heap.  (queue · linked_list · dynamic_array: one breath in the next map.)", 5.8, { color: MUTE, size: 13 });
}
{ // 5.2 DEPENDENCY MAP
  const s = newSlide(); sectionTag(s, "5 · Data structures");
  title(s, "Built from class data structures");
  keyline(s, "Two tiers: building blocks → structures.  Every compare delegates to Data::operator<.");
  imgBox(s, "fig_dependency.png", 921, 601, { x: 2.3, y: 2.6, w: 8.7, h: 4.2 });
}
{ // 5.3a DATA CONTRACT (base)
  const s = newSlide(); sectionTag(s, "5 · Data structures  ·  the Data contract");
  title(s, "Every structure holds Data*");
  keyline(s, "virtual ~Data → polymorphic delete (owner deletes, child cleans up).  virtual operator< → comparison delegated.", 2.0, { size: 15 });
  imgBox(s, "data_h.png", 969, 664, { x: 2.7, y: 2.6, w: 7.9, h: 4.4 });
}
{ // 5.3b CONCRETE CHILDREN (operator< overloading)
  const s = newSlide(); sectionTag(s, "5 · Data structures  ·  operator< overloading");
  title(s, "The children override operator<");
  keyline(s, "Interval (by start) makes a generic AVL an interval tree · Agent (by priority) makes a generic heap a priority queue.", 2.0, { size: 15 });
  imgBox(s, "interval.png", 759, 541, { x: 0.9, y: 2.7, w: 5.7, h: 4.2 });
  imgBox(s, "agent_operator_overloading.png", 896, 179, { x: 6.9, y: 3.7, w: 5.6, h: 1.5 });
  s.addText("Agent : public Data — operator< by priority", { x: 6.9, y: 3.2, w: 5.6, h: 0.4, margin: 0, fontFace: BODY, fontSize: 13, color: MUTE, italic: true });
}
{ // 5.4a GRAPH (figure)
  const s = newSlide(); sectionTag(s, "5 · Data structures  ·  graph ★");
  title(s, "Graph — adjacency list");
  keyline(s, "layout & TEG as adjacency lists — sparse, O(V+E), fast neighbor scan.");
  imgBox(s, "fig_adjlist.png", 921, 481, { x: 1.6, y: 2.95, w: 10.1, h: 4.0 });
}
{ // 5.4b GRAPH (code)
  const s = newSlide(); sectionTag(s, "5 · Data structures  ·  graph ★ code");
  title(s, "Graph — adjacency on dynamic_array");
  // code on the left, vertically centered
  imgBox(s, "graph.png", 1009, 1352, { x: 0.7, y: 1.9, w: 5.7, h: 5.2 });
  // right column: title-ish lead + bullets, vertically centered against the code
  s.addText("What the interface gives", { x: 7.1, y: 2.5, w: 5.4, h: 0.4, margin: 0, fontFace: BODY, fontSize: 16, bold: true, color: INK });
  s.addText([
    { text: "addEdge(from, to)", options: { fontFace: BODY, fontSize: 15, color: ACC, bold: true, bullet: { code: "2022" }, breakLine: true } },
    { text: "appends a neighbor to a per-node dynamic_array", options: { fontFace: BODY, fontSize: 13, color: MUTE, breakLine: true, paraSpaceAfter: 10 } },
    { text: "neighbor(u, i)", options: { fontFace: BODY, fontSize: 15, color: ACC, bold: true, bullet: { code: "2022" }, breakLine: true } },
    { text: "i-th out-neighbor of u — O(1) indexed access", options: { fontFace: BODY, fontSize: 13, color: MUTE, breakLine: true, paraSpaceAfter: 10 } },
    { text: "one Graph type, two uses", options: { fontFace: BODY, fontSize: 15, color: ACC, bold: true, bullet: { code: "2022" }, breakLine: true } },
    { text: "the layout, and the TEG over (node, t)", options: { fontFace: BODY, fontSize: 13, color: MUTE } },
  ], { x: 7.1, y: 3.0, w: 5.4, h: 3.6, margin: 0, valign: "top" });
}
{ // 5.5a AVL (figure)
  const s = newSlide(); sectionTag(s, "5 · Data structures  ·  AVL ★");
  title(s, "Reservation table = AVL tree of intervals");
  keyline(s, "Per-node occupancy as sorted, coalesced intervals.  Recorded by PP, read by BFS via isFree.", 2.0, { size: 15 });
  imgBox(s, "fig_reservation.png", 921, 541, { x: 0.6, y: 2.6, w: 7.2, h: 4.2 });
  imgBox(s, "fig_coalesce.png", 921, 281, { x: 7.9, y: 3.0, w: 5.0, h: 2.0 });
}
{ // 5.5b WHY AVL
  const s = newSlide(); sectionTag(s, "5 · Data structures  ·  AVL ★ why");
  title(s, "Why a balanced tree — not array / hash");
  const hdr = (t) => ({ text: t, options: { fill: { color: INK }, color: "FFFFFF", bold: true, fontFace: BODY, fontSize: 13, valign: "middle" } });
  const c = (t, g = false) => ({ text: t, options: { color: g ? ACC : INK, bold: g, fontFace: BODY, fontSize: 13, valign: "middle" } });
  const cb = (t) => ({ text: t, options: { color: "B23A3A", fontFace: BODY, fontSize: 13, valign: "middle" } });
  s.addTable([
    [hdr("candidate"), hdr("insert"), hdr("ordered / range query"), hdr("verdict")],
    [c("sorted array"), cb("O(n) shift"), c("O(log n)"), cb("insert too slow")],
    [c("hash table"), c("O(1)"), cb("none — no order"), cb("can't do \"next ≥ t\"")],
    [c("unbalanced BST"), c("O(log n) avg"), c("O(log n) avg"), cb("O(n) worst case")],
    [c("AVL tree", true), c("O(log n)", true), c("O(log n) + pred/succ", true), c("balanced — chosen", true)],
  ], { x: 0.7, y: 2.9, w: 11.9, h: 2.5, rowH: 0.58, colW: [2.6, 2.6, 3.7, 3.0], border: { pt: 0.5, color: FAINT }, valign: "middle" });
  keyline(s, "Workload: insert occupancy + find first interval ≥ t, constantly.  AVL gives both fast insert AND sorted neighbor queries.", 6.0, { color: MUTE, size: 13 });
}
{ // 5.5c COALESCING CODE
  const s = newSlide(); sectionTag(s, "5 · Data structures  ·  AVL ★ coalescing");
  title(s, "Coalescing — reserve() folds neighbors");
  // reserve() body on the left, vertically centered
  imgBox(s, "reservation_tree.png", 976, 1616, { x: 0.7, y: 1.85, w: 5.7, h: 5.3 });
  s.addText("Who does what", { x: 7.1, y: 2.4, w: 5.4, h: 0.4, margin: 0, fontFace: BODY, fontSize: 16, bold: true, color: INK });
  s.addText([
    { text: "Interval :: touches / merge", options: { fontFace: BODY, fontSize: 15, color: ACC, bold: true, bullet: { code: "2022" }, breakLine: true } },
    { text: "the geometry — do two ranges touch, and their union", options: { fontFace: BODY, fontSize: 13, color: MUTE, breakLine: true, paraSpaceAfter: 10 } },
    { text: "AVL :: predecessor / successor", options: { fontFace: BODY, fontSize: 15, color: ACC, bold: true, bullet: { code: "2022" }, breakLine: true } },
    { text: "find touching neighbors, then remove / insert", options: { fontFace: BODY, fontSize: 13, color: MUTE, breakLine: true, paraSpaceAfter: 10 } },
    { text: "reserve() orchestrates", options: { fontFace: BODY, fontSize: 15, color: ACC, bold: true, bullet: { code: "2022" }, breakLine: true } },
    { text: "absorbs touching neighbors — the AVL never knows merge", options: { fontFace: BODY, fontSize: 13, color: MUTE } },
  ], { x: 7.1, y: 2.9, w: 5.4, h: 3.8, margin: 0, valign: "top" });
}
{ // 5.6a MIN-HEAP (figure)
  const s = newSlide(); sectionTag(s, "5 · Data structures  ·  min-heap ★");
  title(s, "Agent priority heap = min-heap");
  keyline(s, "PP pulls the next agent by priority — repeated extract-min, O(log n).  Why heap: array insert/extract is O(n).");
  imgBox(s, "fig_minheap.png", 921, 501, { x: 1.6, y: 2.85, w: 10.1, h: 3.6 });
}
{ // 5.6b MIN-HEAP (code, encapsulation)
  const s = newSlide(); sectionTag(s, "5 · Data structures  ·  min-heap ★ code");
  title(s, "Encapsulation by private inheritance");
  keyline(s, "MinHeap privately inherits DynamicArray — hides get/set/pushBack, so the heap invariant can't be broken.", 2.0, { size: 15 });
  imgBox(s, "minheap.png", 842, 924, { x: 3.2, y: 2.6, w: 6.9, h: 4.5 });
}

// ============================================================ 6. RESULTS
{ // 6.1 RESULTS
  const s = newSlide(); sectionTag(s, "6 · Results");
  title(s, "Collision-free at fab scale");
  keyline(s, "21,178 nodes · 500 agents · H=1000 · 1,596 goals · throughput 0.798 · collision-free, leak-free");
  imgBox(s, "main_cpp_result.png", 1086, 688, { x: 2.4, y: 2.9, w: 8.1, h: 4.0 });
}

// ============================================================ 7. CONCLUSION
{ // 7.1 TAKEAWAY (dark)
  const s = newSlide(); s.background = { color: INK };
  s.addText("CONCLUSION", { x: 0.7, y: 0.55, w: 6, h: 0.3, margin: 0, fontFace: BODY, fontSize: 11, color: "8FD3D8", bold: true, charSpacing: 3 });
  s.addText("Choosing the data structures is what\ndecides whether it works — and how far it scales.", { x: 0.9, y: 2.5, w: 11.5, h: 1.8, margin: 0, fontFace: HEAD, fontSize: 30, bold: true, color: "FFFFFF", lineSpacingMultiple: 1.12 });
  s.addText("Correctness rests on the reservation table's coalescing invariant; scale is bounded by data layout, not the search.", { x: 0.9, y: 4.7, w: 11.0, h: 0.8, margin: 0, fontFace: BODY, fontSize: 15, color: "CADCFC" });
}

pres.writeFile({ fileName: "OHT_MAPF_presentation.pptx" }).then(f => console.log("WROTE", f));
