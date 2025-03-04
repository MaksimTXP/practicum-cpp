#include "postgres.h"

#include <pqxx/zview.hxx>
#include <pqxx/pqxx>
#include <string>
#include <sstream>
#include <iostream>


namespace postgres {

using namespace std::literals;
using pqxx::operator"" _zv;

void AuthorRepositoryImpl::Save(const domain::Author& author) {
    work_.exec_params(
        R"(
        INSERT INTO authors (id, name) VALUES ($1, $2)
        ON CONFLICT (id) DO UPDATE SET name=$2;
        )"_zv,
        author.GetId().ToString(), author.GetName());
}

void AuthorRepositoryImpl::Delete(const domain::AuthorId& author_id) {
 
  work_.exec_params(R"(DELETE FROM authors WHERE id = $1)", author_id.ToString());
}

void AuthorRepositoryImpl::Edit(const domain::AuthorId& author_id, const std::string& name) {
  work_.exec_params(R"(UPDATE authors SET name = $1 WHERE id = $2)", name, author_id.ToString());

}

domain::Author AuthorRepositoryImpl::GetAuthorById(const domain::AuthorId& author_id) {

  const auto query_text = "SELECT id, name FROM authors WHERE id = " + work_.quote(author_id.ToString()) + ";";
  const auto& [_, name] = work_.query1<std::string, std::string>(query_text);
  return domain::Author{ author_id, name };
}

std::optional<domain::Author> AuthorRepositoryImpl::GetAuthorByName(const std::string& name) {
  const auto query_text = "SELECT id, name FROM authors WHERE name = " + work_.quote(name) + ";";

   const auto& data = work_.query01<std::string, std::string>(query_text);
   if (!data) {
     return std::nullopt;
   }
   const auto& [id, author_name] = *data;
   return domain::Author{ domain::AuthorId::FromString(id), author_name };

}

std::vector<domain::Author> AuthorRepositoryImpl::GetAuthors() {
  std::vector<domain::Author> authors;
  const auto query_text = "SELECT id, name FROM authors ORDER BY name"_zv;
 
  for (const auto& [id, name] :
    work_.query<std::string, std::string>(query_text)) {
    authors.emplace_back(domain::AuthorId::FromString(id), name);
  }

  return authors;

}


void BookRepositoryImpl::Save(const domain::Book& book) {
  work_.exec_params(R"(
        INSERT INTO books (id, author_id, title, publication_year) VALUES ($1, $2, $3, $4)
        ON CONFLICT (id) DO UPDATE SET author_id=$2, title=$3, publication_year=$4;

  )"_zv, book.GetId().ToString(), book.GetAuthorId().ToString(), book.GetTitle(), book.GetPublicationYear());
}

void BookRepositoryImpl::Delete(const domain::BookId& book_id) {
  work_.exec_params("DELETE FROM books WHERE id = $1", book_id.ToString());
}

void BookRepositoryImpl::Edit(const domain::BookId book_id, const std::string& title,
  int publication_year) {
  work_.exec_params(R"(
            UPDATE books
            SET title = $2, publication_year = $3
            WHERE id = $1)"_zv, book_id.ToString(), title, publication_year);
}

void BookRepositoryImpl::DeleteByAuthorId(const domain::AuthorId& author_id) {

  work_.exec_params(R"(DELETE FROM books WHERE author_id=$1;)"_zv, author_id.ToString());

}

std::vector<domain::Book> BookRepositoryImpl::GetBooks() {
  
  std::vector<domain::Book> books;

  const auto query_text = R"(
        SELECT id, author_id, title, publication_year
        FROM books
        ORDER BY title
    )"_zv;
 

  for (const auto& [id, author_id, title, publication_year] :
    work_.query<std::string, std::string, std::string, int>(query_text)) {
    auto book_id = domain::BookId::FromString(id);
    books.emplace_back(
      book_id,
      domain::AuthorId::FromString(author_id),
      title,
      publication_year
    );
  }
  
  return books;
}

std::vector<domain::Book> BookRepositoryImpl::GetBooksByAuthor(const domain::AuthorId& author_id) {
  const auto query_text = "SELECT id, author_id, title, publication_year FROM books WHERE author_id = "
    + work_.quote(author_id.ToString())
    + " ORDER BY publication_year, title;";

  std::vector<domain::Book> books;

  for (const auto& [id, author_id, title, publication_year] :
    work_.query<std::string, std::string, std::string, int>(query_text)) {
    auto book_id = domain::BookId::FromString(id);
    books.emplace_back(
      book_id,
      domain::AuthorId::FromString(author_id),
      title,
      publication_year
    );
  }

  return books;

}

void BookTagRepositoryImpl::Save(const domain::BookTag& book_tag) {
  work_.exec_params(R"(INSERT INTO book_tags (book_id, tag) VALUES ($1, $2);)"_zv,
book_tag.GetBookId().ToString(), book_tag.GetTag());
}

void BookTagRepositoryImpl::Delete(const domain::BookId& book_id) {
  work_.exec_params(R"(DELETE FROM book_tags WHERE book_id=$1;)"_zv,
  book_id.ToString());
}

std::vector<std::string> BookTagRepositoryImpl::GetBookTags(const domain::BookId& book_id) const {
  auto result = work_.exec_params(R"(SELECT * FROM book_tags WHERE book_id=$1 ORDER BY tag;)"_zv,
    book_id.ToString());
  std::vector<std::string> book_tags;
  book_tags.reserve(result.size());
  for (const auto& tag : result) {
    book_tags.emplace_back(tag.at("tag"s).as<std::string>() );
  }
  return book_tags;
}

Database::Database(pqxx::connection connection)
    : connection_{std::move(connection)} {

    pqxx::work work{connection_};
    work.exec(R"(
CREATE TABLE IF NOT EXISTS authors (
    id UUID CONSTRAINT author_id_constraint PRIMARY KEY,
    name varchar(100) UNIQUE NOT NULL
);
)"_zv);
    
    work.exec(R"(
CREATE TABLE IF NOT EXISTS books (
    id UUID CONSTRAINT book_id_constraint PRIMARY KEY,
    author_id UUID NOT NULL,
    title varchar(100) NOT NULL,
    publication_year INTEGER NOT NULL
);
)"_zv);

    work.exec(R"(
        CREATE TABLE IF NOT EXISTS book_tags (
            book_id UUID NOT NULL,
            tag varchar(30) NOT NULL
        );
    )"_zv);

    work.commit();
}

}  // namespace postgres