//
// Created by DohyunKim on 26. 6. 6..
//

#include "planner.h"

#include <cassert>

// ── Path ──────────────────────────────────────────────────────────────────

Path::Path() : nodes(nullptr), len(0) {}

Path::Path(int length) : nodes(nullptr), len(length) {
    assert(length >= 0 && "Path: negative length");
    if (length > 0) {
        nodes = new int[length];
        for (int i = 0; i < length; ++i) nodes[i] = -1;
    }
}

Path::~Path() {
    delete[] nodes;
}

Path::Path(Path&& other) noexcept : nodes(other.nodes), len(other.len) {
    other.nodes = nullptr;
    other.len = 0;
}

Path& Path::operator=(Path&& other) noexcept {
    if (this != &other) {
        delete[] nodes;
        nodes = other.nodes;
        len = other.len;
        other.nodes = nullptr;
        other.len = 0;
    }
    return *this;
}

int Path::at(int t) const {
    assert(t >= 0 && t < len && "Path::at out of range");
    return nodes[t];
}

void Path::set(int t, int node) {
    assert(t >= 0 && t < len && "Path::set out of range");
    nodes[t] = node;
}

int Path::length() const { return len; }

bool Path::empty() const { return len == 0; }
