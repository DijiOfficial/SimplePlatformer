#pragma once
// QuadTree.h
// Single-header templated quadtree suitable for 2D AABB broad-phase usage.
// Put this file in your project (e.g. include/Physics/QuadTree.h) and include it where needed.
//
// Usage notes:
//  - This file is a full template implementation (no .inl/.cpp required).
//  - Payload type T can be a pointer (Collider*), an index (size_t), or any small handle.
//  - The Insert() policy places an item into a child only if the item's rect *fully fits*
 //   inside that child. Items that span multiple children remain in the parent node.
//  - Query(range, out) returns payloads whose rect intersects `range`.
//  - Templated to keep compile-time flexibility and avoid linker issues.

#include <memory>
#include <vector>
#include <algorithm>
#include <SFML/Graphics/Rect.hpp>
#include <cstddef>

namespace diji
{

    template<typename T>
    struct QuadTreeItem
    {
        sf::FloatRect rect;
        T payload;

        QuadTreeItem() = default;
        QuadTreeItem(const sf::FloatRect& r, const T& p) : rect(r), payload(p) {}
    };

    template<typename T>
    class QuadTree
    {
    public:
        // bounds: world rectangle for this node.
        // capacity: how many items a leaf keeps before subdividing.
        // maxDepth: maximum allowed depth (root depth = 0).
        QuadTree(const sf::FloatRect& bounds, int capacity = 6, int maxDepth = 6);
        ~QuadTree() = default;

        // Clear all items and child nodes.
        void Clear();

        // Insert an item. Returns true if inserted (should be true unless item is outside root bounds).
        bool Insert(const QuadTreeItem<T>& item);

        // Query: append payloads of items whose rect intersects 'range' to 'out'.
        void Query(const sf::FloatRect& range, std::vector<T>& out) const;

        // Convenience: number of items directly stored in this node (does not include children).
        std::size_t LocalItemCount() const { return m_Items.size(); }

        // Debug helper: check if node is leaf
        bool IsLeaf() const { return !m_NW; }

    private:
        // Non-copyable for simplicity
        QuadTree(const QuadTree&) = delete;
        QuadTree& operator=(const QuadTree&) = delete;

        // Internal recursive helpers
        void Subdivide();
        bool InsertIntoChildren(const QuadTreeItem<T>& item);
        void QueryInternal(const sf::FloatRect& range, std::vector<T>& out) const;

        // Helper: does rect A fully fit inside rect B?
        static bool FitsIn(const sf::FloatRect& inner, const sf::FloatRect& outer)
        {
            return (inner.position.x >= outer.position.x) &&
                   (inner.position.y >= outer.position.y) &&
                   (inner.position.x + inner.size.x <= outer.position.x + outer.size.x) &&
                   (inner.position.y + inner.size.y <= outer.position.y + outer.size.y);
        }

    private:
        sf::FloatRect m_Bounds;
        int m_Capacity;
        int m_MaxDepth;
        int m_Depth;

        std::vector< QuadTreeItem<T> > m_Items;

        // children: NW, NE, SW, SE
        std::unique_ptr< QuadTree<T> > m_NW, m_NE, m_SW, m_SE;
    };

    //
    // Implementation (templated — must live in header)
    //

    template<typename T>
    QuadTree<T>::QuadTree(const sf::FloatRect& bounds, int capacity, int maxDepth)
        : m_Bounds(bounds),
          m_Capacity(std::max(1, capacity)),
          m_MaxDepth(std::max(0, maxDepth)),
          m_Depth(0),
          m_Items()
    {}

    template<typename T>
    void QuadTree<T>::Clear()
    {
        m_Items.clear();
        if (m_NW)
        {
            m_NW->Clear(); m_NE->Clear(); m_SW->Clear(); m_SE->Clear();
            m_NW.reset(); m_NE.reset(); m_SW.reset(); m_SE.reset();
        }
    }

    template<typename T>
    void QuadTree<T>::Subdivide()
    {
        // create four children with same capacity and with depth incremented
        const float hw = m_Bounds.size.x * 0.5f;
        const float hh = m_Bounds.size.y * 0.5f;
        const float x = m_Bounds.position.x;
        const float y = m_Bounds.position.y;

        sf::FloatRect rectNW(x,       y,       hw, hh);
        sf::FloatRect rectNE(x + hw,  y,       hw, hh);
        sf::FloatRect rectSW(x,       y + hh,  hw, hh);
        sf::FloatRect rectSE(x + hw,  y + hh,  hw, hh);

        // children inherit the same capacity and maxDepth but we need to track depth.
        m_NW = std::make_unique< QuadTree<T> >(rectNW, m_Capacity, m_MaxDepth);
        m_NE = std::make_unique< QuadTree<T> >(rectNE, m_Capacity, m_MaxDepth);
        m_SW = std::make_unique< QuadTree<T> >(rectSW, m_Capacity, m_MaxDepth);
        m_SE = std::make_unique< QuadTree<T> >(rectSE, m_Capacity, m_MaxDepth);

        // child's depth is parent's depth + 1 — we store depth in the object by modifying member directly.
        // (We could add constructor param for depth, but modifying here is simpler.)
        m_NW->m_Depth = m_Depth + 1;
        m_NE->m_Depth = m_Depth + 1;
        m_SW->m_Depth = m_Depth + 1;
        m_SE->m_Depth = m_Depth + 1;
    }

    template<typename T>
    bool QuadTree<T>::InsertIntoChildren(const QuadTreeItem<T>& item)
    {
        // Insert into a child only if the item's rect fully fits inside that child's bounds.
        // This prevents duplication when items span multiple children.
        if (FitsIn(item.rect, m_NW->m_Bounds)) { return m_NW->Insert(item); }
        if (FitsIn(item.rect, m_NE->m_Bounds)) { return m_NE->Insert(item); }
        if (FitsIn(item.rect, m_SW->m_Bounds)) { return m_SW->Insert(item); }
        if (FitsIn(item.rect, m_SE->m_Bounds)) { return m_SE->Insert(item); }
        return false;
    }

    template<typename T>
    bool QuadTree<T>::Insert(const QuadTreeItem<T>& item)
    {
        // Reject if completely outside this node's bounds (no partial insertion)
        if (!m_Bounds.intersects(item.rect))
            return false;

        // If this is a leaf and has capacity, store here
        if (!m_NW && static_cast<int>(m_Items.size()) < m_Capacity)
        {
            m_Items.push_back(item);
            return true;
        }

        // If leaf and can subdivide (depth < maxDepth), create children
        if (!m_NW && m_Depth < m_MaxDepth)
        {
            Subdivide();

            // Move existing items down to children when they fully fit; otherwise keep here.
            std::vector< QuadTreeItem<T> > remaining;
            remaining.reserve(m_Items.size());
            for (const auto& e : m_Items)
            {
                if (!InsertIntoChildren(e))
                    remaining.push_back(e);
            }
            m_Items.swap(remaining);
        }

        // Try to insert into a child (item must fully fit into that child)
        if (m_NW)
        {
            if (InsertIntoChildren(item)) return true;
        }

        // Otherwise keep item in this node
        m_Items.push_back(item);
        return true;
    }

    template<typename T>
    void QuadTree<T>::QueryInternal(const sf::FloatRect& range, std::vector<T>& out) const
    {
        if (!m_Bounds.intersects(range)) return;

        // Check items in this node
        for (const auto& it : m_Items)
        {
            if (it.rect.intersects(range))
                out.push_back(it.payload);
        }

        // Recurse into children if present
        if (!m_NW) return;
        m_NW->QueryInternal(range, out);
        m_NE->QueryInternal(range, out);
        m_SW->QueryInternal(range, out);
        m_SE->QueryInternal(range, out);
    }

    template<typename T>
    void QuadTree<T>::Query(const sf::FloatRect& range, std::vector<T>& out) const
    {
        QueryInternal(range, out);
    }

}
