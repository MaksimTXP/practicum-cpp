#pragma once

#include "ranges.h"

#include <cstdlib>
#include <vector>

namespace graph {

    using VertexId = size_t; // Определение типа идентификатора вершины
    using EdgeId = size_t;   // Определение типа идентификатора ребра

    template <typename Weight>
    struct Edge {
        std::string name;
        size_t weight_edge;
        VertexId from; // Идентификатор вершины начала ребра
        VertexId to;   // Идентификатор вершины конца ребра
        Weight weight; // Вес ребра
    };

    template <typename Weight>
    class DirectedWeightedGraph {
    private:
        using IncidenceList = std::vector<EdgeId>; // Список инцидентных ребер для каждой вершины
        using IncidentEdgesRange = ranges::Range<typename IncidenceList::const_iterator>; // Диапазон для итерации по инцидентным ребрам

    public:
        DirectedWeightedGraph() = default; // Конструктор по умолчанию
        explicit DirectedWeightedGraph(size_t vertex_count); // Конструктор с количеством вершин
        EdgeId AddEdge(const Edge<Weight>& edge); // Метод для добавления ребра

        size_t GetVertexCount() const; // Метод для получения количества вершин
        size_t GetEdgeCount() const; // Метод для получения количества ребер
        const Edge<Weight>& GetEdge(EdgeId edge_id) const; // Метод для получения ребра по его идентификатору
        IncidentEdgesRange GetIncidentEdges(VertexId vertex) const; // Метод для получения инцидентных ребер вершины

    private:
        std::vector<Edge<Weight>> edges_; // Вектор всех ребер
        std::vector<IncidenceList> incidence_lists_; // Вектор списков инцидентных ребер для каждой вершины
    };

    template <typename Weight>
    DirectedWeightedGraph<Weight>::DirectedWeightedGraph(size_t vertex_count)
        : incidence_lists_(vertex_count) {
        // Конструктор, инициализирующий списки инцидентных ребер для каждой вершины
    }

    template <typename Weight>
    EdgeId DirectedWeightedGraph<Weight>::AddEdge(const Edge<Weight>& edge) {
        edges_.push_back(edge); // Добавление ребра в список всех ребер
        const EdgeId id = edges_.size() - 1; // Получение идентификатора добавленного ребра
        incidence_lists_.at(edge.from).push_back(id); // Добавление идентификатора ребра в список инцидентных ребер для вершины "from"
        return id; // Возврат идентификатора добавленного ребра
    }

    template <typename Weight>
    size_t DirectedWeightedGraph<Weight>::GetVertexCount() const {
        return incidence_lists_.size(); // Возврат количества вершин в графе
    }

    template <typename Weight>
    size_t DirectedWeightedGraph<Weight>::GetEdgeCount() const {
        return edges_.size(); // Возврат количества ребер в графе
    }

    template <typename Weight>
    const Edge<Weight>& DirectedWeightedGraph<Weight>::GetEdge(EdgeId edge_id) const {
        return edges_.at(edge_id); // Возврат ребра по его идентификатору
    }

    template <typename Weight>
    typename DirectedWeightedGraph<Weight>::IncidentEdgesRange
        DirectedWeightedGraph<Weight>::GetIncidentEdges(VertexId vertex) const {
        return ranges::AsRange(incidence_lists_.at(vertex)); // Возврат диапазона инцидентных ребер для вершины
    }

}  // namespace graph
