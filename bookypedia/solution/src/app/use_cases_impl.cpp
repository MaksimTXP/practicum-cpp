#include "use_cases_impl.h"

#include "../domain/author.h"
#include <iostream>

namespace app {
using namespace domain;


void UseCasesImpl::AddAuthor(const std::string& name) {
  auto work = unit_of_work_factory_.CreateUnitOfWork();
  work->Authors().Save({AuthorId::New(), name});
  work->Commit();
}

std::vector<domain::Author> UseCasesImpl::GetAuthors() {
  auto work = unit_of_work_factory_.CreateUnitOfWork();
  return work->Authors().GetAuthors();
}

domain::Author UseCasesImpl::GetAuthorById(const domain::AuthorId& author_id) {
  auto work = unit_of_work_factory_.CreateUnitOfWork();
  return work->Authors().GetAuthorById(author_id);
}

std::optional<domain::Author> UseCasesImpl::GetAuthorByName(const std::string& name) {
  auto work = unit_of_work_factory_.CreateUnitOfWork();
  return work->Authors().GetAuthorByName(name);
}

void UseCasesImpl::DeleteAuthor(const domain::AuthorId& author_id) {
  auto work = unit_of_work_factory_.CreateUnitOfWork();
  const auto author = work->Authors().GetAuthorById(author_id);
  const auto books = work->Books().GetBooksByAuthor(author_id);
  if (!books.empty()) {
    for (const auto& book : books) {
      const auto book_tags = work->BookTags().GetBookTags(book.GetId());
      if (!book_tags.empty()) {
        work->BookTags().Delete(book.GetId());
      }
    }
    work->Books().DeleteByAuthorId(author.GetId());
  }
  work->Authors().Delete(author.GetId());

  work->Commit();

}


void UseCasesImpl::EditAuthor(const domain::AuthorId& author_id, const std::string& name) {
  auto work = unit_of_work_factory_.CreateUnitOfWork();
  work->Authors().Edit(author_id, name);
  work->Commit(); 
  
}


void UseCasesImpl::AddBook(const domain::AuthorId author_id, const std::string& title,
  int publication_year, const std::vector<std::string>& book_tags) {

  auto work = unit_of_work_factory_.CreateUnitOfWork();
  auto book_id = BookId::New();
  work->Books().Save({ book_id, author_id, title, publication_year});

  for (const auto& tag : book_tags) {
    work->BookTags().Save({ book_id, tag });
  }
  work->Commit();
}

std::vector<domain::Book> UseCasesImpl::GetBooks() {
  auto work = unit_of_work_factory_.CreateUnitOfWork();
  return work->Books().GetBooks();
}

std::vector<domain::Book> UseCasesImpl::GetBooksByAuthor(const domain::AuthorId& author_id) {
  auto work = unit_of_work_factory_.CreateUnitOfWork();
  return work->Books().GetBooksByAuthor(author_id);
}

void UseCasesImpl::EditBook(const domain::BookId book_id, const std::string& title,
  int publication_year, const std::vector<std::string>& book_tags) {

  auto work = unit_of_work_factory_.CreateUnitOfWork();

  work->Books().Edit(book_id, title, publication_year);
  
  if (book_tags.empty()) {
    work->BookTags().Delete(book_id);
  }

  else
  
  {
    work->BookTags().Delete(book_id);

    for (const auto& tag : book_tags) {
      work->BookTags().Save(BookTag{ book_id, tag });
    }

  }

  work->Commit();

}

void UseCasesImpl::DeleteBook(const domain::BookId& book_id) {
  auto work = unit_of_work_factory_.CreateUnitOfWork();
 

  work->BookTags().Delete(book_id);
  work->Books().Delete(book_id);
  work->Commit();


}
std::vector<std::string> UseCasesImpl::GetBookTags(const domain::BookId& book_id) const {
  auto work = unit_of_work_factory_.CreateUnitOfWork();
  return work->BookTags().GetBookTags(book_id);

}


}  // namespace app