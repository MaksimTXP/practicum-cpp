#include "json.h"
#include <optional>

namespace json {

	class Builder {

	private:

		class BaseContext;
		class DictContext;
		class ArrayContext;
		class KeyContext;

	public:

		Builder();
		Node Build();
		KeyContext Key(std::string key);
		BaseContext Value(Node::Value value);
		DictContext StartDict();
		BaseContext EndDict();
		ArrayContext StartArray();
		BaseContext EndArray();

	private:

		void AddObject(Node::Value value);

		Node root_;
		std::vector<Node*> nodes_stack_;
		std::optional<std::string> current_key_;

		class BaseContext {
		public:

			BaseContext(Builder& builder) : builder_(builder) {}

			Node Build() {
				return builder_.Build();
			}

			KeyContext Key(std::string key) {
				return builder_.Key(std::move(key));
			}

			BaseContext Value(Node::Value value) {
				return builder_.Value(std::move(value));
			}

			DictContext StartDict() {
				return builder_.StartDict();
			}

			ArrayContext StartArray() {
				return builder_.StartArray();
			}

			BaseContext EndDict() {
				return builder_.EndDict();
			}

			BaseContext  EndArray() {
				return builder_.EndArray();
			}

		private:
			Builder& builder_;
		};

		class DictContext : public BaseContext {
		public:
			DictContext(BaseContext base) : BaseContext(base) {}

			Node Build() = delete;
			BaseContext Value(Node::Value value) = delete;
			BaseContext EndArray() = delete;
			DictContext StartDict() = delete;
			ArrayContext StartArray() = delete;
		};

		class ArrayContext : public BaseContext {
		public:
			ArrayContext(BaseContext base) : BaseContext(base) {}
			ArrayContext Value(Node::Value value) {
				return BaseContext::Value(std::move(value));
			}
			Node Build() = delete;
			KeyContext Key(std::string key) = delete;
			BaseContext EndDict() = delete;
		};

		class KeyContext : public BaseContext {
		public:
			KeyContext(BaseContext base) : BaseContext(base) {}
			DictContext Value(Node::Value value) {
				return BaseContext::Value(std::move(value));
			}
			Node Build() = delete;
			KeyContext Key(std::string key) = delete;
			BaseContext EndDict() = delete;
			BaseContext EndArray() = delete;
		};
	};

}  // namespace json