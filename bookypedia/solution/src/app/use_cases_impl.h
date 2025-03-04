#pragma once
#include "../domain/author_fwd.h"
#include "use_cases.h"
#include "unit_of_work.h"

namespace app {

class UseCasesImpl : public UseCases {
public:
  explicit UseCasesImpl(UnitOfWorkFactory& unit_of_work_factory)
    : unit_of_work_factory_(unit_of_work_factory) {}


  void AddAuthor(const std::string& name) override;
  std::vector<domain::Author> GetAuthors() override;
  domain::Author GetAuthorById(const domain::AuthorId& author_id) override;
  std::optional<domain::Author> GetAuthorByName(const std::string& name) override;
  void DeleteAuthor(const domain::AuthorId& author_id) override;
  void EditAuthor(const domain::AuthorId& author_id, const std::string& name) override;

  void AddBook(const domain::AuthorId author_id, const std::string& title,
    int publication_year, const std::vector<std::string>& book_tags) override;
  std::vector<domain::Book> GetBooks() override;
  std::vector<domain::Book> GetBooksByAuthor(const domain::AuthorId& author_id) override;
  void EditBook(const domain::BookId book_id, const std::string& title,
    int publication_year, const std::vector<std::string>& book_tags) override;
  void DeleteBook(const domain::BookId& book_id) override;
  std::vector<std::string> GetBookTags(const domain::BookId& book_id) const override;


private:

  UnitOfWorkFactory& unit_of_work_factory_;
};

}  // namespace app
