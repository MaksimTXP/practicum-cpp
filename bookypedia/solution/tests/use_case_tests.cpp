#include <catch2/catch_test_macros.hpp>
#include <algorithm>


#include "../src/app/use_cases_impl.h"
#include "../src/domain/author.h"
#include "../src/domain/book_tags.h"

namespace {

struct MockAuthorRepository : domain::AuthorRepository {
    std::vector<domain::Author> saved_authors;

    void Save(const domain::Author& author) override {
        saved_authors.emplace_back(author);
    }

    void Delete(const domain::AuthorId& author_id) override {
      std::erase_if(saved_authors, [&](const auto& author) {
        return author.GetId() == author_id;
        });
    }

    void Edit(const domain::AuthorId& author_id, const std::string& name) override {
      auto it = std::find_if(saved_authors.begin(), saved_authors.end(),
        [&](const auto& author) { return author.GetId() == author_id; 
        });
      it->EditName(name);
    }

    domain::Author GetAuthorById(const domain::AuthorId& author_id) override {
      auto it = std::find_if(saved_authors.begin(), saved_authors.end(),
        [&](const auto& author) { 
          return author.GetId() == author_id; 
        });

      return *it;
    }

    std::optional<domain::Author> GetAuthorByName(const std::string& name) override {
      auto it = std::find_if(saved_authors.begin(), saved_authors.end(),
        [&](const auto& author) { return author.GetName() == name; 
        });

      if (it == saved_authors.end()) {
        return std::nullopt;
      }
      return *it;
    }

    std::vector<domain::Author> GetAuthors() override {
      return saved_authors;
    }


};


struct MockBookRepository : domain::BookRepository {

  std::vector<domain::Book> saved_books;

  void Save(const domain::Book& book) override {
    saved_books.emplace_back(book);
  }

  void Delete(const domain::BookId& book_id) {
  
    auto it = std::find_if(saved_books.begin(), saved_books.end(),
      [&](const auto& book) { 
        return book.GetId() == book_id; 
      });

  }

  std::vector<domain::Book>  GetBooks() override {
    return saved_books;
  }

  std::vector<domain::Book> GetBooksByAuthor(const domain::AuthorId& author_id) override {
    std::vector<domain::Book> books = saved_books;
    std::erase_if(books, [&](const auto& book) {
      return book.GetAuthorId() != author_id;
      });
    return books;
  }
 
  virtual void DeleteByAuthorId(const domain::AuthorId& author_id) override {
    std::erase_if(saved_books, [&](const auto& book) {
      return book.GetAuthorId() == author_id;
      });
  }

  void Edit(const domain::BookId book_id, const std::string& title, int publication_year) override {
    auto it = std::find_if(saved_books.begin(), saved_books.end(),
      [&](const auto& book) { 
        return book.GetId() == book_id; 
      });
    it->EditTitle(title);
    it->EditPublicationYear(publication_year);
  }


};

struct MockBookTagRepository : domain::BookTagRepository {
  
  std::vector<domain::BookTag> saved_book_tag;

  void Save(const domain::BookTag& book_tag) {
    saved_book_tag.emplace_back(book_tag);
  }

  void Delete(const domain::BookId& book_id) {

    auto it = std::find_if(saved_book_tag.begin(), saved_book_tag.end(),
      [&](const auto& book_tag) {
        return book_tag.GetBookId() == book_id;
      });
  }

  std::vector<std::string> GetBookTags(const domain::BookId& book_id) const {
    std::vector<std::string> book_tags = {};
    for (const auto& book_tag : saved_book_tag) {
      if (book_tag.GetBookId() == book_id) {
        book_tags.push_back(book_tag.GetTag());
      }
    }
    return book_tags;
  }

};


struct MockUnitOfWork : app::UnitOfWork {
  MockAuthorRepository& authors;
  MockBookRepository& books;
  MockBookTagRepository& book_tags;

  MockUnitOfWork(MockAuthorRepository& authors, MockBookRepository& books , MockBookTagRepository& book_tags) :
    authors(authors),
    books(books),
    book_tags(book_tags)
  {}

  virtual void Commit() {}

  virtual domain::AuthorRepository& Authors() {
    return authors;
  }

  virtual domain::BookRepository& Books() {
    return books;
  }

  virtual domain::BookTagRepository& BookTags() {
    return book_tags;
  }

};

struct MockUnitOfWorkFactory : app::UnitOfWorkFactory {

  MockAuthorRepository& authors;
  MockBookRepository& books;
  MockBookTagRepository& book_tags;

  MockUnitOfWorkFactory(MockAuthorRepository& authors, MockBookRepository& books, MockBookTagRepository book_tags) :
    authors(authors),
    books(books),
    book_tags(book_tags)
  {}

  virtual std::unique_ptr<app::UnitOfWork> CreateUnitOfWork() {
    return std::make_unique<MockUnitOfWork>(authors, books, book_tags);
  }
};


struct Fixture {

    MockAuthorRepository authors;
    MockBookRepository books;
    MockBookTagRepository book_tags;
    MockUnitOfWorkFactory unit_of_work_factory{ authors, books, book_tags };

};

}  // namespace

SCENARIO_METHOD(Fixture, "Add Author") {
  GIVEN("Use cases") {
    app::UseCasesImpl use_cases{ unit_of_work_factory };

    WHEN("Adding an author") {
      const auto author_name = "Joanne Rowling";
      use_cases.AddAuthor(author_name);

      THEN("author with the specified name is saved to repository") {
        REQUIRE(authors.saved_authors.size() == 1);
        CHECK(authors.saved_authors.at(0).GetName() == author_name);
        CHECK(authors.saved_authors.at(0).GetId() != domain::AuthorId{});
      }
    }
  }
}

