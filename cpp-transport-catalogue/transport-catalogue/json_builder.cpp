#include "json_builder.h"
#include <stdexcept>
#include <utility>

namespace json {

    Builder::Builder() {
        nodes_stack_.emplace_back(&root_);
    }

    Node Builder::Build() {
        if (root_.IsNull() || nodes_stack_.size() > 1) {
            throw std::logic_error("Build called with incomplete object structure.");
        }
        return root_;
    }

    Builder::KeyContext Builder::Key(std::string key) {
        if (!nodes_stack_.back()->IsMap() || current_key_) {
            throw std::logic_error("Key called in invalid context.");
        }
        current_key_ = std::move(key);
        return BaseContext(*this);
    }

    Builder::BaseContext Builder::Value(Node::Value value) {
        Node* top = nodes_stack_.back();

        if (top->IsMap()) {
            if (!current_key_) {
                throw std::logic_error("Value called without key in dict context.");
            }
            auto& dict = std::get<Dict>(top->GetValue());
            auto pos = dict.emplace(current_key_.value(), Node{});
            current_key_.reset();
            pos.first->second.GetValue() = std::move(value);
        }
        else if (top->IsArray()) {
            auto& array = std::get<Array>(top->GetValue());
            auto type_value = std::visit([](auto&& arg) -> Node {
                return Node(std::forward<decltype(arg)>(arg));
                }, value);
            array.emplace_back(type_value);
        }
        else if (top == &root_ && root_.IsNull()) {
            root_.GetValue() = std::move(value);
        }
        else {
            throw std::logic_error("Value called in invalid context.");
        }
        return *this;
    }

    Builder::DictContext Builder::StartDict() {
        AddObject(Dict());
        return BaseContext(*this);
    }

    Builder::BaseContext Builder::EndDict() {
        if (!nodes_stack_.back()->IsMap()) {
            throw std::logic_error("EndDict called in invalid context.");
        }
        nodes_stack_.pop_back();
        return *this;
    }

    Builder::ArrayContext Builder::StartArray() {
        AddObject(Array());
        return BaseContext(*this);
    }

    Builder::BaseContext Builder::EndArray() {
        if (!nodes_stack_.back()->IsArray()) {
            throw std::logic_error("EndArray called in invalid context.");
        }
        nodes_stack_.pop_back();
        return *this;
    }

    void Builder::AddObject(Node::Value value) {
        Node* top = nodes_stack_.back();
        if (top->IsMap()) {
            if (!current_key_) {
                throw std::logic_error("AddObject called without a key in dict context.");
            }
            auto& dict = std::get<Dict>(top->GetValue());
            auto pos = dict.emplace(current_key_.value(), Node{ std::move(value) });
            current_key_.reset();
            nodes_stack_.emplace_back(&pos.first->second);
        }
        else if (top->IsArray()) {
            auto& array = std::get<Array>(top->GetValue());
            array.emplace_back(std::move(value));
            nodes_stack_.emplace_back(&array.back());
        }
        else if (top == &root_ && root_.IsNull()) {
            root_.GetValue() = std::move(value);
        }
        else {
            throw std::logic_error("AddObject called in invalid context.");
        }
    }

} // namespace json