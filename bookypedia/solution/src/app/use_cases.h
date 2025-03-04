#pragma once

#include <string>
#include <vector>
#include <optional>

#include "../domain/author.h"
#include "../domain/book.h"
#include "../domain/book_tags.h"


namespace app {

class UseCases {
public:

    virtual void AddAuthor(const std::string& name) = 0;
    virtual std::vector<domain::Author> GetAuthors() = 0;
    virtual domain::Author GetAuthorById(const domain::AuthorId& author_id) = 0;
    virtual std::optional<domain::Author> GetAuthorByName(const std::string& name) = 0;
    virtual void DeleteAuthor(const domain::AuthorId& author_id) = 0;
    virtual void EditAuthor(const domain::AuthorId& author_id, const std::string& name) = 0;
    virtual void AddBook(const domain::AuthorId author_id, const std::string& title, 
      int publication_year, const std::vector<std::string>& book_tags) = 0;
    virtual std::vector<domain::Book> GetBooks() = 0;
    virtual std::vector<domain::Book> GetBooksByAuthor(const domain::AuthorId& author_id) = 0;
    virtual void EditBook(const domain::BookId book_id, const std::string& title,
      int publication_year, const std::vector<std::string>& book_tags) = 0;
    virtual void DeleteBook(const domain::BookId& book_id) = 0;
    virtual std::vector<std::string> GetBookTags(const domain::BookId& book_id) const = 0;
    

protected:

    ~UseCases() = default;
};

}  // namespace app
